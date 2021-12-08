#include "state_machine.h"

#define NOMINAL_HEIGHT 0.3f
#define BATTERY_LEVEL_THRESHOLD 30
#define TAKEOFF_SPEED 0.25f
#define LAND_SPEED 0.1f
#define RETURN_BASE_LAND_DISTANCE 0.7f 
#define LAND_THRESHOLD 0.1f
#define BATTERY_DEBOUNCE 500

state_fsm_t state = NOT_READY;

static void setNextState();
static void executeState();
static void executeSGBA(bool outbound);
static bool tryAvoidObstacles(range_t range, velocity2d_t* velocity);


void stateMachineStep(){
    if(supervisorIsTumbled() && state != CRASHED){
        state = CRASHED;
        stateControl.is_on_exploration_mode = false;
        DEBUG_PRINT("I am crashed \n");
    };
    setNextState();
    executeState();
}

static void setNextState(){
    static int batteryCounterStart = 0;
    static int batteryCounterExploration = 0;
    switch (state)
    {
        case (NOT_READY): 
        {
            paramVarId_t idPositioningDeck = paramGetVarId("deck", "bcFlow2");
            paramVarId_t idMultiranger = paramGetVarId("deck", "bcMultiranger");
            uint8_t positioningInit = paramGetUint(idPositioningDeck);
            uint8_t multirangerInit = paramGetUint(idMultiranger);
            if(positioningInit && multirangerInit && sensorsData.batteryLevel > BATTERY_LEVEL_THRESHOLD){
                state = READY;
                DEBUG_PRINT("I am ready \n");
            }
            break;
        }
            
        case (READY):
        { 
            if (sensorsData.batteryLevel >  BATTERY_LEVEL_THRESHOLD){
                batteryCounterStart++;
            }
            if(batteryCounterStart >= BATTERY_DEBOUNCE && stateControl.is_on_exploration_mode){
                state = TAKING_OFF;
                DEBUG_PRINT("I will take off = %d \n", sensorsData.batteryLevel);
                batteryCounterStart = 0;
                storeInitialPos();
            }
            break;
        }

        case (TAKING_OFF):
        { 
            if ( sensorsData.position.z > NOMINAL_HEIGHT){
                state = HOVERING;
                DEBUG_PRINT("I will start exploration\n");
            } 
            break;
        }
        
        case (HOVERING):
        {
            state = EXPLORATION;
            break;
        } 
            
        case (LANDING):
        {
            if ( sensorsData.position.z < LAND_THRESHOLD){
                state = NOT_READY;
                DEBUG_PRINT("I finished landing\n");
            } 
            break;
        }

        case (EXPLORATION): 
        {
            if (sensorsData.batteryLevel <= BATTERY_LEVEL_THRESHOLD){
                batteryCounterExploration++;
            }
            if(batteryCounterExploration >= BATTERY_DEBOUNCE){
                state = RETURNING_BASE;
                stateControl.is_on_exploration_mode = false;
                DEBUG_PRINT("I will return to base battery level = %d \n", sensorsData.batteryLevel);
                batteryCounterExploration = 0;
            }
            break;
        }

        case (RETURNING_BASE): 
        {
            float diffX = sensorsData.position.x - initialPos.x;  
            float diffY = sensorsData.position.y - initialPos.y;
            float distance = sqrt(diffX * diffX + diffY * diffY); 
            if(distance < RETURN_BASE_LAND_DISTANCE) {
                state = LANDING;
                DEBUG_PRINT("I will land \n");
            }
            break;   
        }
             
        case (CRASHED): 
        {
            if(!supervisorIsTumbled()){
                state = NOT_READY;
                DEBUG_PRINT("I am not crashed anymore! \n");
            }
            break; 
        }
        
        default:
            break;
    }

}

static void executeState(){

    switch (state)
    {
        case NOT_READY:
            shut_off_engines(&setpoint);
            break;

        case READY:
            // do nothing           
            break;

        case TAKING_OFF:
            take_off(&setpoint, TAKEOFF_SPEED);
            break;

        case LANDING:
            land(&setpoint, LAND_SPEED);
            break;

        case HOVERING:
            initSGBA();
            hover(&setpoint, NOMINAL_HEIGHT);
            break;  

        case (EXPLORATION) : {
            velocity2d_t velocity;
            if(tryAvoidObstacles(sensorsData.range, &velocity)){
                vel_command(&setpoint, velocity.x, velocity.y, velocity.w, NOMINAL_HEIGHT);
                initSGBA();
                DEBUG_PRINT("I have a close obstacle \n");
            }
            else {
                executeSGBA(true);
            }
            trySendBroadcast();
            break;
        }
            
        case (RETURNING_BASE) : {
            velocity2d_t velocity;
            if(tryAvoidObstacles(sensorsData.range, &velocity)){
                vel_command(&setpoint, velocity.x, velocity.y, velocity.w, NOMINAL_HEIGHT);
                DEBUG_PRINT("I have a close obstacle \n");
            }
            else {
                executeSGBA(false);
            }
            break; 
        }
               
        case CRASHED:
            shut_off_engines(&setpoint);
            break; 
        
        default:
            break;
    }
}


static bool tryAvoidObstacles(range_t range, velocity2d_t* velocity){
    const float emergencyDistance = .1f;
    
    velocity->x = .0f;
    velocity->y = .0f;
    velocity->w = .0f;

    const bool isLeftObstacle = range.left < emergencyDistance;
    const bool isRightObstacle = range.right < emergencyDistance;
    const bool isFrontObstacle = range.front < emergencyDistance;
    const bool isBackObstacle = range.back < emergencyDistance;

    const float max_speed = 0.4;
    if (isLeftObstacle) {
        velocity->y = - max_speed;
    }
    if (isRightObstacle) {
        velocity->y = max_speed;
    }
    if (isFrontObstacle) {
        velocity->x = - max_speed;
    }
    if (isBackObstacle) {
        velocity->x = max_speed;
    }

    return isLeftObstacle || isRightObstacle || isFrontObstacle || isBackObstacle;
}

static void executeSGBA(bool outbound){
    SGBA_output_t SGBA_output;
    callSGBA(&SGBA_output, outbound);
    vel_command(&setpoint, SGBA_output.vel_cmd.x, SGBA_output.vel_cmd.y, SGBA_output.vel_cmd.w, NOMINAL_HEIGHT);
}

