#include "servomotor.h"
extern TIM_HandleTypeDef htim3;
extern volatile int TIM2_servo_motor_count;

static uint8_t servo_state = 0;
static uint8_t servo_openFlag = 0;

void set_servo_openFlag(uint8_t pServoFlag){
	servo_openFlag = pServoFlag;
}
void set_servo_state(uint8_t pServoState){
	servo_state = pServoState;
}

void servo_motor_main(void) {
	if(servo_openFlag==1){
		// every 1sec
		/* rotation order of dgrees : 180 --> 90 --> 0 */
		switch (servo_state) {
		case 0:
			// 1.rotation of 180 dgrees
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 126); // 2ms
			break;
		case 1:
			if (TIM2_servo_motor_count >= 1000) {
				TIM2_servo_motor_count = 0;
				// 1.rotation of 90 dgrees
				__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 75);
			}
			break;
		case 2:
			// 1.rotation of 0 dgrees
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 28);
			break;
		}
		set_servo_state(2);
//			set_servo_flag(0);



	}

}
