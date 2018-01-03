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
  CirGate::setVisitFlag();
  int times = 1;
  while (times--) {
    for (ID &id: _ins)
      static_cast<GateIn*>(getGate(id))->setSim(
        Value(my_random()) << 62 |
        Value(my_random()) << 31 |
        Value(my_random()));
  for (unsigned i=0; i<MILOA[3]; ++i)
    goSim(MILOA[0] + i + 1);
  }
}

void CirMgr::fileSim(ifstream& patternFile)
{
  string s;
  unsigned sz = _ins.size();
  int n = 0;
  Value sim[sz + 1] = {0}; // for zero case
  bool con = true;
  while (con) {
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
      CirGate::setVisitFlag();
      for (unsigned i=0; i<MILOA[3]; ++i)
        goSim(MILOA[0] + i + 1);
    }
  }
  cout << n << " patterns simulated.\n";
}

void CirMgr::goSim(ID id)
{
  CirGate *gate = getGate(id);
  assert(gate);
  if (gate->isVisit())
    return ;
  for (unsigned i=0; i<gate->fanInSize(); ++i)
    goSim(gate->getFanin()[i] >> 1);

  if (gate->getType() != AIG_GATE && gate->getType() != PO_GATE)
    return ;

  gate->simulate();
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
