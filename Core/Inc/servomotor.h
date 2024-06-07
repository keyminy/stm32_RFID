/*
 * servomotor.h
 *
 *  Created on: Jun 5, 2024
 *      Author: HARMAN-27
 */

#ifndef INC_SERVOMOTOR_H_
#define INC_SERVOMOTOR_H_
#include "main.h"

typedef enum {
    OPEN = 0,
    LOCKED = 90
} ServoStateEnum;

void servo_motor_main(void);
void set_servo_state(uint8_t pServoState);
void set_servo_open_flag(uint8_t pServoFlag);

#endif /* INC_SERVOMOTOR_H_ */
