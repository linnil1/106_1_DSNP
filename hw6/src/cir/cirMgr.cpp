/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include <sstream>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;
unsigned CirGate::_max_level = 0;
unsigned CirGate::_visited_flag= 0;

enum CirParseError {
  EXTRA_SPACE,
  MISSING_SPACE,
  ILLEGAL_WSPACE,
  ILLEGAL_NUM,
  ILLEGAL_IDENTIFIER,
  ILLEGAL_SYMBOL_TYPE,
  ILLEGAL_SYMBOL_NAME,
  MISSING_NUM,
  MISSING_IDENTIFIER,
  MISSING_NEWLINE,
  MISSING_DEF,
  CANNOT_INVERTED,
  MAX_LIT_ID,
  REDEF_GATE,
  REDEF_SYMBOLIC_NAME,
  REDEF_CONST,
  NUM_TOO_SMALL,
  NUM_TOO_BIG,

  DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool parseError(CirParseError err)
{
  switch (err) {
    case EXTRA_SPACE:
      cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
           << ": Extra space character is detected!!" << endl;
      break;
    case MISSING_SPACE:
      cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
           << ": Missing space character!!" << endl;
      break;
    case ILLEGAL_WSPACE: // for non-space white space character
      cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
           << ": Illegal white space char(" << errInt
           << ") is detected!!" << endl;
      break;
    case ILLEGAL_NUM:
      cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
           << errMsg << "!!" << endl;
      break;
    case ILLEGAL_IDENTIFIER:
      cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
           << errMsg << "\"!!" << endl;
      break;
    case ILLEGAL_SYMBOL_TYPE:
      cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
           << ": Illegal symbol type (" << errMsg << ")!!" << endl;
      break;
    case ILLEGAL_SYMBOL_NAME:
      cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
           << ": Symbolic name contains un-printable char(" << errInt
           << ")!!" << endl;
      break;
    case MISSING_NUM:
      cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
           << ": Missing " << errMsg << "!!" << endl;
      break;
    case MISSING_IDENTIFIER:
      cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
           << errMsg << "\"!!" << endl;
      break;
    case MISSING_NEWLINE:
      cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
           << ": A new line is expected here!!" << endl;
      break;
    case MISSING_DEF:
      cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
           << " definition!!" << endl;
      break;
    case CANNOT_INVERTED:
      cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
           << ": " << errMsg << " " << errInt << "(" << errInt/2
           << ") cannot be inverted!!" << endl;
      break;
    case MAX_LIT_ID:
      cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
           << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
           << endl;
      break;
    case REDEF_GATE:
      cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
           << "\" is redefined, previously defined as "
           << errGate->getTypeStr() << " in line " << errGate->getLineNo()
           << "!!" << endl;
      break;
    case REDEF_SYMBOLIC_NAME:
      cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
           << errMsg << errInt << "\" is redefined!!" << endl;
      break;
    case REDEF_CONST:
      cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
           << ": Cannot redefine constant (" << errInt << ")!!" << endl;
      break;
    case NUM_TOO_SMALL:
      cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
           << " is too small (" << errInt << ")!!" << endl;
      break;
    case NUM_TOO_BIG:
      cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
           << " is too big (" << errInt << ")!!" << endl;
      break;
    default: break;
  }
  return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool CirMgr::readCircuit(const string& fileName)
{
  // TODO without handling Errors
  std::fstream fs(fileName);
  if (!fs.is_open())
    return false;
  string s;

  // MILOA
  getline(fs, s);
  stringstream ss(s);
  string aag; ss >> aag;
  for (int i=0; i<5; ++i)
    ss >> MILOA[i];
  // M
  _gates.resize(MILOA[0] + MILOA[3] + 1);
  // I
  int nos = 2;
  for (unsigned i=0; i<MILOA[1]; ++i) {
    getline(fs, s);
    stringstream ss(s);
    int ind;
    ss >> ind;
    _gates[ind >> 1] =  new GateIn(ind >> 1, nos++);
    _ins.push_back(ind >> 1);
  }
  // O
  for (unsigned i=0; i<MILOA[3]; ++i) {
    getline(fs, s);
    stringstream ss(s);
    unsigned ind;
    ss >> ind;
    _gates[MILOA[0] + i + 1] = new GateOut(MILOA[0] + i + 1, nos++);
    _gates[MILOA[0] + i + 1]->setFanin(ind);
    _outs.push_back(MILOA[0] + i + 1);
  }
  // A
  for (unsigned i=0; i<MILOA[4]; ++i) {
    getline(fs, s);
    stringstream ss(s);
    unsigned ind, in0, in1;
    ss >> ind >> in0 >> in1;
    _gates[ind >> 1] = new GateAnd(ind >> 1, nos++);
    _gates[ind >> 1]->setFanin(in0);
    _gates[ind >> 1]->setFanin(in1);
  }

  // comments and symbols
  while (getline(fs, s)) {
    // comments
    if (s == "c") {
      _comments << fs.rdbuf();
      break;
    }
    stringstream ss(s);
    string name;
    ss >> s >> name;
    int ind;
    myStr2Int(string(s.begin()+1, s.end()), ind);
    if (s[0] == 'i')
      _gates[_ins[ind]]->setName(name);
    else if (s[0] == 'o')
      _gates[_outs[ind]]->setName(name);
    else
      cerr << "ERROR\n";
  }
  fs.close();

  // const
  _gates[0] = new GateConst();

  // backward
  for (unsigned i=0; i<_gates.size(); ++i)
    if (_gates[i])
      for (unsigned &j:_gates[i]->getFanin())
        if (getGate(j >> 1))
          getGate(j >> 1)->setFanout((i << 1) + (j & 1));

  // find floating
  for (unsigned i=1; i<_gates.size(); ++i)
    if (_gates[i]) {
      // float fanin
      bool ok = true;
      for (const unsigned &j:_gates[i]->getFanin()) {
        CirGate *now = getGate(j >> 1);
        if (!now || now->getType() == UNDEF_GATE) {
          // Undef
          ok = false;
          if (!now)
            _gates[j >> 1] = new GateUndef(j >> 1);
        }
      }
      if ((!ok || !_gates[i]->getFanin().size()) && _gates[i]->getType() != PI_GATE)
        _floats[0].push_back(i);

      // float fanout
      if (!_gates[i]->getFanout().size() && _gates[i]->getType() != PO_GATE)
        _floats[1].push_back(i);
    }

  return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
PI          20
PO          12
AIG        130
------------------
Total      162
*********************/
void CirMgr::printSummary() const
{
  cout << endl
       << "Circuit Statistics" << endl
       << "==================" << endl
       << "  PI"    << setw(12) << right << MILOA[1] << endl
       << "  PO"    << setw(12) << right << MILOA[3] << endl
       << "  AIG"   << setw(11) << right << MILOA[4] << endl
       << "------------------" << endl
       << "  Total" << setw( 9) << right << MILOA[1] + MILOA[3] + MILOA[4] << endl;
}

void CirMgr::printNetlist() const
{
  CirGate::setVisitFlag();
  cout << endl;
  unsigned num = 0;
  for (unsigned i=0; i<MILOA[3]; ++i)
      goNetlist(MILOA[0] + i + 1, num);
}

void CirMgr::goNetlist(unsigned id, unsigned& num) const
{
  CirGate *gate = getGate(id);
  assert(gate);
  if (gate->getType() == UNDEF_GATE || gate->isVisit())
    return ;
  for (const unsigned &i: gate->getFanin())
    goNetlist(i >> 1, num);
  cout << "[" << num++ << "] ";
  gate->printGate();
  cout << endl;
}

void CirMgr::printPIs() const
{
  cout << "PIs of the circuit:";
  printVector(_ins);
}

void CirMgr::printPOs() const
{
  cout << "POs of the circuit:";
  printVector(_outs);
}

void CirMgr::printFloatGates() const
{
  // output
  if (_floats[0].size()) {
    cout << "Gates with floating fanin(s):";
    printVector(_floats[0]);
  }
  if (_floats[1].size()) {
    cout << "Gates defined but not used  :";
    printVector(_floats[1]);
  }
}

void CirMgr::printVector(const IdList &v) const
{
  for (const unsigned &i: v)
    cout << " " << i;
  cout << endl;
}

void CirMgr::writeAag(ostream& outfile) const
{
  // M
  outfile << "aag";
  printVector(IdList(MILOA, MILOA+5));
  // I
  for (const unsigned &i: _ins)
    outfile << i * 2 << endl;
  // O
  for (const unsigned &i: _outs)
    cout << getGate(i)->getFanin()[0] << endl;
  // A
  CirGate::setVisitFlag();
  IdList v;
  for (unsigned i=0; i<MILOA[3]; ++i)
      findAnd(MILOA[0] + i + 1, v);
  for (const unsigned &i: v) {
    outfile << i * 2;
    printVector(getGate(i)->getFanin());
  }
  // symbols
  for (unsigned i=0; i<_ins.size(); ++i) {
    CirGate *gate = getGate(_ins[i]);
    if (gate->getName().size())
      outfile << 'i' << i << " " << gate->getName() << endl;
  }
  for (unsigned i=0; i<_outs.size(); ++i) {
    CirGate *gate = getGate(_outs[i]);
    if (gate->getName().size())
      outfile << 'o' << i << " " << gate->getName() << endl;
  }
  // comments. Use default for easier debugging
  outfile << "c\nAAG output by Chung-Yang (Ric) Huang\n";
}

void CirMgr::findAnd(unsigned id, IdList& v) const
{
  CirGate *gate = getGate(id);
  assert(gate);
  if (gate->getType() == UNDEF_GATE || gate->isVisit())
    return ;
  for (const unsigned &i: gate->getFanin())
    findAnd(i >> 1, v);
  if (gate->getType() == AIG_GATE)
    v.push_back(id);
}
