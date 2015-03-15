/* initialize.cpp: Initialize the starting graphics view of editor.
 *                 Create all windows,panels,menus neccessary.
 */
#include "global.h" /* all global variables and functions */

void initEditor() {
	initscr();	start_color();	noecho();
	cbreak(); /* get the characters as soon as it is typed */
	raw();    /* get the interrupt keys also like Ctrl + C... */

	init_pair( EDITCOLOR,    COLOR_BLACK, COLOR_WHITE );
	init_pair( MENUCOLOR,    COLOR_WHITE, COLOR_BLACK );
	init_pair( NAMECOLOR,    COLOR_YELLOW, COLOR_BLACK );
	init_pair( STATUSCOLOR,  COLOR_WHITE, COLOR_BLACK  );
	init_pair( DLGBOXCOLOR,  COLOR_WHITE, COLOR_BLACK  );

	init_pair( POPMNUCOLOR,  COLOR_WHITE, COLOR_BLACK  );

	menuCreate();

	edit = newwin( 24, 80, 0, 0 );
	editpan = new_panel( edit ); /* helps in hiding panel */
	wbkgd( edit, COLOR_PAIR( EDITCOLOR ) );

	scrollok( edit, TRUE ); /* enables the 'wscrl(...)' to work on 'edit' WINDOW */

	keypad( edit, TRUE );
	wrefresh( edit );

	status = newwin( 1, 80, 24, 0 );
	statuspan = new_panel( status ); /* helps in hiding panel */
	wbkgd( status, COLOR_PAIR( STATUSCOLOR ));
	keypad( status, TRUE );
	wrefresh( status );

	update_panels();	doupdate();
}

void menuCreate() {
	/* create menu bar */
	mnubar = newwin( 1, 72, 0, 0 );
	mnupan = new_panel( mnubar ); /* helps in hiding panel */

	wbkgd( mnubar, COLOR_PAIR( MENUCOLOR ) | A_BOLD );
	mvwprintw( mnubar, 0, 1,  " File "    );
	mvwprintw( mnubar, 0, 14, " Edit "    );
	mvwprintw( mnubar, 0, 25, " Search "  );
	mvwprintw( mnubar, 0, 38, " Options " );
	mvwprintw( mnubar, 0, 51, " Help "    );

	keypad( mnubar, TRUE );
	wrefresh( mnubar );

	/* create a name bar */
	myname = newwin( 1, 8, 0, 72 );
	namepan = new_panel( myname ); /* helps in hiding panel */
	wbkgd( myname, COLOR_PAIR( NAMECOLOR ) | A_BOLD );
	mvwprintw( myname, 0, 0, "PEDIT");
	wrefresh( myname );

	/* hide the menu bar and name bar after creation */
	hide_panel( namepan ); hide_panel( mnupan );

	/* creating file pop down menu */
	string pdFileItem[][2] = {
		" New       ","Ctrl+N ",
		" Open      ","Ctrl+O ",
		" ----------","------ ",
		" Save      ","    F2 ",
		" Save As...","   F12 ",
  		" ----------","------ ",
		" Close     ","   F10 ",
		" Exit      "," Alt+X "
	};
	/* popdown::create( int y, int x,string a[][2],int size,int mnuNumber ); */
	pdFile.create( 1, 1, pdFileItem, 8,1, " File " );

	/* creating edit pop down menu */
	string pdEditItem[][2] = {
		" Undo    ","Ctrl+U ",
		" --------","------ ",
		" Copy    ","    F3 ",
		" Paste   ","    F5 ",
		" Cut     ","    F6 ",
		" --------","------ ",
		" Del Word","Ctrl+W ",
		" Del Line","Ctrl+L ",
		" Del Selc","Ctrl+D "
	};
	pdEdit.create( 1, 14, pdEditItem, 9, 2, " Edit " );

	/* creating search pop down menu */
	string pdSearchItem[][2] = {
		" Find       ","Ctrl+F ",
		" FindAgain  ","Ctrl+A ",
		" FindReplace","Ctrl+R "
	};
	pdSearch.create( 1, 25, pdSearchItem, 3, 3, " Search " );

	/* creating options pop down menu */
	string pdOptionItem[][2] = {
		" LinShell   ","Alt+L ",
		" ShowSpecial","   F8 "
	};
	pdOption.create( 1, 38, pdOptionItem, 2, 4, " Options " );

	/* creating help pop down menu */
	string	pdHelpItem[][2] = {
		" User Help ","F1 ",
		" About...  ","   "
	};
	pdHelp.create( 1, 51, pdHelpItem, 2, 5, " Help " );
}
