/****************************************************************************
  FileName     [ cirDef.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic data or var for cir package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_DEF_H
#define CIR_DEF_H

#include <vector>
#include <set>
#include "myHashMap.h"

using namespace std;

// define your own typedef or enum

class CirGate;
class CirMgr;
class SatSolver;

typedef vector<CirGate*> GateList;
typedef unsigned         ID;
typedef unsigned         DID;
typedef vector<ID>       IdList;
typedef vector<DID>      DIdList;
typedef unsigned long    Value;

enum GateType
{
  UNDEF_GATE = 0,
  PI_GATE    = 1,
  PO_GATE    = 2,
  AIG_GATE   = 3,
  CONST_GATE = 4,

  TOT_GATE
};

enum SimType
{
  Find_AND     = 1,
  Find_FEC     = 4,
  Find_FECBase = 2,
  SIM_DONE     = 8
};
#endif // CIR_DEF_H
