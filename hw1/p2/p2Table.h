/****************************************************************************
  FileName     [ p2Table.h]
  PackageName  [ p2 ]
  Synopsis     [ Header file for class Row and Table ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#ifndef P2_TABLE_H
#define P2_TABLE_H

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;

class Row
{
public:
    // constructor and member functions on your own
    Row(vector<int>& v)
    {
        // not work
        // _data = v.data();
        _data = new int [v.size()];
        for(unsigned int i=0; i<v.size(); ++i)
            _data[i] = v[i];
    }

    // Basic access functions
    const int operator[] (size_t i) const { return _data[i]; }
    int& operator[] (size_t i) { return _data[i]; }

private:
    int *_data; // DO NOT change this definition. Use it to store data.
};

class Table
{
public:
    // constructor
    Table()
    {
        _nCols = 0;
        _rows = vector<Row>();
    }

    // read table
    bool read(const string&);

    // cmd function
    vector<int> rowGet(int&);
    void PRINT(void);
    int SUM(vector<int>&);
    int MAX(vector<int>&);
    int MIN(vector<int>&);
    int DIST(vector<int>&);
    double AVE(vector<int>&);
    void rowAdd(vector<int>&);
    void ADD(void);

    // Basic access functions
    size_t nCols() const { return _nCols; }
    size_t nRows() const { return _rows.size(); }
    const Row& operator[] (size_t i) const { return _rows[i]; }
    Row& operator[] (size_t i) { return _rows[i]; }

private:
    size_t       _nCols; // You should record the number of columns.
    vector<Row>  _rows;  // DO NOT change this definition. Use it to store rows.
};

#endif // P2_TABLE_H
