/****************************************************************************
  FileName     [ cmdParser.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command parsing member functions for class CmdParser ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cctype>
#include "util.h"
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    External funcitons
//----------------------------------------------------------------------
void mybeep();


//----------------------------------------------------------------------
//    Member Function for class cmdParser
//----------------------------------------------------------------------
// return false if file cannot be opened
// Please refer to the comments in "DofileCmd::exec", cmdCommon.cpp

#define DOFILE_STACK_LIMIT 256
bool CmdParser::openDofile(const string& dof)
{
  if (_dofileStack.size() == DOFILE_STACK_LIMIT) // prevent infinity recursive
    return false;
  ifstream* tmp_dofile = new ifstream(dof.c_str());
  if (!tmp_dofile->is_open()) {
    delete tmp_dofile;
    return false;
  }
  _dofileStack.push(_dofile);
  _dofile = tmp_dofile;
  return true;
}

// Must make sure _dofile != 0
void CmdParser::closeDofile()
{
  assert(_dofile != 0);
  _dofile->close();
  delete _dofile;
  if (_dofileStack.size()) {
    _dofile = _dofileStack.top();
    _dofileStack.pop();
  }
  else
    _dofile = NULL;
}

// Return false if registration fails
bool CmdParser::regCmd(const string& cmd, unsigned nCmp, CmdExec* e)
{
  // Make sure cmd hasn't been registered and won't cause ambiguity
  string str = cmd;
  unsigned s = str.size();
  if (s < nCmp) return false;
  while (true) {
    if (getCmd(str)) return false;
    if (s == nCmp) break;
    str.resize(--s);
  }

  // Change the first nCmp characters to upper case to facilitate
  //    case-insensitive comparison later.
  // The strings stored in _cmdMap are all upper case
  //
  assert(str.size() == nCmp);  // str is now mandCmd
  string& mandCmd = str;
  for (unsigned i = 0; i < nCmp; ++i)
    mandCmd[i] = toupper(mandCmd[i]);
  string optCmd = cmd.substr(nCmp);
  assert(e != 0);
  e->setOptCmd(optCmd);

  // insert (mandCmd, e) to _cmdMap; return false if insertion fails.
  return (_cmdMap.insert(CmdRegPair(mandCmd, e))).second;
}

// Return false on "quit" or if excetion happens
CmdExecStatus CmdParser::execOneCmd()
{
  bool newCmd = false;
  if (_dofile != 0)
    newCmd = readCmd(*_dofile);
  else
    newCmd = readCmd(cin);

  // execute the command
  if (newCmd) {
    string option;
    CmdExec* e = parseCmd(option);
    if (e != 0)
      return e->exec(option);
  }
  return CMD_EXEC_NOP;
}

// For each CmdExec* in _cmdMap, call its "help()" to print out the help msg.
// Print an endl at the end.
void CmdParser::printHelps() const
{
  for(auto& cm: _cmdMap)
    cm.second->help();
  cout << endl;
}

void CmdParser::printHistory(int nPrint) const
{
  assert(_tempCmdStored == false);
  if (_history.empty()) {
    cout << "Empty command history!!" << endl;
    return;
  }
  int s = _history.size();
  if ((nPrint < 0) || (nPrint > s))
    nPrint = s;
  for (int i = s - nPrint; i < s; ++i)
    cout << "   " << i << ": " << _history[i] << endl;
}


//
// Parse the command from _history.back();
// Let string str = _history.back();
//
// 1. Read the command string (may contain multiple words) from the leading
//    part of str (i.e. the first word) and retrive the corresponding
//    CmdExec* from _cmdMap
//    ==> If command not found, print to cerr the following message:
//        Illegal command!! "(string cmdName)"
//    ==> return it at the end.
// 2. Call getCmd(cmd) to retrieve command from _cmdMap.
//    "cmd" is the first word of "str".
// 3. Get the command options from the trailing part of str (i.e. second
//    words and beyond) and store them in "option"
//
CmdExec* CmdParser::parseCmd(string& option)
{
  assert(_tempCmdStored == false);
  assert(!_history.empty());
  string str = _history.back();

  assert(str[0] != 0 && str[0] != ' ');
  size_t space_pos = str.find_first_of(' ');
  if (space_pos == string::npos)
    space_pos = str.length();

  string head = str.substr(0, space_pos),
         tail = str.substr(space_pos);
  CmdExec *ce = getCmd(head);

  if (!ce) {
    cerr << "Illegal command!! (" << head << ")\n";
    return NULL;
  }
  else {
    option = tail;
    return ce;
  }
}

// This function is called by pressing 'Tab'.
// It is to list the partially matched commands.
// "str" is the partial string before current cursor position. It can be
// a null string, or begin with ' '. The beginning ' ' will be ignored.
//
// Several possibilities after pressing 'Tab'
// (Let $ be the cursor position)
// 1. LIST ALL COMMANDS
//    --- 1.1 ---
//    [Before] Null cmd
//    cmd> $
//    --- 1.2 ---
//    [Before] Cmd with ' ' only
//    cmd>     $
//    [After Tab]
//    ==> List all the commands, each command is printed out by:
//           cout << setw(12) << left << cmd;
//    ==> Print a new line for every 5 commands
//    ==> After printing, re-print the prompt and place the cursor back to
//        original location (including ' ')
//
// 2. LIST ALL PARTIALLY MATCHED COMMANDS
//    --- 2.1 ---
//    [Before] partially matched (multiple matches)
//    cmd> h$                   // partially matched
//    [After Tab]
//    HELp        HIStory       // List all the parially matched commands
//    cmd> h$                   // and then re-print the partial command
//    --- 2.2 ---
//    [Before] partially matched (multiple matches)
//    cmd> h$llo                // partially matched with trailing characters
//    [After Tab]
//    HELp        HIStory       // List all the parially matched commands
//    cmd> h$llo                // and then re-print the partial command
//
// 3. LIST THE SINGLY MATCHED COMMAND
//    ==> In either of the following cases, print out cmd + ' '
//    ==> and reset _tabPressCount to 0
//    --- 3.1 ---
//    [Before] partially matched (single match)
//    cmd> he$
//    [After Tab]
//    cmd> heLp $               // auto completed with a space inserted
//    --- 3.2 ---
//    [Before] partially matched with trailing characters (single match)
//    cmd> he$ahah
//    [After Tab]
//    cmd> heLp $ahaha
//    ==> Automatically complete on the same line
//    ==> The auto-expanded part follow the strings stored in cmd map and
//        cmd->_optCmd. Insert a space after "heLp"
//    --- 3.3 ---
//    [Before] fully matched (cursor right behind cmd)
//    cmd> hElP$sdf
//    [After Tab]
//    cmd> hElP $sdf            // a space character is inserted
//
// 4. NO MATCH IN FITST WORD
//    --- 4.1 ---
//    [Before] No match
//    cmd> hek$
//    [After Tab]
//    ==> Beep and stay in the same location
//
// 5. FIRST WORD ALREADY MATCHED ON FIRST TAB PRESSING
//    --- 5.1 ---
//    [Before] Already matched on first tab pressing
//    cmd> help asd$gh
//    [After] Print out the usage for the already matched command
//    Usage: HELp [(string cmd)]
//    cmd> help asd$gh
//
// 6. FIRST WORD ALREADY MATCHED ON SECOND AND LATER TAB PRESSING
//    ==> Note: command usage has been printed under first tab press
//    ==> Check the word the cursor is at; get the prefix before the cursor
//    ==> So, this is to list the file names under current directory that
//        match the prefix
//    ==> List all the matched file names alphabetically by:
//           cout << setw(16) << left << fileName;
//    ==> Print a new line for every 5 commands
//    ==> After printing, re-print the prompt and place the cursor back to
//        original location
//    --- 6.1 ---
//    [Before] if prefix is empty, print all the file names
//    cmd> help $sdfgh
//    [After]
//    .               ..              Homework_3.docx Homework_3.pdf  Makefile
//    MustExist.txt   MustRemove.txt  bin             dofiles         include
//    lib             mydb            ref             src             testdb
//    cmd> help $sdfgh
//    --- 6.2 ---
//    [Before] with a prefix and with mutiple matched files
//    cmd> help M$Donald
//    [After]
//    Makefile        MustExist.txt   MustRemove.txt
//    cmd> help M$Donald
//    --- 6.3 ---
//    [Before] with a prefix and with mutiple matched files,
//             and these matched files have a common prefix
//    cmd> help Mu$k
//    [After]
//    ==> auto insert the common prefix and make a beep sound
//    ==> DO NOT print the matched files
//    cmd> help Must$k
//    --- 6.4 ---
//    [Before] with a prefix and with a singly matched file
//    cmd> help MustE$aa
//    [After] insert the remaining of the matched file name followed by a ' '
//    cmd> help MustExist.txt $aa
//    --- 6.5 ---
//    [Before] with a prefix and NO matched file
//    cmd> help Ye$kk
//    [After] beep and stay in the same location
//    cmd> help Ye$kk
//
//    [Note] The counting of tab press is reset after "newline" is entered.
//
// 7. FIRST WORD NO MATCH
//    --- 7.1 ---
//    [Before] Cursor NOT on the first word and NOT matched command
//    cmd> he haha$kk
//    [After Tab]
//    ==> Beep and stay in the same location

string samePrefixGet(const vector<string> &v, const string &target)
{
  string same;
  if (v.size())
    same = v[0];

  for (auto &s: v)
    for (size_t i=0; i<same.length(); ++i)
      if (same[i] != s[i]) {
        same.resize(i); // get same prefix
        break;
      }
  if (same.empty())
    return same;
  return string(same.begin() + target.length(), same.end()); // same part after target
}

void prettyPrint(const vector<string>& v, int sw) {
  int num=-1;
  for (auto &i: v) {
    if (++num%5==0)
      cout << endl;
    cout << setw(sw) << left << i;
  }
}

void CmdParser::listCmd(const string& str)
{
  // remove heading space
  char *head = _readBuf;
  for(;head<_readBufPtr && *head==' '; ++head);
  string cmd = string(head, _readBufPtr);

  // get possible cmd
  vector<string> cmd_possible;
  for (CmdRegPair &cm: _cmdMap) {
    string all = cm.first + cm.second->getOptCmd();
    if (cmd.empty() || (cmd.length() <= all.length() &&
        myStrNCmp(all, cmd, cmd.length()) == 0))
      cmd_possible.push_back(all);
  }

  // 1. show all command
  // 2. show possible cmd
    // same_part not exist in these commands
  if (cmd_possible.size() > 1) {
    prettyPrint(cmd_possible, 12);
    reprintCmd();
    _tabPressCount = 0;
    return ;
  }

  // 3. auto complete cmd
  else if (cmd_possible.size() == 1) {
    for (size_t i=cmd.length(); i<cmd_possible[0].size(); ++i)
      insertChar(cmd_possible[0][i]);
    insertChar(' ');
    _tabPressCount = 0;
    return ;
  }

  CmdExec* e = getCmd(cmd.substr(0, cmd.find_first_of(' ')));
  // 4. command no match
  // 7. command no match
  if (!e) {
    mybeep();
    _tabPressCount = 0;
    return ;
  }

  // 5. show usage
  if (_tabPressCount == 1) {
    cout << endl;
    e->usage(cout);
    reprintCmd();
    return ;
  }

  // 6. list file
//  assert(_tabPressCount > 1);
  string filename = cmd.substr(cmd.find_last_of(' ') + 1), filedir = ".";
  // support folder browering
  size_t dirpos = filename.find_last_of('/');
  if (dirpos != string::npos) {
    filedir = filename.substr(0, dirpos);
    filename = filename.substr(dirpos+1);
  }
  vector<string> file_possible;
  listDir(file_possible, filename, filedir); // need to check return code

  // 6.5 no match
  if (file_possible.empty()) {
    mybeep();
    return ;
  }
  // 6.1 no name
  if (filename.empty()) {
    prettyPrint(file_possible, 16);
    reprintCmd();
    return ;
  }

  string same_part = samePrefixGet(file_possible, filename);
  // 6.2 list file with prefix
  if (same_part.empty() and file_possible.size() > 1) {
    prettyPrint(file_possible, 16);
    reprintCmd();
    return ;
  }
  else {
  // 6.4 match one file
    if (file_possible.size() == 1)
      same_part += ' ';
  // 6.3 write prefix without list
    else
      mybeep();
    for (char &c: same_part)
      insertChar(c);
    return ;
  }
}

// cmd is a copy of the original input
//
// return the corresponding CmdExec* if "cmd" matches any command in _cmdMap
// return 0 if not found.
//
// Please note:
// ------------
// 1. The mandatory part of the command string (stored in _cmdMap) must match
// 2. The optional part can be partially omitted.
// 3. All string comparison are "case-insensitive".
//
CmdExec* CmdParser::getCmd(string cmd)
{
  CmdExec* e = NULL;
  for (CmdRegPair &cm: _cmdMap) {
    string all = cm.first + cm.second->getOptCmd();
    if (myStrNCmp(all, cmd, cm.first.length()) == 0)
      return cm.second;
  }
  return e;
}


//----------------------------------------------------------------------
//    Member Function for class CmdExec
//----------------------------------------------------------------------
// Return false if error options found
// "optional" = true if the option is optional XD
// "optional": default = true
//
bool CmdExec::lexSingleOption
(const string& option, string& token, bool optional) const
{
  size_t n = myStrGetTok(option, token);
  if (!optional) {
    if (token.size() == 0) {
      errorOption(CMD_OPT_MISSING, "");
      return false;
    }
  }
  if (n != string::npos) {
    errorOption(CMD_OPT_EXTRA, option.substr(n));
    return false;
  }
  return true;
}

// if nOpts is specified (!= 0), the number of tokens must be exactly = nOpts
// Otherwise, return false.
//
bool CmdExec::lexOptions
(const string& option, vector<string>& tokens, size_t nOpts) const
{
  string token;
  size_t n = myStrGetTok(option, token);
  while (token.size()) {
    tokens.push_back(token);
    n = myStrGetTok(option, token, n);
  }
  if (nOpts != 0) {
    if (tokens.size() < nOpts) {
      errorOption(CMD_OPT_MISSING, "");
      return false;
    }
    if (tokens.size() > nOpts) {
      errorOption(CMD_OPT_EXTRA, tokens[nOpts]);
      return false;
    }
  }
  return true;
}

CmdExecStatus CmdExec::errorOption(CmdOptionError err, const string& opt) const
{
  switch (err) {
    case CMD_OPT_MISSING:
      cerr << "Error: Missing option";
      if (opt.size()) cerr << " after (" << opt << ")";
      cerr << "!!" << endl;
    break;
    case CMD_OPT_EXTRA:
      cerr << "Error: Extra option!! (" << opt << ")" << endl;
    break;
    case CMD_OPT_ILLEGAL:
      cerr << "Error: Illegal option!! (" << opt << ")" << endl;
    break;
    case CMD_OPT_FOPEN_FAIL:
      cerr << "Error: cannot open file \"" << opt << "\"!!" << endl;
    break;
    default:
      cerr << "Error: Unknown option error type!! (" << err << ")" << endl;
    exit(-1);
  }
  return CMD_EXEC_ERROR;
}

