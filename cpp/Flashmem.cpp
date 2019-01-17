/*
 * Flashmem.cpp
 *
 *  Created on: 15 янв. 2019 г.
 *      Author: yura
 */

#include <Flashmem.h>

#ifdef FLASH_PAGE_SIZE
Flashmem::Flashmem(uint8_t page_num) {
#else
	Flashmem::Flashmem(uint8_t page_num, uint32_t flash_page_size) {
#endif
	this->page_num = page_num;
#ifdef FLASH_PAGE_SIZE
	this->main_mem_start = MAIN_MEM_START_ADDR | (FLASH_PAGE_SIZE * this->page_num);
#else
	this->main_mem_start = MAIN_MEM_START_ADDR | (flash_page_size * this->page_num);
#endif
}

Flashmem::~Flashmem() {
	// TODO Auto-generated destructor stub
}

void Flashmem::flash_mem_write(uint8_t *data, uint32_t address, uint32_t count) {
	// Todo - Дописать проверку на выход за пределы страницы
	this->unlock_flash();
	this->flash_mem_erase(this->main_mem_start);
	uint32_t i;

	while (FLASH->SR & FLASH_SR_BSY);
	if (FLASH->SR & FLASH_SR_EOP) {
		FLASH->SR = FLASH_SR_EOP;
	}

	FLASH->CR |= FLASH_CR_PG;

	for (i = 0; i < count; i += 2) {
		*(volatile uint16_t *)(this->main_mem_start+address + i) =
				(((uint16_t)data[i + 1]) << 8) + data[i];
		while (!(FLASH->SR & FLASH_SR_EOP));
		FLASH->SR = FLASH_SR_EOP;
	}

	FLASH->CR &= ~(FLASH_CR_PG);

	this->lock_flash();
}

//Функция стирает ВСЕ страницы. При её вызове прошивка самоуничтожается
void Flashmem::flash_erase_all_pages(void) {
	FLASH->CR |= FLASH_CR_MER; //Устанавливаем бит стирания ВСЕХ страниц
	FLASH->CR |= FLASH_CR_STRT; //Начать стирание
	while(!this->flash_ready()) // Ожидание готовности.. Хотя оно уже наверное ни к чему здесь...
		FLASH->CR &= FLASH_CR_MER;
}

void Flashmem::unlock_flash() {
	FLASH->KEYR = 0x45670123;
	FLASH->KEYR = 0xCDEF89AB;
}

void Flashmem::lock_flash() {
	FLASH->CR |= FLASH_CR_LOCK;
}

uint8_t Flashmem::flash_ready(){
	return !(FLASH->SR & FLASH_SR_BSY);
}

void Flashmem::flash_mem_erase(uint32_t pageAddress) {
	while (!this->flash_ready());

	if (FLASH->SR & FLASH_SR_EOP) {
		FLASH->SR = FLASH_SR_EOP;
	}

	FLASH->CR |= FLASH_CR_PER;
	FLASH->AR = pageAddress;
	FLASH->CR |= FLASH_CR_STRT;

	while (!(FLASH->SR & FLASH_SR_EOP));

	FLASH->SR = FLASH_SR_EOP;
	FLASH->CR &= ~FLASH_CR_PER;
}
