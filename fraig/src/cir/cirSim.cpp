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

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void CirMgr::randomSim()
{
  simInit();
  int times = 1;
  while (times--) {
    for (ID &id: _ins)
      static_cast<GateIn*>(getGate(id))->setSim(
        Value(my_random()) << 62 |
        Value(my_random()) << 31 |
        Value(my_random()));
    simulate(64);
  }
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
      ++n;
      // error
      if (s.size() != sz) {
        cerr << "Error: Pattern(" << s << ") length(" << s.size()
             << ") does not match the number of inputs(" << sz << ") in a circuit!!\n"
             << (n / 64 * 64) << " patterns simulated.\n";
        return ;
      }
      for (unsigned i=0; i<sz; ++i) {
        // error
        if (s[i] != '0' && s[i] != '1') {
          cerr << "Error: Pattern(" << s << ")  contains a non-0/1 character('"
               << s[i] << "').\n"
               << (n / 64 * 64) << " patterns simulated.\n";
          return ;
        }
        // input
        else if (s[i] == '1')
          sim[i] |= (1UL << ((n - 1) & 63));
      }
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
      simulate((n & 63) ? (n & 63) : 64);
    }
  }
  cout << n << " patterns simulated.\n";
  collectFec();
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
void CirMgr::simInit()
{
  if (!_simStart) {
    CirGate::setVisitFlag();
    _listAnd = IdList();
    _FECs = IdList();
    for (unsigned i=0; i<MILOA[3]; ++i)
      goFindAnd(MILOA[0] + i + 1, _listAnd);
  }
  _simStart = true;
}

void CirMgr::simulate(int n)
{
  assert(n);
  // simulate
  for (ID &i: _listAnd)
    getGate(i)->simulate();

  // init fec if first time use
  if (!_FECs.size()) {
    _FECs = _listAnd;
    _FECs.push_back(0);
    sort(_FECs.begin(), _FECs.end());
    for (ID &i: _FECs)
      i = (i << 1) | (getVal(i << 1) & 1);
  }

  // regroup fec
  Value mask = n == 64 ? ULONG_MAX : ((1UL << n) - 1);
  typedef pair<unsigned, Value> PV; // orignal group, new group
  map<PV, pair<unsigned, unsigned> > m; // size, groupId
  //// classify and count
  for (ID &i: _FECs)
    ++m[PV(getGate(i >> 1)->getFec(), getVal(i) & mask)].first;

  //// change groupID for each AIG
  unsigned ri = _groupMax = 0;
  for (ID &i: _FECs) {
    // remove separated fec
    unsigned s = m[PV(getGate(i >> 1)->getFec(), getVal(i) & mask)].first;
    if (s <= 1) {
      getGate(i >> 1)->setFec(0);
      continue;
    }
    // set to each gate
    _FECs[ri++] = i;
    unsigned& g = m[PV(getGate(i >> 1)->getFec(), getVal(i) & mask)].second;
    if (!g)
      g = ++_groupMax;
    getGate(i >> 1)->setFec(g);
  }
  _FECs.resize(ri);
}

void CirMgr::collectFec()
{
  _fecCollect = vector<IdList>(_groupMax + 1);
  for (ID &i: _FECs) {
    unsigned g = getGate(i >> 1)->getFec();
    _fecCollect[g].push_back(i);
  }
}

void CirMgr::printVector2(const IdList &v, bool std, ID skip/*=UINT_MAX*/) const
{
  for (const unsigned& gid:v) {
    if (gid >> 1 == skip)
      continue;
    cout << ' ';
    if ((getVal(gid) & 1) ^ std ^ (gid & 1))
      cout << '!';
    cout << (gid >> 1);
  }
}

void CirMgr::printFEC(const ID id) const
{
  if (!_simStart)
    return ;
  CirGate *gate = getGate(id);
  printVector2(_fecCollect[gate->getFec()], gate->getSim() & 1, id);
}
