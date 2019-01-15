/*
 * queue.c
 *
 *  Created on: 7 ÿíâ. 2019 ã.
 *      Author: yura
 */

#include "custom_queue.h"

// Î÷èñòêà î÷åğåäè
void queue_init(c_queue *q) {
  q->r_adr = 0;
  q->w_adr = 0;
  return;
}

// Ïğîâåğêà íà çàïîëíåíîñòü î÷åğåäè
bool queue_is_full(const c_queue *q){
  if(q->w_adr < QMAX-1) {
	  return false;
  } else {
	  return true;
  }
}

// Äîáàâëåíèå ıëåìåíòà â î÷åğåäü
// âåğíåò false åñëè î÷åğåäü ïîëíàÿ
bool queue_insert(c_queue *q, const q_int x) {
	if(queue_is_full(q)) {
		// Î÷åğåäü ïîëíàÿ
		return false;
	}
	else {
		q->qu[q->w_adr++] = x;
	}
	return true;
}

// Ïğîâåğêà ïóñòîòû î÷åğåäè
bool queue_is_empty(const c_queue *q) {
  if(q->w_adr == q->r_adr)
	  return true;
  else
	  return false;
}

// Óäàëåíèå è âûâîä ïåğâîãî ıëåìåíòà èç î÷åğåäè
q_int queue_pop(c_queue *q) {
	q_int x;
	index_int h;
	if(queue_is_empty(q) == true) {
		// Î÷åğåäü ïóñòà
		return 0;
	}
	x = q->qu[q->r_adr];
	for(h = q->r_adr; h < q->w_adr; h++) {
		q->qu[h] = q->qu[h+1];
	}
	q->w_adr--;
	return x;
}

// Âåğíåò ÷èñëî ıëåìåíòîâ â î÷åğåäè
index_int queue_size(const c_queue *q){
	return (q->w_adr - q->r_adr);
}















