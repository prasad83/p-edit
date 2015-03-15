/* global.h: Contains all global variables and global function prototypes */
#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <iostream>
#include <fstream>
#include <curses.h>
#include <panel.h>
#include <menu.h>
#include <vector>
#include <string>
#include <cctype>
#include <stack>
using namespace std;

#include "popdown.h" /* for definition of CPopdownMenu */

WINDOW       *mnubar, *edit, *status, *myname;
PANEL        *mnupan, *editpan, *statuspan, *namepan;
CPopdownMenu pdFile, pdEdit, pdSearch, pdOption, pdHelp;

void initEditor();
void menuCreate();
int  getUsrResponse( const string msg, int btnStyle );
bool getUsrInput( const string msg, string &inputStr );
void mnuDisplayHandler();
void setNextMenu( chtype ch );

void CommandHandler( int mnuItemIndex );
void UserHelp();
string CreateUserHelpFile();
#endif
