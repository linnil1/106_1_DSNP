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
// TODO: Define your own data members and member functions, or classes
class CirGate
{
public:
  CirGate(int type=0, unsigned lineNo=0, unsigned ind=0):
    _type(type), _line_no(lineNo), _ind(ind) {};
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

  virtual string getName() const { return ""; }
  virtual void setName(string &s) {}

  // fanin and out
  void setFanin (unsigned num) { _fanin .push_back(num); }
  void setFanout(unsigned num) { _fanout.push_back(num); }
  vector<unsigned> getFanin () const { return _fanin ; }
  vector<unsigned> getFanout() const { return _fanout; }

private:
  int _type;
  unsigned _line_no;
  int _ind;

protected:
  vector<unsigned> _fanin;
  vector<unsigned> _fanout;

};

class GateUndef: public CirGate
{
public:
  GateUndef(unsigned ind):
    CirGate(UNDEF_GATE, 0, ind) {};
  ~GateUndef() {};
  void printGate() const {};
};

class GateConst: public CirGate
{
public:
  GateConst():
    CirGate(CONST_GATE, 0, 0) {};
  ~GateConst() {};
  void printGate() const {};
};

class GateIn: public CirGate
{
public:
  GateIn(unsigned ind, unsigned lineNo=0):
    CirGate(PI_GATE, lineNo, ind) {};
  ~GateIn() {};
  void printGate() const {};
  string getName() const { return _name; }
  void setName(string &s) { _name = s; }

private:
  string _name;
};

class GateOut: public CirGate
{
public:
  GateOut(unsigned ind, unsigned lineNo=0):
    CirGate(PO_GATE, lineNo, ind) {};
  ~GateOut() {};
  void printGate() const {};
  string getName() const { return _name; }
  void setName(string &s) { _name = s; }

private:
  string _name;
};

class GateAnd: public CirGate
{
public:
  GateAnd(unsigned ind, unsigned lineNo=0):
    CirGate(AIG_GATE, lineNo, ind) {};
  ~GateAnd() {};
  void printGate() const {};
};

#endif // CIR_GATE_H
