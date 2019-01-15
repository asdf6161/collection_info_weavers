/*
 * queue.h
 *
 *  Created on: 7 янв. 2019 г.
 *      Author: yura
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include "stdbool.h"
#include "stdint.h"

typedef  struct queue_struct c_queue;
typedef volatile uint8_t q_int;
typedef volatile int16_t index_int;
#define QMAX 64

// После объявления очистить
struct queue_struct {
  q_int qu[QMAX];
  // frnt – позиция первого элемента в очереди;
  // rear – позиция последнего элемента в очереди
  index_int w_adr, r_adr;
};

// Проверка на заполненость очереди
bool queue_is_full(const c_queue *q);

// Очистка очереди
void queue_init(c_queue *q);

// Добавление элемента в очередь
// вернет false если очередь полная
bool queue_insert(c_queue *q, const q_int x);

// Проверка пустоты очереди
bool queue_is_empty(const c_queue *q);

// Удаление и вывод первого элемента из очереди
// Вернет 0 если очередь пуста
q_int queue_pop(c_queue *q);

// Вернет число элементов в очереди
index_int queue_size(const c_queue *q);

#endif /* QUEUE_H_ */
