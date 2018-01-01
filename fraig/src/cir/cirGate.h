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

const static IdSet null_idset;
const static string null_str;

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
  void netPrint() const;

  // fanin fanout
  virtual unsigned fanInSize() const { return 0; }
  virtual const ID* getFanin() const { return NULL; }
  virtual const IdSet& getFanout() const { return null_idset; }
  virtual unsigned fanOutSize() const { return 0; }

  // visit
  bool isVisit() const;
  static void setVisitFlag() { ++_visited_flag; }
  static void resetVis() { _max_level = _visited_flag = 0; }

  // name
  virtual const string& getName() const { return null_str; }
  virtual void setName(string &s) {}

  // virtual void printGate() const {} // no used
private:
  int              _type;
  unsigned         _line_no;

  // visit
  mutable unsigned _visited;
  static unsigned  _max_level;
  static unsigned  _visited_flag;

protected:
  ID               _ind;

  // report gate by dfs
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

class CirGateName
{
public:
  CirGateName() {};
  ~CirGateName() {};
  const string& getName() const { return _name; }
  void setName(string &s) { _name = s; }
private:
  string _name;
};

class CirGateIn
{
public:
  CirGateIn() {};
  ~CirGateIn() {};
  void setFanin (ID* num) {
    for (unsigned i=0; i<fanInSize(); ++i)
      const_cast<ID*>(getFanin())[i] = num[i];
  }
  void updateFanin(ID from, ID to) {
    for (unsigned i=0; i<fanInSize(); ++i)
      if ((getFanin()[i] ^ from) <= 1)
        const_cast<ID*>(getFanin())[i] = to ^ (getFanin()[i] & 1);
  }
  virtual unsigned fanInSize() const = 0;
  virtual const ID* getFanin() const = 0;
};

// Five type of AIG
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

class GateIn: public CirGateOut, public CirGateName
{
public:
  GateIn(ID ind, unsigned lineNo=0):
    CirGateOut(PI_GATE, ind, lineNo) {};
  ~GateIn() {};
  // name
  const string& getName() const { return CirGateName::getName(); }
  void setName(string &s) { CirGateName::setName(s); }
};

class GateOut: public CirGate, public CirGateName, public CirGateIn
{
public:
  GateOut(ID ind, unsigned lineNo=0):
    CirGate(PO_GATE, ind, lineNo) {};
  ~GateOut() {};

  // name
  const string& getName() const { return CirGateName::getName(); }
  void setName(string &s) { CirGateName::setName(s); }

  // virtual
  const ID* getFanin () const { return &_fanin; }
  unsigned fanInSize() const { return 1; }

private:
  ID _fanin;
};

class GateAnd: public CirGateOut, public CirGateIn
{
public:
  GateAnd(ID ind=0, unsigned lineNo=0):
    CirGateOut(AIG_GATE, ind, lineNo) { ++_num; };
  ~GateAnd() { --_num; };

  // virtual
  const ID* getFanin() const { return _fanin; }
  unsigned fanInSize() const { return 2; }

  // num of aig
  static void resetNum() { _num = 0; }
  static unsigned getNum() { return _num; }
private:
  static unsigned _num;
  ID _fanin[2];
};

#endif // CIR_GATE_H
