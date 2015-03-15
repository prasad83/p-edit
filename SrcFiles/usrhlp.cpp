/* UserHelp, Load the help file(./UserHelpFile.hlp) from disk and display it in a
 * seperate window if the file is found.
 * If the file is not found create help file( ./UserHelpFile.hlp or /tmp/peditUserHelp.hlp
 * if write permission is not there for current directory. */

void UserHelp() {
    /* VERY IMPORTANT: save previous edit window */
	WINDOW *prevEditPtr = edit;

	WINDOW *nmhlpwn = newwin( 22, 74, 1, 3 ); /* just to show name and box */
	wbkgd( nmhlpwn, COLOR_PAIR( DLGBOXCOLOR ));
	box( nmhlpwn, 0, 0 );                     /* box    */
	wattron( nmhlpwn, A_BOLD );               /* bold   */
	mvwprintw( nmhlpwn, 0, 32," User Help "); /* name   */
	mvwprintw( nmhlpwn, 21,51," Designed By Prasad.A " );
	wattroff( nmhlpwn, A_BOLD );              /* nobold */
	wrefresh( nmhlpwn );

	WINDOW *hlpwn = newwin( 20, 72, 2, 4 );
	wbkgd( hlpwn, COLOR_PAIR( DLGBOXCOLOR ));
	keypad( hlpwn, TRUE );
	wrefresh( hlpwn );

	/* show help on status bar */
	wmove( status, 0, 0 ); wclrtoeol( status );
	wattron(   status, A_BOLD );
	mvwprintw( status, 0, 0, "Press 'Esc' or 'Enter' to return back for editing." );
	wattroff(  status, A_BOLD );
	wrefresh(  status );

    /* VERY IMPORTANT */
	edit = hlpwn; /* This is neccessay as all action take place with respect
                   * to 'edit' in CView, CDocument etc... */

	/* set left, right, top, bottom margin depending on window widht, height */
	CDocument usrDocOb(0, 71, 0, 19 );

	/* no need to show special type display in help */
	if( usrDocOb.getShowSpecial() )	usrDocOb.setShowSpecial();

	/* read the help file and display it. */
	string hlpFlPath = "./UserHelp.hlp";
	if( !usrDocOb.ReadFile( hlpFlPath.c_str() ) ){ /* could not find the help file */
		hlpFlPath = CreateUserHelpFile(); /* create the help file */
		usrDocOb.ReadFile( hlpFlPath.c_str() ); /* this will be successful */
	}
	/* now the help file will be loaded */
	usrDocOb.DisplayFile( usrDocOb.gettopline(), usrDocOb.gettopchar() );
	/* go to start position */
	wmove( edit, usrDocOb.getCursy(), usrDocOb.getCursx() );

	chtype ch = wgetch( hlpwn );
	while( ch != 27 && ch != '\n' ) { /* till ESC or Enter is pressed */
		switch( ch ) { /* no editing allowed */
		case KEY_LEFT: usrDocOb.mvleft();   break;
		case KEY_RIGHT:usrDocOb.mvright();  break;
		case KEY_UP:   usrDocOb.mvup();     break;
		case KEY_DOWN: usrDocOb.mvdown();   break;
		case KEY_HOME: usrDocOb.mvhome();   break;
		case KEY_END : usrDocOb.mvend ();   break;
		case KEY_PPAGE:usrDocOb.pageup();   break;
		case KEY_NPAGE:usrDocOb.pagedown(); break;
		}
		if( usrDocOb.getRefreshEdit() ) {
			usrDocOb.DisplayFile( usrDocOb.gettopline(), usrDocOb.gettopchar() );
		}
		wmove( edit, usrDocOb.getCursy(), usrDocOb.getCursx() );
		ch = wgetch( hlpwn ); /* take the input */
	}
	delwin( nmhlpwn ); delwin( hlpwn ); /* remove nmhlpwn, hlpwn from memory stack */

	/* VERY IMPROTANT: re-assign the edit pointer, remove hlpwn from display */
	edit = prevEditPtr; touchwin( edit ); wrefresh( edit );
}

string CreateUserHelpFile(){
	/* try creating the file in current directory */
	string filePath = "./UserHelp.hlp";
	ofstream hlpfl( filePath.c_str() );

	if( !hlpfl ) {
		/* file could not be created in current directory,
		 * may be no directory write permission. */
		filePath =	"/tmp/peditUserHelp.hlp"; /* try creating file in /tmp */
		hlpfl.open( filePath.c_str() ); /* will be successfull at most of times */
	}
	/* file is created put the contents into it. */

	/* Note:'\' is added to put the string on next line it will not appear in the file */

	hlpfl <<
"General:\n\
********\n\
        'Pedit' was written as project work of 5th semester,B.E.\n\
It is a very simple editor which allows you to edit documents.\n\
It has a special property which can display the tab, newline and\n\
space with visible special symbols ( » ((char)187), ¶((char)182)\n\
·((char)183) ) respectively.\n\
The Status bar shows the filename followed by (=) symbol which means\n\
the file contents is not changed when file contents changes the (*)\n\
appears.\n\
\n\
Usage:\n\
******\n\
'Pedit' accepts file-name at the command line, which is optional.\n\
\n\
The Menu bar is not shown always this is main advantage for full\n\
scale display to be made. You can make it visible by invoking it.\n\
\n\
Menu Invocation:\n\
****************\n\
    As already said Menu bar is not always displayed to make it\n\
available you have to press 'F9' key.\n\
\n\
You can now move to left or right adjacent menu by pressing Left\n\
or Right arrow Key.\n\
\n\
You can make the pull down menu to appear by pressing Down arrow\n\
or Enter key or ( f|F, e|E, s|S, o|O, h|H to pull File | Edit |\n\
Search | Options | Help menu ).\n\
\n\
To make selection in pull down menu use Up or Down Arrow.\n\
\n\
To Select the options from the menu use Enter key.\n\
\n\
To Cancel the menu selection use Esc key.\n\
\n\
Menu Options:\n\
*************\n\
File Menu:\n\
**********\n\
*New - Ctrl+N:\n\
--------------\n\
    This option will create a new blank file named(\"noname\") for\n\
editing.\n\
\n\
*Open - Ctrl+O:\n\
---------------\n\
    This option will display a selection dialog box from which\n\
you can select a file for editing. You can also traverse into\n\
different directories and select the files in them. Pressing Esc\n\
key will cancel the selection.\n\
\n\
*Save - F2:\n\
-----------\n\
This option will save the currently editing file.\n\
\n\
*Save As... - F12:\n\
------------------\n\
    This option will lets you to save the currently editing file\n\
under a different name as specified by you in the dialog box.\n\
\n\
*Close - F10:\n\
-------------\n\
    This option will close the currently opened file for editing.\n\
\n\
*Exit - Alt+X:\n\
--------------\n\
    This option will let you 'quit' editor. Before quiting you\n\
will asked to keep or delete the backup's made (if any) in /tmp.\n\
Backup's will be made as /tmp/<filename>._prtmp when file is\n\
over-written during Save or Save As... options.\n\
\n\
Edit Menu:\n\
**********\n\
*Undo - Ctrl+U:\n\
---------------\n\
    This option will undo last editing operation.\n\
\n\
*Copy - F3:\n\
-----------\n\
    This option will select the part of the document for cutting,\n\
pasting or deleting. The first press of 'F3' key will start the\n\
selection of document. You are now in document selection process\n\
where you are not allowed to perform editing but can traverse the\n\
document. The second press of 'F3' key will end the selection of\n\
document.\n\
\n\
*Paste - F5:\n\
------------\n\
    This option will paste the previously selected document,which\n\
is not deleted, at the current cursor position.\n\
\n\
*Cut - F6:\n\
----------\n\
    This option will cut the selected document.The cut operation\n\
should be invoked before any editing operation is done on the\n\
document.The Cut operation will allow Paste to be made.\n\
\n\
*Del Word - Ctrl+W:\n\
-------------------\n\
    This option will delete the whole word/ part of the word to\n\
the right of the word. The deletion if word is alpha-numeric.\n\
\n\
*Del Line - Ctrl+L:\n\
-------------------\n\
    This option will delete the whole line on which cursor is\n\
presently present.\n\
\n\
*Del Selc - Ctrl+D:\n\
-------------------\n\
    This option will delete selected document. This operation\n\
should be invoked before any editing operation is done on the\n\
document.The Del Selc operation will not allow Paste to be made.\n\
\n\
Search Menu:\n\
************\n\
*Find - Ctrl+F:\n\
---------------\n\
    This option will allow you to specify the word to be searched\n\
in the document.\n\
\n\
*FindAgain - Ctrl+A:\n\
--------------------\n\
    This option will continue search for the previously specified\n\
word by user. If no word is specified then Find is called.\n\
\n\
*FindReplace - Ctrl+R:\n\
----------------------\n\
    This option will take the word to be replaced from you and will\n\
be searched in the document. If search is successfull another word\n\
is taken from you which is the replacing word. The replacing can\n\
be done interactively one at a time or all at once.\n\
\n\
Options Menu:\n\
*************\n\
*LinShell - Alt+L:\n\
------------------\n\
    This option will take you to the Linux Shell as specified by the\n\
environmental variable '$SHELL' for future terminal operations. You\n\
can get back to editing of document by exiting the Shell.\n\
\n\
*ShowSpecial - F8:\n\
------------------\n\
    This option will toggle the display of special symbols for tab,\n\
newline and space.\n\
\n\
Help Menu:\n\
**********\n\
*User Help - F1:\n\
----------------\n\
    This option will open the currently shown window.\n\
\n\
*About... :\n\
-----------\n\
    This option will show the information about the author of Pedit.\n\
\n\
** END **" ; /* no endline */

	hlpfl.close(); /* close the file */
	return filePath; /* return the path to file created */
}
