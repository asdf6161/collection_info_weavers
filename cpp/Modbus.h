/*
 * modbus.h
 *
 *  Created on: 13 янв. 2019 г.
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
#define REG_CNT 5  				// Число регистров и размера modbus_reg_enum
#define FUNC_CNT 4  			// Число функций и размера modbus_func_enum
#define DENSITY_BUFFER_LEN 100	// Число функций и размера modbus_func_enum
#define TIMEOUT 50
typedef uint32_t REG_TYPE;  	// Размер регистра
typedef uint16_t DENSITY_TYPE;  // Тип для хранения плотности
typedef void (*send_foo_type)(USART_TypeDef *usart, uint8_t *arr, uint16_t len);

// Функции modbus
enum modbus_func_enum {
	FUNC_LENGTH,  // Длинна полотна
	FUNC_ANS,     // Используется для ответа Хранит код ошибки
	FUNC_SET_REG, // Функция установки регистра
	FUNC_CLEAR_LENGTH,  // Очистка регистра длинны
	FUNC_GET_ERROR,  	// Отправка регистра с последней ошибкой
};

// Хранит номер регистра в массиве регистров REG_TYPE *reg
// Максимальное их число 255
enum modbus_reg_enum {
	REG_LENGTH,  	// Регистр хранит длинну полотна
	REG_ANS_CODE, // Регистр хранит номер ошибки
	REG_SET,		// Регистр функции SET_REG
	REG_NOT_USED3,	//
	REG_NOT_USED4
};

// Перечисления кодов ошибок
enum modbus_ans_code {
	ANS_CODE_CRC_ERR = 1,			// Ошибка CRC
	ANS_CODE_NOT_HAVE_FUNC_ERR,		// Нету такой функции
	ANS_CODE_QUEUE_FULL_ERR,		// Очередь переполнена
	ANS_CODE_OK, 				// OK!!!
};

// Режим работы modbus
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

	// Переменные
	send_foo_type send_foo;
	USART_TypeDef *usart;
	modbus_mode_enum mode;
	uint8_t addr;
	REG_TYPE *reg;

	// Побайтовый прием
	void recv(c_queue *);

	// Очистка регистра reg_num
	void clear_reg(modbus_reg_enum reg_num);

	// Очистка всех регистров
	void clear_all_reg();

	// Расчет CRC16
	uint16_t CRC_16(uint8_t *arr, uint16_t len);

	// Отправка ответа
	void answer(modbus_func_enum func);
	// Отправка ответа строкой
	void answer_str(modbus_func_enum func);

private:
	// Формирует пакет основываясь на функции modbus_func_enum
	// Возвращает массив и его длинну через ret_len
	std::string build_packet_str(const modbus_func_enum func);
	std::vector<uint8_t> build_packet(const modbus_func_enum func);

	// пишет сообщение в usart
	void write(const std::string);
	void write(uint8_t *, const uint16_t);

	// Поиск addr в очереди
	bool __find_id(c_queue *);

	// Ожидает пока не придут данные
	bool wait_data(c_queue *q, uint8_t cnt, uint32_t timeout_ms);


#ifdef TEST
// Тесты
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
