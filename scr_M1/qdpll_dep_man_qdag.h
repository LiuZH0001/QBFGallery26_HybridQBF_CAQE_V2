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

#ifndef QDPLL_DEPMAN_QDAG_H_INCLUDED
#define QDPLL_DEPMAN_QDAG_H_INCLUDED

#include "qdpll_dep_man_generic.h"
#include "qdpll_pcnf.h"
#include "qdpll_mem.h"

typedef struct QDPLLDepManQDAG QDPLLDepManQDAG;

/* QDAG Dependency manager. */

/* Creates a qdag dependency manager. Last parameter indicates whether
to print dependencies by explicit search of CNF or by graph (for
testing only). */
QDPLLDepManQDAG *qdpll_qdag_dep_man_create (QDPLLMemMan * mm,
                                            QDPLLPCNF * pcnf,
                                            QDPLLDepManType type,
                                            int print_deps_by_search,
                                            QDPLL * qdpll);

/* Deletes a qdag dependency manager. */
void qdpll_qdag_dep_man_delete (QDPLLDepManQDAG * dm);

#endif
