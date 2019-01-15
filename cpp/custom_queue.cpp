/*
 * queue.c
 *
 *  Created on: 7 ���. 2019 �.
 *      Author: yura
 */

#include "custom_queue.h"

// ������� �������
void queue_init(c_queue *q) {
  q->r_adr = 0;
  q->w_adr = 0;
  return;
}

// �������� �� ������������ �������
bool queue_is_full(const c_queue *q){
  if(q->w_adr < QMAX-1) {
	  return false;
  } else {
	  return true;
  }
}

// ���������� �������� � �������
// ������ false ���� ������� ������
bool queue_insert(c_queue *q, const q_int x) {
	if(queue_is_full(q)) {
		// ������� ������
		return false;
	}
	else {
		q->qu[q->w_adr++] = x;
	}
	return true;
}

// �������� ������� �������
bool queue_is_empty(const c_queue *q) {
  if(q->w_adr == q->r_adr)
	  return true;
  else
	  return false;
}

// �������� � ����� ������� �������� �� �������
q_int queue_pop(c_queue *q) {
	q_int x;
	index_int h;
	if(queue_is_empty(q) == true) {
		// ������� �����
		return 0;
	}
	x = q->qu[q->r_adr];
	for(h = q->r_adr; h < q->w_adr; h++) {
		q->qu[h] = q->qu[h+1];
	}
	q->w_adr--;
	return x;
}

// ������ ����� ��������� � �������
index_int queue_size(const c_queue *q){
	return (q->w_adr - q->r_adr);
}















