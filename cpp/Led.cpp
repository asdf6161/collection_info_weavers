/*
 * led.cpp
 *
 *  Created on: 13 янв. 2019 г.
 *      Author: yura
 */

#include <Led.h>
#include <exception>

Led::Led(GPIO_TypeDef *GPIO, uint32_t pin, bool state) {
	this->gpio = GPIO;
	this->pin = pin;
	this->state = state;
	this->__set_state();
}

Led::~Led() {

}

void Led::init(){
	if (this->pin < 8){
		// Todo - Дописать инициализацию
	} else
	if (this->pin < 16){
		// Todo - Дописать инициализацию
	}
}

void Led::set_state(bool on){
	this->state = on;
	this->__set_state();
}

void Led::toggle(){
	this->state = !this->state;
	this->__set_state();
}

void Led::__set_state(){
	if (this->state)
		this->gpio->BSRR |= 1 << this->pin;
	else
		this->gpio->BRR |= 1 << this->pin;
}
