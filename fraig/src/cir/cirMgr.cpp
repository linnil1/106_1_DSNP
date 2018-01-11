/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include <sstream>
#include "cirMgr.h"
#include "util.h"

using namespace std;

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;
unsigned CirGate::_max_level = 0;
unsigned CirGate::_visited_flag= 0;
unsigned GateAnd::_num = 0;

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
  CANNOT_OPEN,

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
           << ": Cannot redefine const (" << errInt << ")!!" << endl;
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
void CirMgr::hasSpace(fstream &fs, string& s, string err)
{
  ++lineNo;
  if (!getline(fs, s)) {
    errMsg = err;
    throw MISSING_DEF;
  }

  bool extra_space = true;
  for (colNo=0; colNo<s.size(); ++colNo)
    if (s[colNo] == ' ') {
      if (extra_space)
        throw EXTRA_SPACE;
      extra_space = true;
    }
    else if(!isprint(s[colNo])) {
      errInt = int(s[colNo]);
      throw ILLEGAL_WSPACE;
    }
    else
      extra_space = false;
  if (extra_space) {
    if (colNo == 0) {
      errMsg = err;
      colNo = 0;
      throw MISSING_DEF;
    }
    else {
      --colNo;
      throw EXTRA_SPACE;
    }
  }
  colNo = 1;
}

void CirMgr::isnotUnsigned(stringstream &ss, unsigned& tar)
{
  string s;
  unsigned len = ss.str().size();
  ss >> s;
  if (ss.fail()) {
    ss.clear();
    errMsg = "number of variables";
    throw MISSING_NUM;
  }
  int k;
  if (!myStr2Int(s, k) || k < 0) {
    errMsg = s;
    throw ILLEGAL_NUM;
  }
  colNo += len - ss.str().size();
  tar = unsigned(k);
}

void CirMgr::isnotGate(stringstream &ss, unsigned& tar, bool checkExist=true)
{
  isnotUnsigned(ss, tar);
  errInt = tar;
  if ((tar >> 1) > MILOA[0])
    throw MAX_LIT_ID;
  if (checkExist && cirMgr->getGate(tar >> 1)) {
    errGate = cirMgr->getGate(tar >> 1);
    throw REDEF_GATE;
  }
}

void CirMgr::hasMore(stringstream &ss)
{
  ss.peek();
  if (!ss.eof())
    throw MISSING_NEWLINE;
}

bool CirMgr::readCircuit(const string& fileName)
{
  try {
    // read file
    std::fstream fs(fileName);
    if (!fs.is_open()) {
      errMsg = fileName;
      throw CANNOT_OPEN;
    }
    lineNo = -1;
    string s;

    // MILOA
    // head -- check space
    hasSpace(fs, s, "aag");
    stringstream ss(s);
    // head -- check aag
    string aag; ss >> aag;
    if (aag != "aag") {
      errMsg = aag;
      throw ILLEGAL_IDENTIFIER;
    }
    // head -- check M I L O A
    colNo = 4;
    for (int i=0; i<5; ++i)
      isnotUnsigned(ss, MILOA[i]);
    hasMore(ss);

    // M
    // M -- latches
    if (MILOA[2] > 0) {
      errMsg = "latches";
      throw ILLEGAL_NUM;
    }
    // M -- too small
    if (MILOA[0] < MILOA[1] + MILOA[4]) {
      errInt = MILOA[0];
      errMsg = "Number of variables";
      throw NUM_TOO_SMALL;
    }
    // M resize
    _gates.resize(MILOA[0] + MILOA[3] + 1);
    // const
    _gates[0] = new GateConst();

    // I
    for (unsigned i=0; i<MILOA[1]; ++i) {
      // I -- read
      hasSpace(fs, s, "PI");
      stringstream ss(s);
      unsigned ind;
      // I -- checkGate
      isnotGate(ss, ind);
      if (ind & 1) {
        errInt = ind;
        errMsg = "PI";
        throw CANNOT_INVERTED;
      }
      hasMore(ss);
      // I add PI
      _gates[ind >> 1] =  new GateIn(ind >> 1, lineNo);
      _ins.push_back(ind >> 1);
    }
    // O
    for (unsigned i=0; i<MILOA[3]; ++i) {
      // O -- read
      hasSpace(fs, s, "PO");
      stringstream ss(s);
      // O -- check fanin
      unsigned ind;
      isnotGate(ss, ind, false);
      hasMore(ss);
      // O add PO
      _gates[MILOA[0] + i + 1] = new GateOut(MILOA[0] + i + 1, lineNo);
      static_cast<GateOut*>(_gates[MILOA[0] + i + 1])->setFanin(&ind);
      _outs.push_back(MILOA[0] + i + 1);
    }
    // A
    for (unsigned i=0; i<MILOA[4]; ++i) {
      // A -- read
      hasSpace(fs, s, "AIG");
      stringstream ss(s);
      // A -- cehck fanin
      unsigned ind, in[2];
      isnotGate(ss, ind);
      isnotGate(ss, in[0], false);
      isnotGate(ss, in[1], false);
      hasMore(ss);
      // A add AIG
      _gates[ind >> 1] = new GateAnd(ind >> 1, lineNo);
      static_cast<GateAnd*>(_gates[ind >> 1])->setFanin(in);
    }

    // comments and symbols
    // Note symbols name can have spaces in it
    IdList* sym;
    while (getline(fs, s)) {
      ++lineNo;
      if (!s.size()) {
        errMsg = "symbols";
        throw MISSING_DEF;
      }
      // first word
      colNo = 0;
      if (s[0] == 'c') {
        // comments
        if (s.size() > 1) {
          colNo = 1;
          throw MISSING_NEWLINE;
        }
        _comments << fs.rdbuf();
        break;
      }
      // in or out or error
      else if (s[0] == 'i')
        sym = &_ins;
      else if (s[0] == 'o')
        sym = &_outs;
      else if (s[0] == ' ')
        throw EXTRA_SPACE;
      else if (!isprint(s[0])) {
        errInt = unsigned(s[0]);
        throw ILLEGAL_WSPACE;
      }
      else {
        errMsg = s[0];
        throw ILLEGAL_SYMBOL_TYPE;
      }

      // symbols -- index read
      ++colNo;
      size_t found = s.find(' ', 1);
      if (found == string::npos)
        found = s.end() - s.begin();
      string sind = string(s.begin() + 1, s.begin() + found);
      int ind;
      // symbols -- index check
      if (!myStr2Int(sind, ind) || ind < 0) {
        errMsg = "symbol index(" + sind + ")";
        throw ILLEGAL_NUM;
      }
      // symbols -- index size
      if (unsigned(ind) >= sym->size()) {
        errInt = ind;
        errMsg = "PI index";
        throw NUM_TOO_BIG;
      }

      // symbols -- name
      colNo = found + 1;
      if (colNo >= s.size()) {
        errMsg = "symbolic name";
        throw MISSING_IDENTIFIER;
      }
      string name = string(s.begin() + colNo, s.end());
      // symbols -- name check
      for (char &i: name)
        if (!isprint(i)) {
          errInt = unsigned(i);
          throw ILLEGAL_SYMBOL_NAME;
        }
      // symbols -- rename
      CirGate *gate = _gates[(*sym)[ind]];
      if (gate->getName().size()) {
        errMsg = s[0];
        errInt = ind;
        throw REDEF_SYMBOLIC_NAME;
      }
      // symbols add name
      gate->setName(name);
      fs.peek();
    }
    fs.close();
  }
  catch(CirParseError err) {
    parseError(err);
    return false;
  };

  // backward
  for (unsigned i=0; i<_gates.size(); ++i)
    if (_gates[i] && _gates[i]->fanInSize())
      for (unsigned j=0; j<_gates[i]->fanInSize(); ++j) {
        ID k = _gates[i]->getFanin()[j];
        if (!getGate(k >> 1)) // add undef
          _gates[k >> 1] = new GateUndef(k >> 1);
        static_cast<CirGateOut*>(getGate(k >> 1))->setFanout((i << 1) + (k & 1));
      }

  findFloat();
  return true;
}

void CirMgr::findFloat()
{
  _floats[0].clear();
  _floats[1].clear();
  for (unsigned i=1; i<_gates.size(); ++i)
    if (_gates[i] && _gates[i]->getType() != UNDEF_GATE) {
      // float fanin
      bool ok = true;
      for (unsigned j=0; j<_gates[i]->fanInSize(); ++j) {
        CirGate *now = getGate(_gates[i]->getFanin()[j] >> 1);
        assert(now);
        if (now->getType() == UNDEF_GATE)
          ok = false;
      }
      if ((!ok || !_gates[i]->fanInSize()) &&
          _gates[i]->getType() != PI_GATE)
        _floats[0].push_back(i);

      // float fanout
      if (!_gates[i]->fanOutSize() &&
          _gates[i]->getType() != PO_GATE)
        _floats[1].push_back(i);
    }
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
       << "  AIG"   << setw(11) << right << GateAnd::getNum() << endl
       << "------------------" << endl
       << "  Total" << setw( 9) << right << MILOA[1] + MILOA[3] + GateAnd::getNum() << endl;
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
  for (unsigned i=0; i<gate->fanInSize(); ++i)
    goNetlist(gate->getFanin()[i] >> 1, num);
  cout << "[" << num++ << "] "
       << setw(4) << left << gate->getTypeStr() << id;
  for (unsigned i=0; i<gate->fanInSize(); ++i) {
    cout << ' ';
    ID gid = gate->getFanin()[i];
    CirGate *gchild = cirMgr->getGate(gid >> 1);
    if (!gchild || gchild->getType() == UNDEF_GATE)
      cout << '*';
    if (gid & 1)
      cout << '!';
     cout << (gid >> 1);
  }
  if (gate->getName().size())
    cout << " (" << gate->getName() << ")";
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
  // search A
  CirGate::setVisitFlag();
  IdList v;
  for (unsigned i=0; i<MILOA[3]; ++i)
      goFindAnd(MILOA[0] + i + 1, v);
  // M
  outfile << "aag";
  IdList miloa(MILOA, MILOA+4);
  miloa.push_back(v.size());
  printVector(miloa);
  // I
  for (const unsigned &i: _ins)
    outfile << i * 2 << endl;
  // O
  for (const unsigned &i: _outs)
    cout << *(getGate(i)->getFanin()) << endl;
  // A
  for (const unsigned &i: v) {
    outfile << i * 2;
    printVector(IdList(getGate(i)->getFanin(),
                       getGate(i)->getFanin() + getGate(i)->fanInSize()));
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

void CirMgr::goFindAnd(unsigned id, IdList& v) const
{
  CirGate *gate = getGate(id);
  assert(gate);
  if (gate->getType() == UNDEF_GATE || gate->isVisit())
    return ;
  for (unsigned i=0; i<gate->fanInSize(); ++i)
    goFindAnd(gate->getFanin()[i] >> 1, v);
  if (gate->getType() == AIG_GATE)
    v.push_back(id);
}

void CirMgr::delGate(ID& gid) {
  takeOutChild(getGate(gid)); // Need make sure
  delete _gates[gid];
  _gates[gid] = NULL;
}

void CirMgr::printFECPairs() const
{
  for (unsigned i=1; i<=_groupMax; ++i) {
    cout << '[' << i - 1 << ']';
    printVector2(_fecCollect[i], (getVal(_fecCollect[i][0]) & 1) ^ (_fecCollect[i][0]  & 1));
    cout << endl;
  }
}

// TODO
void CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
}
