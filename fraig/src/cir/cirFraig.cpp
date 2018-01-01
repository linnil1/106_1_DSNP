/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void CirMgr::strash()
{
  mergeStr = "Strashing";
  _hash.init(GateAnd::getNum() << 4);
  CirGate::setVisitFlag();
  for (unsigned i=0; i<MILOA[3]; ++i)
    goStrash(MILOA[0] + i + 1);
  _hash.reset();
}

void CirMgr::goStrash(ID id)
{
  CirGate *gate = getGate(id);
  assert(gate);
  if (gate->isVisit())
    return ;
  for (unsigned i=0; i<gate->fanInSize(); ++i)
    goStrash(gate->getFanin()[i] >> 1);
  if (!gate->isAig())
    return ;

  GateAnd *gateAnd = static_cast<GateAnd*>(gate);
  GateAnd *to = _hash.insert(gateAnd);
  if (to)
    merge(id, (to->getIndex() << 1) | (gateAnd->isInv(*to)));
}

void CirMgr::fraig()
{
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
