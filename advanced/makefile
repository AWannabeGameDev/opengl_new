# Just change these variables and everything just magically works :D

# TODO: Handle subproject dependencies properly

srcDirs = src
incDirs = ..\dependencies\include
libDirs = ..\dependencies\lib "C:\Program^Files^(x86)\Windows^Kits\10\Lib\10.0.22000.0\um\x64"
intermDir = interms

libFilesDebug = utility\utility_debug.lib GLFW\libglfw3.a Gdi32.lib
libFilesRelease = utility\utility_release.lib GLFW\libglfw3.a Gdi32.lib

projectFolder = ..
subProjDeps = utility

outDebug = bin\advanced_debug.exe
outRelease = bin\advanced_release.exe

preDefines = 

#-----------------------------------------------------------------------

CC = ccache g++

empty = 

srcs = $(foreach dir, $(srcDirs), $(wildcard $(dir)/*.cpp))

depFlags = -MP -MD
incFlags = $(foreach dir, $(incDirs), -I$(dir))
preDefFlags = $(foreach def, $(preDefines), -D$(def))
linkFlags = $(foreach dir, $(libDirs), $(subst ^, $(empty), -L$(dir)))

#------------------------------------------------------------------------

libFilesDebugFlag = $(foreach file, $(libFilesDebug), -l:$(file))
libFilesReleaseFlag = $(foreach file, $(libFilesRelease), -l:$(file))

debugOptim = -O0
releaseOptim = -O2

debugCompFlags = -g $(incFlags) $(preDefFlags) $(debugOptim) $(depFlags) -std=c++20
releaseCompFlags = $(incFlags) $(preDefFlags) $(releaseOptim) $(depFlags) -std=c++20

debugIntermDir = $(intermDir)\debug
releaseIntermDir = $(intermDir)\release

debugObjs = $(patsubst %.cpp, $(debugIntermDir)/%.o, $(srcs))
releaseObjs = $(patsubst %.cpp, $(releaseIntermDir)/%.o, $(srcs))

debugDeps = $(patsubst %.cpp, $(debugIntermDir)/%.d, $(srcs))
releaseDeps = $(patsubst %.cpp, $(releaseIntermDir)/%.d, $(srcs))

#--------------------------------------------------------------------

debug: makeTarget = debug
debug: libFilesFlag = $(libFilesDebugFlag)
debug: compFlags = $(debugCompFlags)

release: makeTarget = release
release: libFilesFlag = $(libFilesReleaseFlag)
release: compFlags = $(releaseCompFlags)

#---------------------------------------------------------------------

.PHONY: debug release test gendirs clean $(subProjDeps)

debug: gendirs $(outDebug)
release: gendirs $(outRelease)

gendirs: 
	@for %%x in ($(srcDirs)) do if not exist $(debugIntermDir)\%%x mkdir $(debugIntermDir)\%%x
	@for %%x in ($(srcDirs)) do if not exist $(releaseIntermDir)\%%x mkdir $(releaseIntermDir)\%%x

define buildExecutable
	@echo Building $@
	@$(CC) $(linkFlags) -o $@ $(filter-out $(subProjDeps), $^) $(libFilesFlag)
endef 

$(outDebug): $(subProjDeps) $(debugObjs) ; $(buildExecutable)
$(outRelease): $(subProjDeps) $(releaseObjs) ; $(buildExecutable)

$(subProjDeps):
	@make $(makeTarget) -s -C $(projectFolder)\$@ 

$(debugIntermDir)/%.o $(releaseIntermDir)/%.o: %.cpp
	@echo Compiling $<
	@$(CC) $(compFlags) -c -o $@ $<

clean: 
	@if exist $(intermDir) rmdir /s /q $(intermDir)
	@if exist $(outDebug) del /q $(outDebug)
	@if exist $(outRelease) del /q $(outRelease)

-include $(debugDeps)
-include $(releaseDeps)