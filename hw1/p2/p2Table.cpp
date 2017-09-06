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
#include <iomanip> 
#include <set>

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
        rowAdd(v);
    }

    return true;
}

void Table::rowAdd(std::vector<int>& v){
    _rows.push_back(Row(v));
}

void Table::PRINT(void){
    for(unsigned int i=0; i<nRows(); ++i,puts(""))
        for(unsigned int j=0; j<nCols(); ++j)
        {
            cout << setw(4) << right;
            if(_rows[i][j] == INT_MAX)
                cout << '.';
            else
                cout << _rows[i][j];
        }
}

vector<int> Table::rowGet(int &c){
    cin >> c;
    vector<int> v;
    for(unsigned int i=0; i<nRows(); ++i)
        if(_rows[i][c] != INT_MAX)
            v.push_back(_rows[i][c]);
    return v;
}

int Table::SUM(vector<int>& v){
    int sum = 0;
    for(auto& i: v)
        sum += i;
    return sum;
}

int Table::MAX(vector<int>& v){
    return *max_element(v.begin(), v.end());
}

int Table::MIN(vector<int>& v){
    return *min_element(v.begin(), v.end());
}

int Table::DIST(vector<int>& v){
    set<int> set_v(v.begin(), v.end());
    return set_v.size();
}

double Table::AVE(vector<int>& v){
    return double(SUM(v)) / v.size();
}

void Table::ADD(){
   string s;
   getline(cin, s);
   stringstream ss(s);
   std::vector<int> v;
   while(ss >> s)
       if(s == ".")
           v.push_back(INT_MAX);
       else
           v.push_back(stoi(s));
   if(v.size() != nCols())
       puts("Error columns");
   rowAdd(v);
}
