/* Source file */
/* Handle the undo functions */
#include "undo.h"

/* definitions for UndoInfo class */
UndoInfo :: UndoInfo(){
	str = ""; signal = 0; lastln = false;  str1 = "";
}

UndoInfo :: UndoInfo( int sg, char ch, int tln, int tch, int d_y, int d_x,
                        int s_y, int s_x, bool lstln, char ch1 ){
	signal = sg; str = ch; tpln = tln; tpch = tch; dy = d_y;
	dx = d_x; sy = s_y; sx = s_x; lastln = lstln; str1 = ch1;
}

UndoInfo :: UndoInfo( int sg, string s, int tln, int tch, int d_y, int d_x,
                        int s_y, int s_x, bool lstln, string s1 ){
	signal = sg; str = s; tpln = tln; tpch = tch; dy = d_y;
	dx = d_x; sy = s_y; sx = s_x; lastln = lstln; str1 = s1;
}
void UndoInfo :: setVal( int sg, char ch, int tln, int tch, int d_y, int d_x,
                           int s_y, int s_x, bool lstln, char ch1 ){
	signal = sg; str = ch; tpln = tln; tpch = tch; dy = d_y;
	dx = d_x; sy = s_y; sx = s_x; lastln = lstln; str1 = ch1;
}
void UndoInfo :: setVal( int sg, string s, int tln, int tch, int d_y, int d_x,
                           int s_y, int s_x, bool lstln, string s1 ){
	signal = sg; str = s; tpln = tln; tpch = tch; dy = d_y;
	dx = d_x; sy = s_y; sx = s_x; lastln = lstln; str1 = s1;
}

/* definitions for UndoBlockInfo class */
UndoBlockInfo :: UndoBlockInfo() {
    bsx = bsy = bex = bey = 0; bclipBuf.clear(); blstLnSel = false;
}
UndoBlockInfo :: UndoBlockInfo( int psx, int psy, int pex, int pey ){
	bsx = psx; bsy = psy; bex = pex; bey = pey;
}
UndoBlockInfo :: UndoBlockInfo( int psx, int psy, int pex, int pey,
	               vector<string> &pob, bool lstLn ){
	bsx = psx; bsy = psy; bex = pex; bey = pey; blstLnSel = lstLn;
	for( int i = 0; i < pob.size(); ++i )
		bclipBuf.insert( bclipBuf.end(), pob[i] );
}

