#ifndef _PEDIT_MACRO_H_
#define _PEDIT_MACRO_H_

#include "global.h"

/* macros for color pairs */
#define EDITCOLOR    1
#define MENUCOLOR    2
#define NAMECOLOR    3
#define STATUSCOLOR  4
#define DLGBOXCOLOR  5
#define POPMNUCOLOR  6
#define MNULINECOLOR 7

/* macros for dlgboxes.cpp */
#define OK                      1000
#define YES                     1001
#define NO                      1002
#define CANCEL                  1003
#define SAVE                    1004
#define SAVEAS                  1005
#define YESTOALL                1006

#define YES_NO                  2000
#define YES_NO_CANCEL           2001
#define SAVE_SAVEAS_CANCEL      2002
#define YES_NO_YESTOALL_CANCEL  2004

/* macros for menucmd.cpp */
/* macros for number of menu options present */
#define FMNUOPT 8
#define EMNUOPT 9
#define SMNUOPT 3
#define OMNUOPT 2
#define HMNUOPT 2

/* macros to define offset of menu's from file menu */
#define FMNUOFFSET 111
#define EMNUOFFSET ( FMNUOFFSET + FMNUOPT )
#define SMNUOFFSET ( EMNUOFFSET + EMNUOPT )
#define OMNUOFFSET ( SMNUOFFSET + SMNUOPT )
#define HMNUOFFSET ( OMNUOFFSET + OMNUOPT )

/* macros for each menu item.
 * number is based on relative
 * position from file menu.
 */
/* file menu */
#define FNEW  			FMNUOFFSET       // 111
#define FOPEN			(FNEW  + 1  )    // 112
// presence of seperator(in menu    )    // 113
#define FSAVE 			(FOPEN + 2  )    // 114
#define FSAVEAS 		(FSAVE + 1  )    // 115
// presence of seperator(in menu    )    // 116
#define FCLOSE  		(FSAVEAS + 2)    // 117
#define FEXIT   		(FCLOSE  + 1)    // 118

/* edit menu */
#define EUNDO           (FEXIT + 1  )    // 119
// presence of seperator( in menu   )    // 120
#define ECOPY    		(EUNDO + 2  )    // 121
#define EPASTE   		(ECOPY + 1  )    // 122
#define ECUT     		(EPASTE+ 1  )    // 123
// presence of seperator( in menu   )    // 124
#define EDELWORD 		(ECUT   + 2 )    // 125
#define EDELLINE 		(EDELWORD + 1)   // 126
#define EDELSEL  		(EDELLINE + 1)   // 127

/* search menu */
#define SFIND        	(EDELSEL + 1 )   // 128
#define SFINDAGAIN   	(SFIND   + 1 )   // 129
#define SFINDREPLACE 	(SFINDAGAIN+1)   // 130

/* option menu */
#define OLINUXSHELL  	(SFINDREPLACE+1) // 131
#define OSHOWSPECIAL    (OLINUXSHELL +1) // 132

/* help menu */
#define HUSERHELP    	(OSHOWSPECIAL+1) // 133
#define HABOUT       	(HUSERHELP + 1 ) // 134

/* macros to identify CTRL + keys */
#define CTRL_A		 1
#define CTRL_D		 4
#define CTRL_F		 6
#define CTRL_H		 8
#define CTRL_L		12
#define CTRL_N		14
#define CTRL_O		15
#define CTRL_R		18
#define CTRL_U      21
#define CTRL_W		23

#endif
