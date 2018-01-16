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
#include <algorithm>
#include "cirDef.h"
#include "sat.h"

using namespace std;

class CirGate;

const static IdList null_idset;
const static string null_str;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGate
{
public:
  CirGate(int type, ID& ind, unsigned& lineNo):
    _type(type), _line_no(lineNo + 1), _ind(ind),
    _fecID(0), _visited(0), _sim(0) {};
  virtual ~CirGate() {}

  // Basic access methods
  int getType() const { return _type; }
  bool isAig() const { return _type == AIG_GATE; }
  string getTypeStr() const;
  unsigned getLineNo() const { return _line_no; }
  ID getIndex() const { return _ind; }

  // Printing functions
  void reportGate() const;
  void reportFanin (int level) const;
  void reportFanout(int level) const;

  // fanin fanout
  virtual unsigned fanInSize() const { return 0; }
  virtual const ID* getFanin() const { return NULL; }
  virtual const IdList& getFanout() const { return null_idset; }
  virtual unsigned fanOutSize() const { return 0; }

  // visit
  bool isVisit() const;
  static void setVisitFlag() { ++_visited_flag; }
  static void resetVis() { _max_level = _visited_flag = 0; }

  // name
  virtual const string& getName() const { return null_str; }
  virtual void setName(string &s) {}

  // sim
  Value getSim() { return _sim; }
  virtual void simulate() {}
  void setFec(unsigned g) { _fecID = g; }
  unsigned getFec() { return _fecID; }

  // sat
  Var getSatVar() const { return _satVar; }
  void setSatVar(const Var& v) { _satVar = v; }

  // virtual void printGate() const {} // no used
private:
  int              _type;
  unsigned         _line_no;
  ID               _ind;
  unsigned         _fecID;
  Var              _satVar;

  // visit
  mutable unsigned _visited;
  static unsigned  _max_level;
  static unsigned  _visited_flag;

  // report gate by dfs
  void goFanin (ID, bool) const;
  void goFanout(ID, bool) const;

protected:
  Value            _sim;
};

class CirGateOut: public CirGate
{
public:
  CirGateOut(int type=0, ID ind=0, unsigned lineNo=0)
    :CirGate(type, ind, lineNo) {};
  ~CirGateOut() {};
  void setFanout(ID num) { _fanout.push_back(num); }
  const IdList& getFanout() const { return _fanout; }
  void removeFanout(ID gid) {
    auto it = std::find(_fanout.begin(), _fanout.end(), gid);
    while (it != _fanout.end()) {
        it = _fanout.erase(it);
        it = find(it, _fanout.end(), gid);
    }
  }
  unsigned fanOutSize() const { return _fanout.size(); }

private:
  IdList _fanout;
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
  void setFanin (ID* num);
  void updateFanin(ID from, ID to);
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
  // sim
  void setSim(Value a) { _sim = a; }
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

  // sim
  void simulate();
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

  // hash
  size_t operator () () const;
  bool operator == (const GateAnd& b) const;

  // num of aig
  static void resetNum() { _num = 0; }
  static unsigned getNum() { return _num; }

  // sim
  void simulate();
private:
  static unsigned _num;
  ID _fanin[2];
};

#endif // CIR_GATE_H
