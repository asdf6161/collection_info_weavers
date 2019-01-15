/*
 * iterrupt.cpp
 *
 *  Created on: 13 янв. 2019 г.
 *      Author: yura
 */

#include "iterrupt.h"
#include "Led.h"
#include "queue"
#include "Modbus.h"

//extern std::queue<uint8_t> recive_buff;
extern c_queue recive_buff_c;
Led led = Led(LED_board_GPIO_Port, 13, true);

void USART_CharReception_Callback(void)
{
	/* Read Received character. RXNE flag is cleared by reading of RDR register */
	uint8_t recived_char = LL_USART_ReceiveData8(USART1);
	led.toggle();
//	recive_buff.push(recived_char);
	if (queue_is_full(&recive_buff_c)){
		// Todo буфер полон
	} else {
		queue_insert(&recive_buff_c, recived_char);
	}
}

/**
  * @brief  Function called in case of error detected in USART IT Handler
  * @param  None
  * @retval None
  */
void Error_Callback(void)
{

  /* Disable USARTx_IRQn */
  NVIC_DisableIRQ(USART1_IRQn);

  /* Error handling example :
    - Read USART ISR register to identify flag that leads to IT raising
    - Perform corresponding error handling treatment according to flag
  */
}
