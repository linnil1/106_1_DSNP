/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <map>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

#define printReturn(sz) {                         \
  cout << '\r' << sz << " patterns simulated.\n"; \
  return ;                                        \
}

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void CirMgr::randomSim()
{
  simInit();
  if (_simStart & SIM_DONE)
    printReturn(0);
  unsigned sz = _ins.size();
  unsigned n = 0;

  // candiate pair from sat
  Value sim[sz + 1] = {0}; // for zero case
  for (string &s: _candiIn) {
    for (unsigned j=0; j<s.size(); ++j)
      if (s[j])
        sim[j] |= Value(1) << (n & 63);
    ++n;
    if (!(n & 63) || n == _candiIn.size()) {
      for (unsigned i=0; i<sz; ++i) {
        static_cast<GateIn*>(getGate(_ins[i]))->setSim(sim[i]);
        sim[i] = 0;
      }
      simulate(n & 63 ? n & 63 : 64);
      if (_simStart & SIM_DONE) {
        _candiIn.clear();
        printReturn(n);
      }
    }
  }
  _candiIn.clear();

  // random
  n += sz * 64;
  while (sz--) {
    for (ID &id: _ins)
      static_cast<GateIn*>(getGate(id))->setSim(
        Value(my_random()) << 62 |
        Value(my_random()) << 31 |
        Value(my_random()));
    unsigned _max = _groupMax;
    simulate(64);
    if (_simStart & SIM_DONE)
      printReturn(n);

    // if no work very well kill it
    _max = _max < _groupMax ? _groupMax - _max :
                              _max - _groupMax ;
    if (_max * 1000 <=  _groupMax) // too small
      break;
    _max = _groupMax;
  }
  cout << '\r' << n << " patterns simulated.\n";
  collectFec();
}

void CirMgr::fileSim(ifstream& patternFile)
{
  simInit();
  string s;
  unsigned sz = _ins.size();
  int n = 0;
  Value sim[sz + 1] = {0}; // for zero case
  bool con = true;
  while (con) {
    // read
    if (patternFile >> s) {
      // error
      if (s.size() != sz) {
        cerr << "\nError: Pattern(" << s << ") length(" << s.size()
             << ") does not match the number of inputs(" << sz << ") in a circuit!!\n";
        n = n / 64 * 64;
        break;
      }
      for (unsigned i=0; i<sz; ++i) {
        // error
        if (s[i] != '0' && s[i] != '1') {
          cerr << "\nError: Pattern(" << s << ") contains a non-0/1 character('"
               << s[i] << "').\n";
          n = n / 64 * 64;
          con = false;
          break;
        }
        // input
        else if (s[i] == '1')
          sim[i] |= 1UL << (n & 63);
      }
      if (!con)
        break;
      ++n;
    }
    else
      con = false;

    // go dfs
    // n & 63 = n % 64
    if ((!con && (n & 63)) || (con && n && !(n & 63))) {
      for (unsigned i=0; i<sz; ++i) {
        static_cast<GateIn*>(getGate(_ins[i]))->setSim(sim[i]);
        sim[i] = 0;
      }
      simulate(n & 63 ? n & 63 : 64);
    }
  }
  cout << "\r" << n << " patterns simulated.\n";
  if (n)
    collectFec();
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
void CirMgr::simInit()
{
  // _simStart -> see cirDef.h

  // find AND list
  if ((_simStart & Find_AND) == 0) {
    CirGate::setVisitFlag();
    _listAnd = IdList();
    for (ID &i: _outs)
      goFindAnd(i, _listAnd);
    _simStart ^= Find_AND;
  }
  if ((_simStart & Find_FECBase) == 0) {
    // init with zero
    // _FECs = DIdList();
    // after friag
    unsigned ri = 0;
    for (DID &id: _FECs) // this is real gate
      if (getGate(id >> 1) && getGate(id >> 1)->getFec())
        _FECs[ri++] = id;
    if (_FECs.size() && ri == 0)
      _simStart |= SIM_DONE;
    _FECs.resize(ri);
    _simStart ^= Find_FECBase;
  }
}

void CirMgr::simulate(int n)
{
  assert(_simStart & Find_AND);
  // simulate
  for (ID &id: _listAnd)
    getGate(id)->simulate();
  for (ID &id: _outs) // this can be skip
    getGate(id)->simulate();

  // write in log if needed
  if (_simLog)
    for (int i=0; i<n; ++i) {
      for (ID &id: _ins)
        *_simLog << ((getGate(id)->getSim() >> i) & 1);
      *_simLog << ' ';
      for (ID &id: _outs)
        *_simLog << ((getGate(id)->getSim() >> i) & 1);
      *_simLog << '\n';
    }

  // kill
  if (_simStart & SIM_DONE)
    return ;
  // init fec if first time use
  // fec will has at least 0 in it
  assert(_simStart & Find_FECBase);
  if ((_simStart & Find_FEC) == 0) {
    _FECs = _listAnd;
    _FECs.push_back(0);
    sort(_FECs.begin(), _FECs.end());
    for (DID &i: _FECs) // ID to DID
      i = (i << 1) | (getVal(i << 1) & 1);
    _simStart ^= Find_FEC;
  }
  assert(_simStart & Find_FEC);

  // regroup fec
  typedef pair<DID, Value> PV; // orignal group, new group
  map<PV, pair<unsigned, DID> > m; // size, groupId
  //// classify and count
  for (DID &i: _FECs)
    ++m[PV(getGate(i >> 1)->getFec(), getVal(i))].first;

  //// change groupID for each AIG
  unsigned ri = _groupMax = 0;
  for (DID &i: _FECs) {
    // remove separated fec
    unsigned s = m[PV(getGate(i >> 1)->getFec(), getVal(i))].first;
    if (s <= 1) {
      getGate(i >> 1)->setFec(0);
      continue;
    }
    // set to each gate
    _FECs[ri++] = i;
    unsigned& g = m[PV(getGate(i >> 1)->getFec(), getVal(i))].second;
    if (!g)
      g = ++_groupMax;
    getGate(i >> 1)->setFec(g);
  }
  _FECs.resize(ri);
  if (!ri || !_groupMax)
    _simStart |= SIM_DONE;
  cout << "\r" << "Total #FEC Group = " << _groupMax;
}

void CirMgr::collectFec()
{
  if (_simStart & SIM_DONE)
    return ;
  // collect
  _fecCollect = vector<DIdList>(_groupMax + 1);
  for (DID &i: _FECs) {
    unsigned g = getGate(i >> 1)->getFec();
    _fecCollect[g].push_back(i);
  }
}
