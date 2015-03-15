/* dlgboxes.cpp: create dialog boxes to take user input */
#include "macro.h"

extern CDocument DocOb;

/* getUsrResponse, show a dialog box with buttons and
 * return appropriate signal when button is selected. */
int getUsrResponse( const string msg, int btnStyle ) {
	/* btnstyl may be OK, YES_NO, YES_NO_CANCEL, SAVE_SAVEAS_CANCEL,
	 * YES_NO_YESTOALL_CANCEL */
	WINDOW *dlgwn, *msgwn;

	int dlgwnlen = 50,   dlgwnht = 6;
	int msgwnlen = 48, msgwnht   = 3;

	WINDOW *btnwn[4];  /* maximum 4 buttons */
	string btnName[4]; /* names to be put   */
	int    nbuttons;   /* number of buttons */
	int    btnXpos[4]; /* btn text placement position */
	int    btnYpos;    /* all buttons at same level */
	int    signal[4];  /* return value of selection */

	/* decide buttons parameters */
	switch( btnStyle ) {
	case OK:
		nbuttons = 1; signal[0]  = OK; btnName[0] = "[ OK ]"; btnXpos[0] = 32;
		break;
	case YES_NO:
		nbuttons = 2;
		signal[0]  = YES;     signal[1]  = NO;
		btnName[0] = "[Yes]"; btnName[1] = "[No]";
		btnXpos[0] = 27;       btnXpos[1] = 42;
		break;
	case YES_NO_CANCEL:
		nbuttons = 3;
		signal[0]  = YES;     signal[1] = NO;      signal[2]  = CANCEL;
		btnName[0] = "[Yes]"; btnName[1] = "[No]"; btnName[2] = "[Cancel]";
		btnXpos[0] = 15;       btnXpos[1] = 32;     btnXpos[2] = 51;
		break;
	case SAVE_SAVEAS_CANCEL:
		nbuttons = 3;
		signal[0]  = SAVE;     signal[1] = SAVEAS;      signal[2]  = CANCEL;
		btnName[0] = "[Save]"; btnName[1] = "[SaveAs]"; btnName[2] = "[Cancel]";
		btnXpos[0] = 15;       btnXpos[1] = 32;          btnXpos[2] = 51;
		break;
	case YES_NO_YESTOALL_CANCEL:
		nbuttons = 4;
		signal[0]  = YES; signal[1] = NO; signal[2]  = YESTOALL; signal[3]  = CANCEL;
		btnName[0] = "[Yes]"; btnName[1] = "[No]";
		btnName[2] = "[YesToAll]";btnName[3] = "[Cancel]";
		btnXpos[0] = 15; btnXpos[1] = 27;  btnXpos[2] = 37; btnXpos[3] = 51;
		break;
	}

	/* create windows */
	dlgwn = newwin( dlgwnht, dlgwnlen, 7, 13 );
	wbkgd( dlgwn, COLOR_PAIR( DLGBOXCOLOR ) );
	box( dlgwn, 0, 0 );
	wrefresh( dlgwn );
	keypad( dlgwn, TRUE );

	/* create button windows */
	btnYpos = 11;
	switch( btnStyle ) {
	case OK:
		btnwn[0] = newwin( 1, btnName[0].length()+2, btnYpos, btnXpos[0] );
		break;
	case YES_NO:
		btnwn[0] = newwin( 1, btnName[0].length()+2, btnYpos, btnXpos[0] );
		btnwn[1] = newwin( 1, btnName[1].length()+2, btnYpos, btnXpos[1] );
		break;
	case YES_NO_CANCEL:
		btnwn[0] = newwin( 1, btnName[0].length()+2, btnYpos, btnXpos[0] );
		btnwn[1] = newwin( 1, btnName[1].length()+2, btnYpos, btnXpos[1] );
		btnwn[2] = newwin( 1, btnName[2].length()+2, btnYpos, btnXpos[2] );
		break;
	case SAVE_SAVEAS_CANCEL:
		btnwn[0] = newwin( 1, btnName[0].length()+2, btnYpos, btnXpos[0] );
		btnwn[1] = newwin( 1, btnName[1].length()+2, btnYpos, btnXpos[1] );
		btnwn[2] = newwin( 1, btnName[2].length()+2, btnYpos, btnXpos[2] );
		break;
	case YES_NO_YESTOALL_CANCEL:
		btnwn[0] = newwin( 1, btnName[0].length()+2, btnYpos, btnXpos[0] );
		btnwn[1] = newwin( 1, btnName[1].length()+2, btnYpos, btnXpos[1] );
		btnwn[2] = newwin( 1, btnName[2].length()+2, btnYpos, btnXpos[2] );
		btnwn[3] = newwin( 1, btnName[3].length()+2, btnYpos, btnXpos[3] );
		break;
	}
	/* set the button window characteristics */
	for( int i = 0; i < nbuttons; ++i ) {
		wcolor_set( btnwn[i], COLOR_PAIR( DLGBOXCOLOR ),0 ); /* set colors */
  		keypad( btnwn[i], TRUE );   /* give keyboard access */

		/* put the names of button */
		mvwprintw( btnwn[i], 0, 1, "%s", btnName[i].c_str() );
		wrefresh( btnwn[i] );
	}

	/* create message window */
	msgwn = newwin( msgwnht, msgwnlen, 8, 14 );
	wbkgd( msgwn, COLOR_PAIR( DLGBOXCOLOR )|A_BOLD ); /* highlight message */
	mvwprintw( msgwn, 0, 1, "*%s", msg.c_str() );
	/* put a '*' so that active selection is seen if no bold is avaiable */
	wrefresh( msgwn );

	int prevopt = 0, curropt = 0;
	bool selected = false;
	chtype ch;

	while( !selected ) {
		/* turn off highlighting previous window */
		wattroff( btnwn[prevopt], A_BOLD );
		mvwprintw( btnwn[prevopt], 0, 1, " %s", btnName[prevopt].c_str() );
		wrefresh( btnwn[prevopt] );

		/* turn on highlighting current window */
		wattron( btnwn[curropt], A_BOLD );
		mvwprintw( btnwn[curropt], 0, 1, "*%s", btnName[curropt].c_str() );
		/* put a '*' so that active selection is seen if no bold is avaiable */
		wrefresh( btnwn[curropt] );

		/* show cursor on current button */
		wmove( edit, DocOb.getCursy(),DocOb.getCursx());
		wrefresh( edit );
		switch( ch = wgetch( dlgwn ) ) {
		case 27: selected = true; curropt = -1; break;
		case KEY_RIGHT:
			prevopt = curropt;
			curropt = ( curropt + 1 ) % nbuttons; /* circular */
			break;
		case KEY_LEFT:
			prevopt = curropt;
			if( !curropt ) curropt = nbuttons - 1;
			else --curropt;
			break;
		case '\t': ungetch( KEY_RIGHT ); break;
		case '\n': selected = true; break;
		}
	}

	/* delete windows */
	delwin( dlgwn ); delwin( msgwn );
	for( int i = 0; i < nbuttons; ++i ) delwin( btnwn[i] );

	/* remove the deleted windows from display */
	touchwin( edit ); /* touch the backwindow on which dlgbox was put */
	wrefresh( edit ); /* refresh the touched window */

	/* check and return correct values */
	if( curropt == -1 ) return CANCEL; /* ESC was pressed */
	else return signal[ curropt ];
}

/* getUsrInput,show window,take input string.
 * if( no-word or ESC ) return false, else return true */
bool getUsrInput( const string msg, string &inputStr ) {
	/* create window and set their properties */
	WINDOW *outwn = newwin( 5, 50, 7, 13 ),
	       *inwn  = newwin( 1, 48, 9, 14 );

	wbkgd( outwn, COLOR_PAIR( DLGBOXCOLOR ) );
	wbkgd( inwn,  COLOR_PAIR( DLGBOXCOLOR )| A_REVERSE );

	box( outwn, 0, 0 );
	wattron( outwn, A_BOLD );
	mvwprintw( outwn, 1, 1, "%s", msg.c_str() ); /* in BOLD */
	wattroff( outwn, A_BOLD );
	wrefresh( outwn ); wrefresh( inwn );

	keypad( inwn, TRUE ); /* get keyboard access to inwn */

	/* control variables */
	int lftMar = 0, rhtMar = 47;
	int curdx = 0, dispFrm = 0, cursx = lftMar;
	int cursor = 1; /* cursor style */
	chtype ch;    /* keyboard input */
	string tmpStr = inputStr; /* temporary input string */
	bool done = false, cancel = false;

	int prevCursorState = curs_set( cursor ); /* save the previous cursor state */
	while( !done && !cancel ) {
		/* display the string in window */
		wmove( inwn, 0, lftMar );
		wclrtobot( inwn ); /* clear inwn window */
		for( int i = lftMar,j = dispFrm;i <= rhtMar && j < tmpStr.length();++i,++j ) {
			if( tmpStr[j] == '\t' )	mvwaddch( inwn, 0, i, 187 ); /* special symbol */
			else mvwaddch( inwn, 0, i, tmpStr[j] );
		}
		wrefresh( inwn );
		curs_set( cursor ); /* reflect the cursor changes made, if any */
		/* move to correct position */
		wmove( inwn, 0, cursx );
		ch = wgetch( inwn );
		switch( ch ) {
		case KEY_LEFT:
			if( curdx == 0 ) beep();
			else {
				 --curdx;
				if( cursx == lftMar ) --dispFrm;
				else --cursx;
			}
			break;
		case KEY_RIGHT:
			if( curdx >= tmpStr.length() ) beep();
			else {
				++curdx;
				if( cursx == rhtMar ) ++dispFrm;
				else ++cursx;
			}
			break;
		case KEY_HOME:	dispFrm = 0; curdx = 0; cursx = lftMar; break;
		case KEY_END:
			if( tmpStr.length() > lftMar + rhtMar ) {
				/* scroll and go to end of current line */
				dispFrm = ( tmpStr.length() - ( lftMar + rhtMar ) );
				curdx = tmpStr.length();
				cursx = rhtMar;
			}
			else { /* go to end of current line */
				curdx = tmpStr.length();
				cursx = lftMar + curdx;
			}
			break;
		case KEY_IC: /* toggle cursor style */
			if( cursor == 1 ) cursor = 2; else cursor = 1;
			break;
		case KEY_DC: /* delete current character */
			if( curdx == tmpStr.length() ) beep();
			else {
				tmpStr.erase( curdx, 1 );
			}
			break;
		case KEY_BACKSPACE:
			if( curdx == 0 ) beep();
			else {	/* delete charcater behind */
				--curdx; tmpStr.erase( curdx, 1 );
				/* adjust cursor position */
				if( dispFrm == 0 ) { /* you have to move back one position */
    				--cursx;
				}
				--dispFrm; /* scroll the characters to display full line */
				if( dispFrm < 0 ) { /* avoid it */ dispFrm = 0; }
			}
			break;
		case 27:	/* input was cancelled */
			tmpStr = ""; /* truncate the input string */
			cancel = true;
			break;
		case '\n':  /* end of string input */
			done = true;
			break;
		default:
			/* add only printable charcater and tab into string */
			if( isprint( ch ) || ch == '\t' ) {
				if( cursor == 1 ) tmpStr.insert( curdx, 1, ch );
				else { /* replace = insert then erase */
				    //tmpStr.replace( curdx, 1, c ); 
					tmpStr.insert( curdx, 1, ch );
					tmpStr.erase( curdx+1, 1 );
				}
				/* modify the control variables */
				++curdx; ++cursx;
				if( cursx > rhtMar ) { ++dispFrm; cursx = rhtMar; }
			}
			break;
		}
	}

	/* delete the windows */
	delwin( inwn ); delwin( outwn );

	/* clear the display of previous windows */
	touchwin( edit ); wrefresh( edit );
	curs_set( prevCursorState ); /* set previous cursor state */

	if( cancel )    return false; /* don't disturb previous inputStr value */
	else if( done ) { /* assign new string to inputStr */
		inputStr = tmpStr;
		return true;
	}
}

