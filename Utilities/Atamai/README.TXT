The main tracking code is in the ndicapi directory.  This code can be
built using cmake, and will provide a library "libndicapi.a". 
If desired, the CMakeLists.txt file can be edited to change the name
of the library.  This library provides communication with the NDI
POLARIS and AURORA devices either under the old POLARIS API or the
new combined API.

The VTK classes for tracking are in the AtamaiTracking directory.  These
classes require the ndicapi library.  These files were made to be built
as part of the VTK directory structure, the CMakeLists.txt files will
have to be edited to build them separately from VTK.
