CC := g++
CFLAGS := -std=c++17 -Wall -Wextra
SRCDIR := src
BUILDDIR := build
RELEASEBUILDDIR := build/release
DEBUGBUILDDIR := build/debug
TESTDIR := tests
INCLUDE := -I include
LIB := -L lib
BINDIR := bin
DEBUGTARGET := $(BINDIR)\debug.exe
RELEASETARGET := $(BINDIR)\release.exe
TESTSTARGET := $(BINDIR)\tests.exe

SOURCES := $(addprefix $(SRCDIR)/,$(shell dir /B $(SRCDIR)\*.cpp))
DEBUGOBJECTS := $(patsubst $(SRCDIR)/%,$(DEBUGBUILDDIR)/%,$(SOURCES:.cpp=.o))
RELEASEOBJECTS := $(patsubst $(SRCDIR)/%,$(RELEASEBUILDDIR)/%,$(SOURCES:.cpp=.o))
TESTS := $(addprefix $(TESTDIR)/,$(shell dir /B $(TESTDIR)\*.cpp))

# vpath %.cpp src
# vpath %.h include

all: $(DEBUGTARGET)

release: $(RELEASETARGET)

$(DEBUGTARGET): $(DEBUGOBJECTS)
	$(shell if not exist "bin" mkdir bin)
	$(CC) $^ -g -o $(DEBUGTARGET)

$(DEBUGBUILDDIR)/%.o: $(SRCDIR)/%.cpp	
	$(shell if not exist "build\debug" mkdir build\debug)
	$(CC) $(CFLAGS) $(INCLUDE) -c -g -o $@ $<

$(RELEASETARGET): $(RELEASEOBJECTS)
	$(shell if not exist "bin" mkdir bin)
	$(CC) $^ -O -o $(RELEASETARGET)

$(RELEASEBUILDDIR)/%.o: $(SRCDIR)/%.cpp	
	$(shell if not exist "build\release" mkdir build\release)
	$(CC) $(CFLAGS) $(INCLUDE) -c -O -o $@ $<

clean:
	del /q "$(TARGET)" "$(TESTSTARGET)" $(BUILDDIR) "$(TESTDIR)\catch2\tests-main.o"
	

.PHONY: tests
tests: $(TESTDIR)\catch2\tests-main.o $(filter-out $(BUILDDIR)/main.o, $(OBJECTS))
	$(CC) $(CFLAGS) $^ $(TESTS) $(INCLUDE) -o $(TESTSTARGET)
	./$(TESTSTARGET)

$(TESTDIR)\catch2\tests-main.o:
	$(CC) $(CFLAGS) $(TESTDIR)\catch2\tests-main.cpp -I tests\catch2 -c -o $@

debug:
	echo $(OBJECTS)