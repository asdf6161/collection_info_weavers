/*
 * queue.h
 *
 *  Created on: 7 ���. 2019 �.
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

// ����� ���������� ��������
struct queue_struct {
  q_int qu[QMAX];
  // frnt � ������� ������� �������� � �������;
  // rear � ������� ���������� �������� � �������
  index_int w_adr, r_adr;
};

// �������� �� ������������ �������
bool queue_is_full(const c_queue *q);

// ������� �������
void queue_init(c_queue *q);

// ���������� �������� � �������
// ������ false ���� ������� ������
bool queue_insert(c_queue *q, const q_int x);

// �������� ������� �������
bool queue_is_empty(const c_queue *q);

// �������� � ����� ������� �������� �� �������
// ������ 0 ���� ������� �����
q_int queue_pop(c_queue *q);

// ������ ����� ��������� � �������
index_int queue_size(const c_queue *q);

#endif /* QUEUE_H_ */
