# Just change these variables and everything just magically works :D

srcDirs = src
incDirs = ..\dependencies\include
libDirs = ..\dependencies\lib "C:\Program^Files^(x86)\Windows^Kits\10\Lib\10.0.22000.0\um\x64"
intermDir = interms

libFilesDebug = utility\utility_debug.lib GLFW\libglfw3.a OpenGL32.lib shell32.lib Gdi32.lib User32.lib
libFilesRelease = opengl32 Shell32 Gdi32 User32 kernel32 :utility\utility_release.lib GLFW\libglfw3.a

outDebug = bin\advanced_debug.exe
outRelease = bin\advanced_release.exe

preDefines = 

#-----------------------------------------------------------------------

temp = 
space = $(temp) $(temp)

srcs = $(foreach dir, $(srcDirs), $(wildcard $(dir)/*.cpp))
depFlags = -MP -MD
linkFlags = $(foreach dir, $(libDirs), $(subst ^,$(space), -L$(dir)))
libFilesDebugFlag = $(foreach file, $(libFilesDebug), -l:$(file))
libFilesReleaseFlag = $(foreach file, $(libFilesRelease), -l$(file))

debugOptim = -O0
releaseOptim = -O2

debugCompFlags = -g $(foreach dir, $(incDirs), -I$(dir)) $(foreach def, $(preDefines), -D$(def)) $(debugOptim) $(depFlags) -std=c++20
releaseCompFlags = $(foreach dir, $(incDirs), -I$(dir)) $(foreach def, $(preDefines), -D$(def)) $(releaseOptim) $(depFlags) -std=c++20

debugIntermDir = $(intermDir)\debug
releaseIntermDir = $(intermDir)\release

debugObjs = $(patsubst %.cpp, $(debugIntermDir)/%.o, $(srcs))
releaseObjs = $(patsubst %.cpp, $(releaseIntermDir)/%.o, $(srcs))

debugDeps = $(patsubst %.cpp, $(debugIntermDir)/%.d, $(srcs))
releaseDeps = $(patsubst %.cpp, $(releaseIntermDir)/%.d, $(srcs))

#--------------------------------------------------------------------

$(outDebug): objs = $(debugObjs)
$(outDebug): deps = $(debugDeps)

$(outRelease): objs = $(releaseObjs)
$(outRelease): deps = $(releaseDeps)

#---------------------------------------------------------------------

all: gendirs debug release
debug: gendirs $(outDebug)
release: gendirs $(outRelease)

test:
	@echo $(linkFlags)

gendirs: 
	@for %%x in ($(srcDirs)) do if not exist $(debugIntermDir)\%%x mkdir $(debugIntermDir)\%%x
	@for %%x in ($(srcDirs)) do if not exist $(releaseIntermDir)\%%x mkdir $(releaseIntermDir)\%%x

$(outDebug): $(debugObjs)
	@echo Building $@
	@g++ $(linkFlags) -o $@ $^ $(libFilesDebugFlag)

$(outRelease): $(releaseObjs)
	@echo Building $@
	@g++ $(linkFlags) -o $@ $^ $(libFilesReleaseFlag)

$(debugIntermDir)/%.o: %.cpp
	@echo Compiling $<
	@g++ $(debugCompFlags) -c -o $@ $<

$(releaseIntermDir)/%.o: %.cpp
	@echo Compiling $<
	@g++ $(releaseCompFlags) -c -o $@ $<

clean: 
	@if exist $(intermDir) rmdir /s /q $(intermDir)
	@if exist $(outDebug) del /q $(outDebug)
	@if exist $(outRelease) del /q $(outRelease)

-include $(debugDeps)
-include $(releaseDeps)