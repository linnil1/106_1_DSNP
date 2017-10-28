/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool initMemCmd()
{
  if (!(cmdMgr->regCmd("MTReset" , 3, new MTResetCmd ) &&
        cmdMgr->regCmd("MTNew"   , 3, new MTNewCmd   ) &&
        cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
        cmdMgr->regCmd("MTPrint" , 3, new MTPrintCmd ) )) {
    cerr << "Registering \"mem\" commands fails... exiting" << endl;
    return false;
  }
  return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus MTResetCmd::exec(const string& option)
{
  // check option
  string token;
  if (!CmdExec::lexSingleOption(option, token))
    return CMD_EXEC_ERROR;
  if (token.size()) {
    int b;
    if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
      cerr << "Illegal block size (" << token << ")!!" << endl;
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
    }
    #ifdef MEM_MGR_H
    mtest.reset(toSizeT(b));
    #else
    mtest.reset();
    #endif // MEM_MGR_H
  }
  else
    mtest.reset();
  return CMD_EXEC_DONE;
}

void MTResetCmd::usage(ostream& os) const
{
  os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void MTResetCmd::help() const
{
  cout << setw(15) << left << "MTReset: "
       << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus MTNewCmd::exec(const string& option)
{
  // read
  vector<string> token;
  int num=-1, arrnum=-1;
  if (!CmdExec::lexOptions(option, token))
    return CMD_EXEC_ERROR;

  // error handle
  for (size_t i=0; i<token.size(); ++i)
    if (myStrNCmp("-Array", token[i], 1) == 0) {
      if (arrnum != -1) {
        CmdExec::errorOption(CMD_OPT_EXTRA, token[i]);
        return CMD_EXEC_ERROR;
      }
      if (i + 1 >= token.size()) {
        CmdExec::errorOption(CMD_OPT_MISSING, token[i]);
        return CMD_EXEC_ERROR;
      }
      if (!myStr2Int(token[i + 1], arrnum) || arrnum <= 0) {
        CmdExec::errorOption(CMD_OPT_ILLEGAL, token[i + 1]);
        return CMD_EXEC_ERROR;
      }
      // true
      i += 1;
    }
    else {
      if (num != -1) {
        CmdExec::errorOption(CMD_OPT_EXTRA, token[i]);
        return CMD_EXEC_ERROR;
      }
      if (!myStr2Int(token[i], num) || num <= 0) {
        CmdExec::errorOption(CMD_OPT_ILLEGAL, token[i]);
        return CMD_EXEC_ERROR;
      }
    }

  if (num == -1) {
    CmdExec::errorOption(CMD_OPT_MISSING, "");
    return CMD_EXEC_ERROR;
  }

  // call function
  if (arrnum == -1)
    mtest.newObjs(num);
  else
    mtest.newArrs(num, arrnum);
  return CMD_EXEC_DONE;
}

void MTNewCmd::usage(ostream& os) const
{
  os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void MTNewCmd::help() const
{
  cout << setw(15) << left << "MTNew: "
       << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus MTDeleteCmd::exec(const string& option)
{
  vector<string> token;
  int num=-1, randnum = -1, doarr=0;
  if (!CmdExec::lexOptions(option, token))
    return CMD_EXEC_ERROR;

  // error handle
  for (size_t i=0; i<token.size(); ++i)
    if (myStrNCmp("-Index", token[i], 1) == 0) {
      if (num != -1 || randnum != -1) {
        CmdExec::errorOption(CMD_OPT_EXTRA, token[i]);
        return CMD_EXEC_ERROR;
      }
      if (i + 1 >= token.size()) {
        CmdExec::errorOption(CMD_OPT_MISSING, token[i]);
        return CMD_EXEC_ERROR;
      }
      if (!myStr2Int(token[i + 1], num) || num < 0) {
        CmdExec::errorOption(CMD_OPT_ILLEGAL, token[i + 1]);
        return CMD_EXEC_ERROR;
      }
      // true
      i += 1;
    }
    else if (myStrNCmp("-Random", token[i], 1) == 0) {
      if (num != -1 || randnum != -1) {
        CmdExec::errorOption(CMD_OPT_EXTRA, token[i]);
        return CMD_EXEC_ERROR;
      }
      if (i + 1 >= token.size()) {
        CmdExec::errorOption(CMD_OPT_MISSING, token[i]);
        return CMD_EXEC_ERROR;
      }
      if (!myStr2Int(token[i + 1], randnum) || randnum <= 0) {
        CmdExec::errorOption(CMD_OPT_ILLEGAL, token[i + 1]);
        return CMD_EXEC_ERROR;
      }
      // true
      i += 1;
    }
    else if (myStrNCmp("-Array", token[i], 1) == 0) {
      if (doarr) {
        CmdExec::errorOption(CMD_OPT_EXTRA, token[i]);
        return CMD_EXEC_ERROR;
      }
      doarr = 1;
    }
    else {
      CmdExec::errorOption(CMD_OPT_ILLEGAL, token[i]);
      return CMD_EXEC_ERROR;
    }


  // TODO random
  if (doarr)
    mtest.deleteArr(num);
  else
    mtest.deleteObj(num);

  return CMD_EXEC_DONE;
}

void MTDeleteCmd::usage(ostream& os) const
{
  os << "Usage: MTDelete <-Index (size_t objId) | "
     << "-Random (size_t numRandId)> [-Array]" << endl;
}

void MTDeleteCmd::help() const
{
  cout << setw(15) << left << "MTDelete: "
       << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus MTPrintCmd::exec(const string& option)
{
  // check option
  if (option.size())
    return CmdExec::errorOption(CMD_OPT_EXTRA, option);
  mtest.print();

  return CMD_EXEC_DONE;
}

void MTPrintCmd::usage(ostream& os) const
{
  os << "Usage: MTPrint" << endl;
}

void MTPrintCmd::help() const
{
  cout << setw(15) << left << "MTPrint: "
       << "(memory test) print memory manager info" << endl;
}
