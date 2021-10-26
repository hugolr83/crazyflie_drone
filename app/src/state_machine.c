#include "state_machine.h"

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
            if(positioningInit && multirangerInit && sensorsData.batteryLevel < 60){
                state = READY;
            }
            break;
        }
            
        case READY: // only command start mission to transition to taking off
            if(sensorsData.batteryLevel < 60){
                DEBUG_PRINT("Please recharge drone to at least 60 !");
                state = NOT_READY;
            }
            break;

        case TAKING_OFF: 
            if ( sensorsData.position.z > 0.5f){
                state = HOVERING;
            } 
            break;
        
        case HOVERING: 
            state = EXPLORATION;
            break;

        case LANDING:
            if ( sensorsData.position.z < 0.001f){
                state = READY;
            } 
            break;

        case EXPLORATION:
            if (sensorsData.batteryLevel < 30){
                state = RETURNING_BASE;
            }
            break;

        case RETURNING_BASE:
            float diffX = sensorsData.position.x - initialPos.x;  
            float diffY = sensorsData.position.y - initialPos.y;  
            float diffZ = sensorsData.position.z - initialPos.z;
            float distance = diffX * diffX + diffY * diffY + diffZ * diffZ;
            if(distance < 0.5 * 0.5) {
                state = LANDING;
            }
            break;    
        
        case CRASHED:
            break; 
        
        default:
            break;
    }

}

static void executeState(){

    switch (state)
    {
        case NOT_READY:
            // do nothing
            break;

        case READY:
            shut_off_engines(&setpoint);
            break;

        case TAKING_OFF:
            take_off(&setpoint, 0.25f);
            break;

        case LANDING:
            land(&setpoint, 0.1f);
            break;

        case HOVERING:
            initSGBA();
            hover(&setpoint, 0.5f);
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
    vel_command(&setpoint, SGBA_output.vel_cmd.x, SGBA_output.vel_cmd.y, SGBA_output.vel_cmd.w, 0.5f);
    trySendBroadcast();
}

