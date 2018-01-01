/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void CirMgr::sweep()
{
  CirGate::setVisitFlag();
  for (unsigned i=0; i<MILOA[3]; ++i)
    goSweep(MILOA[0] + i + 1);
  for (unsigned i=1; i<=MILOA[0]; ++i) {
    CirGate *gate = getGate(i);
    if (gate && gate->getType() != PI_GATE && gate->getType() != PO_GATE && !gate->isVisit()) {
      cout << "Sweeping: " << gate->getTypeStr()
           << '(' << i << ") removed..." << endl;
      for (unsigned j=0; j<gate->fanInSize(); ++j) {
        CirGate *gchild = getGate(gate->getFanin()[j] >> 1);
        if (gchild)
          static_cast<CirGateOut*>(gchild)->removeFanout((i << 1) | (gate->getFanin()[j] & 1));
      }
      delete gate;
      _gates[i] = NULL;
    }
  }
}

void CirMgr::goSweep(ID id)
{
  CirGate *gate = getGate(id);
  assert(gate);
  if (gate->getType() == UNDEF_GATE || gate->isVisit())
    return ;
  for (unsigned i=0; i<gate->fanInSize(); ++i)
    goSweep(gate->getFanin()[i] >> 1);
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void CirMgr::optimize()
{
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
