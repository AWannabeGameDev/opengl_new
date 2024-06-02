# Just change these variables and everything just magically works :D

srcDirs = src
incDirs = ..\dependencies\include
intermDir = interms

outDebug = ..\dependencies\lib\utility\utility_debug.lib
outRelease = ..\dependencies\lib\utility\utility_release.lib

#-----------------------------------------------------------------------

srcs = $(foreach dir, $(srcDirs), $(wildcard $(dir)/*.cpp))
depFlags = -MP -MD
incFlags = $(foreach dir, $(incDirs), -I$(dir))

debugOptim = -O0
releaseOptim = -O2

debugCompFlags = -g $(incFlags) $(debugOptim) $(depFlags) -std=c++20
releaseCompFlags = $(incFlags) $(releaseOptim) $(depFlags) -std=c++20

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

gendirs: 
	@for %%x in ($(srcDirs)) do if not exist $(debugIntermDir)\%%x mkdir $(debugIntermDir)\%%x
	@for %%x in ($(srcDirs)) do if not exist $(releaseIntermDir)\%%x mkdir $(releaseIntermDir)\%%x

$(outDebug): $(debugObjs)
	@echo Building $@
	@ar rcs -o $@ $^

$(outRelease): $(releaseObjs)
	@echo Building $@
	@ar rcs -o $@ $^

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