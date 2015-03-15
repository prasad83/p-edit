/* Source File */
#include "openmenubox.h"
bool COpenMenuBox :: getFileName( string &fileName ) {
	fileName = "";	/*	re-initialize input string */
	int prevCursorStyle = curs_set(0);
	int	getval = handle_getfilename( dirName, fileName ); /* dirName = class variable */

	if( getval == 1 ) {	/* then file is selected */
		/*	append the fileName with it's corresponding directory
		 *	use "/"	in between as a seperator
		 */
		string	tmpFName = dirName + "/";	/*	"/directory" + "/"              */
				tmpFName += fileName;		/*	"/directory" + "/" + "filename" */
		fileName = tmpFName;				/*	reset the fileName              */
	}

	/* remove the menu from view */
	touchwin( edit ); wrefresh( edit );
	curs_set( prevCursorStyle );

	if( getval ) /* file was selected */
		return true;
	else /* nothing was selected */
		return false;
}

int COpenMenuBox :: handle_getfilename( string &dirName, string &fileName ) {
	int		fileType = NOSELECTION;
	bool	getname = true;
	int		len = 0;

	getCurrentDir( dirName );				/* get the current working directory name   */
	while( getname ) {
		get_dir_filenames( dirName );		/* get file information of current directory*/
		fileName = "";	fileType = 0;		/*	re-initialize fileNames and fileType    */
		getflnm_from_menu( dirName,fileName,fileType);/* provide menu box for selection */

		switch( fileType ) {
		case NOSELECTION:	case FILES:	/*	no fileselected or file is selected */
			getname = false;			/*	end the loop */
			break;
		case PARENDIR :
			len = dirName.rfind( "/" );
			/*	strip off "/name" from last
			 *	if current directory is not root "/" directory
			 */
			(len != 0)? (dirName = dirName.erase( len, dirName.length() )): (dirName = "/");
			break;
		case DIR:
			if( dirName.length() != 1 )	/*	present directory is other than "/" */
				dirName += "/";			/*	so add the seperator "/"            */
			dirName += fileName;		/*	append new selected directory Name  */
			break;
		}
	}
	int retval = ( fileType == FILES )? 1 : 0;	/*	file selected = 1, else 0 */
	return retval;
}

void COpenMenuBox :: getflnm_from_menu( string dirName, string &fileName, int &fileType ) {
	int n_choices, indx;
	ITEM **items;    MENU *menu;    WINDOW *mnu_win, *mnu_derwin;

	/* Create items */
	n_choices = vecChoice.size();

    items = (ITEM **)calloc(n_choices+1, sizeof(ITEM *));
	for(indx = 0; indx < n_choices; ++indx)
		items[indx] = new_item( vecChoice[indx].c_str(),
						 ( vecFtype[indx]== FILES ? "  File" : "  Dir" ) );
	items[n_choices] = (ITEM *)NULL;

	/* Crate menu */
	menu = new_menu((ITEM **)items);

 	/* Screen size parameters */
 	int	row = 14,col = 68,ypos = 5,xpos = 5;

	/* Create the window to be associated with the menu */
	mnu_win = newwin( row, col, ypos-1, xpos-1);
	wbkgd( mnu_win, COLOR_PAIR( DLGBOXCOLOR ) | WA_BOLD );
	mvwaddstr( mnu_win, 1, 1, dirName.c_str() );
	keypad(mnu_win, TRUE);

	/* Create derived window for displaying file names & its type */
	mnu_derwin = derwin(mnu_win, 9, col-4, 3, 1);
	wbkgd( mnu_derwin, COLOR_PAIR( DLGBOXCOLOR ) );

	/* Set main window and sub window */
	set_menu_win(menu, mnu_win );
	set_menu_sub(menu, mnu_derwin );
	set_menu_back( menu, COLOR_PAIR( DLGBOXCOLOR ) );
	set_menu_fore( menu, WA_BOLD ); /* highlight the file selection */
	set_menu_grey( menu, COLOR_PAIR( DLGBOXCOLOR ));
	set_menu_format(menu, 9, 1);	/*	9 row and 1 column display for scrolling */

	/* Set menu mark to the string "* " */
	set_menu_mark(menu, " * ");

	/* Print a border around the main window and print a title */
	box(mnu_win, 0, 0);
	mvwaddch(mnu_win, 2, 0, ACS_LTEE);
	mvwhline(mnu_win, 2, 1, ACS_HLINE, col-2);
	mvwaddch(mnu_win, 2, col-1, ACS_RTEE);

	/* Post the menu */
	post_menu(menu);
	wrefresh(mnu_win);

	bool mnuSelection = true;
	int	 c = 0;
	while( mnuSelection ){
		switch( c = wgetch(mnu_win) ){
		case 27 :
			indx = -1;	/*	indicates no selection */
			fileName = "";		fileType = NOSELECTION;
			mnuSelection = false;
			break;
		case '\n':
			indx = item_index( current_item( menu) );
			mnuSelection = false;
			break;

		case KEY_DOWN:
			menu_driver(menu, REQ_DOWN_ITEM);
			break;
		case KEY_UP:
	        menu_driver(menu, REQ_UP_ITEM);
    	    break;
		case KEY_NPAGE:
        	menu_driver(menu, REQ_SCR_DPAGE);
        	break;
		case KEY_PPAGE:
	        menu_driver(menu, REQ_SCR_UPAGE);
    	    break;
		}
		wrefresh(mnu_win);
	}

	if( indx != -1 ) {
		fileName = vecChoice[indx];
		fileType = vecFtype[indx];
	}

	/* Unpost and free all the memory taken up */
	unpost_menu(menu);	free_menu(menu);	free( items );
	delwin(mnu_win);
	delwin(mnu_derwin);

	return;
}

void COpenMenuBox :: getCurrentDir( string &dirName ) {
	/*	get the present working directory */
	system( "pwd > /tmp/pedit_curdir 2> /dev/null" );
	ifstream curDir( "/tmp/pedit_curdir" );
	if( curDir ) {
		dirName = "";			/*	clear the contents */
		char   ch = curDir.get();
		while( curDir ) {
			if( isprint(ch) )	/*	if ch is a printable character */
				dirName += ch;	/*	add the characters to string   */
			ch = curDir.get();	/*	get next character             */
		}
		curDir.close();			/*	close the file opened */
	}
	else
		dirName = "/";			/*	default directory "/" */
	system( "rm -f /tmp/pedit_curdir 2> /dev/null" );	/*	remove the created file */
	return;
}

void COpenMenuBox :: get_dir_filenames( string &dirName ) {
	/*	Check if current directory can be opened
	 *	if it cannot be opened the reset directory to current directory
	 *	if it is opened close that directroy
	 */

	/*	Command Used: echo `ls >/dev/null 2> /dev/null dirName` $?	> /tmp/pedit_tstDir
	 *		 Remark : command substitution is used ( `..` ) for echo and output of
	 *				  $? = stores exit status of (`..`) execution and is put to the file
	 *  $? = 0 if command excutes as expected; $? = 1 if command fails to execute
	 */
	string	tstDir = "echo `";
	tstDir  += "ls > /dev/null 2> /dev/null ";
	tstDir  += dirName;
	tstDir  += "` $? > /tmp/pedit_tstDir 2> /dev/null";
	system( tstDir.c_str() );

	ifstream tstDirFile( "/tmp/pedit_tstDir" );	/* open the file created */
	if( tstDirFile ) {							/* if file is opened     */
		char ch = tstDirFile.get();				/* get the number written as output 0 or 1 */
		switch( ch ) {
		case '0':								/* if 0 then current directory has access  */
			break;
		case '1':								/* if 1 then current directory permission denied */
			getCurrentDir( dirName );			/* so get current directory name as default */
			break;
		}
		tstDirFile.close();
	}

	/*	ls 	  : take listing of directory
	 *	ls -1 : take listing of directory with one file per line
	 *	ls -F : take listing of directory with filetype specifiers ( /,*,=,@.| )
	 */
	string 	sysCommand = "ls -1F ";		/*	"ls -1F"                             */
	sysCommand += dirName;				/*	"ls -1F dirName"                     */
	sysCommand += " > /tmp/pedit_dir";	/*	"ls -1F dirName > /tmp/pedit_dir"    */
	sysCommand += " 2> /dev/null";		/*	"ls -1F dirName > /tmp/pedit_dir 2> /dev/null" */
	system( sysCommand.c_str() );		/*	execute the above command            */

	ifstream curDir( "/tmp/pedit_dir" );
 	if( curDir ) {
		/*	Clear the previous contents in the vector */
		vecChoice.erase( vecChoice.begin(), vecChoice.end() );
		vecFtype.erase( vecFtype.begin(), vecFtype.end() );

		string str = "";
		char   ch = curDir.get();
		while( curDir ) {
			switch( ch ) {
			case '/' :
				/* represents directory next character will be a '\n' so eat it up */
				ch = curDir.get();
				vecChoice.insert( vecChoice.end(), str );	/*	store the string content   */
				vecFtype.insert( vecFtype.end(), DIR );		/*	store the file type as DIR */
				str ="";									/*	re-initialize the string   */
				break;
			case '*': case '=': case '@' :	case '|': /*	represents special files */
				ch = curDir.get();			/*	eat up the '\n' character but DON'T BREAK */
			case '\n':						/*	no symbol for ordinary files */
				vecChoice.insert( vecChoice.end(), str );	/*	store the string content */
				vecFtype.insert( vecFtype.end(), FILES );	/*	store the file type as FILES */
				str ="";									/*	re-initialize the string */
				break;
			default:
				str += ch;		/*	add the characters to string */
				break;
			}
			ch = curDir.get();	/*	get next character */
		}
		curDir.close();			/*	close the file */

		vecChoice.insert( vecChoice.begin(), ".." );	/*	store the string ".." */
		vecFtype.insert( vecFtype.begin(), PARENDIR );	/*	store the file type as PARENDIR */
	}

	/*	remove the dir listing file created */
	system( "rm -f /tmp/pedit_dir /tmp/pedit_tstDir 2> /dev/null" );
	return;
}
