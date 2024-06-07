#include "servomotor.h"

extern TIM_HandleTypeDef htim3;
extern volatile int TIM2_opentime_servo_1ms;

static uint8_t servo_state = OPEN;
static uint8_t servo_open_flag = 0;

void set_servo_open_flag(uint8_t pServoFlag){
	servo_open_flag = pServoFlag;
}
void set_servo_state(uint8_t pServoState){
	servo_state = pServoState;
}

void servo_motor_main(void) {
	if(servo_open_flag==1){
		switch (servo_state) {
		case OPEN:
			TIM2_opentime_servo_1ms=0;
			// 1.rotation of 0 dgrees
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 28);
			servo_state = LOCKED;
			break;
		case LOCKED:
			if (TIM2_opentime_servo_1ms >= 3000) {
				servo_state = OPEN;
				// 1.rotation of 90 dgrees
				__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 75);
				servo_open_flag = 0;
			}
			break;
		}

	}

}
