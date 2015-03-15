/* Header file */
#ifndef __DOC_H__
#define __DOC_H__

#include "undo.h"
#include "view.h"
#include "openmenubox.h"

/* Handle the document editing */
class CDocument : public CView {
protected:
	bool diskFile;      /* whether opened file exist on disk */
	string fullFlName;  /* file name with full path */
    /* curFlName, present in CView will hold only currently opened file name */
	int backUp;         /* number of backup made */

	vector <string> clipBoard;
	bool clipFlag;      /* valid contents present in clipBoard ? */
	bool clipAct;       /* nothing selected, clipAct is assumed true */
	int sdx, sdy, edx, edy;

	string fndword, rplcword;
	stack<UndoInfo> unob; /* undo object for simple editing operation */
	stack<UndoBlockInfo> unBlckob; /* undo object for block operation */
public:
	CDocument( int lm = DEFAULT_LM,int rm = DEFAULT_RM, int tm = DEFAULT_TM,
	           int bm = DEFAULT_BM,int tab = DEFAULT_TAB );
	bool ReadFile( const char *fname );
/* bool undoCall: used as default value, decides pushing undo information on to stack.
 * The undo information need not be pushed when the call is from Undo() function which
 * sets undoCall to true. */
	void insChar( char ch,bool undoCall = false );
	void rplcChar( char ch,bool undoCall = false);
	void delChar( bool undoCall = false );
	void backSpace(bool undoCall = false);
	void keyEnter(bool undoCall = false);
	void delWord(bool undoCall = false);
	void delLine(bool undoCall = false);

	void insDelWord( string word, bool undoCall = false );
	void insDelLine( string ln, bool lastln, bool undoCall = false );

	bool FindWord( int frmdy, int frmdx, int tody = -1, int todx = -1 );
	void mvtopos( int dy, int dx );
	bool FindFirst( bool directCall = true );
	bool FindAgain( bool directCall = true );
	void Replace();
	void ReplaceWord( bool callUndo = false );
	void unReplaceWord( string fdword, string rpword, bool callUndo = false );

	bool copyHandler();
	void Copy();
	void Paste( bool undoCall = false );
	void Cut( bool undoCall = false );
	void delSelected();
	void unPaste( bool undoCall = false );
	void unCut( bool undoCall = false );

    void Undo();

    void HandleEditorExit();
    void reInitDocVars();
	bool NewFile();
	bool OpenFile();
	bool OpenFileSpecified( string ofname );
	string getStrippedName( const string fullName );
	bool SaveFile();
	bool SaveToFile( bool fileExist = false );
	bool SaveAsFile();
	bool CloseFile();

	void linShell();
	void About();
};

#endif


