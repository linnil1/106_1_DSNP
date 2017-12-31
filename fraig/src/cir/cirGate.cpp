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
  stringstream ss;
  ss <<  "= " <<  getTypeStr() << "(" <<  _ind <<  ")";
  if (getName().size())
    ss << "\"" << getName() << "\"";
  ss <<  ", line " <<  _line_no;
  cout << "==================================================" << endl
       << setw(49) << left << ss.str() << "=" << endl
       << "==================================================" << endl;
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
  if (level < _max_level && getFanout().size() && isVisit()) {
    cout << " (*)\n";
    return ;
  }
  cout << endl;
  for (const unsigned& i: getFanout())
    cirMgr->getGate(i >> 1)->goFanout(level + 1, i & 1);
}

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

void CirGate::netPrint() const
{
  cout << setw(4) << left << getTypeStr() << _ind;
  for (unsigned i=0; i<fanInSize(); ++i) {
    cout << ' ';
    ID gid = getFanin()[i];
    CirGate *gate = cirMgr->getGate(gid >> 1);
    if (!gate || gate->getType() == UNDEF_GATE)
      cout << '*';
    if (gid & 1)
      cout << '!';
     cout << (gid >> 1);
  }
  if (getName().size())
    cout << " (" << getName() << ")";
  cout << endl;
}

bool CirGate::isVisit() const
{
  if (_visited_flag == _visited)
    return true;
  _visited = _visited_flag;
  return false;
}
