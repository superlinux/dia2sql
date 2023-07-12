all: 
	@echo -n "Building dia2sql..";
	@if [ -e /usr/include/pugixml.hpp ]; then g++ -static -w -fpermissive dia2sql.cpp -lpugixml -lgzstream -lz -o dia2sql.exe; echo "done"; else echo " \nError! Package libpugixml-dev is not installed.\nPlease install libpugixml-dev package."; fi;
clean:
	rm -f dia2sql.exe
	
	 
