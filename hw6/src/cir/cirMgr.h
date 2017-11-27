/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

#include "cirDef.h"

extern CirMgr *cirMgr;

class CirMgr
{
public:
  CirMgr() {}
  ~CirMgr() {
    for (unsigned i=0; i<_gates.size(); ++i)
      if (_gates[i])
        delete _gates[i];
  }

  // Access functions
  // return '0' if "gid" corresponds to an undefined gate.
  CirGate* getGate(unsigned gid) const { return _gates[gid]; }

  // Member functions about circuit construction
  bool readCircuit(const string&);

  // Member functions about circuit reporting
  void printSummary() const;
  void printNetlist() const;
  void printPIs() const;
  void printPOs() const;
  void printFloatGates() const;
  void writeAag(ostream&) const;
  unsigned getSize() const { return MILOA[0]; }

private:
  void printVector(const IdList &v) const;
  void goNetlist(unsigned, unsigned&) const;
  void findAnd(unsigned, IdList&) const;
  GateList _gates;
  unsigned MILOA[5];
  IdList _ins, _outs,
         _floats[2];
  stringstream _comments;
};

#endif // CIR_MGR_H
