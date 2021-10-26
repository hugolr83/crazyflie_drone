/*
 * com_bug_with_looping.c
 *
 *  Created on: Nov 8, 2018
 *      Author: knmcguire
 */
#include "SGBA.h"
#include "wallfollowing_multiranger_onboard.h"

#include <math.h>
#include <stdlib.h>


#include "usec_time.h"

float state_start_time;

//static variables only used for initialization


//Make variable
const uint8_t rssi_threshold = 58;// normal batteries 50/52/53/53 bigger batteries 55/57/59
const uint8_t rssi_collision_threshold = 50; // normal batteris 43/45/45/46 bigger batteries 48/50/52



static void setNextState(float* wanted_angle_dir, orientation2d_t current_orientation, int state, range_t range,
                  float* direction, bool priority, rssi_data_t rssi_data, bool outbound, int state_wf);

static void executeState(int state, SGBA_output_t* output, range_t range,
                  float current_heading, float wanted_angle_dir, int direction, int state_wf);


static int transition(int new_state)
{

  float t =  usecTimestamp() / 1e6;
  state_start_time = t;

  return new_state;

}

// statemachine functions
static float ref_distance_from_wall = 0;
static float max_speed = 0.5;
static float wanted_angle = 0;
static bool first_run = true;

void init_SGBA_controller(SGBA_init_t SGBA_init)
{
  ref_distance_from_wall = SGBA_init.distance_from_wall;
  max_speed = SGBA_init.max_speed;
  wanted_angle = SGBA_init.wanted_angle;
  first_run = true;
}


int SGBA_controller(SGBA_output_t* output, range_t range, orientation2d_t current_orientation, rssi_data_t rssi_data, bool priority, bool outbound)
{

  // Initalize static variables
  static int state = 2;
  static int state_wf = 0;
  static float wanted_angle_dir = 0;
  
  static float direction = 1;
  
  setNextState(&wanted_angle_dir, current_orientation, state,
                range,
                &direction, priority, 
                rssi_data, outbound, state_wf);
  
  executeState(state, output, 
                range,
                current_orientation.w, wanted_angle_dir, direction, state_wf);
                
  return state;
}



static void setNextState(float* wanted_angle_dir, orientation2d_t current_orientation, int state, range_t range,
                  float* direction, bool priority, rssi_data_t rssi_data, bool outbound,int state_wf)
{


    /***********************************************************
   * State definitions
   ***********************************************************/
  // 1 = forward
  // 2 = rotate_to_goal
  // 3 = wall_following
  // 4 = move out of way

  /***********************************************************
   * Handle state transitions
   ***********************************************************/

  // Initalize static variables

  static bool overwrite_and_reverse_direction = false;
  static bool cannot_go_to_goal = false;
  static uint8_t prev_rssi = 150;
  static int diff_rssi = 0;
  static bool rssi_sample_reset = false;
  static float heading_rssi = 0;
  static uint8_t correct_heading_array[8] = {0};
  static bool first_time_inbound = true;
  static float wanted_angle_hit = 0;
  static float pos_x_hit = 0;
  static float pos_y_hit = 0;
  static float pos_x_sample = 0;
  static float pos_y_sample = 0;

  // if it is reinitialized
  if (first_run) {

    *wanted_angle_dir = wraptopi(current_orientation.w - wanted_angle); // to determine the direction when turning to goal

    overwrite_and_reverse_direction = false;
    state = 2;

    float t =  usecTimestamp() / 1e6;
    state_start_time = t;
    first_run = false;
  }

  if (first_time_inbound) {
    wraptopi(wanted_angle - 3.14f);
    *wanted_angle_dir = wraptopi(current_orientation.w - wanted_angle);
    state = transition(2);
    first_time_inbound = false;
  }



  //FORWARD
  if (state == 1) {     
    if (range.front < ref_distance_from_wall + 0.2f) {

    // if looping is detected, reverse direction (only on outbound)
      if (overwrite_and_reverse_direction) {
        *direction = -1.0f * *direction;
        overwrite_and_reverse_direction = false;
      } else {
        if (range.left < range.right && range.left < 2.0f) {
          *direction = -1.0f;
        } else if (range.left > range.right && range.right < 2.0f) {
          *direction = 1.0f;

        } else if (range.left > 2.0f && range.right > 2.0f) {
          *direction = 1.0f;
        } else {

        }
      }

      pos_x_hit = current_orientation.x;
      pos_y_hit = current_orientation.y;
      wanted_angle_hit = wanted_angle;

      wall_follower_init(0.4, 0.5, 3);

      for (int it = 0; it < 8; it++) { correct_heading_array[it] = 0; }

      state = transition(3); //wall_following

    }
    return;
  } 

  //ROTATE_TO_GOAL
  if (state == 2) { 
    // check if heading is close to the preferred_angle
    bool goal_check = logicIsCloseTo(wraptopi(current_orientation.w - wanted_angle), 0, 0.1f);
    if (range.front < ref_distance_from_wall + 0.2f) {
      cannot_go_to_goal =  true;
      wall_follower_init(0.4, 0.5, 3);

      state = transition(3); //wall_following

    }
    if (goal_check) {
      state = transition(1); //forward
    }

    return;
  } 

  //WALL_FOLLOWING
  if (state == 3) {      

    // if another drone is close and there is no right of way, move out of the way
    if (priority == false && rssi_data.inter < rssi_threshold) {
      if (outbound) {
        if ((rssi_data.angle_inter < 0 && wanted_angle < 0) || (rssi_data.angle_inter > 0 && wanted_angle > 0)) {
          wanted_angle = -1 * wanted_angle;
          *wanted_angle_dir = wraptopi(current_orientation.w - wanted_angle);
          //state= transition(2);
        }
      }
      if (rssi_data.inter < rssi_collision_threshold) {
        state = transition(4);

      }
    }

    // If going forward with wall following and cannot_go_to_goal bool is still on
    //    turn it off!
    if (state_wf == 5 && cannot_go_to_goal) {
      cannot_go_to_goal  = false;
    }



    // Check if the goal is reachable from the current point of view of the agent
    float bearing_to_goal = wraptopi(wanted_angle - current_orientation.w);
    bool goal_check_WF = false;
    if (*direction == -1) {
      goal_check_WF = (bearing_to_goal < 0 && bearing_to_goal > -1.5f);
    } else {
      goal_check_WF = (bearing_to_goal > 0 && bearing_to_goal < 1.5f);
    }

    // Check if bug went into a looping while wall following,
    //    if so, then forse the reverse direction predical.
    float rel_x_loop = current_orientation.x - pos_x_hit;   //  diff_rssi = (int)prev_rssi - (int)rssi_beacon;
    float rel_y_loop = current_orientation.y - pos_y_hit;
    float loop_angle = wraptopi(atan2(rel_y_loop, rel_x_loop));

    //if(outbound)
    //{


    if (fabs(wraptopi(wanted_angle_hit + 3.14f - loop_angle)) < 1.0) {
      overwrite_and_reverse_direction = true;
    } else {
    }

    // if during wallfollowing, agent goes around wall, and heading is close to rssi _angle
    //      got to rotate to goal
    if ((state_wf == 6 || state_wf == 8) && goal_check_WF && range.front > ref_distance_from_wall + 0.4f
        && !cannot_go_to_goal) {
      *wanted_angle_dir = wraptopi(current_orientation.w - wanted_angle); // to determine the direction when turning to goal
      state = transition(2); //rotate_to_goal
    }

    // If going straight
    //    determine by the gradient of the crazyradio what the approx direction is.
    if (state_wf == 5) {


      if (!outbound) {
        // Reset sample gathering
        if (rssi_sample_reset) {
          pos_x_sample = current_orientation.x;
          pos_y_sample = current_orientation.y;
          rssi_sample_reset = false;
          prev_rssi = rssi_data.beacon;
        }


        // if the crazyflie traveled for 1 meter, than measure if it went into the right path
        float rel_x_sample = current_orientation.x - pos_x_sample;
        float rel_y_sample = current_orientation.y - pos_y_sample;
        float distance = sqrt(rel_x_sample * rel_x_sample + rel_y_sample * rel_y_sample);
        if (distance > 1.0f) {
          rssi_sample_reset = true;
          heading_rssi = current_orientation.w;
          int diff_rssi_unf = (int)prev_rssi - (int)rssi_data.beacon;

          //rssi already gets filtered at the radio_link.c
          diff_rssi = diff_rssi_unf;

          // Estimate the angle to the beacon
          wanted_angle = fillHeadingArray(correct_heading_array, heading_rssi, diff_rssi, 5);
        }
      }

    } else {
      rssi_sample_reset = true;
    }

    return;
  } 

  //MOVE_OUT_OF_WAY
  if (state == 4) {    
    // once the drone has gone by, rotate to goal
    if (rssi_data.inter >= rssi_collision_threshold) {

      state = transition(2); //rotate_to_goal
    }
    return;
  }
}



static void executeState(int state, SGBA_output_t* output, range_t range,
                  float current_heading, float wanted_angle_dir, int direction, int state_wf){
  /***********************************************************
   * Handle state actions
   ***********************************************************/

  float temp_vel_x = 0;
  float temp_vel_y = 0;
  float temp_vel_w = 0;

  //FORWARD
  if (state == 1) {        
    // stop moving if there is another drone in the way
    // forward max speed
    if (range.left < ref_distance_from_wall) {
      temp_vel_y = -0.2f;
    }
    if (range.right < ref_distance_from_wall) {
      temp_vel_y = 0.2f;
    }
    temp_vel_x = 0.5;

  } 

  //ROTATE_TO_GOAL
  if (state == 2) {  
    // rotate to goal, determined on the sign
    if (wanted_angle_dir < 0) {
      commandTurn(&temp_vel_w, 0.5);
    } else {
      commandTurn(&temp_vel_w, -0.5);
    }


  } 

  //WALL_FOLLOWING
  if (state == 3) {       
    //Get the values from the wallfollowing
    if (direction == -1) {
      state_wf = wall_follower(&temp_vel_x, &temp_vel_y, &temp_vel_w, range.front, range.left, current_heading, direction);
    } else {
      state_wf = wall_follower(&temp_vel_x, &temp_vel_y, &temp_vel_w, range.front, range.right, current_heading, direction);
    }
  } 

  //MOVE_AWAY
  if (state == 4) {      

    float save_distance = 0.7f;
    if (range.left < save_distance) {
      temp_vel_y = temp_vel_y - 0.5f;
    }
    if (range.right < save_distance) {
      temp_vel_y = temp_vel_y + 0.5f;
    }
    if (range.front < save_distance) {
      temp_vel_x = temp_vel_x - 0.5f;
    }
    if (range.back < save_distance) {
      temp_vel_x = temp_vel_x + 0.5f;
    }

  }


  output->rssi_angle = wanted_angle;
  output->state_wallfollowing = state_wf;

  output->vel_cmd.x = temp_vel_x;
  output->vel_cmd.y = temp_vel_y;
  output->vel_cmd.w = temp_vel_w;

}