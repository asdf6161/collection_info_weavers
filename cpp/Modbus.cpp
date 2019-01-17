/*
 * modbus.cpp
 *
 *  Created on: 13 янв. 2019 г.
 *      Author: yura
 */

#include <Modbus.h>
#include <exception>

Modbus::Modbus(	USART_TypeDef *usart,
				uint8_t addr,
				modbus_mode_enum mode,
				send_foo_type foo) {
	this->usart = usart;
	this->mode = mode;
	this->addr = addr;
	this->send_foo = foo;
	this->reg = (REG_TYPE *) calloc(REG_CNT, sizeof(REG_TYPE));
}

Modbus::~Modbus() {
	delete [] this->reg;
}

bool Modbus::__find_id(c_queue *q){
	while(queue_pop(q) != this->addr){
		// Если очередь пуста и id не найден то дальше обрабатывать смысла нет
		if (queue_is_empty(q)){
			return false;
		}
	}
	return true;
}

bool Modbus::wait_data(c_queue *q, uint8_t cnt, uint32_t timeout_ms){
	while(queue_size(q) < cnt){
		// Todo Дать таймаут на таймере
	};
	return true;
}

void Modbus::recv(c_queue *q){
	if (queue_is_empty(q))
		return;
	if (!__find_id(q))
		return;

	// Ждем func
	this->wait_data(q, 1, TIMEOUT);

	// Анонимная функция проверки crc
	auto check_crc = [this](c_queue *q, modbus_func_enum foo){
		// Ждем CRC
		this->wait_data(q, 2, TIMEOUT);
		uint8_t crc_1 = queue_pop(q);
		uint8_t crc_2 = queue_pop(q);
		uint16_t crc = (uint16_t)(crc_2 << 8) | crc_1;

		uint8_t dt_crc[2] = {this->addr, foo};
		uint16_t calc_crc = CRC_16(dt_crc, 2);
		if (crc == calc_crc)
			return true;
		return false;
	};

	switch (queue_pop(q)) {
		case FUNC_LENGTH: {
			if (check_crc(q, FUNC_LENGTH)){
				this->answer(FUNC_LENGTH);
			} else {
				this->reg[REG_ANS_CODE] = ANS_CODE_CRC_ERR;
			}
			break;
		}
		case FUNC_CLEAR_LENGTH: {
			if (check_crc(q, FUNC_CLEAR_LENGTH)){
				this->reg[REG_ANS_CODE] = ANS_CODE_OK;
				this->answer(FUNC_ANS);
			} else {
				this->reg[REG_ANS_CODE] = ANS_CODE_CRC_ERR;
			}

			break;
		}
		case FUNC_GET_ERROR: {
			if (check_crc(q, FUNC_GET_ERROR)){
				this->answer(FUNC_ANS);
			}
			break;
		}
		default: {
//			this->reg[REG_ANS_CODE] = ANS_CODE_NOT_HAVE_FUNC_ERR;
//			this->answer(FUNC_ANS);
			break;
		}
	}
}

void Modbus::clear_reg(modbus_reg_enum reg_num){
	// Todo clear_reg
	this->reg[reg_num] = 0;
}

void Modbus::clear_all_reg(){
	for (uint8_t i = 0; i < REG_CNT; ++i) {
		this->reg[i] = 0;
	}
}

uint16_t Modbus::CRC_16(uint8_t *arr, uint16_t len){
	uint16_t crc = 0xFFFF;
	for (uint16_t pos = 0; pos < len; pos++)
	{
		crc ^= (uint16_t)arr[pos];
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
	// Помните, что младший и старший байты поменяны местами, используйте соответственно (или переворачивайте)
	return crc;
}

void Modbus::answer(modbus_func_enum func){
	switch (this->mode) {
		case MODE_SLAVE: {
			std::vector<uint8_t> vc = this->build_packet(func);
			this->write(vc.data(), vc.size());
			break;
		}
		case MODE_MASTER: {
			// Todo - дописать работу мастера в modbus
			break;
		}
		default:
			break;
	}
}

void Modbus::answer_str(modbus_func_enum func){
	switch (this->mode) {
		case MODE_SLAVE: {
			std::string str = this->build_packet_str(func);
			this->write(str);
			break;
		}
		case MODE_MASTER: {
			// Todo - дописать работу мастера в modbus
			break;
		}
		default:
			break;
	}
}

std::string Modbus::build_packet_str(const modbus_func_enum func){
	std::string str = "";
	str += std::to_string(this->addr) + ';';
	str += std::to_string((uint8_t)func) + ';';
	switch (func) {
		// Формат {ID;, FUNC;, REG;, CRC;}
		case FUNC_LENGTH: {
			str += std::to_string(this->reg[REG_LENGTH]) + ';';
			break;
		}
		case FUNC_ANS: {
			// Формат {ID;, FUNC;, REG_ANS;, CRC;}
			str += std::to_string(this->reg[REG_ANS_CODE]) + ';';
			break;
		}
		case FUNC_SET_REG: {
			// Формат {ID;, FUNC;, REG_ANS;, CRC;}
			str += std::to_string(this->reg[REG_SET]) + ';';
			break;
		}
		default:
			break;
	}
	str += std::to_string(CRC_16((uint8_t *)str.c_str(), str.length())) + ';';
	return str;
}

std::vector<uint8_t> Modbus::build_packet(const modbus_func_enum func){
	std::vector<uint8_t> vc = std::vector<uint8_t>();
	vc.push_back(this->addr);
	vc.push_back(func);

	// анонимная функция для разбиения регистра на 8 бит
	auto f = [&vc, this](uint8_t len, modbus_reg_enum reg){
		for (int i = 0; i < len; i++) {
			vc.push_back(this->reg[reg] >> 8*i);
		}
	};

	switch (func) {
		// Формат {ID, FUNC, REG, CRC}
		case FUNC_LENGTH: {
			f(sizeof(REG_TYPE), REG_LENGTH);
			break;
		}
		case FUNC_ANS: {
			f(sizeof(REG_TYPE), REG_ANS_CODE);
			break;
		}
		case FUNC_SET_REG: {
			f(sizeof(REG_TYPE), REG_SET);
			break;
		}
		default:
			break;
	}

	uint16_t crc = CRC_16(vc.data(), vc.size());
	vc.push_back((uint8_t)crc);
	vc.push_back((uint8_t)(crc >> 8));

	return vc;
}

void Modbus::write(const std::string str){
	this->send_foo(this->usart, (uint8_t *)str.c_str(), str.length());
}

void Modbus::write(uint8_t *str, const uint16_t len){
	this->send_foo(this->usart, str, len);
}












