/*
 *
 *
 *  Created on: 2025年1月11日
 *      Author: yongjie
 */

#ifndef OS_SCHEDULE_H_
#define OS_SCHEDULE_H_

#include "fsl_pit.h"

void OS_TIMER_Init(void);
void OS_Schedule(void);
void free_rtos_schedule(void);


#endif /* OS_SCHEDULE_H_ */
