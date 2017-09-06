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
//     if (true){
       string cmd;
       cin >> cmd;
       int c;

       if(cmd == "PRINT") 
           table.PRINT();
       else if(cmd == "SUM")
       {
           vector<int> col = table.rowGet(c);
           if(!col.size())
               cout << "Error: This is a NULL column!!\n";
           else
               cout << "The summation of data in column #" << c << " is " << table.SUM(col) << ".\n";
       }
       else if(cmd == "MAX")
       {
           vector<int> col = table.rowGet(c);
           if(!col.size())
               cout << "Error: This is a NULL column!!\n";
           else
               cout << "The maximum of data in column #"  << c << " is " << table.MAX(col) << ".\n";
       }
       else if(cmd == "MIN")
       {
           vector<int> col = table.rowGet(c);
           if(!col.size())
               cout << "Error: This is a NULL column!!\n";
           else
               cout << "The minimum of data in column #"  << c << " is " << table.MIN(col) << ".\n";
       }
       else if(cmd == "DIST")
       {
           vector<int> col = table.rowGet(c);
           if(!col.size())
               cout << "Error: This is a NULL column!!\n";
           else
               cout << "The distinct of data in column #" << c << " is " << table.DIST(col) << ".\n";
       }
       else if(cmd == "AVE")
       {
           vector<int> col = table.rowGet(c);
           if(!col.size())
               cout << "Error: This is a NULL column!!\n";
           else
               cout << "The average of data in column #"  << c << " is " <<
                   fixed << setprecision(1) << table.AVE(col) << ".\n";
       }
       else if(cmd == "ADD")
           table.ADD();
       else if(cmd == "EXIT")
           break;
   }
}
