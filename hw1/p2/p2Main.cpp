/****************************************************************************
  FileName     [ p2Main.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define main() function ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip> 
#include <string>
#include "p2Table.h"

using namespace std;

int main()
{
   Table table;

   // Read in the csv file. Do NOT change this part of code.
   string csvFile;
   cout << "Please enter the file name: ";
   cin >> csvFile;
   // csvFile = "test2.csv";
   if (table.read(csvFile))
      cout << "File \"" << csvFile << "\" was read in successfully." << endl;
   else {
      cerr << "Failed to read in file \"" << csvFile << "\"!" << endl;
      exit(-1); // csvFile does not exist.
   }

// read and execute commands
   while (true) {
       string cmd;
       cin >> cmd;

       if(cmd == "PRINT") 
           table.PRINT();
       else if(cmd == "SUM" || cmd == "MAX" || cmd == "MIN" || cmd == "DIST" || cmd == "AVE")
       {
           int col;
           cin >> col;
           if(!table.rowGet(col).size())
               cout << "Error: This is a NULL column!!\n";
           else if(cmd == "SUM")
               cout << "The summation of data in column #" << col << " is " << table.SUM(col) << ".\n";
           else if(cmd == "MAX")
               cout << "The maximum of data in column #"  << col << " is " << table.MAX(col) << ".\n";
           else if(cmd == "MIN")
               cout << "The minimum of data in column #"  << col << " is " << table.MIN(col) << ".\n";
           else if(cmd == "DIST")
               cout << "The distinct count of data in column #" << col << " is " << table.DIST(col) << ".\n";
           else if(cmd == "AVE")
               cout << "The average of data in column #"  << col << " is " <<
                   fixed << setprecision(1) << table.AVE(col) << ".\n";
       }
       else if(cmd == "ADD")
       {
           string s;
           getline(cin, s);
           table.ADD(s);
       }
       else if(cmd == "EXIT")
           break;
   }
}
