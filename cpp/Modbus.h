/*
 * modbus.h
 *
 *  Created on: 13 ���. 2019 �.
 *      Author: yura
 */

#ifndef MODBUS_H_
#define MODBUS_H_

#include <vector>
#include <functional>
#include "usart.h"
#include <string>
#include "custom_queue.h"
#include "queue"

extern "C" {
#ifdef STM32F103xB
#include "stm32f103xb.h"
#endif
}

#define TEST
#define REG_CNT 5  				// ����� ��������� � ������� modbus_reg_enum
#define FUNC_CNT 4  			// ����� ������� � ������� modbus_func_enum
#define DENSITY_BUFFER_LEN 100	// ����� ������� � ������� modbus_func_enum
#define TIMEOUT 50
typedef uint32_t REG_TYPE;  	// ������ ��������
typedef uint16_t DENSITY_TYPE;  // ��� ��� �������� ���������
typedef void (*send_foo_type)(USART_TypeDef *usart, uint8_t *arr, uint16_t len);

// ������� modbus
enum modbus_func_enum {
	FUNC_LENGTH,  // ������ �������
	FUNC_ANS,     // ������������ ��� ������ ������ ��� ������
	FUNC_SET_REG, // ������� ��������� ��������
	FUNC_CLEAR_LENGTH,  // ������� �������� ������
	FUNC_GET_ERROR,  	// �������� �������� � ��������� �������
};

// ������ ����� �������� � ������� ��������� REG_TYPE *reg
// ������������ �� ����� 255
enum modbus_reg_enum {
	REG_LENGTH,  	// ������� ������ ������ �������
	REG_ANS_CODE, // ������� ������ ����� ������
	REG_SET,		// ������� ������� SET_REG
	REG_NOT_USED3,	//
	REG_NOT_USED4
};

// ������������ ����� ������
enum modbus_ans_code {
	ANS_CODE_CRC_ERR = 1,			// ������ CRC
	ANS_CODE_NOT_HAVE_FUNC_ERR,		// ���� ����� �������
	ANS_CODE_QUEUE_FULL_ERR,		// ������� �����������
	ANS_CODE_OK, 				// OK!!!
};

// ����� ������ modbus
enum modbus_mode_enum {
	MODE_SLAVE,
	MODE_MASTER
};


class Modbus {
public:
	Modbus(USART_TypeDef *usart,
			uint8_t addr,
			modbus_mode_enum,
			send_foo_type);
	virtual ~Modbus();

	// ����������
	send_foo_type send_foo;
	USART_TypeDef *usart;
	modbus_mode_enum mode;
	uint8_t addr;
	REG_TYPE *reg;

	// ���������� �����
	void recv(c_queue *);

	// ������� �������� reg_num
	void clear_reg(modbus_reg_enum reg_num);

	// ������� ���� ���������
	void clear_all_reg();

	// ������ CRC16
	uint16_t CRC_16(uint8_t *arr, uint16_t len);

	// �������� ������
	void answer(modbus_func_enum func);
	// �������� ������ �������
	void answer_str(modbus_func_enum func);

private:
	// ��������� ����� ����������� �� ������� modbus_func_enum
	// ���������� ������ � ��� ������ ����� ret_len
	std::string build_packet_str(const modbus_func_enum func);
	std::vector<uint8_t> build_packet(const modbus_func_enum func);

	// ����� ��������� � usart
	void write(const std::string);
	void write(uint8_t *, const uint16_t);

	// ����� addr � �������
	bool __find_id(c_queue *);

	// ������� ���� �� ������ ������
	bool wait_data(c_queue *q, uint8_t cnt, uint32_t timeout_ms);


#ifdef TEST
// �����
public:
	void run_all_test();
	bool test_clear_reg();
	bool test_clear_all_reg();
	bool test_CRC_16();
	bool test_answer();
	bool test_recv();
	bool test_queue_std();
	void out_regs(std::string);
	std::function<bool(Modbus&)> test_f[6]= {	&Modbus::test_clear_reg,
												&Modbus::test_clear_all_reg,
												&Modbus::test_CRC_16,
												&Modbus::test_answer,
												&Modbus::test_queue_std,
												&Modbus::test_recv};
#endif /* TEST */
};

#endif /* MODBUS_H_ */
