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
    static int counter1 = 0;
    static int counter2 = 0;
    switch (state)
    {
        case (NOT_READY) : {
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
            
        case READY: // only command start mission to transition to taking off
            if (sensorsData.batteryLevel >  2 * BATTERY_LEVEL_THRESHOLD){
                counter1++;
            }
            if(counter1 >= BATTERY_DEBOUNCE && stateControl.is_on_exploration_mode){
                state = TAKING_OFF;
                DEBUG_PRINT("I will take off = %d \n", sensorsData.batteryLevel);
                counter1 = 0;
                storeInitialPos();
            }
            break;

        case TAKING_OFF: 
            if ( sensorsData.position.z > NOMINAL_HEIGHT){
                state = HOVERING;
                DEBUG_PRINT("I will start exploration\n");
            } 
            break;
        
        case HOVERING: 
            state = EXPLORATION;
            break;

        case LANDING:
            if ( sensorsData.position.z < LAND_THRESHOLD){
                state = NOT_READY;
                DEBUG_PRINT("I finished landing\n");
            } 
            break;

        case EXPLORATION:
            if (sensorsData.batteryLevel < BATTERY_LEVEL_THRESHOLD){
                counter2++;
            }
            if(counter2 >= BATTERY_DEBOUNCE){
                state = RETURNING_BASE;
                stateControl.is_on_exploration_mode = false;
                DEBUG_PRINT("I will return to base battery level = %d \n", sensorsData.batteryLevel);
                counter2 = 0;
            }
            break;

        case (RETURNING_BASE):{
            float diffX = sensorsData.position.x - initialPos.x;  
            float diffY = sensorsData.position.y - initialPos.y;
            float distance = diffX * diffX + diffY * diffY; 
            if(distance < RETURN_BASE_LAND_DISTANCE * RETURN_BASE_LAND_DISTANCE) {
                state = LANDING;
                DEBUG_PRINT("I will land \n");
            }
            break;   

        }
             
        
        case CRASHED:
            if(!supervisorIsTumbled()){
                state = NOT_READY;
                DEBUG_PRINT("I am not crashed anymore! \n");
            }
            break; 
        
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
            const range_t range = sensorsData.range;
            bool reset = false;
            const float emergency = .1f;
            
            float temp_vel_x = 0;
            float temp_vel_y = 0;
            const float max_speed = 0.4;
            if (range.left < emergency) {
                temp_vel_y = - max_speed;
                reset = true;
            }
            if (range.right < emergency) {
                temp_vel_y = max_speed;
                reset = true;
            }
            if (range.front < emergency) {
                temp_vel_x = - max_speed;
                reset = true;
            }
            if (range.back < emergency) {
                temp_vel_x = max_speed;
                reset = true;
            }
            if(reset){
                vel_command(&setpoint, temp_vel_x, temp_vel_y, 0, NOMINAL_HEIGHT);
                initSGBA();
                DEBUG_PRINT("I have a close obstacle \n");
            }
            else {
                executeSGBA(true);
            }
            
            break;
        }
            

        case RETURNING_BASE:
            executeSGBA(false);
            break;    
        
        case CRASHED:
            shut_off_engines(&setpoint);
            //DEBUG_PRINT("I am crashed \n");

            break; 
        
        default:
            break;
    }
}



static void executeSGBA(bool outbound){
    SGBA_output_t SGBA_output;
    callSGBA(&SGBA_output, outbound);
    vel_command(&setpoint, SGBA_output.vel_cmd.x, SGBA_output.vel_cmd.y, SGBA_output.vel_cmd.w, NOMINAL_HEIGHT);
    trySendBroadcast();
}

