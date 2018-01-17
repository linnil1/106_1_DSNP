#include <iostream>
#include <fstream>
#include <vector>
#include "sat.h"

using namespace std;
typedef unsigned ID;
typedef vector<ID> IdList;

void getIn (IdList &ins, int is, IdList &outs, int os, fstream &f)
{
  // I
  for (int i=0; i<is; ++i) {
    ID a;
    f >> a;
    ins.push_back(a >> 1);
  }
  // O
  for (int i=0; i<os; ++i) {
    ID a;
    f >> a;
    outs.push_back(a);
  }
}

void getAnd (SatSolver &solver, int as, Var *gate, fstream& f)
{
  for (int i=0; i<as; ++i) {
    ID a, b, c;
    f >> a >> b >> c;
   solver.addAigCNF(gate[a >> 1],
       gate[b >> 1], b & 1,
       gate[c >> 1], c & 1);
  }
}

void getAll (SatSolver &solver, int* MILOA, Var* &gate, fstream& f)
{
  char aag[10];
  f >> aag;
  for (int i=0; i<5; ++i)
    f >> MILOA[i];
  // M
  gate = new Var[MILOA[0] + 1];
  for (int i=0; i<MILOA[0]; ++i)
    gate[i] = solver.newVar();
}

int main() {
  SatSolver solver;
  solver.initialize();

  IdList ins[2], outs[2];
  int MILOA[5];
  Var *gate0, *gate1;

  // first one
  fstream f0("w0");
  getAll(solver, MILOA, gate0, f0);
  getIn(ins[0], MILOA[1], outs[0], MILOA[3], f0);
  getAnd(solver, MILOA[4], gate0, f0);

  // second one
  fstream f1("w1");
  getAll(solver, MILOA, gate1, f1);
  // same input
  for (ID& id: ins[0])
    gate1[id] = gate0[id];
  gate1[0] = gate0[0];
  getIn(ins[1], MILOA[1], outs[1], MILOA[3], f1);
  getAnd(solver, MILOA[4], gate1, f1);

  // assert
  assert(ins[0].size() == ins[1].size());
  assert(outs[0].size() == outs[1].size());
  for (int i=0; i<MILOA[1]; ++i)
    assert(ins[0][i] == ins[1][i]);
  cout << "OK\n";
  
  // solve
  bool bad = false;
  for (int i=0; i<MILOA[3]; ++i) {
    Var newV = solver.newVar();
    solver.addXorCNF(newV, gate0[outs[0][i] >> 1], outs[0][i] & 1,
                           gate1[outs[1][i] >> 1], outs[1][i] & 1);
    solver.assumeRelease(); 
    solver.assumeProperty(gate0[0], false);
    solver.assumeProperty(newV, true);
    bool res = solver.assumpSolve();
    if (res) {
      cout << outs[0][i] << " vs " << outs[1][i] << " : " << res << endl;
      bad = true;
      cout << "Wrong AT\n";
      for (ID &j: ins[0])
        cout << solver.getValue(gate0[j]);
      cout << endl;
      for (int j=ins[0].size()-1; j>=0; --j)
        cout << solver.getValue(gate0[ins[0][j]]);
      cout << endl;
    }
  }
  if (!bad)
    cout << "GOOD\n";

  delete [] gate0;
  delete [] gate1;
}
