/*
 * modbus_test.cpp
 *
 *  Created on: 13 янв. 2019 г.
 *      Author: yura
 */

#include <Modbus.h>
#include <random>

#ifdef TEST
void Modbus::run_all_test(){
	this->write("run_all_test\n");
	this->test_clear_reg();
	this->test_clear_all_reg();
	this->test_CRC_16();
	this->test_answer();
	this->test_queue_std();
	// Todo - Дописать запуск всех тестов
}

void Modbus::out_regs(std::string str){
	std::string str_out = "";
	this->write(str);
	for (uint8_t i = 0; i < REG_CNT; ++i) {
		str_out = std::to_string(this->reg[i]) + "\n";
		this->write(str_out);
	}
}

bool Modbus::test_clear_reg(){
	this->write("clear_reg\n");
	// Заполнение случайными значениями
	for (uint8_t i = 0; i < REG_CNT; ++i) {
		this->reg[i] = (uint32_t)std::rand();
	}
	this->out_regs("\ninit value: ");

	// Очистка
	for(uint8_t i=0; i<REG_CNT; i++){
		this->clear_reg((modbus_reg_enum)i);
	}
	this->out_regs("\ncleared value: ");

	return true;
}

bool Modbus::test_clear_all_reg(){
	this->write("clear_all_reg\n");
	// Заполнение случайными значениями
	for (uint8_t i = 0; i < REG_CNT; ++i) {
		this->reg[i] = (uint32_t)std::rand();
	}
	this->out_regs("\ninit value: ");

	this->clear_all_reg();
	this->out_regs("\ninit value: ");

	return true;
}

bool Modbus::test_CRC_16(){
	this->write("\nTest CRC16 input str 12345:\n");
	uint16_t crc = this->CRC_16((uint8_t *)"12345", 5);
	this->write("Calculated CRC: " + std::to_string(crc));
	return true;
}

bool Modbus::test_answer(){
	this->write("\nAnswer (from SLAVE) with len 12345678\n");
	modbus_mode_enum mode = this->mode;
	this->mode = MODE_SLAVE;

	// Заполняем
	uint32_t length = this->reg[REG_LENGTH];
	uint32_t error_code = this->reg[REG_ANS_CODE];
	this->reg[REG_LENGTH] = 0x41424344;
	this->reg[REG_ANS_CODE] = 0x41424344;

	// Проверка функции длинны
	this->write("\nFUNC_LENGTH bytes\n");
	this->answer(FUNC_LENGTH);
	this->write("\nFUNC_LENGTH str\n");
	this->answer_str(FUNC_LENGTH);

	// Проверка функции ошибки
	this->write("\n\nFUNC_ERROR bytes\n");
	this->answer(FUNC_ANS);
	this->write("\nFUNC_ERROR str\n");
	this->answer_str(FUNC_ANS);

	// Проверка функции установки реистра
	this->write("\n\nFUNC_SET_REG bytes\n");
	this->answer(FUNC_SET_REG);
	this->write("\nFUNC_SET_REG str\n");
	this->answer_str(FUNC_SET_REG);

	// Очистка параметров
	this->reg[REG_LENGTH] = length;
	this->reg[REG_ANS_CODE] = error_code;

	this->mode = mode;
	return true;
}

bool Modbus::test_queue_std(){
	std::queue<uint8_t> *q = new std::queue<uint8_t>();
	this->write("\nTEST QUEUE\n");
	q->push(5);
	this->write("\nq.size(): " + std::to_string(q->size()) + "\n");
	this->write("q.empty(): " + std::to_string(q->empty()) + "\n");
	this->write("q.front(): " + std::to_string(q->front()) + "\n");
	q->pop();
	this->write("\nq.size(): " + std::to_string(q->size()) + "\n");
	this->write("q.empty(): " + std::to_string(q->empty()) + "\n");
	this->write("q.front(): " + std::to_string(q->front()) + "\n");
	q->pop();
	this->write("\nq.size(): " + std::to_string(q->size()) + "\n");
	this->write("q.empty(): " + std::to_string(q->empty()) + "\n");
	this->write("q.front(): " + std::to_string(q->front()) + "\n");
	q->pop();
	this->write("\nq.size(): " + std::to_string(q->size()) + "\n");
	this->write("q.empty(): " + std::to_string(q->empty()) + "\n");
	this->write("q.front(): " + std::to_string(q->front()) + "\n");

	return true;
}

bool Modbus::test_recv(){

	// Todo дописать тест
	return true;
}
#endif /* TEST */
