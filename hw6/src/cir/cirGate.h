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
#include <vector>
#include <iostream>
#include "cirDef.h"

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGate
{
public:
  CirGate(int type=0, unsigned ind=0, unsigned lineNo=0):
    _type(type), _line_no(lineNo), _visited(0), _ind(ind) {};
  virtual ~CirGate() {}

  // Basic access methods
  int getType() const { return _type; }
  string getTypeStr() const;
  unsigned getLineNo() const { return _line_no; }

  // Printing functions
  virtual void printGate() const = 0;
  void reportGate() const;
  void reportFanin (int level) const;
  void reportFanout(int level) const;

  // names for pi po
  virtual string getName() const { return ""; }
  virtual void setName(string &s) {}

  // fanin and out
  void setFanin (unsigned num) { _fanin .push_back(num); }
  void setFanout(unsigned num) { _fanout.push_back(num); }
  vector<unsigned> getFanin () const { return _fanin ; }
  vector<unsigned> getFanout() const { return _fanout; }

  // visit
  bool isVisit() const;
  static void setVisitFlag() { ++_visited_flag; }

private:
  int _type;
  unsigned _line_no;

  // visit
  mutable unsigned _visited;
  static unsigned _max_level;
  static unsigned _visited_flag;

protected:
  int _ind;
  string netPrint(const unsigned&) const;
  vector<unsigned> _fanin;
  vector<unsigned> _fanout;

  void goFanin (unsigned ,bool) const;
  void goFanout(unsigned ,bool) const;
};

class GateUndef: public CirGate
{
public:
  GateUndef(unsigned ind):
    CirGate(UNDEF_GATE, ind, 0) {};
  ~GateUndef() {};
  void printGate() const {};
};

class GateConst: public CirGate
{
public:
  GateConst():
    CirGate(CONST_GATE, 0, 0) {};
  ~GateConst() {};
  void printGate() const {
    cout << "CONST0";
  };
};

class GateIn: public CirGate
{
public:
  GateIn(unsigned ind, unsigned lineNo=0):
    CirGate(PI_GATE, ind, lineNo) {};
  ~GateIn() {};
  void printGate() const {
    cout << setw(4) << left << getTypeStr() << _ind;
    if (_name.size())
      cout << " (" << _name << ")";
  };
  string getName() const { return _name; }
  void setName(string &s) { _name = s; }

private:
  string _name;
};

class GateOut: public CirGate
{
public:
  GateOut(unsigned ind, unsigned lineNo=0):
    CirGate(PO_GATE, ind, lineNo) {};
  ~GateOut() {};
  void printGate() const {
    cout << setw(4) << left << getTypeStr() << _ind;
    for (const unsigned &i: _fanin)
      cout << " " << netPrint(i);
    if (_name.size())
      cout << " (" << _name << ")";
  };
  string getName() const { return _name; }
  void setName(string &s) { _name = s; }

private:
  string _name;
};

class GateAnd: public CirGate
{
public:
  GateAnd(unsigned ind, unsigned lineNo=0):
    CirGate(AIG_GATE, ind, lineNo) {};
  ~GateAnd() {};
  void printGate() const {
    cout << setw(4) << left << getTypeStr() << _ind;
    for (const unsigned &i: _fanin)
      cout << " " << netPrint(i);
  };
};

#endif // CIR_GATE_H
