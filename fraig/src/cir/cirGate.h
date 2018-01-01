/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <iostream>
#include "cirDef.h"
#include "sat.h"
using namespace std;
class CirGate;

static IdSet null_idset;
//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGate
{
public:
  CirGate(int type, ID& ind, unsigned& lineNo):
    _type(type), _line_no(lineNo + 1), _visited(0), _ind(ind) {};
  virtual ~CirGate() {}

  // Basic access methods
  int getType() const { return _type; }
  bool isAig() const { return _type == AIG_GATE; }
  string getTypeStr() const;
  unsigned getLineNo() const { return _line_no; }

  // Printing functions
  void reportGate() const;
  void reportFanin (int level) const;
  void reportFanout(int level) const;

  // fanin fanout
  virtual unsigned fanInSize() const { return 0; }
  virtual const ID* getFanin() const { return NULL; }
  virtual const IdSet& getFanout() const { return null_idset; }
  virtual unsigned fanOutSize() const { return 0; }

  // names for pi po
  virtual string getName() const { return ""; }
  virtual void setName(string &s) {}

  // visit
  bool isVisit() const;
  static void setVisitFlag() { ++_visited_flag; }
  void netPrint() const;
  static void resetVis() { _max_level = _visited_flag = 0; }

  // virtual void printGate() const {} // no used
private:
  int _type;
  unsigned _line_no;

  // visit
  mutable unsigned _visited;
  static unsigned _max_level;
  static unsigned _visited_flag;

protected:
  ID _ind;

  void goFanin (ID, bool) const;
  void goFanout(ID, bool) const;
};

class CirGateOut: public CirGate
{
public:
  CirGateOut(int type=0, ID ind=0, unsigned lineNo=0)
    :CirGate(type, ind, lineNo) {};
  ~CirGateOut() {};
  void setFanout(ID num) { _fanout.insert(num); }
  const IdSet& getFanout() const { return _fanout; }
  void removeFanout(ID gid) { _fanout.erase(gid); }
  unsigned fanOutSize() const { return _fanout.size(); }

private:
  IdSet _fanout;
};

class GateUndef: public CirGateOut
{
public:
  GateUndef(ID ind):
    CirGateOut(UNDEF_GATE, ind, -1) {};
  ~GateUndef() {};
};

class GateConst: public CirGateOut
{
public:
  GateConst():
    CirGateOut(CONST_GATE, 0, -1) {};
  ~GateConst() {};
};

class GateIn: public CirGateOut
{
public:
  GateIn(ID ind, unsigned lineNo=0):
    CirGateOut(PI_GATE, ind, lineNo) {};
  ~GateIn() {};
  string getName() const { return _name; }
  void setName(string &s) { _name = s; }

private:
  string _name;
};

class GateOut: public CirGate
{
public:
  GateOut(ID ind, unsigned lineNo=0):
    CirGate(PO_GATE, ind, lineNo) {};
  ~GateOut() {};
  void setName(string &s) { _name = s; }
  void setFanin (ID num) { _fanin = num; }
  void updateFanin(ID from, ID to) {
    if ((_fanin ^ from) <= 1)
      _fanin = to ^ (_fanin & 1);
  }

  // virtual
  string getName() const { return _name; }
  const ID* getFanin () const { return &_fanin ; }
  unsigned fanInSize() const { return 1; }

private:
  ID _fanin;
  string _name;
};

class GateAnd: public CirGateOut
{
public:
  GateAnd(ID ind=0, unsigned lineNo=0):
    CirGateOut(AIG_GATE, ind, lineNo) { ++num; };
  ~GateAnd() { --num; };
  void setFanin (ID num0, ID num1) {
    _fanin[0] = num0;
    _fanin[1] = num1;
  }
  void updateFanin(ID from, ID to) {
    for (unsigned j=0; j<2; ++j)
      if ((_fanin[j] ^ from) <= 1)
        _fanin[j] = to ^ (_fanin[j] & 1);
  }

  // virtual
  const ID* getFanin() const { return _fanin ; }
  unsigned fanInSize() const { return 2; }

  // num of aig
  static void resetNum() { num = 0; }
  static unsigned getNum() { return num; }
private:
  static unsigned num;
  ID _fanin[2];
};

#endif // CIR_GATE_H
