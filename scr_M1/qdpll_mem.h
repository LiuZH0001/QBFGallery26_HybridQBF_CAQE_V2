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

#ifndef QDPLL_MEM_H_INCLUDED
#define QDPLL_MEM_H_INCLUDED

#include <stddef.h>

struct QDPLLMemMan
{
  size_t cur_allocated;
  size_t max_allocated;
  size_t limit;
};

typedef struct QDPLLMemMan QDPLLMemMan;

QDPLLMemMan *qdpll_create_mem_man ();

void qdpll_delete_mem_man (QDPLLMemMan * mm);

void *qdpll_malloc (QDPLLMemMan * mm, size_t size);

void *qdpll_realloc (QDPLLMemMan * mm, void *ptr, size_t old_size,
                     size_t new_size);

void qdpll_free (QDPLLMemMan * mm, void *ptr, size_t size);

size_t qdpll_max_allocated (QDPLLMemMan * mm);

size_t qdpll_cur_allocated (QDPLLMemMan * mm);

void qdpll_set_mem_limit (QDPLLMemMan * mm, size_t limit);

size_t qdpll_get_mem_limit (QDPLLMemMan * mm);

#endif
