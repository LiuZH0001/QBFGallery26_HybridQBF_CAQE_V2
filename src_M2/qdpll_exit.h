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

#ifndef QDPLL_EXIT_H_INCLUDED
#define QDPLL_EXIT_H_INCLUDED

enum QDPLLExitCodes
{
  QDPLL_EXIT_SUCC = 0,
  QDPLL_EXIT_ERR = 1,
  QDPLL_EXIT_SAT = 10,
  QDPLL_EXIT_UNSAT = 20
};

typedef enum QDPLLExitCodes QDPLLExitCodes;

#endif
