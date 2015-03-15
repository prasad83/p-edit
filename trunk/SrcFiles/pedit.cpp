/* Linux editor */
#include "macro.h"
#include "global.h"

#include "doc.cpp"
#include "initialize.cpp"
#include "menucmd.cpp"
#include "openmenubox.cpp"
#include "popdown.cpp"
#include "undo.cpp"
#include "view.cpp"
#include "dlgboxes.cpp"
#include "usrhlp.cpp"

/* Create editor object */
CDocument DocOb;
/* use the default settings for left,right,top,bottom margin and tab */

int main( int argc, char **argv ) {
	initEditor(); /* initialize the editor */

	if( argc > 1 ) { /* file was specified on command line */
		if(	!DocOb.OpenFileSpecified( argv[1] ) ) { /* try to open the file */
			DocOb.NewFile();  /* create a blank file if falied to open file */
		}
	}
	else { /* file name not given on command line */
		DocOb.NewFile(); /* create a blank file */
	}

	/* display the buffer contents */
	DocOb.DisplayFile( DocOb.gettopline(), DocOb.gettopchar() );
	DocOb.DisplayStatus( DocOb.getCurdy(), DocOb.getCurdx());
  
	wmove( edit, DocOb.getCursy(), DocOb.getCursx() );

	chtype ch = wgetch( edit );
	int ptopchar;/* needed for cursor movement decision */
	while( 1 ) { /* an infinite loop to exit user has to press ALT_X */
		switch( ch ) {
		case 27: /* ALT or ESC */
			switch( ch = wgetch( edit ) ) {
			case 'l': case 'L': DocOb.linShell(); break;
			case 'x': case 'X': DocOb.HandleEditorExit(); break; /* stop running editor */
			default: ungetch( ch );break;/* put back the input taken in this switch */
			}
			break;

		case KEY_F(8) :
		    /* Linux 9's terminal cannot handle special characters */
			//DocOb.setShowSpecial(); /* toggle special display */
			//DocOb.setRefreshEdit(); /* re-display the content */
			break;

		case KEY_F(9) :
			mnuDisplayHandler(); /* handle menu show, take command and hide menu */
			break;

		case KEY_RIGHT:	DocOb.mvright();	break;
		case KEY_LEFT :	DocOb.mvleft();		break;
		case KEY_DOWN :
			/* save current references */
			ptopchar = DocOb.gettopchar();
			DocOb.mvdown();

			/* check if topchar has changed if so you need to refresh full screen */
			/* because previous page contents has to be displayed */
			if( DocOb.gettopchar() != ptopchar ) {
				break;
			}
			else if( DocOb.getRefreshEdit() && DocOb.getCursy() == DocOb.getBM() ) {
				/* don't refresh whole display using normal DisplayFile(...)
			 	 * this may make the display flicker for each refresh */
				/* Use ScrlDownDisplayFile() which uses 'wscrl(...)' */
				DocOb.ScrlDownDisplayFile();
			}
			break;
		case KEY_UP:
			/* save current references */
			ptopchar = DocOb.gettopchar();
			DocOb.mvup();
			/* check if topchar has changed if so you need to refresh full screen */
			/* because previous page contents has to be displayed */
			if( DocOb.gettopchar() != ptopchar ) {
				break;
			}
			else if( DocOb.getRefreshEdit() && DocOb.getCursy() == DocOb.getTM() ) {
				/* don't refresh whole display using normal DisplayFile(...)
			 	 * this may make the display flicker for each refresh */
				/* Use ScrlUpDisplayFile() which uses 'wscrl(...)' */
				DocOb.ScrlUpDisplayFile();
			}
			break;
		case KEY_END:	DocOb.mvend();		break;
		case KEY_HOME:  DocOb.mvhome();		break;
		case KEY_NPAGE: DocOb.pagedown(); 	break;
		case KEY_PPAGE: DocOb.pageup();   	break;
		case KEY_IC:
			switch( DocOb.getCursorStyle() ) {
			case 1:  /* toggle from 'insert' to 'replace */
				DocOb.setCursorStyle(2); break;
			case 2:  /* toggle from 'replace'to 'insert' */
				DocOb.setCursorStyle(1); break;
			}
			break;
		case KEY_BACKSPACE: DocOb.backSpace(); break;
		case KEY_DC:	    DocOb.delChar();   break;
		case '\n':          DocOb.keyEnter();  break;

		case KEY_F(1): UserHelp();             break;

		case CTRL_N:   DocOb.NewFile();        break;
		case CTRL_O:   DocOb.OpenFile();       break;
		case KEY_F(2): DocOb.SaveFile();       break;
		case KEY_F(12):DocOb.SaveAsFile();     break;

		case CTRL_U:   DocOb.Undo();           break;
		case CTRL_W:   DocOb.delWord();        break;
		case CTRL_L:   DocOb.delLine();	       break;
		case CTRL_D:   DocOb.delSelected();    break;
        case KEY_F(3): DocOb.copyHandler();	   break;
		case KEY_F(5): DocOb.Paste();		   break;
		case KEY_F(6): DocOb.Cut();			   break;

		case CTRL_F:   DocOb.FindFirst();      break;
		case CTRL_A:   DocOb.FindAgain();      break;
		case CTRL_R:   DocOb.Replace();        break;

		default:
			if( isprint(ch) || ch == '\t' || ch == '\n' ) { /* avoids CTRL_V...   */
				/* allow only printable characters, '\t', '\n' to enter editor    */
				switch( DocOb.getCursorStyle() ) {
				case 1: DocOb.insChar( ch );  break;/* insert mode  */
				case 2: DocOb.rplcChar( ch ); break;/* replace mode */
				}
			}
			break;
 		}

		DocOb.DisplayStatus( DocOb.getCurdy(), DocOb.getCurdx());

		if( DocOb.getRefreshEdit() )
			DocOb.DisplayFile( DocOb.gettopline(), DocOb.gettopchar() );

		wmove( edit, DocOb.getCursy(), DocOb.getCursx() );
		curs_set( DocOb.getCursorStyle() );

		ch = wgetch( edit );
	}
	endwin();
	system( "clear" );
	return 0;
}
