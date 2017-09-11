/****************************************************************************
  FileName     [ p2Table.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define member functions of class Row and Table ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include "p2Table.h"
#include <fstream>
#include <climits>
#include <iostream>
#include <sstream>
#include <iomanip> 
#include <set>
#include <algorithm>

using namespace std;

// Implement member functions of class Row and Table here
bool Table::read(const string& csvFile)
{
    // read csv
    std::fstream fs;
    fs.open(csvFile);
    if (!fs.is_open())
        return false;

    // read each line
    string s, brs;
    while(fs >> s)
    {
        // caluate cols
        unsigned int cols = count(s.begin(), s.end(), ',') + 1;
        if( _nCols == 0)
            _nCols = cols;
        else if( _nCols != cols)
            return false;

        // to row
        stringstream ss(s);
        std::vector<int> v;
        while(getline(ss, brs, ',')) {
            if( brs.length() == 0)
                v.push_back(INT_MAX);
            else
                v.push_back(stoi(brs));
            --cols;
        }
          // "1,3," this condition
        if(cols == 1)
            v.push_back(INT_MAX);
        // add row to table
        rowAdd(v);
    }

    return true;
}

void Table::rowAdd(const std::vector<int>& v){
    if(v.size() != nCols()) // input error
        cout << "Error columns\n";
    else
        _rows.push_back(Row(v));
}

void Table::PRINT(void){
    for(unsigned int i=0; i<nRows(); ++i)
    {
        for(unsigned int j=0; j<nCols(); ++j)
        {
            cout << setw(4) << right;
            if(_rows[i][j] == INT_MAX)
                cout << '.';
            else
                cout << _rows[i][j];
        }
        cout << endl;
    }
}

vector<int> Table::rowGet(const int& col){
    vector<int> v;
    if(col < 0 || (unsigned int)col >= nCols()) // over the range
        return v;
    for(unsigned int i=0; i<nRows(); ++i)
        if(_rows[i][col] != INT_MAX)
            v.push_back(_rows[i][col]);
    return v;
}

int Table::SUM(const int& col){
    vector<int> v = rowGet(col);
    int sum = 0;
    for(auto& i: v)
        sum += i;
    return sum;
}

int Table::MAX(const int& col){
    vector<int> v = rowGet(col);
    return *max_element(v.begin(), v.end());
}

int Table::MIN(const int& col){
    vector<int> v = rowGet(col);
    return *min_element(v.begin(), v.end());
}

int Table::DIST(const int& col){
    vector<int> v = rowGet(col);
    set<int> set_v(v.begin(), v.end());
    return set_v.size();
}

double Table::AVE(const int& col){
    vector<int> v = rowGet(col);
    return double(SUM(col)) / v.size();
}

void Table::ADD(const string& s){
   stringstream ss(s);
   std::vector<int> v;
   string sin;
   while(ss >> sin)
       if(sin == ".")
           v.push_back(INT_MAX);
       else
           v.push_back(stoi(sin));
   rowAdd(v);
}
