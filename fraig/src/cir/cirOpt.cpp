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

// Keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.

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
      takeOutChild(gate, i);
      delete gate;
      _gates[i] = NULL;
    }
  }
  findFloat();
}

void CirMgr::goSweep(ID id)
{
  CirGate *gate = getGate(id);
  assert(gate);
  if (gate->isVisit())
    return ;
  for (unsigned i=0; i<gate->fanInSize(); ++i)
    goSweep(gate->getFanin()[i] >> 1);
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void CirMgr::optimize()
{
  CirGate::setVisitFlag();
  for (unsigned i=0; i<MILOA[3]; ++i)
    goOptimize(MILOA[0] + i + 1);
  findFloat();
}

void CirMgr::goOptimize(ID id)
{
  CirGate *gate = getGate(id);
  assert(gate);
  if (gate->isVisit())
    return ;
  for (unsigned i=0; i<gate->fanInSize(); ++i)
    goOptimize(gate->getFanin()[i] >> 1);

  if (!gate->isAig())
    return ;
  for (unsigned i=0; i<2; ++i)
    if (gate->getFanin()[i] == 0) {
      merge(id, 0);
      return ;
    }
    else if (gate->getFanin()[i] == 1) {
      merge(id, gate->getFanin()[i ^ 1]);
      return ;
    }
  if ((gate->getFanin()[0] ^ gate->getFanin()[1]) == 0)
    merge(id, gate->getFanin()[0]);
  else if ((gate->getFanin()[0] ^ gate->getFanin()[1]) == 1)
    merge(id, 0);
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
void CirMgr::takeOutChild(CirGate* gate, ID id)
{
  for (unsigned j=0; j<gate->fanInSize(); ++j) {
    CirGate *gchild = getGate(gate->getFanin()[j] >> 1);
    if (gchild)
      static_cast<CirGateOut*>(gchild)->removeFanout((id << 1) | (gate->getFanin()[j] & 1));
  }
}

// Be care for from is gate index , to is 2*ind | inv
void CirMgr::merge(ID from, ID to)
{
  cout << "Simplifying: " << (to >> 1)<< " merging "
       << ((to & 1) ? "!" : "") << from << "...\n";
  assert(from != (to >> 1)); // merge itself
  CirGate *gateFrom = getGate(from),
          *gateTo   = getGate(to >> 1);
  ID inv = to & 1;
  takeOutChild(gateFrom, from);

  for (const ID &i: gateFrom->getFanout()) {
    static_cast<CirGateOut*>(gateTo)->setFanout(i ^ inv);
    CirGate* gate = getGate(i >> 1);
    if      (gate->getType() == PO_GATE )
      static_cast<GateOut*> (gate)->updateFanin(from << 1, to);
    else if (gate->getType() == AIG_GATE)
      static_cast<GateAnd*>(gate)->updateFanin(from << 1, to);
    else
      assert(false);
  }

  delete gateFrom;
  _gates[from] = NULL;
}
