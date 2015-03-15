/* Header file */
#ifndef __UNDO_H__
#define __UNDO_H__

#define INSCHAR   	 1001
#define RPLCCHAR  	 1002
#define DELCHAR    	 1003
#define BACKSPACE 	 1004
#define KEYENTER   	 1005
#define DELWORD   	 1006
#define INSDELWORD   1007
#define DELLINE   	 1008
#define INSDELLINE   1009
#define REPLACEWORD  1010
#define UNRPLCWORD   1011
#define PASTE        1012
#define CUT          1013

class UndoInfo {
public:
	int signal;   /* a code to describe the function  */
	string str;   /* store the character/string value */
	int tpln,tpch;/* topline, topchar information     */
	int dy, dx;   /* cursor data information          */
	int sy, sx;   /* cursor screen information        */
	bool lastln;  /* whether action was on last line, used on when undoing delLine */
	string str1;  /* find word information needed for undo replace word */
public:
	UndoInfo();
	UndoInfo( int sg, char ch, int tln, int tch, int d_y, int d_x,
	          int s_y, int s_x, bool lstln = false, char ch1 = ' ' );
	UndoInfo( int sg, string s, int tln, int tch, int d_y, int d_x,
	          int s_y, int s_x, bool lstln = false, string s1 = "" );
	void setVal( int sg, char ch, int tln, int tch, int d_y, int d_x,
	             int s_y, int s_x, bool lstln = false, char ch1 = ' ' );
	void setVal( int sg, string s, int tln, int tch, int d_y, int d_x,
	             int s_y, int s_x, bool lstln = false, string s1 = "" );
};


class UndoBlockInfo {
public:
	int bsx, bsy; /* start position of block operation */
	int bex, bey; /* end position of block operation   */
	vector<string> bclipBuf; /* the content of clipBoard, useful for undo Cut */
	bool blstLnSel; /* was last buffer line was selected */
public:
	UndoBlockInfo();
	UndoBlockInfo( int psx, int psy, int pex, int pey );
	UndoBlockInfo( int psx, int psy, int pex, int pey,
	               vector<string> &pob, bool lstLn = false );
};
#endif

