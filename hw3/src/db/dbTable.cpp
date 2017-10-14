/****************************************************************************
  FileName     [ dbTable.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Table member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <string>
#include <cctype>
#include <cassert>
#include <set>
#include <algorithm>
#include <sstream>
#include "dbTable.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream& operator << (ostream& os, const DBRow& r)
{
  // to print out a row.
  // - Data are seperated by a space. No trailing space at the end.
  // - Null cells are printed as '.'
  for (size_t i=0; i<r.size(); ++i) {
    if (i)
      os << ' ';
    if (r[i] != INT_MAX)
      os << r[i];
    else
      os << ".";
  }
  return os;
}

ostream& operator << (ostream& os, const DBTable& t)
{
  // to print out a table
  // - Data are seperated by setw(6) and aligned right.
  // - Null cells are printed as '.'
  // ending newline
  for (size_t i=0; i<t.nRows(); ++i) {
    for (size_t j=0; j<t.nCols(); ++j)
      if (t[i][j] != INT_MAX)
        os << right << setw(6) << t[i][j];
      else
        os << right << setw(6) << '.';
    os << endl;
  }
  return os;
}

ifstream& operator >> (ifstream& ifs, DBTable& t)
{
  // to read in data from csv file and store them in a table
  // - You can assume the input file is with correct csv file format
  // - NO NEED to handle error file format
  // copy from hw1/p2Table.cpp
  // read csv
  // read each line
  string s, brs;
  while (ifs >> s) {
    unsigned int cols = count(s.begin(), s.end(), ',') + 1;
    // error handle should write here
    // ...

    // to row
    stringstream ss(s);
    std::vector<int> v;
    while (getline(ss, brs, ',')) {
      if (brs.length() == 0)
        v.push_back(INT_MAX);
      else
        v.push_back(stoi(brs));
      --cols;
    }
      // "1,3," this condition
    if (cols == 1)
      v.push_back(INT_MAX);

    // add row to table
    t.addRow(DBRow(v));
  }
  return ifs;
}

/*****************************************/
/*   Member Functions for class DBRow    */
/*****************************************/
void DBRow::removeCell(size_t c)
{
  _data.erase(_data.begin() + c);
}

/*****************************************/
/*   Member Functions for struct DBSort  */
/*****************************************/
bool DBSort::operator() (const DBRow& r1, const DBRow& r2) const
{
  // called as a functional object that compares the data in r1 and r2
  // based on the order defined in _sortOrder
  for (const size_t &i: _sortOrder)
    if (r1[i] != r2[i])
      return r1[i] < r2[i];
  return 0; // equal
}

/*****************************************/
/*   Member Functions for class DBTable  */
/*****************************************/
void DBTable::reset()
{
  _table = vector<DBRow>();
}

void DBTable::addCol(const vector<int>& d)
{
  // add a column to the right of the table. Data are in 'd'.
  for (size_t i=0; i<d.size(); ++i)
    _table[i].addData(d[i]);
}

void DBTable::delRow(int c)
{
  // delete row #c. Note #0 is the first row.
  _table.erase(_table.begin() + c);
}

void DBTable::delCol(int c)
{
  // delete col #c. Note #0 is the first row.
  for (size_t i = 0, n = _table.size(); i < n; ++i)
    _table[i].removeCell(c);
  if (_table[0].empty()) // numCols becomes 0
    reset();
}

// For the following getXXX() functions...  (except for getCount())
// - Ignore null cells
// - If all the cells in column #c are null, return NAN
// - Return "float" because NAN is a float.
float DBTable::getMax(size_t c) const
{
  // get the max data in column #c
  vector<int> v = getColStrip(c);
  if (v.size())
    return *max_element(v.begin(), v.end());
  else
    return NAN;
}

float DBTable::getMin(size_t c) const
{
  // get the min data in column #c
  vector<int> v = getColStrip(c);
  if (v.size())
    return *min_element(v.begin(), v.end());
  else
    return NAN;
}

float DBTable::getSum(size_t c) const
{
  // compute the sum of data in column #c
  vector<int> v = getColStrip(c);
  float sum = 0;
  for (int &i: v)
    sum += i;
  if (v.size())
    return sum;
  else
    return NAN;
}

int DBTable::getCount(size_t c) const
{
  // compute the number of distinct data in column #c
  // - Ignore null cells
  vector<int> v = getColStrip(c);
  set<int> set_v(v.begin(), v.end());
  return set_v.size();
}

float DBTable::getAve(size_t c) const
{
  // compute the average of data in column #c
  vector<int> v = getColStrip(c);
  if (v.size())
    return getSum(c) / v.size();
  else
    return NAN;
}

void DBTable::sort(const struct DBSort& s)
{
  // sort the data according to the order of columns in 's'
  stable_sort(_table.begin(), _table.end(), s);
}

void DBTable::printCol(size_t c) const
{
  // to print out a column.
  // - Data are seperated by a space. No trailing space at the end.
  // - Null cells are printed as '.'

  cout << getCol(c) << endl;
}

void DBTable::printSummary() const
{
  size_t nr = nRows(), nc = nCols(), nv = 0;
  for (size_t i = 0; i < nr; ++i)
    for (size_t j = 0; j < nc; ++j)
      if (_table[i][j] != INT_MAX) ++nv;
        cout << "(#rows, #cols, #data) = (" << nr << ", " << nc << ", "
             << nv << ")" << endl;
}

vector<int> DBTable::getCol(size_t c) const
{
  // get #c col
  vector<int> v;
  for (size_t i=0; i<nRows(); ++i)
    v.push_back(_table[i][c]);
  return v;
}
vector<int> DBTable::getColStrip(size_t c) const
{
  // get #c col without null
  vector<int> v;
  for (size_t i=0; i<nRows(); ++i)
    if (_table[i][c] != INT_MAX)
      v.push_back(_table[i][c]);
  return v;
}
