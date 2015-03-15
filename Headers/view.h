/* Header file */
#ifndef __VIEW_H__
#define __VIEW_H__

#define DEFAULT_TAB  4 /* number of spaces that will be skipped for tab */
/* Tabstops : TAB = 4:
 * 0  3   7   11  15  19  23  27  31  35  39  43  47  51  55  59  63  67  71  75  79
 * LM)                                                                       (RM
 */

/* Note: LM, RM, TM, BM are limiting position for data display on the window
 * and not the window sizes. But they must be less than the window size. */
#define DEFAULT_TM   0 /* with respect to edit window */
#define DEFAULT_BM  23 /* with respect to edit window */
#define DEFAULT_LM   0 /* with respect to edit window */
#define DEFAULT_RM  75 /* RM is set to last tab stop  feasible for scrolling */
/* Reasons for not choosing 79 as RM:
 * I define the edit window to be of atmost width 80, it means I can access 0 - 79 columns
 * but if I set 79 as RM full (terminal) screen line can be filled by the characters by
 * the user. When I call 'wscrl( edit, 1 )' to scroll one line down at the last line (BM)
 * actually two rows are scrolled, one row for complete screen line of characters and the
 * cursor will be placed at the next row which is empty. This creates a problem for further
 * display according to working of the 'ScrlDownDisplayFile()'.
 * So I limit the RM to 75 so one full (terminal) screen line is not filled by characters
 * and scrolling will be only one line at BM now due to 'wscrl( edit, 1 )' call in the
 * function 'ScrlDownDisplayFile()'.
 */

/* Handle the viewing of document display */
class CView {
protected:
	vector<string> buffer;
	int            lines;

	string curFlName;   /* currently opened file name  */
	bool flChange;      /* file content was changed or not? */
	int topline; /* starting index of topmargin line in data structure         */
	int topchar; /* starting index of firstchar on display topline of topline  */

    bool ShowSpecial; /* put a different symbol for '\t' and space in editor */
    bool refreshEdit; /* content of full display is to be refreshed or not   */
    int  cursorStyle; /* type of cursor to be displayed normal, no or insert */

	int data_x, data_y, screen_x, screen_y;

	int LM,RM,TM,BM,ONEVPAGE,TAB;

public:
 	CView( int lm = DEFAULT_LM,int rm = DEFAULT_RM, int tm = DEFAULT_TM,
	       int bm = DEFAULT_BM,int tab = DEFAULT_TAB );
	void DisplayStatus( int dy, int dx );
	void DisplayFile( int stline, int offset );
	void DisplayLine( int dy, int sy );
	int mvNextTabStop( int &sx );
	int mvPrevTabStop( int &sx );
	int mapScreenPos( int dy, int dx, int &sx );
	void mvright();
	void mvleft();
	void mvup();
	void mvdown();
	int scrollDown();
	int scrollUp();
	int scrollRight();
	int scrollLeft();
	int setRightPageIndex();
	int setLeftPageIndex();
	void mvend();
	void mvhome();
	void pageup();
	void pagedown();
	int mapCharPos( int dy, int sx );
	int mvCharPos( int dy, int dx );
	int mapLineDisplay( int fromline, int toline );

	bool getShowSpecial();
	void setShowSpecial();
	bool getRefreshEdit();
	void setRefreshEdit();
	int  getCursorStyle();
	void setCursorStyle( int curStyl );

	int gettopchar();
	int gettopline();
	char getCurChar();
	int getCurdx();
	int getCurdy();
	int getCursx();
	int getCursy();
	int getBM();
	int getTM();
	int getLM();
	int getRM();
	void ScrlDownDisplayFile();
	void ScrlUpDisplayFile();

};
#endif
