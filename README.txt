Dependencies :

* boost
* cpprestsdk
* graphicsmagick (prebuild x86/x64 binaries and headers are available in 3rdParty.zip)

Building :

1. Extract 3rdParty.zip to $(SolutionDir)
2. Load to VS2017 and build

Running :

#svg2raster -u "https://localhost/test.svg?rasterize&fit=50x50" -f png -p E:\
#svg2raster -u "https://localhost/test.svg?rasterize&fit=50x50" -f jpeg -p E:\

Notes :

* cpprestsdk is used to parse URL passed in command line arguments
* graphicsmagick is used to make svg->png or svg->jpeg convesion
