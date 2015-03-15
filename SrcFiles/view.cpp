/* Source file */
/* Handle the viewing of document display */

#include "view.h"

extern WINDOW *edit;     /* needed to display file */

CView :: CView( int lm, int rm, int tm, int bm, int tab ) {
	LM = lm; RM = rm; TM = tm; BM = bm; TAB = tab;
	ONEVPAGE = (BM - TM);
	topline = 0; topchar = 0;
	data_x = topchar; data_y = topline; screen_x = LM; screen_y = TM;
	curFlName = ""; /* this gets initialized in CDocument functions */
	flChange  = false;
}

void CView :: DisplayStatus( int dy, int dx ) {
	wmove( status, 0, 0 ); wclrtoeol( status );
 	mvwprintw( status, 0, 0, "%s (%c);      F9 - MENU;    ALT_X - EXIT",
		                     curFlName.c_str(), (flChange ? '*':'=') );
	mvwprintw( status, 0, 62, "Line:%-4d Col:%-4d", dy + 1, dx + 1 );
	wrefresh( status );
}

void CView :: DisplayFile( int stline, int offset ) {
	int dy = stline;
	topchar = offset;

	/* clear the whole display */
	wmove( edit,LM,TM );
	wclrtobot( edit );
	wrefresh( edit );

	for( int sy = TM; sy <= BM               /* until full screen      */
	   	 && dy < buffer.size();              /* or buffer is displayed */
	     ++sy, ++dy ) {  /* goto next screen line and next data line   */

		int dx = offset; /* helps in horizontal scrolling */
		DisplayLine( dy, sy );
	}
	wrefresh( edit );
	refreshEdit = false; /* editor was just refreshed */
}

void CView :: DisplayLine( int dy, int sy ) {
	/* clear the line sy */
	wmove( edit, sy, LM ); wclrtoeol( edit ); wrefresh( edit );

	/* create a screen line buffer for display of line */
	char   scrLnBuf[80]; /* display of normal characters */
	chtype scrLnSpl[80]; /* display of special symbols   */

	/* initialize all position of buffers to spaces  */
	for( int i = 0; i < 80; ++i ) {
		scrLnBuf[i] = scrLnSpl[i] = ' '; /* both have same sizes */
	}

	int bufPos = 0; /* the position character in the line buffer  */
	int sx = LM, dx = topchar;       /* start from LM and topchar */
	for(       ; sx <= RM               /* until full column      */
		&& dx < buffer[dy].length();    /* full line is displayed */
	   ){     /* the increment is done inside loop based on '\t'  */

		if( buffer[dy][dx] == '\t') {  /* put symbol (char)187    */
			if( ShowSpecial ) {
				scrLnSpl[bufPos]=187; /* store in scrLnSpl buffer*/
			}
		}
		else if( buffer[dy][dx] == '\n') {  /* put symbol (char)182*/
			if( ShowSpecial ) {
				scrLnSpl[bufPos]=182; /* store in scrLnSpl buffer*/
			}
		}
		else if( buffer[dy][dx] == ' ' ) { /* put symbol (cahr)183 */
			if( ShowSpecial ) {
				scrLnSpl[bufPos]=183;     /* store in scrLnSpl buffer*/
			}
		}
		else
			scrLnBuf[bufPos] = buffer[dy][dx];/* store in line buffer */

		/* adjust next screen position and data position */
		if( buffer[dy][dx] == '\t' ) {
			++sx; ++dx; mvNextTabStop( sx );
			++bufPos; mvNextTabStop( bufPos ); /* move same as on screen */
		}
		else { ++sx; ++dx; ++bufPos; }
	}// end for

	/* terminate the line buffer string */
	scrLnBuf[bufPos] = '\0';

	/* Now, put the actual line on the screen */
	mvwprintw( edit, sy, LM, "%s", scrLnBuf );

	/* display the special characters if special display is needed */
	if( ShowSpecial ) {
		wattron( edit, A_BOLD );
		for( int i = 0; i < 80; ++i ) {
			chtype ch = scrLnSpl[i];
			if( ch == 182 || ch == 183 || ch == 187 ) /* if special symbol */
				mvwprintw( edit, sy, i, "%c", ch );
		}
		wattroff( edit, A_BOLD );
	}

	/* RM if set to 75(DEFAULT_RM) and window width is 80 then still place exists
	 * after RM so, put a mark after RM if the current line spans to next page also */
	if( RM == DEFAULT_RM ) { /* see the definition of DEFAULT_RM in view.h */
		/* check why the for loop stopped */
		if( sx <= RM ) { /* full line was displayed so for loop broke */
			/* clear any previous mark for showing the extension of line */
			mvwprintw( edit, sy, RM + 2, "%c ", ' ' ); /* RM + 2 + "%c  " = RM + 2 + 2 */
			/* don't exceed 79 as it may create unexpected scrolling display */
		}
		else if( dx < buffer[dy].length() ){ /* full line not displayed but RM was encountered */
			/* put the mark for showing the extension of line */
			wattron( edit, A_REVERSE );
			mvwprintw( edit, sy, RM + 2, "%c ", '>' ); /* RM + 2 + "%c  " = RM + 2 + 2 */
			/* don't exceed 79 as it may create unexpected scrolling display */
			wattroff( edit, A_REVERSE );
		}
	}
	wrefresh( edit ); /* update the changes made */
}

int CView :: mvNextTabStop( int &sx ) {
	int psx = sx; /* previous value of sx */
	while( ( sx + 1 ) % TAB ) /* sx + 1 because total screen is (0 to 79) */
		++sx;
	return psx;
}

int CView :: mvPrevTabStop( int &sx ) {
	int psx = sx; /* previous value of sx */
	while( sx % TAB )
		--sx;
	return psx;
}

int CView :: mapScreenPos( int dy, int dx, int &sx ) {
	int psx = sx; /* previous value of sx */

	sx = LM;  /* data is put from left margin */
	for( int i = topchar; /* starting index of display in the line   */
		 i < dx; /* map screen positions till value of dx is reached */
		++i ) {
		++sx;    /* screen position is incremented for each character*/
		if( buffer[dy][i] == '\t' ) mvNextTabStop( sx );
	}
	return psx;
}

void CView :: mvright() {
	int ptopline = topline, ptopchar = topchar; /* previous values */

	if( data_x == buffer[data_y].length() - 1 ) { /* cursor at end of line */
		return; /* no moving right is possible */
	}

	if( data_x < buffer[data_y].length() - 1 && data_y < buffer.size() - 1
		&& screen_x != RM ) { /* move to next character */

		++screen_x;
		++data_x; /* move to next character in data structure   */

		if( buffer[data_y][data_x - 1] == '\t' )
			mvNextTabStop( screen_x );
	}
	else if( screen_x == RM ) { /* screen at right margin, scroll to right page */
		scrollRight();
	}
	/* if topline or topchar changes then re-display needed */
	if( topline != ptopline || topchar != ptopchar )
		refreshEdit = true;
}

void CView :: mvleft() {
	int ptopline = topline, ptopchar = topchar; /* previous values */

	if( data_x > 0 && data_y < buffer.size() - 1
	    && screen_x != LM ) { /* move to previous character */
		--screen_x;
		--data_x; /* move to previous character in data structure   */

		if( buffer[data_y][data_x] == '\t' )
			/* you are now at (tabstop -1 ) position */

			/* now, you have to move to previous tabbed position and
			 * NOT to previous tab stop, as tab may not be previously
			 * placed at tab stop position only.
			 */
			 mapScreenPos( data_y, data_x, screen_x );

	}
	else if( screen_x == LM ) { /* screen at left margin, scroll to left page */
		scrollLeft();
	}
	/* if topline or topchar changes then re-display needed */
	if( topline != ptopline || topchar != ptopchar )
		refreshEdit = true;
}


int CView :: scrollDown() {
	int ptopline = topline; /* previous topline */

	if( data_y < buffer.size() - 2 && topline < buffer.size() - 2 ) {
		/* take index of character on line where cursor is present */
		int prevdx = mapCharPos( data_y, screen_x );

		++topline; /* re-display skip one line at beginning */
		++data_y;  /* go to next line in buffer */

		/* set valid values for data_x and screen_x */
		/* take index of character on line where cursor should be present */
		int curdx = mapCharPos( data_y, screen_x );

		if( curdx != -1 ) { /* valid index is found */
			data_x = curdx; /* set new values */
			screen_x = mvCharPos( data_y, data_x );
		}
		else {
			/* the index curdx is invalid on new line as the line is shorter
			 * so position the cursor at the end of line */

			/* valid setting for first character on line */
			data_x = 0; screen_x = LM; topchar = 0;
			mvend(); /* move to end of line */
		}
	}
	/* any change in the topline, re-display needed */
	if( topline != ptopline )
		refreshEdit = true;
	return ptopline;
}

int CView :: scrollUp() {
	int ptopline = topline; /* previous topline */

	if( data_y > 0 && topline > 0 ) {
		/* take index of character on line where cursor is present */
		int prevdx = mapCharPos( data_y, screen_x );

		--topline; /* re-display skip one line from bottom */
		--data_y; /* go to previous line in buffer */

		/* set valid values for data_x and screen_x */
		/* take index of character on line where cursor should be present */
		int curdx = mapCharPos( data_y, screen_x );

		if( curdx != -1 ) { /* valid index is found */
			data_x = curdx; /* set new values */
			screen_x = mvCharPos( data_y, data_x );
		}
		else {
			/* the index curdx is invalid on new line as the line is shorter
			 * so position the cursor at the end of line */

			/* valid setting for first character on line */
			data_x = 0; screen_x = LM; topchar = 0;
			mvend(); /* move to end of line */
		}
	}
	/* any change in the topline, re-display needed */
	if( topline != ptopline )
		refreshEdit = true;
	return ptopline;
}

int CView :: scrollRight() {
	if( data_x >= buffer[data_y].length() ) {
		/* Oh! now no right scroll is needed */
		return topchar;
	}
	int ptopchar = topchar; /* previous topchar */
	setRightPageIndex();

	/* on new right page */
	screen_x = LM;
	++data_x;

	data_y = data_y;     /* CHANGE ONLY IF YOU WANT TO PUT CURSOR AT NEXT LINE */
	screen_y = screen_y; /* IF NO CHARACTER IS THERE FURTHER ON CURRENT LINE */

	refreshEdit = true; /* re-display the editor content as topchar changed */
	return ptopchar;
}

int CView :: scrollLeft() {
	if( data_x == 0 ) {
		/* Oh! now no scroll is needed */
		return topchar;
	}

	int ptopchar = topchar; /* previous topchar */
	setLeftPageIndex();

	/* on new left page */
	--data_x;
	screen_x = mvCharPos( data_y, data_x );

	data_y = data_y;     /* CHANGE ONLY IF YOU WANT TO PUT CURSOR AT PREVIOUS LINE */
	screen_y = screen_y; /* IF NO CHARACTER IS THERE FURTHER ON CURRENT LINE */

	refreshEdit = true; /* re-display the editor content as topchar changed */
	return ptopchar;
}

int CView :: setRightPageIndex() {
	int ptopchar = topchar; /* previous topchar */

	int dx, sx;
 	for( dx = topchar, /* start from present topchar and find last character index */
         sx = LM ;
		 sx < RM && dx < buffer[data_y].length(); /* till RM or bufferLine end */
	  	 ++dx ) /* sx will be set inside loop */
 	{
		++sx;
		if( buffer[data_y][dx] == '\t' )
			mvNextTabStop( sx );
	}

	/* now, dx points to character on RM. So next page on right should be displayed
	 * from (dx+1) character onwards.
	 */
	if( dx < buffer[data_y].length() - 1 )  /* check if end of line is not reached */
		topchar = dx + 1; /* new topchar */
	return ptopchar;
}

void CView :: mvend() {
	int ptopchar = topchar;

	while( data_x < buffer[data_y].length() - 1 ) {
		if( screen_x == RM )	scrollRight();
		else					mvright();
	}
	if( topchar != ptopchar ) /* topchar has changed so refresh needed */
		refreshEdit = true;
}

void CView :: mvhome() {
	int ptopchar = topchar;

	while( data_x > 0 ) {
		if( screen_x == LM )	scrollLeft();
		else					mvleft();
	}
	if( topchar != ptopchar ) /* topchar has changed so refresh needed */
		refreshEdit = true;
}

/* map the index of character which is present in line dy at screen position sx */
int CView :: mapCharPos( int dy, int sx ) {
	/* move from topchar and LM till sx is reached */
	int isx = LM, dx = topchar;
	for( ; isx < sx; ++dx ) {
		++isx; /* move to next screen position */
		if( buffer[dy][dx] == '\t' )
			mvNextTabStop( isx );
	}
	/* check if dx is valid index in buffer[dy]    */
	if( dx < buffer[dy].length() ) return dx;
	else return -1; /* invalid index as error flag */
}

/* map the screen position for character present in line dy at dx */
int CView :: mvCharPos( int dy, int dx ) {
	/* move from topchar and LM till dx or RM is reached */
	int sx = LM, idx = topchar;

	for( ; sx <= RM && idx < dx; ++idx ) {
		++sx; /* move to next screen position */
		if( buffer[dy][idx] == '\t' )
			mvNextTabStop( sx );
	}
	/* check if idx is valid index in buffer[dy]   */
	if( idx < buffer[dy].length() )	return sx;
	else return -1; /* invalid index as error flag */
}

void CView :: mvdown() {
	int ptopline = topline, ptopchar = topchar; /* previous values */

	if( data_y < buffer.size() - 2 /* if last line is not reached  */
		&& screen_y != BM ) {      /* if cursor is not at BM       */

		/* take index of character on line where cursor is present */
		int prevdx = mapCharPos( data_y, screen_x );

		/* move to line where cursor should be present on screen & buffer */
		++data_y;  ++screen_y;

		/* set valid values for data_x and screen_x */
		/* take index of character on line where cursor should be present */
		int curdx = mapCharPos( data_y, screen_x );

		if( curdx != -1 ) { /* valid index is found */
			data_x = curdx; /* set new values */
			screen_x = mvCharPos( data_y, data_x );
		}
		else {
			/* the index curdx is invalid on new line as the line is shorter
 			 * so position the cursor at the end of line */

			/* valid setting for first character on line */
			data_x = 0; screen_x = LM; topchar = 0;
			mvend(); /* move to end of line */
		}
	}
	else if( screen_y == BM ) { /* cursor is at BM, so scroll down on line */
		scrollDown();
	}

	/* if topline or topchar changes then re-display needed */
	if( topline != ptopline || topchar != ptopchar )
		refreshEdit = true;
}

void CView :: mvup() {
	int ptopline = topline, ptopchar = topchar; /* previous values */

	if( data_y > 0            /* if other than first line */
		&& screen_y != TM ) { /* if cursor is not at TM   */

		/* take index of character on line where cursor is present */
		int prevdx = mapCharPos( data_y, screen_x );

		/* move to line where cursor should be present on screen & buffer */
		--data_y;  --screen_y;

		/* set valid values for data_x and screen_x */
		/* take index of character on line where cursor should be present */
		int curdx = mapCharPos( data_y, screen_x );

		if( curdx != -1 ) { /* valid index is found */
			data_x = curdx; /* set new values */
			screen_x = mvCharPos( data_y, data_x );
		}
		else {
			/* the index curdx is invalid on new line as the line is shorter
			 * so position the cursor at the end of line */

			/* valid setting for first character on line */
			data_x = 0; screen_x = LM; topchar = 0;
			mvend(); /* move to end of line */
		}
	}
	else if( screen_y == TM ) { /* cursor is at TM, so scroll up on line */
		scrollUp();
	}
	/* if topline or topchar changes then re-display needed */
	if( topline != ptopline || topchar != ptopchar )
		refreshEdit = true;
}

/* calculate number of screen lines taken to display "fromline" at TM till "toline" */
int CView :: mapLineDisplay( int fromline, int toline ) {
	int dy = fromline, sy = TM;
	for( ; dy < toline; ++dy ) /* move to next screen line till toline is reached */
		++sy;
	return sy;
}

void CView :: pagedown() {
	int ptopchar = topchar, ptopline = topline; /* previous values */

	/* try to scroll one full page */
	if( topline +  ONEVPAGE + 1 < buffer.size() - 1 ) { /* scroll one vertical page lines */
		/* take index of character on line where cursor is present */
		int prevdx = mapCharPos( data_y, screen_x );

		/* scrolling one page up = skip display lines at top */
		topline = topline + ONEVPAGE + 1;

		/* move to line where cursor should be present on screen & buffer */
		/* see if screen_y's current value is valid in next page to be displayed */
		if( topline + screen_y < buffer.size() - 2 ) {
			/* Good, screen_y value is valid in next page */
			screen_y = screen_y;
		}
		else { /* Sorry!, screen_y is not valid on next page */
			/* calculate new value for screen_y */
			screen_y = mapLineDisplay( topline, buffer.size() - 2 );
		}
		data_y = topline + screen_y; /* data line will change */

		/* set valid values for data_x and screen_x */
		/* take index of character on line where cursor should be present */
		int curdx = mapCharPos( data_y, screen_x );

		if( curdx != -1 ) { /* valid index is found */
			data_x = curdx; /* set new values */
			screen_x = mvCharPos( data_y, data_x );
		}
		else {
			/* the index curdx is invalid on new line as the line is shorter
		 	 * so position the cursor at the end of line */

			/* valid setting for first character on line */
			data_x = 0; screen_x = LM; topchar = 0;
			mvend(); /* move to end of line */
		}
	}
	else { /* Oh! full page could not be scrolled */
		/* take index of character on line where cursor is present */
		int prevdx = mapCharPos( data_y, screen_x );

		/* so go to last line of display */
		topline = topline; /* no change for display topline */

		screen_y = mapLineDisplay( topline, buffer.size() - 2 );
		data_y   = topline + screen_y;  /* data line is now last line */

		/* set valid values for data_x and screen_x */
		/* take index of character on line where cursor should be present */
		int curdx = mapCharPos( data_y, screen_x );

		if( curdx != -1 ) { /* valid index is found */
			data_x = curdx; /* set new values */
			screen_x = mvCharPos( data_y, data_x );
		}
		else {
			/* the index curdx is invalid on new line as the line is shorter
			 * so position the cursor at the end of line */

			/* valid setting for first character on line */
			data_x = 0; screen_x = LM; topchar = 0;
			mvend(); /* move to end of line */
		}
	}
	/* any change in the topline or topchar, re-display needed */
	if( topline != ptopline || topchar != ptopchar )
		refreshEdit = true;
}

void CView :: pageup() {
	int ptopchar = topchar, ptopline = topline; /* previous values */

	/* try to scroll one full page */
	if( topline - ONEVPAGE - 1 >= 0 ) { /* scroll one vertical page lines */
		/* take index of character on line where cursor is present */
		int prevdx = mapCharPos( data_y, screen_x );

		/* scrolling one page up = display previous lines at top */
		topline = topline - ONEVPAGE - 1;

		/* move to line where cursor should be present on screen & buffer */
		screen_y = screen_y; /* no change the to cursor line */
		data_y   = topline + screen_y; /* data line will change */

		/* set valid values for data_x and screen_x */
		/* take index of character on line where cursor should be present */
		int curdx = mapCharPos( data_y, screen_x );

		if( curdx != -1 ) { /* valid index is found */
			data_x = curdx; /* set new values */
			screen_x = mvCharPos( data_y, data_x );
		}
		else {
			/* the index curdx is invalid on new line as the line is shorter
		 	 * so position the cursor at the end of line */

			/* valid setting for first character on line */
			data_x = 0; screen_x = LM; topchar = 0;
			mvend(); /* move to end of line */
		}
	}
	else { /* Oh! full page could not be scrolled */
		/* take index of character on line where cursor is present */
		int prevdx = mapCharPos( data_y, screen_x );

		/* so go to first line of buffer */
		topline = 0;

		/* move to line where cursor should be present on screen & buffer */
		screen_y = TM; /* first line is displayed on TM */
		data_y   = 0;  /* data line is now first line   */

		/* set valid values for data_x and screen_x */
		/* take index of character on line where cursor should be present */
		int curdx = mapCharPos( data_y, screen_x );

		if( curdx != -1 ) { /* valid index is found */
			data_x = curdx; /* set new values */
			screen_x = mvCharPos( data_y, data_x );
		}
		else {
			/* the index curdx is invalid on new line as the line is shorter
			 * so position the cursor at the end of line */

			/* valid setting for first character on line */
			data_x = 0; screen_x = LM; topchar = 0;
			mvend(); /* move to end of line */
		}
	}
	/* any change in the topline or topchar, re-display needed */
	if( topline != ptopline || topchar != ptopchar )
		refreshEdit = true;
}

int CView :: setLeftPageIndex() {
	int ptopchar = topchar; /* previous topchar */

    /* calculate from beginning, the topchar that was for previous page displayed */
	topchar = -1; /* NOTE: the first step in while loop is ++topchar */
	int endCharIndex = 0;

	/* search for previous horizontal page topchar by calculating endchar index on each
	 * page. Stop when endchar + 1 will be equal to the present page topchar=ptopchar.
	 */
	while( endCharIndex + 1 < ptopchar ) {
		++topchar; /* start new page with next topchar      */

		/* calculate the endcharacter index of present page */
		endCharIndex = mapCharPos( data_y, RM );

		/* if endcharacter index is less than current data_x on which cursor is present
		 * then topchar is within first page only.
		 */
		if( endCharIndex > ptopchar ) {
			topchar = 0; screen_x = LM;
			return ptopchar;
		}
	}
	/* check for topchar value for and set if invalid */
	if( topchar == -1 ) {  /* Oh! the while loop was not executed */
		topchar = ptopchar;/* So, previous topchar is retained */
	}
	/* topchar was set inside the while loop */
	return ptopchar;
}

int CView :: gettopline() { return topline; }
int CView :: gettopchar() { return topchar; }
char CView :: getCurChar() { return buffer[data_y][data_x]; }
int CView :: getCurdx() { return data_x; }
int CView :: getCurdy() { return data_y; }
int CView :: getCursx() { return screen_x; }
int CView :: getCursy() { return screen_y; }

bool CView :: getShowSpecial() { return ShowSpecial; }
void CView :: setShowSpecial() { ShowSpecial = !ShowSpecial; /* toggle */ }
bool CView :: getRefreshEdit() { return refreshEdit; }
void CView :: setRefreshEdit() { refreshEdit = true; }
int  CView :: getCursorStyle() { return cursorStyle; }
void CView :: setCursorStyle( int curStyl ) { cursorStyle = curStyl; }

int  CView :: getBM() { return BM; }
int  CView :: getTM() { return TM; }
int  CView :: getLM() { return LM; }
int  CView :: getRM() { return RM; }

void CView :: ScrlDownDisplayFile() {
	/* scroll down one line */
	wscrl( edit, 1 ); /* works only if 'edit' WINDOW is enabled as 'scrollok(edit,TRUE)'*/
	wrefresh( edit );
	DisplayLine( data_y, BM );

	refreshEdit = false; /* editor was just refreshed */
}

void CView :: ScrlUpDisplayFile() {
	/* scroll up one line */
	wscrl( edit, -1 ); /* works only if 'edit' WINDOW is enabled as 'scrollok(edit,TRUE)'*/
	DisplayLine( data_y, TM );
	wrefresh( edit );

	refreshEdit = false; /* editor was just refreshed */
}
