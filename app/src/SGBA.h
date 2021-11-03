/*
 * com_bug_with_looping.h
 *
 *  Created on: Nov 8, 2018
 *      Author: knmcguire
 */

#ifndef SRC_LIB_WALLFOLLOWING_MULTIRANGER_ONBOARD_GRADIENT_BUG_WITH_LOOPING_H_
#define SRC_LIB_WALLFOLLOWING_MULTIRANGER_ONBOARD_GRADIENT_BUG_WITH_LOOPING_H_
#include <stdint.h>
#include <stdbool.h>
#include "SGBA_utils.h"


#include "models.h"

void init_SGBA_controller(SGBA_init_t SGBA_init, point_t initialPos);
int SGBA_controller(SGBA_output_t* output, range_t range, orientation2d_t current_orientation, rssi_data_t rssi_data, bool priority, bool outbound);

#endif /* SRC_LIB_WALLFOLLOWING_MULTIRANGER_ONBOARD_COM_BUG_WITH_LOOPING_H_ */
