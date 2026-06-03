/*
 This file is part of HybridQBF_CAQE_V2.

 HybridQBF_CAQE_V2, a solver for quantified boolean formulae (QBF).
 Copyright (C) 2026 Zhaohui Liu

 HybridQBF_CAQE_V2 is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or (at
 your option) any later version.

 HybridQBF_CAQE_V2 is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with HybridQBF_CAQE_V2.  If not, see <http://www.gnu.org/licenses/>.

 This program is based on DepQBF (Copyright 2010-2017 Florian Lonsing).
 Modifications and new code Copyright (C) 2026 Zhaohui Liu.
*/

#ifndef QDPLL_PQUEUE_H_INCLUDED
#define QDPLL_PQUEUE_H_INCLUDED

#include <limits.h>
#include "qdpll_mem.h"

struct PriorityQueueElem
{
  void *data;
  unsigned int pos;
  double priority;
};

typedef struct PriorityQueueElem PriorityQueueElem;

struct PriorityQueue
{
  unsigned int size;
  unsigned int cnt;
  PriorityQueueElem *queue;
};

typedef struct PriorityQueue PriorityQueue;

#define PQUEUE_INVALID_POS UINT_MAX

PriorityQueue *pqueue_create (QDPLLMemMan * mm, unsigned int init_size);

void pqueue_delete (QDPLLMemMan * mm, PriorityQueue * pqueue);

void pqueue_adjust (QDPLLMemMan * mm, PriorityQueue * pqueue,
                    unsigned int size);

void pqueue_insert (QDPLLMemMan * mm, PriorityQueue * pqueue,
                    void *data, double priority);

void *pqueue_remove_first (PriorityQueue * pqueue);

void *pqueue_remove_min (PriorityQueue * pqueue);

void *pqueue_access_min (PriorityQueue * pqueue);

#endif
