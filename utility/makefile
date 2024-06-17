# Just change these variables and everything just magically works :D

srcDirs = src
incDirs = ..\dependencies\include
intermDir = interms

outDebug = ..\dependencies\lib\utility\utility_debug.lib
outRelease = ..\dependencies\lib\utility\utility_release.lib

#-----------------------------------------------------------------------

CC = ccache g++

srcs = $(foreach dir, $(srcDirs), $(wildcard $(dir)/*.cpp))

depFlags = -MP -MD
incFlags = $(foreach dir, $(incDirs), -I$(dir))

#------------------------------------------------------------------------

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

#---------------------------------------------------------------------

debug: compFlags = $(debugCompFlags)

release: compFlags = $(releaseCompFlags)

#---------------------------------------------------------------------

.PHONY: debug release gendirs clean

debug: gendirs $(outDebug)
release: gendirs $(outRelease)

gendirs: 
	@for %%x in ($(srcDirs)) do if not exist $(debugIntermDir)\%%x mkdir $(debugIntermDir)\%%x
	@for %%x in ($(srcDirs)) do if not exist $(releaseIntermDir)\%%x mkdir $(releaseIntermDir)\%%x

define buildLib
	@echo Building $@
	@ar rcs -o $@ $^
endef

$(outDebug): $(debugObjs) ; $(buildLib)
$(outRelease): $(releaseObjs) ; $(buildLib)

$(debugIntermDir)/%.o $(releaseIntermDir)/%.o: %.cpp
	@echo Compiling $<
	@$(CC) $(compFlags) -c -o $@ $<

clean: 
	@if exist $(intermDir) rmdir /s /q $(intermDir)
	@if exist $(outDebug) del /q $(outDebug)
	@if exist $(outRelease) del /q $(outRelease)

-include $(debugDeps)
-include $(releaseDeps)