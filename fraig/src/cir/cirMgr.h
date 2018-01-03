/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

#include "cirDef.h"
#include "cirGate.h"
#include "myHashSet.h"

extern CirMgr *cirMgr;

class CirMgr
{
public:
  CirMgr() {
    CirGate::resetVis(); // make sure (opt)
    GateAnd::resetNum(); // make sure (opt)
  }
  ~CirMgr() {
    for (unsigned i=0; i<_gates.size(); ++i)
      if (_gates[i])
        delete _gates[i];
  }

  // Access functions
  // return '0' if "gid" corresponds to an undefined gate.
  CirGate* getGate(ID gid) const { return _gates[gid]; }
  void delGate(ID&);

  // Member functions about circuit construction
  bool readCircuit(const string&);

  // Member functions about circuit optimization
  void sweep();
  void optimize();

  // Member functions about simulation
  void randomSim();
  void fileSim(ifstream&);
  void setSimLog(ofstream *logFile) { _simLog = logFile; }

  // Member functions about fraig
  void strash();
  void printFEC() const;
  void fraig();

  // Member functions about circuit reporting
  void printSummary() const;
  void printNetlist() const;
  void printPIs() const;
  void printPOs() const;
  void printFloatGates() const;
  void printFECPairs() const;
  void writeAag(ostream&) const;
  void writeGate(ostream&, CirGate*) const;

private:
  // read
  void hasSpace(fstream&, string&, string);
  void isnotUnsigned(stringstream&, unsigned&);
  void isnotGate(stringstream&, unsigned&, bool);
  void hasMore(stringstream&);

  // gate Operator
  void findFloat();
  void takeOutChild(CirGate*);
  void merge(ID, ID); // real, with-inverse
  string mergeStr;

  // others
  void printVector(const IdList &v) const;

  // dfs
  void goNetlist(unsigned, unsigned&) const;
  void goSweep(ID);
  void goOptimize(ID);
  void goFindAnd(unsigned, IdList&) const;
  void goStrash(ID);

  // var
  GateList          _gates;
  unsigned          MILOA[5];
  IdList            _ins, _outs,
                    _floats[2];
  stringstream      _comments;
  ofstream          *_simLog;
  HashSet<GateAnd*> _hash;
};

#endif // CIR_MGR_H
