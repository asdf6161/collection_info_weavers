/*
 * pack.h
 *
 *  Created on: 7 янв. 2019 г.
 *      Author: yura
 */

#ifndef PACK_H_
#define PACK_H_

#include "stdbool.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"

#include "main.h"
#include "custom_queue.h"
#include "usart.h"

extern volatile uint16_t systick_cnt;
extern volatile uint32_t len_bag;


// Задежка на systick
void delay_ms(uint16_t ms);

// Передает ошибку по rs485 формат ошибки {ID, "err\0", CRC}
void send_error(const uint8_t str_err[], const uint16_t len);

// Передача данных по команде RS485_CMD_GET_LEN
// Структура {ID, "cmd\0", RS485_CMD_GET_LEN, счетчик длинны (4 байта), CRC}
void send_len();

// Выполняет запуск переданной команды
void run_command(uint8_t command);

// Поиск заданного символа (id) в буфере
bool find_id(const uint8_t id, c_queue *buff, uint8_t *out_id);

// ЖДет пока буфер не заполниться на cnt_buff
bool wait_data_cnt(c_queue *buff, uint8_t cnt_buff, uint16_t timeout_ms);

// Формат проверяемого пакета {ID, COMMAND, L_END, H_END, L_CRC, H_CRC}
bool check_recive_data(c_queue *buff);
void rs485_transmit_with_CRC(USART_TypeDef *USARTx, uint8_t *aStringToSend, uint16_t len);
void rs485_transmit_byte(USART_TypeDef*, uint8_t);
void rs485_transmit(USART_TypeDef*, uint8_t*, uint16_t);

// Расчет CRC16
uint16_t calc_crc16(uint8_t buffer[], uint8_t size);

// Ожидает момента когда регистр передачи опустеет
__STATIC_INLINE void wait_transmit_register_empty(USART_TypeDef *USARTx){
	while (!LL_USART_IsActiveFlag_TXE(USARTx)){}
}

// Ожидает окончани¤ передачи
__STATIC_INLINE void wait_transmission_complite(USART_TypeDef *USARTx){
	while (!LL_USART_IsActiveFlag_TC(USARTx)){}
}

__STATIC_INLINE void rs485_prepare_transmit(USART_TypeDef *USARTx){
	wait_transmission_complite(USARTx);
	delay_ms(2);
	LL_USART_DisableIT_RXNE(USART1);
	LL_GPIO_SetOutputPin(USART_DE_GPIO_Port, USART_DE_Pin);
}

__STATIC_INLINE void rs485_prepare_recive(USART_TypeDef *USARTx){
	wait_transmission_complite(USARTx);
	LL_GPIO_ResetOutputPin(USART_DE_GPIO_Port, USART_DE_Pin);
    LL_USART_EnableIT_RXNE(USART1);
}

#endif /* PACK_H_ */
