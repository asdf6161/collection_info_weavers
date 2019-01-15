/*
 * pack.cpp
 *
 *  Created on: 7 ���. 2019 �.
 *      Author: yura
 */

#include "pack.h"

void delay_ms(uint16_t ms){
	systick_cnt = 0;

	// ��������� systick
	LL_SYSTICK_EnableIT();

	while (systick_cnt < ms){
		// ���� ���� systick ���������
	}

	// ������������� systick
	LL_SYSTICK_EnableIT();
}

// �������� ������ �� rs485 ������ ������ {"err\0", ID, "str_error\0" CRC}
void send_error(const uint8_t str_err[], const uint16_t len){
	uint8_t const const_len = 5;
	uint8_t str_send[const_len+len];
	uint8_t const_arr[] = { 'e','r','r','\0', RS485_MY_ID};

	// ��������� �� ����������� �������
	for (uint8_t var = 0; var < const_len; ++var) {
		str_send[var] = const_arr[var];
	}

	// ��������� �������������� ���������
	for (uint32_t var = const_len; var < len; ++var) {
		str_send[var] = str_err[var - const_len];
	}

	// ����������
	rs485_transmit_with_CRC(USART1, str_send, sizeof(str_send));
}

// �������� ������ �� ������� RS485_CMD_GET_LEN
// ��������� {"cmd\0"(RS485_CMD_GET_LEN), ID, ������� ������ (4 �����), CRC}
void send_len(){
	uint8_t len_1 = (uint8_t)  len_bag;
	uint8_t len_2 = (uint8_t) (len_bag >> 8);
	uint8_t len_3 = (uint8_t) (len_bag >> 16);
	uint8_t len_4 = (uint8_t) (len_bag >> 24);
	uint8_t str_send[] = {'l', 'e', 'n', '\0', RS485_MY_ID, len_1, len_2, len_3, len_4};
	rs485_transmit_with_CRC(USART1, str_send, sizeof(str_send));
}

// ��������� ������ ���������� �������
void run_command(uint8_t command){
	if (command == RS485_CMD_GET_LEN){
		send_len();
	}
}

// ����� ��������� ������� (id) � ������
bool find_id(const uint8_t id, c_queue *buff, uint8_t *out_id){
	*out_id = queue_pop(buff);
	while(*out_id != id){
		// ���� ������� ����� � id �� ������ �� ������ ������������ ������ ���
		if (queue_is_empty(buff)){
			return false;
		}
		*out_id = queue_pop(buff);
	}
	return true;
}

// ���� ���� ����� �� ����������� �� cnt_buff
bool wait_data_cnt(c_queue *buff, uint8_t cnt_buff, uint16_t timeout_ms){

	// ��������� systick
	LL_SYSTICK_EnableIT();

	// �������� ������� systick
	systick_cnt = 0;

	// � ������� timeout_ms ������� ������
	bool flag = false;
	while(systick_cnt < timeout_ms){
		if (queue_size(buff) >= cnt_buff){
			flag = true;
			break;
		}
	}

	// ������������� systick
	LL_SYSTICK_EnableIT();

	// ���� flag �� ��������� �� ������ �� �������
	if (!flag){
		send_error((uint8_t *)"integrity error", 15);
		// ������� �������
		queue_init(buff);
		return false;
	}

	return true;
}

// ������ ������������ ������ {ID, COMMAND, L_END, H_END, L_CRC, H_CRC}
bool check_recive_data(c_queue *buff){
	// ����� � ������ ��������� ������ ����� ���� ������ ��� ������ ��� ������� 1 �����
	//if (queue_size(buff) >= 6) {

		// ���� ���� ID
		uint8_t recived_id;
		bool res = find_id(RS485_MY_ID, buff, &recived_id);
		if (!res)
			return false;

		// ������ �� �� ������� ������ ���� ��������� ������
		res = wait_data_cnt(buff, 5, 50);
		if (!res)
			return false;


		// ��������� ����� *********************************
		uint8_t command = queue_pop(buff);  // COMMAND
		uint8_t l_end 	= queue_pop(buff);  // END
		uint8_t h_end 	= queue_pop(buff);  // END
		uint8_t l_crc 	= queue_pop(buff);  // CRC
		uint16_t crc 	= (queue_pop(buff) << 8) | l_crc;
		// ��������� ����� *********************************

		// ������� CRC
		uint8_t recived[4] = {recived_id, command, l_end, h_end};
		uint16_t calc_crc = calc_crc16(recived, sizeof(recived));

		// ��������� CRC
		if (calc_crc == crc){
			// CRC ������
			run_command(command);
		} else {
			// CRC �� ������
			return false;
		}
	//}
	return true;
}


// ��������� CRC ��� � ���������� ������ � ���������� ������ � CRC, ����������� out_len �� ����������� ��������
uint8_t *add_CRC(uint8_t *aStringToSend, uint16_t len, uint16_t *out_len){
	uint16_t crc_code = calc_crc16(aStringToSend, len);
	uint8_t* out_str = (uint8_t *) malloc(len+2);
	*out_len = len + 2;
	strcpy((char *)out_str, (char *)aStringToSend);

	out_str[len] = (uint8_t)(crc_code);
	out_str[len + 1] = (uint8_t)(crc_code >> 8);

	return out_str;
}

// ��������� �������� ���� ������ � ����������� CRC ����
void rs485_transmit_with_CRC(USART_TypeDef *USARTx, uint8_t *aStringToSend, uint16_t len){
	uint8_t arr_with_crc[len + 2];
	uint16_t crc_code = calc_crc16(aStringToSend, len);

	for (uint16_t i = 0; i < len; ++i) {
		arr_with_crc[i] = aStringToSend[i];
	}
	arr_with_crc[len] = (uint8_t)(crc_code);
	arr_with_crc[len + 1] = (uint8_t)(crc_code >> 8);
	rs485_transmit(USARTx, arr_with_crc, len + 2);
}

// ��������� �������� ������ ����� ��� CRC
void rs485_transmit_byte(USART_TypeDef *USARTx, uint8_t bt){
	rs485_prepare_transmit(USARTx);
	wait_transmit_register_empty(USARTx);

	/* Write character in Transmit Data register.
	   TXE flag is cleared by writing data in TDR register */
	LL_USART_TransmitData8(USARTx, bt);

	rs485_prepare_recive(USARTx);
}

// �������� ������ ��� CRC
void rs485_transmit(USART_TypeDef *USARTx, uint8_t *aStringToSend, uint16_t len){
	rs485_prepare_transmit(USARTx);
	/* Send characters one per one, until last char to be sent */
	for (uint16_t i = 0; i < len; i++)
	{
		wait_transmit_register_empty(USARTx);

		/* Write character in Transmit Data register.
		   TXE flag is cleared by writing data in TDR register */
		LL_USART_TransmitData8(USARTx, aStringToSend[i]);
	}
	rs485_prepare_recive(USARTx);
}

// ������ CRC16
uint16_t calc_crc16(uint8_t buffer[], uint8_t size){
	uint16_t crc = 0xFFFF;
	for (uint16_t pos = 0; pos < size; pos++)
	{
		crc ^= (uint16_t)buffer[pos];
		for (int i = 8; i != 0; i--)
		{
			if ((crc & 0x0001) != 0)
			{
				crc >>= 1;
				crc ^= 0xA001;
			}
			else
				crc >>= 1;
		}
	}
	// �������, ��� ������� � ������� ����� �������� �������, ����������� �������������� (��� ���������������)
	return crc;
}
