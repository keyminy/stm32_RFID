/*
 * button.h
 *
 *  Created on: Jun 5, 2024
 *      Author: HARMAN-27
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "main.h"   // for GPIO Handling

#define BUTTON0   0
#define BUTTON1   1
#define BUTTON2   2

#define BUTTON_PRESS 0   // 버튼을 누르면 high  active-low
#define BUTTON_RELEASE 1  // 버튼을 떼면 low
#define BUTTON_NUMBER 3   // 버튼 갯수

int get_button(GPIO_TypeDef  *GPIO, int button_pin, int button_num);
void button0_check(void);
void ctrl_btn_RFID_status(void);

#endif /* INC_BUTTON_H_ */
