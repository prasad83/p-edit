/* Header File */
#ifndef __OPENMENUBOX_H__
#define __OPENMENUBOX_H__

#define NOSELECTION 0
#define	FILES	 	1
#define DIR		 	2
#define PARENDIR 	3

/*	COpenMenuBox : Opens a menu oriented box to select the files
 *	and can also select files from different directory.
 */
class COpenMenuBox {
	vector<string> 	vecChoice;	/*	stores the contents of file       */
	vector<int>		vecFtype;	/*	stores information about dir/file */
	string			dirName;	/*	selected file directory info      */
private:
	int  handle_getfilename( string &dirName, string &fileName );
	void getflnm_from_menu ( string dirName , string &fileName, int &fileType );
	void getCurrentDir	   ( string &dirName );
	void get_dir_filenames ( string &dirName );
public:
	COpenMenuBox() { dirName = ""; }
	bool getFileName( string &fileName );
};

#endif
