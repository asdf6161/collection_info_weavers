/*
 * Flashmem.h
 *
 *  Created on: 15 €нв. 2019 г.
 *      Author: yura
 *  have code - https://habr.com/ru/post/213771/
 */

/*Example
 * uint8_t *res = flashmem.test_write((uint8_t *)"Hellow flash mem\0", 17);
 * */

#ifndef FLASHMEM_H_
#define FLASHMEM_H_

#include "stdint.h"

#ifdef STM32F103xB
#include "stm32f103xb.h"
#define FLASH_PAGE_SIZE 1024  // bytes
#endif

#define MAIN_MEM_START_ADDR 0x08000000

class Flashmem {
public:
	/*  онструктор принимает пор€дковый номер страницы
	 * ≈сли не определена директива FLASH_PAGE_SIZE, то придетс€ указать вручную
	 * flash_page_size ”казываетс€ в байтах
	 * */
#ifdef FLASH_PAGE_SIZE
	Flashmem(uint8_t page_num);
#else
	Flashmem(uint8_t page_num, uint32_t flash_page_size);
#endif
	virtual ~Flashmem();

	// vars
	uint8_t page_num;
	uint32_t main_mem_start;


	// data 	- указатель на записываемые данные
	// address  - адрес на странице начина€ с нул€
	// count 	- количество записываемых байт, должно быть кратно 2
	void flash_mem_write(uint8_t *data, uint32_t address, uint32_t count);
	void flash_erase_all_pages();

private:
	// clear page on flash
	void flash_mem_erase(uint32_t pageAddress);
	void unlock_flash();
	void lock_flash();
	uint8_t flash_ready();

// “есты
public:
	/*«аписывает str в начало страницы и возвращает указатель на неЄ*/
	uint8_t *test_write(uint8_t *str, uint16_t len);
};

#endif /* FLASHMEM_H_ */
