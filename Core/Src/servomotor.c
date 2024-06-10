#include "servomotor.h"

extern TIM_HandleTypeDef htim3;
extern volatile int TIM2_opentime_servo_1ms;
extern volatile int TIM2_off_servo_1ms;

static uint8_t servo_state = LOCKED;
static uint8_t servo_open_flag = 0;
static uint8_t servo_off_flag = 0;

void set_servo_open_flag(uint8_t pServoFlag){
	servo_open_flag = pServoFlag;
}
void set_servo_off_flag(uint8_t pServoFlag){
	servo_off_flag = pServoFlag;
}
void set_servo_state(uint8_t pServoState){
	servo_state = pServoState;
}

void init_servo_LOCKED(void){
	set_servo_state(LOCKED);
	set_servo_open_flag(0);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2); // Start PWM for the servo motor
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 75); // Set servo to locked position
	servo_off_flag = 1;
	TIM2_off_servo_1ms = 0;
}

void servo_motor_main(void) {
	if(servo_open_flag==1){
		switch (servo_state) {
		case OPEN:
			HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2); // ADD_PWM_SERVO_MOTOR
			TIM2_opentime_servo_1ms=0;
			// 1.rotation of 0 dgrees
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 30);
			servo_state = LOCKED;
			break;
		case LOCKED:
			if(TIM2_opentime_servo_1ms>=3000){
				// 1.rotation of 90 dgrees
				__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 75);
				servo_off_flag = 1;
				servo_open_flag = 0;
				TIM2_off_servo_1ms = 0;
			}
			break;
		}
	}
	if(servo_off_flag == 1){
		if(TIM2_off_servo_1ms>=2000){
			TIM2_off_servo_1ms = 0;
			HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_2); // servo motor pwm 떨림 현상 방지.
			servo_off_flag = 0;
		}
	}
}
