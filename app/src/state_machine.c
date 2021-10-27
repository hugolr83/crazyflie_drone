#include "state_machine.h"

#define NOMINAL_HEIGHT 0.5f
#define BATTERY_LEVEL_THRESHOLD 30
#define TAKEOFF_SPEED 0.25f
#define LAND_SPEED 0.1f
#define RETURN_BASE_LAND_DISTANCE 0.5f 
#define LAND_THRESHOLD 0.001f

state_fsm_t state = NOT_READY;
static point_t initialPos;

static void setNextState();
static void executeState();


void stateMachineStep(){
    setNextState();
    executeState();
}

static void setNextState(){
    switch (state)
    {
        case (NOT_READY) : {
            paramVarId_t idPositioningDeck = paramGetVarId("deck", "bcFlow2");
            paramVarId_t idMultiranger = paramGetVarId("deck", "bcMultiranger");
            uint8_t positioningInit = paramGetUint(idPositioningDeck);
            uint8_t multirangerInit = paramGetUint(idMultiranger);
            if(positioningInit && multirangerInit && sensorsData.batteryLevel > BATTERY_LEVEL_THRESHOLD){
                state = READY;
                DEBUG_PRINT("drone is ready\n");
            }
            break;
        }
            
        case READY: // only command start mission to transition to taking off
            if(sensorsData.batteryLevel < BATTERY_LEVEL_THRESHOLD){
                DEBUG_PRINT("Please recharge drone to at least 60 !");
                state = NOT_READY;
            }
            break;

        case TAKING_OFF: 
            if ( sensorsData.position.z > NOMINAL_HEIGHT){
                state = HOVERING;
                DEBUG_PRINT("Switched from takingoff to not hovering\n");
            } 
            break;
        
        case HOVERING: 
            state = EXPLORATION;
            break;

        case LANDING:
            if ( sensorsData.position.z < LAND_THRESHOLD){
                state = NOT_READY;
                DEBUG_PRINT("Switched from landing to not ready\n");
            } 
            break;

        case EXPLORATION:
            if (sensorsData.batteryLevel < BATTERY_LEVEL_THRESHOLD){
                state = RETURNING_BASE;
                DEBUG_PRINT("Switched from exploration to returning base battery level = %d \n", sensorsData.batteryLevel);
            }
            break;

        case (RETURNING_BASE):{
            // float diffX = sensorsData.position.x - initialPos.x;  
            // float diffY = sensorsData.position.y - initialPos.y;
            // float distance = diffX * diffX + diffY * diffY; //+ diffZ * diffZ;
            // if(distance < RETURN_BASE_LAND_DISTANCE * RETURN_BASE_LAND_DISTANCE) {
            //     state = LANDING;
            //     DEBUG_PRINT("Switched from returning base to landing \n");
            // }

            if (getBeaconRSSI() < RSSI_BEACON_THRESHOLD) {
                state = LANDING;
                DEBUG_PRINT("Switched from returning base to landing \n");
            }
            break;   

        }
             
        
        case CRASHED:
            break; 
        
        default:
            break;
    }

}

static void executeSGBA(bool outbound);

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

        case EXPLORATION:
            executeSGBA(true);
            break;

        case RETURNING_BASE:
            executeSGBA(false);
            break;    
        
        case CRASHED: // TODO
            break; 
        
        default:
            break;
    }
}

void storeInitialPos() {
    initialPos.x = sensorsData.position.x;
    initialPos.y = sensorsData.position.x;
    initialPos.z = sensorsData.position.x;
    initialPos.timestamp = sensorsData.position.x;
}

static void executeSGBA(bool outbound){
    SGBA_output_t SGBA_output;
    callSGBA(&SGBA_output, outbound);
    vel_command(&setpoint, SGBA_output.vel_cmd.x, SGBA_output.vel_cmd.y, SGBA_output.vel_cmd.w, NOMINAL_HEIGHT);
    trySendBroadcast();
}

