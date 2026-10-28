/*
 * Command Handler
 *
 *  Created on: Jan 15, 2026
 *      Author: AU
 */

#ifndef INC_CLITASK_H_
#define INC_CLITASK_H_

#include "stdbool.h"

bool CliTask_Is_Usb_Ready(void);

void CliTaskHandler(void *argument);

#endif /* INC_CLITASK_H_ */
