#include "Flashmem.h"

uint8_t *Flashmem::test_write(uint8_t *str, uint16_t len){
	this->flash_mem_write(str, 0, len);
	return (uint8_t *)(void *)this->main_mem_start;
}
