/*
 * led.h
 *
 *  Created on: 13 янв. 2019 г.
 *      Author: yura
 */

#ifndef LED_H_
#define LED_H_

#ifdef STM32F103xB
#include "stm32f103xb.h"
#endif

#ifdef STM32F030x6
#include <stm32f030x6.h>
#endif

#include <cstddef>

class Led {
public:
	// Переменные
	GPIO_TypeDef *gpio = NULL;
	uint32_t pin = (uint32_t)NULL;
	bool state;

	// Кноструктов деструктор
	Led(GPIO_TypeDef *GPIO, uint32_t pin, bool state);
	virtual ~Led();

	// Методы
	void init();  // initialize port with CMSIS
	void set_state(bool on);
	void toggle();

private:
	void __set_state();
};

#endif /* LED_H_ */
