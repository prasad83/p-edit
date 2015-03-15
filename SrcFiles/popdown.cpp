/* Source File */
#include "popdown.h"

void CPopdownMenu::create(int y, int x,string a[][2],int size,int mnuNumber, string name){
	/* set the values of data members */
	ypos = y;  xpos = x; w_num = mnuNumber; sz = size; mnuName = name;
	mnuretval = -1;
	for( int i = 0; i < sz; i++ ) {	/* store string inputs */
		str[i][0] = a[i][0];   	    /* store menu option names */
		str[i][1] = a[i][1];	    /* description of options  */
	}

	/* create a name window */
	nmwin = newwin( 1, strlen( mnuName.c_str() ), 0, x );

	/* highlight the menu name */
	wcolor_set( nmwin, DLGBOXCOLOR, 0 );/* menu name is displayed */
	wattron( nmwin, A_REVERSE );
	nmpan = new_panel( nmwin );

    mvwprintw( nmwin, 0, 0, "%s", mnuName.c_str() );
}

void CPopdownMenu :: hghtName() {
	top_panel( nmpan );
	update_panels();		/* display backpan back of */
	doupdate();				/* pan.					   */
}

void CPopdownMenu :: nohghtName() {
	hide_panel( nmpan ); update_panels(); doupdate();
}

/* getMenuSelection(): sends the index of menu command choosen
 */
int CPopdownMenu :: getMenuSelection() {
	/* display menu and get index of selection */
	mnuretval = showmenu();

	int retval = mnuretval;
	mnuretval = -1;     /* reset the mnuretval of this object */

	if( retval == -2 || retval == -3 || retval == 27 )
		return retval;	/* KEY_LEFT | KEY_RIGHT | ESC | ALT   */

	switch( w_num ) {	/* commands are selected */
	case 1:	return	retval + FMNUOFFSET;
	case 2:	return	retval + EMNUOFFSET;
	case 3:	return	retval + SMNUOFFSET;
	case 4:	return	retval + OMNUOFFSET;
	case 5:	return	retval + HMNUOFFSET;
	default:	return -1;
	}
}

/* showmenu(): Creates a menu, displays it. Returns value of selection.
 */
int CPopdownMenu :: showmenu( ) {
	ITEM	*menu_item[sz],	*choosen_item;
	WINDOW	*menuwin, *subwin;
	MENU	*menu;

	chtype 	ch;
	int		mnurows = 0, mnucols = 0, choice = 0, retval = 0;
	bool 	run = true, no_choice = FALSE;

	/* create item list to be added to menu */
	for( int i = 0; i < sz; i++ )
		menu_item[i] = new_item( str[i][0].c_str(),
								str[i][1].c_str() );
	menu_item[sz] = NULL;	/* end of menu_item */

	/* create menu using menu items created.
 	 * scale menu to get number of rows and columns
	 */
	menu = new_menu( menu_item );
	scale_menu( menu, &mnurows, &mnucols );

	/* create window based on rows and cols of menu */
	menuwin = newwin( mnurows + 2, mnucols + 2, ypos, xpos );
	box( menuwin, ACS_VLINE, ACS_HLINE );

	/* derive a subwindow */
	subwin 	= derwin( menuwin, mnurows, mnucols, 1, 1 );

	wbkgd( menuwin, COLOR_PAIR( POPMNUCOLOR) );

	set_menu_win	( menu, menuwin );
	set_menu_sub	( menu, subwin );
	set_menu_back	( menu, COLOR_PAIR( POPMNUCOLOR ) );
	set_menu_fore	( menu, WA_REVERSE ); /* highlight the selection */
	set_menu_grey	( menu, COLOR_PAIR( POPMNUCOLOR ) );
	set_menu_mark	( menu, " " );

	/* disable aceess to line displayed in menu */
	menu_opts_off (menu, O_NONCYCLIC);
	for( int i = 0; i < sz; ++i ) {
		/* check for atleast first 2 characters
 		 * which is a line
		 */
		if( strstr(str[i][0].c_str(),"--") )  {
			item_opts_off( menu_item[i], O_SELECTABLE ) ;
		}
	}

	post_menu( menu );	/* show menu */
	wrefresh( menuwin );
	wrefresh( subwin );

	keypad( menuwin, TRUE );

	while( run ) {
		ch = wgetch( menuwin );
		switch (ch) {
		case KEY_UP:
			menu_driver( menu, REQ_UP_ITEM );
			if( item_opts( current_item( menu ) ) != O_SELECTABLE )
				while( item_opts( current_item( menu ) )
					!= O_SELECTABLE )
					menu_driver( menu, REQ_UP_ITEM );
			run = true;		/* take another input */
			break;
		case KEY_DOWN:
			menu_driver( menu, REQ_DOWN_ITEM );
			if( item_opts( current_item( menu )) != O_SELECTABLE )
				while( item_opts( current_item( menu ) )
					!= O_SELECTABLE )
					menu_driver( menu, REQ_DOWN_ITEM );
			run = true;		/* take another input */
			break;
		case '\n':
			run = false ;	/* menu is selected; break the loop */
			break;
		case KEY_LEFT:
			retval = -2;	/* set retval to indicate selection */
			run = false;	/* break the loop */
			no_choice = TRUE;
			break;
		case KEY_RIGHT:
			retval = -3;	/* set retval to indicate selection */
			run = false;	/* break the loop */
			no_choice = TRUE;
			break;
		case 27:			/* ESC or ALT */
			retval = 27;
			run = false;	/* break the loop */
			no_choice = TRUE;
		default: /* neglect all other input */
			break;
		}
	}
	if( no_choice == FALSE ) {
		choosen_item = current_item( menu );
		choice = item_index( choosen_item );
	}
	/* hide the menu from display */
	unpost_menu( menu );

	/* free memory take up for ITEM, MENU, WINDOW */
	for( int i = 0; i < sz; ++i )
		free_item( menu_item[i] );
	free_menu( menu );

	delwin( menuwin );
	delwin( subwin );

	/* hide the panel so that menu is now not highlighted */
	hide();

	/* completely remove menu from view */
	touchwin( edit ); wrefresh( edit );
	/* refresh edit because it is the central main window */

	if( no_choice == FALSE ) return choice;
	else					 return retval;
}

void CPopdownMenu :: hide() {	hide_panel( nmpan ); update_panels(); doupdate(); }

