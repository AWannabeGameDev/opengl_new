Building only works on windows. You must have GNU make, ccache, the g++ compiler and the Windows SDK to build this project.

Go to the "advanced" folder and run "make debug" or "make release" to build the debug or release versions. The executable is located within the "bin" folder in "advanced".

You might have to go to the makefile in "advanced" and update the "libDirs" variable to include the path where Gdi32.lib is present (from the Windows SDK). All spaces in the folder name must be replaced with "^".
