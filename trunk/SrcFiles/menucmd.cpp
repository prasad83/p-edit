/* menucmd.cpp: Handles the pop down menu display and gets the user selection */
#include "macro.h"
#include "global.h"

extern CDocument DocOb;

static CPopdownMenu *pdPtr;     /* pdPtr is pointer to popdown item to display */
static CPopdownMenu *prevpdPtr; /* prevoious pdPtr */
static int curMenuNum;          /* currently on file menu */
static bool dispSelect = false, mnuCancel = false;

void mnuDisplayHandler() {
	int prevCursorStyle = curs_set( 0 ); /* hide the cursor */

	top_panel( namepan );
	top_panel( mnupan );
	update_panels();	doupdate();

	/* help user to select item */
	wmove( status,0,0 ); wclrtoeol(status);
	wattron( status, A_BOLD );
	mvwaddstr( status, 0, 0, "[Left/Right]:ChooseMenu" );
	mvwaddstr( status, 0,24, "[Down/Enter]:ShowMenu" );
	mvwaddstr( status, 0,46, "[Enter]:Item Select" );
	mvwaddstr( status, 0,67, "[Esc]:Cancel." );

	wattroff( status, A_BOLD );
	wrefresh( status );

	wmove( mnubar, 0 , 0 );

	/* initialize values */
	pdPtr = &pdFile; prevpdPtr = pdPtr; curMenuNum = 1;
	dispSelect = false; mnuCancel = false;

	chtype ch;
	/* till display of popdown menu is not requested and menu selection is not cancelled
     * highlight only the strips. */
	while( !dispSelect && !mnuCancel ) {
		/* turn on highlighting current menu strip */
		pdPtr -> hghtName();

		switch( ch =  wgetch( mnubar ) ) {
		case 27: mnuCancel = true; break; /* menu selection is cancelled */
		case KEY_DOWN: ungetch( '\n' );   /* same as pressing enter key  */
		case KEY_LEFT:      case KEY_RIGHT:    /* valid key inputs */
		case 'f': case 'F':	case 'e': case 'E':
		case 's': case 'S':	case 'o': case 'O':
		case 'h': case 'H': case '\n':
			setNextMenu( ch );
			break;
		}
		/* turn off highlighting previous menu strip */
		prevpdPtr -> nohghtName();
	}

	int mnuItemSelected = 0; /* the command that was selected from menu */
	bool cmdSelect = false; /* whether any command is selected */
	if( dispSelect ) {
		/* now, here you have to pull the menu down and take selected value */
		while( !cmdSelect && !mnuCancel ) {
			top_panel( namepan );
			top_panel( mnupan );
			update_panels();	doupdate();

			/* turn off highlighting previous menu strip */
			prevpdPtr -> nohghtName();

			/* turn on highlighting current menu strip */
			pdPtr -> hghtName();

			/* show menu and get the selection */
			mnuItemSelected = pdPtr -> getMenuSelection();

			switch( mnuItemSelected ) {
			case -2: /* left key was pressed when menu was pull down */
				setNextMenu( KEY_LEFT ); break;

			case -3: /* right key was pressed when menu was pull down */
				setNextMenu( KEY_RIGHT ); break;

			case 27: /* menu selection is cancelled */
				mnuCancel = true; break;

			case FNEW...HABOUT: /* command was selected from menu */
				cmdSelect = true;
				break;
			}
		}
	}

    /* hide the menu bar and name bar */
	hide_panel( mnupan );  hide_panel( namepan );

	/* action should now be made of edit window */
	top_panel( editpan );   /* after the menu is hidden */
	curs_set( prevCursorStyle );/* restore cursor state */
	update_panels(); doupdate();

	/* now, take actions selected from menu */
	if( cmdSelect ) {
		CommandHandler( mnuItemSelected );
	}
}

void setNextMenu( chtype ch ) {

	prevpdPtr = pdPtr; /* set prev pdPtr */

	switch( ch ) {
	case KEY_LEFT: /* highlight next menu strip */
		switch( curMenuNum ) {
		case 1: pdPtr = &pdHelp;   curMenuNum = 5; break; /* circular */
		case 2: pdPtr = &pdFile;   curMenuNum = 1; break;
		case 3: pdPtr = &pdEdit;   curMenuNum = 2; break;
		case 4: pdPtr = &pdSearch; curMenuNum = 3; break;
		case 5: pdPtr = &pdOption; curMenuNum = 4; break;
		}
		break;
	case KEY_RIGHT:	/* highlight previous menu strip */
		switch( curMenuNum ) {
		case 1: pdPtr = &pdEdit;   curMenuNum = 2; break;
		case 2: pdPtr = &pdSearch; curMenuNum = 3; break;
		case 3: pdPtr = &pdOption; curMenuNum = 4; break;
		case 4: pdPtr = &pdHelp;   curMenuNum = 5; break;
		case 5: pdPtr = &pdFile;   curMenuNum = 1; break; /* circular */
	}
	break;
	case 'f': case 'F': /* file pull down menu to be shown */
		pdPtr = &pdFile;   curMenuNum = 1; dispSelect = true; break;
	case 'e': case 'E': /* edit pull down menu to be shown */
		pdPtr = &pdEdit;   curMenuNum = 2; dispSelect = true; break;
	case 's': case 'S': /* search pull down menu to be shown */
		pdPtr = &pdSearch; curMenuNum = 3; dispSelect = true; break;
	case 'o': case 'O': /* option pull down menu to be shown */
		pdPtr = &pdOption; curMenuNum = 4; dispSelect = true; break;
	case 'h': case 'H': /* help pull down menu to be shown */
		pdPtr = &pdHelp;   curMenuNum = 5; dispSelect = true; break;

	case '\n': dispSelect = true; break;
	}
}

void CommandHandler( int mnuItemIndex ) {
	switch( mnuItemIndex ) {
	/* file menu */
	case FNEW:     DocOb.NewFile();   break;
	case FOPEN:    DocOb.OpenFile();  break;
	case FSAVE:    DocOb.SaveFile();  break;
	case FSAVEAS:  DocOb.SaveAsFile();break;
	case FCLOSE:   DocOb.CloseFile(); break;
	case FEXIT:    DocOb.HandleEditorExit(); break;

	/* edit menu */
	case EUNDO:    DocOb.Undo();        break;
	case ECOPY:    DocOb.copyHandler(); break;
	case EPASTE:   DocOb.Paste();       break;
	case ECUT:     DocOb.Cut();         break;
	case EDELWORD: DocOb.delWord();     break;
	case EDELLINE: DocOb.delLine();     break;
	case EDELSEL:  DocOb.delSelected(); break;

	/* search menu */
	case SFIND:        DocOb.FindFirst();  break;
	case SFINDAGAIN:   DocOb.FindAgain();  break;
	case SFINDREPLACE: DocOb.Replace();    break;

	/* options menu */
	case OSHOWSPECIAL: DocOb.setShowSpecial(); /* toggle special symbol show */
					   DocOb.setRefreshEdit(); /* force re-display of editor */
					   break;

	case OLINUXSHELL:  DocOb.linShell();   break;

	/* help menu */
	case HUSERHELP:    UserHelp();      break;
	case HABOUT:       DocOb.About();   break;
	}
}
