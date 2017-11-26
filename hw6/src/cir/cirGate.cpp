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

// TODO: Implement memeber functions for class(es) in cirGate.h

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
}

void CirGate::reportFanout(int level) const
{
  assert (level >= 0);
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
