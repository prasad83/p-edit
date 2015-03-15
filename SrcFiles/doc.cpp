/* Source file */
/* Handle the document editing */

#include "doc.h"
#define DEFAULT_SHOWSPECIAL_VALUE false
/* Linux 9's terminal has problem to display special characters */

CDocument :: CDocument( int lm, int rm, int tm, int bm, int tab ):
                 CView( lm, rm, tm, bm, tab ) /* send to Cview() */
{
	ShowSpecial = DEFAULT_SHOWSPECIAL_VALUE;
	/* don't put a different symbol for '\t' and space in editor */
	cursorStyle = 1;    /* type of cursor to be displayed normal, no or insert */
	refreshEdit = true; /* content of full display is to be refreshed now      */

	fullFlName = "./noname"; /* file name with full path */
	curFlName = "noname";    /* get only name part */
	flChange = false; /* file content was not changed */
	diskFile = false; /* opened file does not exist on disk currently */
	backUp   = 0; /* number of back up's made */

	clipBoard.clear();
	clipFlag = false; /* no valid contents in clipBoard */
	clipAct  = true;  /* nothing selected, clipAct is assumed true */
	sdx = 0; sdy = 0; edx = 0; edy = 0;

	fndword = "";	rplcword = "";

	while( !unob.empty() )     unob.pop();
	while( !unBlckob.empty() ) unBlckob.pop();
}

bool CDocument :: ReadFile( const char *fname ) {
	ifstream infile( fname );
 	if( !infile ) return false; /* file could not be opened */

	buffer.erase( buffer.begin(), buffer.end() );
	lines = 0;

 	string str = "";
	char   ch = infile.get();
	while( infile ) {
		switch( ch ) {
		case '\n':
			str += ch; /* insert the '\n' into the line */
			buffer.insert( buffer.end(), str );
			++lines;
			str = "";
			break;

		case EOF:
			/* check for zero length file having no data in it except EOF */
			if( str == "" && lines == 0 ) {
				/* Beware, to work with kind of file insert '\n' into buffer
				 * which make buffer to contain at least one character in it
				 * Otherwise, the editor may not work properly.
				 */
				str = "\n";
			}

			break; /* exceptional care not to insert EOF to buffer */
		default:
			str += ch; /* insert characters into line */
			break;
		}
		ch = infile.get();
	}
	if( str != "" ) { /* '\n' not found but EOF was encountered */
		buffer.insert( buffer.end(), str ); /* put the last line without any '\n' */
		++lines;
	}
	buffer.insert( buffer.end(), "" ); /* complete the last line */
	infile.close();

	return true;
}

/* insert character at current cursor position data_x */
void CDocument :: insChar( char inch, bool undoCall ) {
	/* single character changes here, ch to be pushed on to undo-stack */
	char ch = buffer[data_y][data_x];

	if( buffer[data_y] == "" ) {
		/* No data in current line, so ch will be the first character */
		buffer[data_y] = inch;
	}
	/* now, insert character before '\n' of a line now after it */
	else if( data_x < buffer[data_y].length() ) {
		/* insert the character at data_x */
		buffer[data_y].insert( data_x, 1, inch );
	}

	if( screen_x != RM ) { /* the change is only in this page */
		/* current data line has changed, so display the line */
		DisplayLine( data_y, screen_y );
	}

	/* move to right as new character is inserted */
	mvright();

	/* document data changes */
	flChange = true;
	clipAct = true; /* don't allow cut operation as document changed */

	/* create UndoInfo object, with current cursor position and other information,
	 * push it to undo stack if change is made to data */
	UndoInfo uob( INSCHAR, ch, /* the previous actual character */
                   topline, topchar, data_y, data_x, screen_y, screen_x );

	/* push undo-object to undo-stack since changes were made */
	if( !undoCall ) unob.push( uob ); /* push if function is in not in undoCall mode */
}

/* replace character at current cursor position data_x */
void CDocument :: rplcChar( char inch, bool undoCall ) {
	/* single character changes here, ch to be pushed on to undo-stack */
	char ch = buffer[data_y][data_x];

	/* check if you are on '\n' character */
	if( buffer[data_y][data_x] == '\n' ) {
		/* Beware! Don't replace '\n' directly as it affects next data line
		 * and working of the editor */
		buffer[data_y].insert( data_x, 1, inch );
	}
	else {
		/* at other position insert first ch and
		 * delete the character that was at position data_x
		 */
		buffer[data_y].insert( data_x, 1, inch );/* inch inserted, char at data_x moves */
		buffer[data_y].erase( data_x + 1, 1 );/* delete char on which cursor was present*/
	}

	if( screen_x != RM ) { /* the change is only in this page */
		/* current data line has changed, so display the line */
		DisplayLine( data_y, screen_y );
	}

	/* move to right as new character is inserted */
	mvright();

	/* document data changes */
	flChange = true;
	clipAct = true; /* don't allow cut operation as document changed */

	/* create UndoInfo object, with current cursor position and other information,
	 * push it to undo stack if change is made to data */
	UndoInfo uob( RPLCCHAR, ch, /* the previous actual character */
                   topline, topchar, data_y, data_x, screen_y, screen_x );

	/* push undo-object to undo-stack since changes were made */
	if( !undoCall ) unob.push( uob ); /* push if function is in not in undoCall mode */
}

/* delete the character at current cursor position */
void CDocument :: delChar( bool undoCall ) {
 	/* single character changes here, ch to be pushed on to undo-stack */
	char ch = buffer[data_y][data_x];

	/* check if file has only line with only '\n' inside it */
	if( data_x == 0 && buffer[data_y][data_x] == '\n' && data_y == buffer.size() - 2 ) {
		/* Beware, Don't delete this '\n' as it won't let the editor to work properly */
		return;
	}

	if( buffer[data_y][data_x] != '\n' ) {
		/* erase the character from line which is other than '\n' */
		buffer[data_y].erase( data_x, 1 );

		/* only current line display will be affected and cursor will be at same position */
		DisplayLine( data_y, screen_y );
	}
	else { /* you are now deleting '\n' */
		/* check if you are in last line of buffer */
		if( data_y == buffer.size() - 2 ) {
			/* at this position you should not delete '\n' */
			return;
		}
		/* you are not in last line, so delete '\n' of current line and attach next line
		 * of buffer to current line and erase the attached line from buffer.
		 */
		buffer[data_y].erase( data_x, 1 ); /* '\n' deleted from line */
		buffer[data_y] += buffer[data_y + 1]; /* attach next line    */

		/* delete the attached line from buffer data_y + 1 */
		buffer.erase( buffer.begin() + data_y + 1, buffer.begin() + data_y + 2 );
		/*     erase( from which vector line, to which vector line ) */

		/* signal re-display as contents was changed */
		refreshEdit = true;
	}

	/* document data changes */
	flChange = true;
	clipAct = true; /* don't allow cut operation as document changed */

	/* create UndoInfo object, with current cursor position and other information,
	 * push it to undo stack if change is made to data */
	UndoInfo uob( DELCHAR, ch, /* the previous actual character */
                   topline, topchar, data_y, data_x, screen_y, screen_x );

	/* push undo-object to undo-stack since changes were made */
	if( !undoCall ) unob.push( uob ); /* push if function is in not in undoCall mode */
}

/* delete the character to left of current cursor position */
void CDocument :: backSpace( bool undoCall ) {
	/* single character changes here, ch to be pushed on to undo-stack */
	char ch; /* set the value when decided below */

	/* if you are in first line, first position simpy return */
	if( data_x == 0 && data_y == 0 ) { /* no action to do */
		return;
	}
	/* if you are at first position on any other line,
	 * move to end position of previous line and delete the '\n'
	 */
	if( data_x == 0 ) {
		/* set the character which will be deleted */
		ch = '\n'; /* previous lines '\n' will be deleted */

		mvup(); mvend(); delChar( true );
 		/* NOTE: set delChar( undoCall = true ) because, backSpace() should push undo
		 * information on stack and not delChar(). */
	}
	else {
		/* set the character which will be deleted */
		ch = buffer[data_y][data_x - 1]; /* previous character will be deleted */

	   /* in any other position, move to left one character and delete that character */
		mvleft();	delChar( true );
 		/* NOTE: set delChar( undoCall = true ) because, backSpace() should push undo
		 * information on stack and not delChar(). */
	}
	/* signal re-display as contents was changed */
	refreshEdit = true;

	/* document data changes */
	flChange = true;
	clipAct = true; /* don't allow cut operation as document changed */

	/* create UndoInfo object, with current cursor position and other information,
	 * push it to undo stack if change is made to data */
	UndoInfo uob( BACKSPACE, ch, /* the previous actual character */
                   topline, topchar, data_y, data_x, screen_y, screen_x );

	/* push undo-object to undo-stack since changes were made */
	if( !undoCall ) unob.push( uob ); /* push if function is in not in undoCall mode */
}

void CDocument :: keyEnter( bool undoCall ) {
	/* '\n' is at the current cursor place, ch to be pushed on to undo-stack */
	char ch = buffer[data_y][data_x];

	/* if cursor is on '\n', start a new line below with '\n' in it */
	if( buffer[data_y][data_x] == '\n' ) {
		buffer.insert( buffer.begin() + data_y + 1, "\n" );
	}
	else {
		/* cursor is at start or middle of line, insert '\n' at current position,
		 * copy the data right to cursor till end to a new line below and
		 * delete all copied characters from current line
		 */
		buffer[data_y].insert( data_x, 1, '\n' );

		string remStr = "";
		remStr.assign( buffer[data_y], data_x + 1, buffer[data_y].length() - 1 - data_x );
		/* string &assign( const string str, size_type index, size_type len ); */

		/* erase the copied characters from line data_y */
		buffer[data_y].erase( data_x + 1, buffer[data_y].length() - 1 - data_x );
		/* string &erase( size_type index, size_type len ); */

		/* create a new line in buffer below current line data_y */
		buffer.insert( buffer.begin() + data_y + 1, remStr );
	}

	/* display buffer contents has changed so signal re-display */
	refreshEdit = true;

	/* position the cursor */
	/* move to next line down and to the home position */
	mvdown(); mvhome();

	/* document data changes */
	flChange = true;
	clipAct = true; /* don't allow cut operation as document changed */

	/* create UndoInfo object, with current cursor position and other information,
	 * push it to undo stack if change is made to data */
	UndoInfo uob( KEYENTER, ch, /* the previous actual character */
                   topline, topchar, data_y, data_x, screen_y, screen_x );

	/* push undo-object to undo-stack since changes were made */
	if( !undoCall ) unob.push( uob ); /* push if function is in not in undoCall mode */

}

/* delete the characters from current cursor position till end of word */
void CDocument :: delWord( bool undoCall ) {
	/* Assumption: 'word' is made up of alphabets/numerals/alpha-numerals.
	 * So delete from current position till you encounter a non alpha-numeric character
	 * You Will definitely encounter at least one non-alpha numeric character at current
	 * postion that is '\n' that will be present at end of every line.
	 */
	/* create a string to note the deleted word helps for undo */
	string word = "";
	while( isalnum( buffer[data_y][data_x] ) ) {
		/* append the character to be deleted to word */
		word += buffer[data_y][data_x];

		buffer[data_y].erase( data_x, 1 ); /* delete the character at data_x */
		/* no need to increment or decrement data_x because deleting character at
		 * data_x will make character at data_x + 1 to move to data_x
		 */
		/* This loop will end finally, because of atleast '\n' encounter */
	}

	/* only current line contents are changed, so display the line */
	DisplayLine( data_y, screen_y );

	/* document data changes */
	flChange = true;
	clipAct = true; /* don't allow cut operation as document changed */

	/* create UndoInfo object, with current cursor position and other information,
	 * push it to undo stack if change is made to data */
	UndoInfo uob( DELWORD, word, /* the deleted string */
                   topline, topchar, data_y, data_x, screen_y, screen_x );

	/* push undo-object to undo-stack since changes were made */
	if( !undoCall ) unob.push( uob ); /* push if function is in not in undoCall mode */

}

/* delete the whole line on which cursor is present */
void CDocument :: delLine( bool undoCall ) {
	/* check whether last line is deleted  helps for undo delLine */
	bool lastln = false;
	if( data_y == buffer.size() - 2 ) { /* action will be deleting last buffer line */
		lastln = true;
	}

	/* create a string to note the deleted line contents helps for undo */
	string ln = "";

	/* check if the line has only '\n' at the beginning position */
	if ( buffer[data_y][0] == '\n' ) {
		ln = "\n"; /* note current line has only '\n' in it */

		/* let delChar() manage the deletion of this '\n' */
		delChar( true );
 		/* NOTE: set delChar( undoCall = true ) because, delLine() should push undo
		 * information on stack and not delChar(). */
	}
	else {
		/* current line has characters in it, copy the line */
		ln = buffer[data_y];

		/* line has characters other than '\n', so from beginning of line delete all
		 * characters till '\n' and later let delChar() handle deletion of left out '\n'
		 */
		mvhome(); /* go to beginning of line */
		buffer[data_y].erase( data_x, buffer[data_y].length()-1); /* delete except '\n' */

		delChar( true );
 		/* NOTE: set delChar( undoCall = true ) because, delLine() should push undo
		 * information on stack and not delChar(). */
	}
	/* signal re-display as contents was changed */
	refreshEdit = true;

	/* document data changes */
	flChange = true;
	clipAct = true; /* don't allow cut operation as document changed */

	/* create UndoInfo object, with current cursor position and other information,
	 * push it to undo stack if change is made to data */
	UndoInfo uob( DELLINE,ln, /* the deleted line */
                  topline, topchar, data_y, data_x, screen_y, screen_x,
				  lastln );   /* inform which line is deleted */

	/* push undo-object to undo-stack since changes were made */
	if( !undoCall ) unob.push( uob ); /* push if function is in not in undoCall mode */
}

/* insert the deleted word at current cursor position helps to undo delWord */
void CDocument :: insDelWord( string word, bool undoCall ) {
	buffer[data_y].insert( data_x, word );

	/* only current line contents are changed, so display the line */
	DisplayLine( data_y, screen_y );

	/* document data changes */
	flChange = true;
	clipAct = true; /* don't allow cut operation as document changed */

	/* create UndoInfo object, with current cursor position and other information,
	 * push it to undo stack if change is made to data */
	UndoInfo uob( INSDELWORD, word, /* the added string */
                   topline, topchar, data_y, data_x, screen_y, screen_x );

	/* push undo-object to undo-stack since changes were made */
	if( !undoCall ) unob.push( uob ); /* push if function is in not in undoCall mode */
}

/* insert the deleted line at next line of cursor position helps to undo delLine */
void CDocument :: insDelLine( string ln, bool lastln, bool undoCall ) {
	if( lastln ) {
		/* the deleted line was last buffer line, so '\n' was not deleted
		 * you now have to re-assign the buffer line data_y with ln will have '\n' */
		 buffer[data_y] = ln;
	}
	else
	  buffer.insert( buffer.begin() + data_y, ln );

	/* signal re-display as contents was changed */
	refreshEdit = true;

	/* document data changes */
	flChange = true;
	clipAct = true; /* don't allow cut operation as document changed */

	/* create UndoInfo object, with current cursor position and other information,
	 * push it to undo stack if change is made to data */
	UndoInfo uob( INSDELLINE,ln, /* the added line */
                   topline, topchar, data_y, data_x, screen_y, screen_x );

	/* push undo-object to undo-stack since changes were made */
	if( !undoCall ) unob.push( uob ); /* push if function is in not in undoCall mode */
}


/* Function to find word from given position till given position or entire buffer
 * and set cursor if found. */
bool CDocument :: FindWord( int frmdy, int frmdx, int tody, int todx ) {
	/* if tody = -1, search proceeds from
	 * (fromdy,fromdx) to (bufferend, *) from (0,0) to (tody,todx).
	 */
	/* if tody != -1, search from (frmdy,frmd) till (tody,todx) */
	int cdx = frmdx, cdy = frmdy;
	bool found = false;

	if( tody != -1 ) { /* valid stop index given */
		while( !found && cdy <= tody ) {
			cdx = buffer[cdy].find( fndword, cdx );
			if( cdx != string :: npos ) {
				found = true;
				if( cdy == tody && cdx > todx ) /* not expected */
					found = false;
			}
			else {	cdx = 0;	 ++cdy; /* next line */	}
		}
	}
	else {
		/* search from frmdy to buffer end */
		while( !found && cdy <= buffer.size()-2 ) {
			cdx = buffer[cdy].find( fndword, cdx );
			if( cdx != string :: npos )
				found = true;
			else {	cdx = 0;	++cdy; 	/* next line */	}
		}
		/* search, now, from 0 to (frmdy,frmdx) if not yet found */
		if( !found ) { /* re-initialize if not yet found */
			cdx = 0; cdy = 0;
		}

		while( !found && cdy <= frmdy ) {
			cdx = buffer[cdy].find( fndword, cdx );
			if( cdx != string :: npos ) {
				found = true;
				if( cdy == frmdy && cdx > frmdx ) /* not expected */
					found = false;

			}
			else {	cdx = 0; ++cdy; /* next line */	}
		}
	}
	if( found ) { /* cdx and cdy has position values */
		mvtopos( cdy, cdx );
	}
	return found;
}

/* set cursor to position (dy,dx) w.r.t data from current display topline */
void CDocument :: mvtopos( int dy, int dx ) {
	int ptopline = topline, ptopchar = topchar; /* previous values */

	/* move to home position in current line */
	mvhome();

	/* check how to move from current data_y (up, down) */
	if( data_y < dy ) { /* you have to move down */
		while( data_y < dy ) mvdown(); /* stop when data_y == dy */
	}
	else if( data_y > dy ) { /* you have to move up */
		while( data_y > dy ) mvup();   /* stop when data_y == dy */
	}
	/* else your at dy line */

	/* now, go to (dy,dx) position */
	while( data_x < dx ) mvright();    /* stop when data_x == dx */

	/* turn on refreshing when needed else turn off */
	if( topline != ptopline || topchar != ptopchar ) refreshEdit = true;
	else refreshEdit = false; /* the position is in same page */
}

/* FindFirst, get a new word to find in editor */
/* directCall parameter decides whether call to function is directly
 * or indirectly, say by Replace(), and display dialog box accordingly */
bool CDocument :: FindFirst( bool directCall ) {
	if( !getUsrInput( "Enter the string to find...",fndword ) )
		return false; /* word not specified */

	bool wrdFound = false;

	/* search from current position to buffer end */
	if(FindWord(data_y, data_x ,buffer.size() - 2, buffer[buffer.size()-2].length()))
		wrdFound = true;

	else { /* search hit bottom, get user response */
		int reply;
		if( !directCall ) /* search from top to again is compulsory */
			reply = YES;
		else
			reply = getUsrResponse( "Search hit Bottom! Continue from Top ?", YES_NO );

		if( reply == YES ) wrdFound = FindWord( 0, 0, data_y, data_x );
		else wrdFound = false; /* search cancelled */
	}
	/* if word not found, tell to user */
	if( !wrdFound ) {
		string msg = "String \"" + fndword +"\" Not Found.";
		getUsrResponse( msg, OK );
	}
	return wrdFound; /* return wordFound status */
}

/* FindAgain the word fndword from current position, if fndword call FindFirst */
/* directCall parameter decides whether call to function is directly
 * or indirectly, say by Replace(), and display dialog box accordingly */
bool CDocument :: FindAgain( bool directCall ) {
	if( fndword == "" ) return FindFirst( directCall ); /* pass default parameter */

	/* word already present, use it */
	/* search from (current_pos + 1 ) to buffer end */
	if( FindWord( data_y, data_x+1, buffer.size()-2, buffer[buffer.size()-2].length()-1))
		return true;

	else { /* search hit bottom, get user response */
		int reply;
		if( !directCall ) /* search from top to again is compulsory */
			reply = YES;
		else
			reply = getUsrResponse( "Search hit Bottom! Continue from Top ?", YES_NO );

		if( reply == NO )	return false;

		return FindWord( 0, 0, data_y, data_x );
	}
}

/* Replace, will replace the word specified to be replaced with another word */
void CDocument :: Replace() {
	/* first get the word to rplcword search it */
	if( !FindFirst( false ) ) return; /* word to replace not found */
	/* false means FindFirst in called indirect from Replace() */

	/* may be word is in next page */
	mvtopos( data_y, data_x );
	if( refreshEdit ) DisplayFile( topchar,topline );
	else DisplayLine( data_y, screen_y );

	/* get replacing word by providing window */
	if( !getUsrInput( "Enter the replacing string...", rplcword ) )
		return; /* replacing word not given */

	/* here, you found word to replace and replacing word */
	/* get reply, before replace */

	string msg = "Replace \"" + fndword + "\" with \"" + rplcword + "\" ?";

	int reply = getUsrResponse( msg, YES_NO_YESTOALL_CANCEL );

	while( reply != CANCEL ) {
		if( reply == YES ) {
			/* replace current word found */
			ReplaceWord();
			DisplayLine( data_y, screen_y ); /* line changes */
		}
		else if( reply == YESTOALL ) {
			int strt_dy = data_y, strt_dx = data_x; /* note start position of replace */

			/* now, you have to replace all word from top to bottom */
			/* go to start of buffer */
			mvtopos( 0, 0 );
			while( FindWord( data_y, data_x,
			       buffer.size() - 2, buffer[buffer.size()-2].length()) ) {
				/* replace word */
				ReplaceWord();
				/* skip the replacing word */
				data_x += rplcword.length();
			}
			/* you had skipped the length inside the loop so adjust it */
			data_x -= rplcword.length();

			/* go to (strt_dy,strt_dx) position */
			mvtopos( strt_dy, strt_dx );

			/* update screen after all replaces */
			DisplayFile( topline, topchar );

			/* return from Replace() */
			return; /* job done */

		}
		/* find word again, if found continue to take reply */
		if( FindWord( data_y, data_x + rplcword.length() ) ) {
			if( refreshEdit ) DisplayFile( topline, topchar );
			reply = getUsrResponse( msg, YES_NO_YESTOALL_CANCEL );
		}
		else { /* failed to find word fndword */
			break;
		}
	}
}

/* ReplaceWord, replaces fndword with rplcword,
 * pushes suitable information onto undo stack */
void CDocument :: ReplaceWord( bool undoCall ) {
	/* replace word */
	buffer[data_y].replace( data_x, fndword.length(), rplcword );

	/* document data changes */
	flChange = true;
	clipAct = true; /* don't allow cut operation as document changed */

	/* create UndoInfo object, with current cursor position and other information,
	 * push it to undo stack if change is made to data */
	UndoInfo uob( REPLACEWORD, fndword, /* the word replaced */
                  topline, topchar, data_y, data_x, screen_y, screen_x, false,
				  rplcword /* the replacing word */  );

	/* push undo-object to undo-stack since changes were made */
	if( !undoCall ) unob.push( uob ); /* push if function is in not in undoCall mode */
}

/* unReplaceWord, replaces rplcword with fndword used to undo ReplaceWord,
 * pushes suitable information onto undo stack */
void CDocument :: unReplaceWord( string fdword, string rpword, bool undoCall ) {
	/* replace word */
	buffer[data_y].replace( data_x, rpword.length(), fdword );

	/* document data changes */
	flChange = true;
	clipAct = true; /* don't allow cut operation as document changed */

	/* create UndoInfo object, with current cursor position and other information,
	 * push it to undo stack if change is made to data */
	UndoInfo uob( UNRPLCWORD, fdword, /* the word replaced */
                  topline, topchar, data_y, data_x, screen_y, screen_x, false,
				  rpword /* the replacing word */  );

	/* push undo-object to undo-stack since changes were made */
	if( !undoCall ) unob.push( uob ); /* push if function is in not in undoCall mode */
}

bool CDocument :: copyHandler() {
	sdx = data_x; sdy = data_y; /* start of copy variables */

	chtype ch;
	bool copyEnd = false, cancel = false;
	while( !copyEnd && !cancel ) {
		wmove( status, 0, 0 ); wclrtoeol( status );
		wattron( status, A_BOLD ); /* highlight the change */
		mvwprintw(status,0,0, "Start Copy: Line:%-4d Col:%-4d", sdy + 1, sdx + 1 );
		mvwprintw(status,0,42,"Now At: Line:%-4d Col:%-4d", data_y + 1, data_x + 1);
		mvwprintw(status,0,68,"F3:End Copy" );
		wattroff( status, A_BOLD );
		wrefresh( status );

		wmove( edit, screen_y, screen_x ); wrefresh( edit );
		switch( ch = wgetch( edit ) ) {
		/* only limited editing operations allowed during copying */
		case KEY_LEFT: 	mvleft();	break;
		case KEY_RIGHT: mvright();	break;
		case KEY_UP:	mvup();		break;
		case KEY_DOWN:	mvdown();	break;
		case KEY_END:   mvend();	break;
		case KEY_HOME:	mvhome();	break;
		case KEY_PPAGE:	pageup();	break;
		case KEY_NPAGE: pagedown();	break;
		case KEY_F(3):	copyEnd = true; break;
		case 27:		cancel = true;	break;
		default: beep();	break;
		}
		if( refreshEdit ) DisplayFile( topline, topchar );
	}

	if( cancel ) { /* copy operation was cancelled */	return false;	}

	if( copyEnd ) { /* valid copy ending */
		edx = data_x; edy = data_y;

		Copy(); /* call copy function to copy data into clipBoard */
		clipFlag = true;	/* valid data present in clipBoard */
		clipAct  = false;   /* the selected portion is not still acted upon */
		return true;
	}
}

void CDocument :: Copy() {
	clipBoard.erase( clipBoard.begin(), clipBoard.end() );
	string str = ""; /* temporary string */
	if( sdy == edy ) {
		if( edx < sdx ) { /* swap these now */
			int tmp = sdx; sdx = edx; edx = tmp;
		}
		/* only one line was selected, put into the clipBoard */
		str.assign( buffer[sdy], sdx, edx - sdx + 1 );
		/* string &assign( const string str, size_type index, size_type len ); */

		/* put the line into the clipBoard */
		clipBoard.insert( clipBoard.end(), str );
		return;
	}

	/* more than one line was copied */
	if( edy < sdy ) { /* Oh! selection was ended above the start point */
		/* swap the variables */
		int tmp = sdy; sdy = edy; edy = tmp;
			tmp = sdx; sdx = edx; edx = tmp;
	}

	/* copy starting line selected data into clipBoard ( it may be partially selected ) */
	str.assign( buffer[sdy], sdx, buffer[sdy].length()-sdx );/* from sdx to end of line */
	clipBoard.insert( clipBoard.end(), str );

	/* copy the full line till the last selected line */
	for( int i = sdy + 1; i < edy; ++i )
		clipBoard.insert( clipBoard.end(), buffer[i] );

	/* copy last selected data into clipBoard ( it may be partially selected ) */
	str.assign( buffer[edy], 0, edx + 1 ); /* from 0 to edx( including ) */
	clipBoard.insert( clipBoard.end(), str );
}

void CDocument :: Paste( bool undoCall ) {
	if( !clipFlag && !undoCall ) {
		getUsrResponse( "Clipboard is empty...", OK );
		return;
	}
	int tsx, tsy, tex, tey; /* useful to create Undo object */
	string remStr = "";     /* remainning string in current line, to be moved */
	int lstCpLnIndx = clipBoard.size() - 1; /* index of last clip line */
	int lstCpLnLen  = clipBoard[lstCpLnIndx].length(); /* length of last clip line */

	if( clipBoard.size() == 1 ) { /* only one line was copied */
		/* set the information to push on stack for undo Paste().For this set
		 * start of selection and end of selection that you now paste. */
		tsx = data_x; tsy = data_y;              /* start of insertion */
		tex = tsx + ( edx - sdx ); tey = data_y; /* end of insertion   */

		if( clipBoard[0][lstCpLnLen - 1] != '\n' ) {
			/* only part of line to be inserted */
			buffer[data_y].insert( data_x, clipBoard[lstCpLnIndx] );
			/* go to end of inserted string from clipBoard */
			mvtopos( data_y, data_x + lstCpLnLen );
		}
		else { /* one clipBoard line has to be inserted */
			remStr.assign( buffer[data_y], data_x, buffer[data_y].length()-data_x );
			buffer[data_y].replace( data_x, remStr.length(), clipBoard[lstCpLnIndx] );

			/* create new line for remStr */
			buffer.insert( buffer.begin() + data_y + 1, remStr );

			/* go to start of remStr */
			mvtopos( data_y + lstCpLnIndx + 1, 0 );
		}
	}
	else { /* more than one line should be inserted from clipBoard */
		/* set the information to push on stack for undo Paste().For this set
		 * start of selection and end of selection that you now paste. */
		tsx = data_x; tsy = data_y;              /* start of insertion */
		tex = edx; tey = tsy + ( edy - sdy );    /* end of insertion   */

		remStr.assign( buffer[data_y], data_x, buffer[data_y].length()-data_x );
		buffer[data_y].replace( data_x, remStr.length(), clipBoard[0] );

		/* create new lines for each clipBoard contents */
		for( int i = 1; i <= lstCpLnIndx; ++i )
			buffer.insert( buffer.begin() + data_y + i, clipBoard[i] );

		/* handle adding the remStr to last clipBoard line added */
		if( clipBoard[lstCpLnIndx][lstCpLnLen-1] != '\n' ) {
			/* attach to the last clipBoard line only */
			buffer[data_y + lstCpLnIndx] += remStr;

			/* go to end of inserted string from clipBoard */
			mvtopos( data_y + lstCpLnIndx, lstCpLnLen );
		}
		else { /* create new line for remStr */
			buffer.insert( buffer.begin() + data_y + lstCpLnIndx + 1, remStr );

			/* go to start of remStr */
			mvtopos( data_y + lstCpLnIndx + 1, 0 );
		}
	}

	/* document data changes */
	flChange = true;
	clipAct = true; /* don't allow cut operation as document changed */

	if( !undoCall ) {
		UndoInfo uob(PASTE,"",topline, topchar, data_y, data_x, screen_y, screen_x);
		UndoBlockInfo ubob( tsx, tsy, tex, tey );
		unob.push( uob );
		unBlckob.push( ubob );
	}

	/* force re-display as contents will be changed */
	refreshEdit = true;
}

void CDocument :: Cut( bool undoCall ) {
	if( clipAct && !undoCall ) { /* selected portion was already operated by paste/cut */
		getUsrResponse( "Document was changed after selection...", OK );
		return;
	}

	/* move to start of selection */
	mvtopos( sdy, sdx );

	int selLen = edx - sdx + 1; /* selected portion length */
	bool lstLnSel = false;/* was last buffer line selected */
	if( edy == buffer.size() - 2 )	lstLnSel = true;

	if( sdy == edy ) { /* only one line/ part of one line was copied */
		if( sdy == buffer.size() - 2 ) {
			/* action being performed on last line */
			if( edx != buffer[edy].length() - 1 ) { /* full last line is not selected */
				lstLnSel = false; /* last line is selected but not fully */
				/* Important with when doing undo Cut() */
			}
			buffer[sdy].erase( sdx, selLen ); /* delete selected */
			/* avoid having no '\n' at last line */
			if( buffer[sdy] == "" || buffer[sdy].find( '\n' ) == string::npos )
				buffer[sdy] += "\n"; /* for safe future editing */
		}
		else { /* line selected is not the last */
			if( buffer[sdy][edx] != '\n' ) {
				/* part of line to be deleted */
				buffer[sdy].erase( sdx, selLen ); /* delete selected */
			}
			else { /* full line to be deleted from selected point */
				buffer[sdy].erase( sdx, selLen ); /* delete selected */
				buffer[sdy] += buffer[sdy + 1]; /* attach next line  */
				/* erase next line attached previously from buffer   */
				buffer.erase( buffer.begin() + sdy + 1, buffer.begin() + sdy + 2 );
  			}
		}
	}
	else { /* more than one line was selected for deletion */

		/* delete the first line */
		buffer[sdy].erase( sdx, buffer[sdy].length() - sdx );

		/* delete all other lines execpt last selected line from buffer */
		buffer.erase( buffer.begin() + sdy + 1, buffer.begin() + sdy + (edy - sdy) );

		if( lstLnSel ) { /* last line of buffer is end of selection */
			/* check if even '\n' of it was also selected */
			if( buffer[sdy + 1][edx] == '\n' ) {
				/* (sdy + 1) is used above because of previous deletion of lines */
				/* delete the last selected line */
				buffer.erase( buffer.begin() + sdy + 1, buffer.begin() + sdy + 2 );
				/* terminate the first selected line that was cut with '\n' */
				buffer[sdy] += '\n';
			}
			else { /* part of last line has to be deleted */
				buffer[sdy + 1].erase( 0, edx+1 ); /* erase selected portion on last line */
				/* sdy + 1 = edy now as intermidiate lines were removed */

				/* attach the remmaining portion to the first selected line that was cut*/
				buffer[sdy] += buffer[sdy + 1]; /* attach the line */

				/* now erase the previously attached line from buffer */
				buffer.erase( buffer.begin() + sdy + 1, buffer.begin() + sdy + 2 );
			}
		}
		else { /* Oh! the last buffer line was not selected */
			/* delete selected contents of last selected line */
			buffer[sdy+1].erase( 0, edx + 1 );

			if( buffer[sdy + 1] != "" ) { /* full line was not selected at last */
				/* attach the remmaining contents to first selected line */
				buffer[sdy] += buffer[sdy + 1];
				/* now erase the previously attached line from buffer */
				buffer.erase( buffer.begin() + sdy + 1, buffer.begin() + sdy + 2 );
			}
			else { /* full selected last line was deleted */
				/* delete the last selected line from buffer */
				buffer.erase( buffer.begin() + sdy + 1, buffer.begin() + sdy + 2 );
				/* attach a line after last selected line to selected first line */
				buffer[sdy] += buffer[sdy + 1];
				/* sdy + 1 = edy + 1,because of previous line deletion action on buffer */

				/* now erase the previously attached line from buffer */
				buffer.erase( buffer.begin() + sdy + 1, buffer.begin() + sdy + 2 );
			}
		}
	}

	/* document data changes */
	flChange = true;
	clipAct = true; /* don't allow cut operation as document changed */

	if( !undoCall ) {
		UndoInfo uob(CUT,"",topline, topchar, data_y, data_x, screen_y, screen_x);
		UndoBlockInfo ubob( sdx, sdy, edx, edy, clipBoard, lstLnSel );
		unob.push( uob );
		unBlckob.push( ubob );
	}

	/* force re-display as contents was changed */
	refreshEdit = true;
}

/* delSelected, Deletes the selected portion of the document */
void CDocument :: delSelected() {
	/* First you have to cut the selected portion */
	Cut(); /* normal cutting */

	/* since selected contents was copied to buffer
	 * indicate that content should not be used for pasting */
	clipFlag = false; /* no valid content for pasting */
}

/* unCut, Handles undoing cut operation or undoing delSelected */
void CDocument :: unCut( bool undoCall ) {
 	UndoBlockInfo ubob = unBlckob.top(); /* get the block operation information done */
	unBlckob.pop(); /* pop the obtained information from stack */

	/* reset the block data variables */
	sdx = ubob.bsx; sdy = ubob.bsy; edx = ubob.bex; edy = ubob.bey;

	clipBoard.clear(); /* clear the clip board contents */
	for( int i =0 ; i < ubob.bclipBuf.size(); ++i ) {
		clipBoard.insert( clipBoard.end(), ubob.bclipBuf[i] ); /* copy old contents */
	}

	clipFlag = true;

	/* call Paste operation as an undoCall */
	Paste( true );

	if( data_y == buffer.size() - 2 && ubob.blstLnSel ) {
		/* last line was selected during block selection and '\n' was
		 * neglected to be deleted on line during Cut(), but was copied
		 * to clipBoard and to stack also, so delete the extra '\n' */
		/* position the cursor to previous line end position */
		mvup();	mvend();
		buffer.erase( buffer.begin() + data_y + 1, buffer.begin() + data_y + 2 );
	}
	mvtopos( sdy, sdx ); /* position the cursor at start of selection */

	/* document data changes */
	flChange = true;
	clipAct = true; /* don't allow cut operation as document changed */

	/* force re-display of contents */
	refreshEdit = true;
}

/* unPaste, Handles undoing paste operation */
void CDocument :: unPaste( bool undoCall ) {
 	UndoBlockInfo ubob = unBlckob.top(); /* get the block operation information done */
	unBlckob.pop(); /* pop the obtained information from stack */

	/* note down the current block data varaibles values */
	int csx = sdx, csy = sdy, cex =edx, cey = edy;

	/* reset the block data variables */
	sdx = ubob.bsx; sdy = ubob.bsy; edx = ubob.bex; edy = ubob.bey;

	/* call Cut as undoCall */
	Cut( true ); /* unPaste job is done */

	/* restore the values */
	sdx = csx; sdy = csy; edx = cex; edy = cey;

	/* document data changes */
	flChange = true;
	clipAct = true; /* don't allow cut operation as document changed */

	/* force re-display of contents */
	refreshEdit = true;
}

void CDocument :: Undo() {
	int ptopchar = topchar, ptopline = topline; /* previous values */

	/* Note: Call any editing function by sending default value to true */
	if( !unob.empty() ) {
		UndoInfo uob = unob.top(); /* take the current value at top of undo stack */
		unob.pop(); /* pop the top element from undo stack */

		/* set correct values of data_x,data_y,screen_x etc., which would have been
		 * changed by key movements. The newly set values creates a environment which
		 * existed after the data changes were actually made.
		 */
		topline = uob.tpln; topchar = uob.tpch; data_y = uob.dy; data_x = uob.dx;
		screen_y = uob.sy; screen_x = uob.sx;

		switch( uob.signal ) {
		case INSCHAR: /* a character was inserted and cursor right */
			/* so, move to left, delete the character */
			mvleft(); delChar( true );
   			break;
		case RPLCCHAR: /* a character was replaced/inserted moved right */
			if( uob.str[0] == '\n' ) {
				/* the character was inserted, not replaced */
				/* so, move to left, delete the character */
				mvleft(); delChar( true );
			}
			else { /* character was replaced and cursor move right */
				/* so, move to left, delete the character and push cursor left back */
				mvleft(); rplcChar( uob.str[0], true ); mvleft();
			}
			break;
		case DELCHAR: /* a character was deleted but cursor was not moved */
			/* so, insert the character and take care to remain at same position */
			if( uob.str[0] == '\n' ) { keyEnter( true ); mvup(); mvend(); }
			else { insChar( uob.str[0], true ); mvleft();}
			break;

		case BACKSPACE:/* a character was deleted and cursor move left/to previous line */
			/* when data_x was equal to 0,cursor moved to previous line,'\n' was deleted*/
			if( uob.str[0] == '\n' ) {
				/* insert '\n' at current cursor position */
				keyEnter( true );
			}
			else {
				/* insert a character, let cursor move right */
				insChar( uob.str[0], true );
			}
			break;
		case KEYENTER :
			/* '\n' was placed at cursor position and cursor move to start on next line
			 * with or without the data in the next line. */
 			backSpace( true );
			break;

		case DELWORD :
			/* a word was deleted, so add the word back */
			insDelWord( uob.str, true );
			break;

		case DELLINE :
			/* a line was deleted, so add the line back */
			insDelLine( uob.str, uob.lastln, true );
			break;

		case REPLACEWORD :
			/* str was found and replaced with str1, so replace the str1 with str back */
			unReplaceWord( uob.str, uob.str1, true );
			break;

		case CUT: /* block operation cut was made */
			unCut( true );
			break;
		case PASTE: /* block operation paste was made */
			unPaste( true );
			break;
		}
		/* any change in the topline or topchar, re-display needed */
		if( topline != ptopline || topchar != ptopchar )
			refreshEdit = true;
		else DisplayLine( data_y, screen_y );
	}
	else { /* undo stack is empty */
		getUsrResponse( "Oh! there is nothing to UNDO now...", OK );
	}
}

void CDocument :: HandleEditorExit(){
	/* check if currently opened file is saved or not */
	if( flChange ) { /* editing file not yet saved */
		int reply = getUsrResponse(
		"The current Document was modifiled.\n Would you like to save it?",YES_NO_CANCEL);
		if( reply == CANCEL ) /* cancel quit */
			return;
 	    if( reply == YES ) /* save the current file and continue */
			SaveFile();
	}

	if( backUp ) { /* if atleast any one backup was made */
		int reply = getUsrResponse("Delete all backup's made in /tmp directory?",YES_NO);
		if( reply == YES )
			system( "rm /tmp/*._prbak 2>/dev/null" ); /* redirect the error */
	}
	nocbreak();
	noraw();
	endwin(); /* end the Extended terminal mode(ETI) or graphics mode */

	system( "clear" ); /* clear the terminal */
	exit(0);  /* exit from the program, don't return back to main */
}

void CDocument :: reInitDocVars() { /* re-initialize the variables */
	/* reset the editor characteristics */
	ShowSpecial = DEFAULT_SHOWSPECIAL_VALUE; 
	/* put a different symbol for '\t' and space in editor */
	cursorStyle = 1;    /* type of cursor to be displayed normal, no or insert */
	refreshEdit = true; /* content of full display is to be refreshed now      */

	/* reset the editor content buffer and other variables */
	buffer.clear();	lines = 0;
	data_x  = 0; data_y = 0; screen_x = LM; screen_y = TM;
	topline = 0; /* starting index of topmargin line in data structure         */
	topchar = 0; /* starting index of firstchar on display topline of topline  */
	backUp   = 0; /* number of back up's made */

	/* reset the file characteristic opened in editor */
	fullFlName = "./noname"; /* file name with full path */
	curFlName = getStrippedName( fullFlName ); /* get only name part */
	flChange = false; /* file content was not changed */
	diskFile = false; /* opened file does not exist on disk currently */

	/* reset the clipBorad content and other variables */
	clipBoard.clear();
	clipFlag = false;
	clipAct  = true;  /* nothing selected, clipAct is assumed true */
	sdx = 0; sdy = 0; edx = 0; edy = 0;

	/* re-initialize fndword and rplcword */
	fndword = "";	rplcword = "";

	/* reset the undo stack and undo blcok stack */
	while( !unob.empty() )     unob.pop();
	while( !unBlckob.empty() ) unBlckob.pop();
}

bool CDocument :: OpenFileSpecified( string ofname ){
	reInitDocVars(); /* re-initialize the document variables */

	/* open the file that is specified by ofname */
	if( !ReadFile( ofname.c_str() ) )
		return false;

	/* file contents was read by ReadFile set other variables */
	fullFlName  = ofname; /* set full file name, including path if any */
	curFlName   = getStrippedName( fullFlName ); /* obtain only file name */
	diskFile    = true;   /* disk file was opened */
 	refreshEdit = true;   /* contents in buffer has to be refreshed */
	return true;
}

string CDocument :: getStrippedName( const string fullName ) {
	string tmpstr = fullName;
	int slshPos = tmpstr.rfind( '/' ); /* find '/' from reverse direction */
	if( slshPos != string :: npos ) { /* '/' was found from back */
		/* strip off all content behind the '/',the remainning is file name */
		tmpstr.erase( 0, slshPos + 1 );
	}
	return tmpstr;
}

bool CDocument :: NewFile() {
	/* check if opened file was changed */
	if( flChange ) { /* Yes */
		int reply = getUsrResponse(
		"The current Document was modifiled.\n Would you like to save it?",YES_NO_CANCEL );

		if( reply == CANCEL ) /* no new file should be opened */
			return false;

		if( reply == YES ) /* save the current file and continue */
			SaveFile();
	}
	/* you now have to create new file */
	reInitDocVars(); /* re-initialize the document variables */
	buffer.insert( buffer.end(), "\n" );/* needed for safe   */
	buffer.insert( buffer.end(), "" );  /* editing in future */
	refreshEdit = true;   /* contents in buffer has to be refreshed */
	return true;
}

bool CDocument :: SaveFile() {
	/* check if the file opened in editor is found on disk */
	if( !diskFile ) { /* Oh! a non-disk file is created on editor */
		return SaveAsFile();
	}
	return SaveToFile();
}

bool CDocument :: SaveToFile( bool fileExist ){
	bool bakDone = false;
	string bakflName = "";

	/* backup only diskfile opened or file present on disk with same fullFlName */
	if( diskFile || fileExist ) {
		/* make a backup file first in /tmp directory */
		bakflName = curFlName + "._prbak";    /* bakflName = curFlName._prtmp */

		/* build command: cp <source_file_with_path> </tmp/tempfilename> */
		string cmd = "cp " + fullFlName + " /tmp/" + bakflName + " 2> /dev/null";
		system( cmd.c_str() );
		++backUp; /* keep count on number of backups */
		bakDone = true; /* flag to show message about backup done */
	}

	/* open the file existing on disk, in output mode */
	ofstream out( fullFlName.c_str() );
	if( !out ) { /* the disk file could not be opened */
		getUsrResponse( "Could not open file for writing...", OK );
		return false;
	}

	/* now, copy the buffer content to file */
	for( int i = 0; i < buffer.size(); ++i )
		out << buffer[i]; /* just copy the buffer contents */
	out.close(); /* close the file */

	diskFile = true; /* now, file exist on disk */
	flChange = false;/* file was just now saved */

	if( bakDone ) { /* if back was done then show message */
		string fbakInfo = "Back up copy made... /tmp/" + bakflName;
		getUsrResponse( fbakInfo, OK );
	}
	/* file name would have changed or (*) so be changed to (=) so display status bar */
	DisplayStatus( data_y, data_x );
	curFlName = getStrippedName( fullFlName ); /* reset to new name of file saved */
	return true;
}

bool CDocument :: SaveAsFile() {
	bool done = false; /* save as is done? */
	bool retVal = false;/* return value    */
	int  reply;        /* use ful to get user response */
	ifstream infl;     /* to check if file exists */

	do {
		/* get the name of the for new file */
		if( !getUsrInput( "Enter the file name to save as...", fullFlName ) ) {
			getUsrResponse( "File name was not specified...\n", OK );
			done = true; /* save as, done as file name was not specified */
			retVal = false; /* file was not saved  */
		}
		else { /* file name was specified by user */
			/* check if the file name specified already exist */
			infl.open( fullFlName.c_str() );
			if( infl ) { /* File already exist, ask overwrite premission */
				/* you now have to overwrite the file */
				infl.close(); /* close the file opened in input mode */

				reply = getUsrResponse(
				"A Document with this name already exist.\n Do you want to overwrite it?",
				YES_NO_CANCEL );
				switch( reply ) {
				case CANCEL:
					done = true; /* save as, was cancelled */
					retVal = false; /* file was not saved  */
					break;
				case YES:
					if( SaveToFile( true ) ) { /* a disk file exist with same name */
						done = true;/* save as, work done */
						retVal = true; /* file was saved  */
					}
					break;
				case NO:
					done = false; /* save as, no yet over */
					retVal = false;
					break;
				}
			}
			else { /* File does not exist, so save it */
				if( SaveToFile() ) {
					done = true;/* save as, work done */
					retVal = true; /* file was saved  */
				}
			}
		}
	} while( !done ); /* loop till save as, is finished */
	return retVal;
}

bool CDocument :: OpenFile() {
	/* check if opened file was changed */
	if( flChange ) { /* Yes */
		int reply = getUsrResponse(
		"The current Document was modifiled.\n Would you like to save it?",YES_NO_CANCEL );

		if( reply == CANCEL ) /* no new file should be opened */
			return false;

		if( reply == YES ) /* save the current file and continue */
			SaveFile();
	}
	/* now present the user with a menu to select a file */
	string selFlName = ""; /* selected file name */

	COpenMenuBox opnMnuob;
	if( opnMnuob.getFileName( selFlName ) ) { /* displays meny and get path + filename */
		/* file was selected from menu */
		OpenFileSpecified( selFlName );
	}
	else { /* no file was selected */
		/* don't make any changes */
	}
	return true;
}

bool CDocument :: CloseFile() {
	/* just open a new black file */
	return NewFile();
}

void CDocument :: linShell() {
   /* end the ETI mode
    * clear the terminal screen using system()
    * put the internal screen buffer contents into a file
    * using scr_dump().
    * provide the help to return back to editor and start shell
    */
    endwin();
    system( "clear" );
    scr_dump("/tmp/_pedit_scr");
    system("echo '\n' TYPE \"exit\" TO RETURN TO THE EDITOR '\n'");
    system("$SHELL"); /* $SHELL - informs about shell that is being used */

   /* after the return push the terminal to ETI mode
    * put the file contents stored using scr_dump() into
    * the internal screen buffer using scr_set().
    */
   initscr();
   scr_set( "/tmp/_pedit_scr" );
   /* delete temp file used for scr_dump
    * after the screen is restored.
    */

   system( "rm -f /tmp/_pedit_scr" );
}

void CDocument :: About() {
	/* show help on status bar */
	wmove( status, 0, 0 ); wclrtoeol( status );
	wattron(   status, A_BOLD );
	mvwprintw( status, 0, 0, "Press 'Esc' or 'Enter' to return back for editing." );
	wattroff(  status, A_BOLD );
	wrefresh(  status );

	/* create a outer window */
	WINDOW *outWn = newwin( 15, 40, 4, 20 ); /* just to show name and box */
	wbkgd( outWn, COLOR_PAIR( DLGBOXCOLOR ));
	box( outWn, 0, 0 );                     /* box    */
	wattron( outWn, A_BOLD );               /* bold   */
	mvwprintw( outWn, 0, 16," About ");     /* name   */
	wattroff( outWn, A_BOLD );              /* nobold */
	wrefresh( outWn );

	/* create a inner window */
	WINDOW *inWn = newwin( 13, 38, 5, 21 ); /* put the actual messages */
	wbkgd( inWn, COLOR_PAIR( DLGBOXCOLOR ));
	keypad( inWn, TRUE );

	wattron( inWn, A_BOLD );
	mvwprintw( inWn, 1, 5, "        Pedit  v3.4           " );  /* bold */
	wattroff( inWn, A_BOLD );
	mvwprintw( inWn, 3, 5, " A very simple editor written " );  /*normal*/
	wattron(inWn, A_BOLD );
	mvwprintw( inWn, 5, 5, "        by Prasad. A          " );  /* bold */
	wattroff( inWn, A_BOLD );
	mvwprintw( inWn, 7, 5, "   as 5th semester project.   " );  /*normal*/
	mvwprintw( inWn, 9, 5, " B.E [CSE], PESIT, Bangalore. " );  /*normal*/
	wattron( inWn, A_BOLD );
	mvwprintw( inWn, 11, 5, "          [ OK ]             " );  /* bold */
	wmove( inWn, 11, 17 );
	wrefresh( inWn );

	chtype ch = wgetch( inWn );
	while( ch != '\n' && ch != 27 ) { /* till Enter or Esc is not pressed */
		ch = wgetch( inWn );
	}

	/* delete the window */
	delwin( outWn ); delwin( inWn );

	/* remove window from display */
	touchwin( edit ); wrefresh( edit );
}
