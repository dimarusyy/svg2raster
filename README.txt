Dependencies :
	* boost
	* graphicsmagick (prebuild x86/x64 binaries and headers are available in 3rdParty.zip)

Building :
1. Requirements 
	* have vcpkg configured and boost installed
2. Windows
	Option #1
		a. Extract 3rdParty.zip to $(SolutionDir)
		b. Load to VS2017 and build
	
	Options #2
		a. cd svg2raster
		b. mkdir build_win32
		c. set VCPKG_ROOT=<path to vcpkg root folder>
		d. cmake .. -DVCPKG_TARGET_TRIPLET=x86-windows-static
		e. cmake --build .
2. Linux
		a. cd svg2raster
		b. mkdir build_linux
		c. VCPKG_ROOT=<path to vcpkg root folder> cmake .. 
		d. make 

Running :
	#svg2raster -u "https://localhost/test.svg?rasterize&fit=50x50" -f png -p E:\
	#svg2raster -u "https://localhost/test.svg?rasterize&fit=50x50" -f jpeg -p E:\

Notes :
	* graphicsmagick is used to make svg->png or svg->jpeg convesion
