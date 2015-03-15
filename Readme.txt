Pedit v3.5 -- Designed by Prasad.A
**********************************
I have maintained two directories 'SrcFiles' and 'Headers'.
These directories have all files needed for compiling the
source code.

This version has improvement over entire display than
previous version (v3.4).

This version gets excuted in Linux 9.0 where there are
few terminal characteristics changes.


When you have to compile the source code:
-----------------------------------------
Step 1:
*******
Put all (*.cpp) and (*.h) files from SrcFiles and Headers
directories into single directory.

Step 2:
*******
Command: c++ -lcurses -lpanel -lmenu -o pedit.exe pedit.cpp

Step 3:
*******
On successfull compilation executable ( pedit.exe ) can be
used for edting.

To easy the task of compiling "makeexe.sh" is provided.
Usage: sh makeexe.sh

**********
Thank You.
**********

