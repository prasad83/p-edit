# makeexe.sh will make executable of PEDIT
# create a temporary directory
mkdir pedit_exe_dir

#copy source files
cp SrcFiles/*.cpp pedit_exe_dir/

#copy header files
cp Headers/*.h pedit_exe_dir/

#complile
c++ -lncurses -lpanel -lmenu -o pedit.exe pedit_exe_dir/pedit.cpp 2>errors.lst

case $? in
	0) echo "Executable Done: pedit.exe" ;;
	[1-9]) echo "Compilation error: Errors stored in errors.lst" ;;
esac

#delete the temporary directory
rm -r pedit_exe_dir

