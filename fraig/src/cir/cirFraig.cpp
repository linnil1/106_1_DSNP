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
  _hash.init(GateAnd::getNum() << 2); // 4
  CirGate::setVisitFlag();
  for (ID &i: _outs)
    goStrash(i);
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
    merge(id, (to->getIndex() << 1));
}

void CirMgr::fraig()
{
  cout << "Total #FEC Group = " << _groupMax << endl;
  if (_simStart & SIM_DONE)
    return;
  // init
  CirGate::setVisitFlag();
  mergeStr = "Fraig";
  _hashMap.init(GateAnd::getNum() << 2); // 4

  // build sat
  _solver.initialize();
  for (ID &id: _ins)
    getGate(id)->setSatVar(_solver.newVar());
  getGate(0)->setSatVar(_solver.newVar());

  // fec group
  _fecNow = vector<IdList>(_groupMax + 1);
  _fecNow[1].push_back(0);

  // dfs
  assert(_simStart & Find_AND);
  for (ID &id: _listAnd)
    goFraig(id);

  // reset fec group
  sweep();
  unsigned _max = 1;
  for (unsigned i=1; i<=_groupMax; ++i) {
    DIdList old;
    for (DID &did: _fecCollect[i])
      if (getGate(did >> 1))
        old.push_back(did);
    if (old.size() > 1) {
      for (DID &j: old)
        getGate(j >> 1)->setFec(_max);
      _fecCollect[_max++] = old;
    }
    else if (old.size() == 1)
      getGate(old[0] >> 1)->setFec(0);
  }
  _groupMax = _max - 1;
  _fecCollect.resize(_max);
  if (!_groupMax)
    _simStart |= SIM_DONE;

  _simStart ^= Find_AND | Find_FECBase; // remove in simInit
  cout << "Total #FEC Group = " << _groupMax << endl;
}

// fraig strash opt all in one
// didn't make sure that no unused gate in input
void CirMgr::goFraig(ID id)
{
  CirGate *gate = getGate(id);
  assert(gate);

  // opt
  for (unsigned i=0; i<2; ++i)
    if (gate->getFanin()[i] == 0) {
      merge(id, 0);
      return ;
    }
    else if (gate->getFanin()[i] == 1) {
      merge(id, gate->getFanin()[i ^ 1]);
      return ;
    }
  if ((gate->getFanin()[0] ^ gate->getFanin()[1]) == 0) {
    merge(id, gate->getFanin()[0]);
    return ;
  }
  else if ((gate->getFanin()[0] ^ gate->getFanin()[1]) == 1) {
    merge(id, 0);
    return ;
  }

  // strash
  GateAnd *gateAnd = static_cast<GateAnd*>(gate);
  DID *to = _hashMap.insert(gateAnd, id << 1);
  if (to) {
    merge(id, *to);
    return ;
  }

  gate->setSatVar(_solver.newVar());
  _solver.addAigCNF(gate->getSatVar(),
                    getGate(gate->getFanin()[0] >> 1)->getSatVar(),
                    gate->getFanin()[0] & 1,
                    getGate(gate->getFanin()[1] >> 1)->getSatVar(),
                    gate->getFanin()[1] & 1);
  // fraig
  ID g = gate->getFec();
  if (!g)
    return;
  IdList &v = _fecNow[g];
  if (v.size() < 1)
    return;

  // random 2 index
  unsigned ind[v.size()];
  for (unsigned i=0; i<v.size(); ++i)
    ind[i] = i;

  // solve it with only 2 index
  for (unsigned k=0; k<2 && k<v.size(); ++k) {
    unsigned index  = my_random() % (v.size() - k);
    ID &i = v[ind[index]];
    ind[index] = ind[v.size() - 1 - k];

    // cout << "Proof : " << i << ' ' << id << endl;
    // solve by SAT
    CirGate *newGate = getGate(i);
    bool inv = newGate->getSim() != gate->getSim();
    Var newV = _solver.newVar();
    _solver.addXorCNF(newV, gate->getSatVar()   , inv  ,
                            newGate->getSatVar(), false);
    _solver.assumeRelease();
    _solver.assumeProperty(getGate(0)->getSatVar(), false);
    _solver.assumeProperty(newV, true);

    // functionally same -> merge it
    if (!_solver.assumpSolve()) {
      cout << "SAT ";
      merge(id, (i << 1) | inv, false); // do not delete
      to = _hashMap.insert(gateAnd, 0);
      assert(to);
      *to = (i << 1) | inv;
      return ;
    }
    else {
      string s;
      for (ID &j: _ins)
        s.push_back(_solver.getValue(getGate(j)->getSatVar()) == 1);
      _candiIn.push_back(s);
    }
  }
  v.push_back(id);
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
