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
  CirGate(int type=0, unsigned lineNo=0):
    _type(type), _line_no(lineNo) {};
  virtual ~CirGate() {}

  // Basic access methods
  int getType() const { return _type; }
  string getTypeStr() const {
    switch (_type) {
      case PI_GATE:
        return "PI";
      case PO_GATE:
        return "PO";
      case AIG_GATE:
        return "AIG";
      default:
        return "";
    }
  }
  unsigned getLineNo() const { return _line_no; }

  // Printing functions
  virtual void printGate() const = 0;
  void reportGate() const;
  void reportFanin (int level) const;
  void reportFanout(int level) const;
  void setFanin (unsigned& num) { _fanin .push_back(num); }
  void setFanout(unsigned& num) { _fanout.push_back(num); }
  vector<unsigned> getFanin () const { return _fanin ; }
  vector<unsigned> getFanout() const { return _fanout; }

private:
  int _type;
  unsigned _line_no;

protected:
  vector<unsigned> _fanin;
  vector<unsigned> _fanout;

};

class GateConst: public CirGate
{
public:
  GateConst(unsigned lineNo=0):
    CirGate(CONST_GATE, lineNo) {};
  ~GateConst() {};
 void printGate() const {};
};

class GateIn: public CirGate
{
public:
  GateIn(unsigned lineNo=0):
    CirGate(PI_GATE, lineNo) {};
  ~GateIn() {};
 void printGate() const {};
};

class GateOut: public CirGate
{
public:
  GateOut(unsigned lineNo=0):
    CirGate(PO_GATE, lineNo) {};
  ~GateOut() {};
 void printGate() const {};
};

class GateAnd: public CirGate
{
public:
  GateAnd(unsigned lineNo=0):
    CirGate(AIG_GATE, lineNo) {};
  ~GateAnd() {};
 void printGate() const {};
};

#endif // CIR_GATE_H
