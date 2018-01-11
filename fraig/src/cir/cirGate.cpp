/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void CirGate::reportGate() const
{
  cout << "================================================================================\n"
       <<  "= " <<  getTypeStr() << "(" <<  _ind <<  ")";
  if (getName().size())
    cout << "\"" << getName() << "\"";
  cout << ", line " << _line_no << endl
       << "= FECs:";
  cirMgr->printFEC(_ind);
  cout << "\n= Value: ";
  for (int i=63; i>=0; --i) {
    cout << ((_sim >> i) & 1);
    if (i && !(i & 0x7))
      cout << '_';
  }
  cout << "\n================================================================================\n";
}

void CirGate::reportFanin(int level) const
{
  assert (level >= 0);
  setVisitFlag();
  _max_level = unsigned(level);
  goFanin(0, 0);
}

void CirGate::reportFanout(int level) const
{
  assert (level >= 0);
  setVisitFlag();
  _max_level = unsigned(level);
  goFanout(0, 0);
}

void CirGate::goFanin (unsigned level, bool no) const
{
  if (level > _max_level)
    return ;
  cout << string(level * 2, ' ')
       << (no ? "!" : "" ) << getTypeStr() << " " << _ind;
  if (level < _max_level && fanInSize() && isVisit()) {
    cout << " (*)\n";
    return ;
  }
  cout << endl;
  unsigned n = fanInSize();
  for (unsigned i=0; i<n; ++i)
    cirMgr->getGate((getFanin()[i]) >> 1)->goFanin(level + 1, getFanin()[i] & 1);
}

void CirGate::goFanout (unsigned level, bool no) const
{
  if (level > _max_level)
    return ;
  cout << string(level * 2, ' ')
       << (no ? "!" : "" ) << getTypeStr() << " " << _ind;
  if (level < _max_level && fanOutSize() && isVisit()) {
    cout << " (*)\n";
    return ;
  }
  cout << endl;
  for (const unsigned& i: getFanout())
    cirMgr->getGate(i >> 1)->goFanout(level + 1, i & 1);
}

/**************************************/
/*   useful                 functions */
/**************************************/
string CirGate::getTypeStr() const
{
  switch (_type) {
    case PI_GATE:
      return "PI";
    case PO_GATE:
      return "PO";
    case AIG_GATE:
      return "AIG";
    case CONST_GATE:
      return "CONST";
    case UNDEF_GATE:
      return "UNDEF";
    default:
      return "";
  }
}

bool CirGate::isVisit() const
{
  if (_visited_flag == _visited)
    return true;
  _visited = _visited_flag;
  return false;
}

/**************************************/
/*   class CirGateIn member functions */
/**************************************/
void CirGateIn::setFanin (ID* num) {
  for (unsigned i=0; i<fanInSize(); ++i)
    const_cast<ID*>(getFanin())[i] = num[i];
}

void CirGateIn::updateFanin(ID from, ID to) {
  for (unsigned i=0; i<fanInSize(); ++i)
    if ((getFanin()[i] ^ from) <= 1)
      const_cast<ID*>(getFanin())[i] = to ^ (getFanin()[i] & 1);
}

/**************************************/
/*   class GateAnd member functions   */
/**************************************/
size_t GateAnd::operator () () const {
  bool c = _fanin[0] > _fanin[1];
  return 888777 * _fanin[c] + _fanin[!c];
}

bool GateAnd::operator == (const GateAnd& b) const {
  bool ia =   _fanin[0] >   _fanin[1],
       ib = b._fanin[0] > b._fanin[1];
  return (_fanin[ia] == b._fanin[ib]) && (_fanin[!ia] == b._fanin[!ib]);
}

/**************************************/
/*   Simulation           functions   */
/**************************************/
void GateAnd::simulate() {
  _sim = cirMgr->getVal(_fanin[0]) &
         cirMgr->getVal(_fanin[1]);
}

void GateOut::simulate() {
  _sim = cirMgr->getVal(_fanin);
}
