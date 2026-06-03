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

#ifndef QDPLL_DYNAMIC_NENOFEX_H_INCLUDED
#define QDPLL_DYNAMIC_NENOFEX_H_INCLUDED

#include "qdpll.h"
//#include "./nenofex/nenofex.h"


//#ifdef __cplusplus
extern "C" {
//#endif

#include "./nenofex/nenofex.h"

//#ifdef __cplusplus
}
//#endif


NenofexResult dynamic_nenofex_test (QDPLL *qdpll);

#endif
