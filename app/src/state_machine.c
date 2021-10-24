#include "state_machine.h"

state_fsm_t state = NOT_READY;


void setNextState(){
    switch (state)
    {
        case (NOT_READY) : {
            paramVarId_t idPositioningDeck = paramGetVarId("deck", "bcFlow2");
            paramVarId_t idMultiranger = paramGetVarId("deck", "bcMultiranger");
            uint8_t positioningInit = paramGetUint(idPositioningDeck);
            uint8_t multirangerInit = paramGetUint(idMultiranger);
            if(positioningInit && multirangerInit){
                state = READY;
            }
            break;
        }
            
        case READY: // only command start mission
            if(sensorsData.batteryLevel < 30){
                DEBUG_PRINT("Please recharge drone to at least 60 !");
                break;
            }
            if(stateControl.is_on_exploration_mode){
               state = TAKING_OFF;
            }
            break;
        case TAKING_OFF: // no commands here
            if ( sensorsData.position.z > 0.5f){
                state = HOVERING;
            } 
            break;
        
        case HOVERING: // no commands here
            state = EXPLORATION;
            break;

        case LANDING:
            if ( sensorsData.position.z < 0.001f){
                state = READY;
            } 
            break;

        case EXPLORATION:
            if (!stateControl.keep_flying){
                state = LANDING;
            }
            break;

        case RETURNING_BASE:
            break;    
        
        case CRASHED:
            break; 
        
        default:
            break;
    }

}


void executeState(){

    switch (state)
    {
        case NOT_READY:
            // do nothing
            break;
        case READY:
            // do nothing
            shut_off_engines(&setpoint);
            break;
        case TAKING_OFF:
            take_off(&setpoint, (double)0.25);
            break;
        case LANDING:
            land(&setpoint, (double)0.1);
            break;
        case HOVERING:
            break;
        case EXPLORATION:
            hover(&setpoint, (double)0.5);
            break;

        case RETURNING_BASE:
            break;    
        
        case CRASHED:
            break; 
        
        default:
            break;
    }
}