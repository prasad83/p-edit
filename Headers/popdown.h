/* Header File */
#ifndef __POPMENU_H__
#define __POPMENU_H__

#include "macro.h"

/* CPopdownMenu: control display and interface of pop down menus */
class CPopdownMenu {
protected:
	WINDOW *nmwin;	PANEL	*nmpan;
	/*  nmwin		- window to hold name of menu
	 *	nmpan		- panel  for nmwin
	 */

	int ypos, xpos, w_num, mnuretval;
	/*	ypos  		- position of menu w.r.t y direction
	 *	xpos  		- position of menu w.r.t x direction
	 *	w_num		- menu number
	 *	mnuretval	- stores retval of menu command choosen
	 */

	string mnuName;    /* name of the menu to display */
	string str[10][2]; /* maximum 10 elements & shortcuts */
	int	   sz;
	/*	str			- stores the name of the menu items
	 *	sz			- stores the number of menu items
	 */
public:
	void create( int y, int x,string a[][2],int size,int mnuNumber, string name );
	void hghtName();
	void nohghtName();
	int  getMenuSelection();
	int  showmenu( );
	void hide();
};
#endif

