CC := g++
CFLAGS := -std=c++17 -Wall -Wextra
SRCDIR := src
BUILDDIR := build
TESTDIR := tests
INCLUDE := -I include
LIB := -L lib
BINDIR := bin
TARGET := $(BINDIR)\main.exe
TESTSTARGET := $(BINDIR)\tests.exe

SOURCES := $(addprefix $(SRCDIR)/,$(shell dir /B $(SRCDIR)\*.cpp))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.cpp=.o))
TESTS := $(addprefix $(TESTDIR)/,$(shell dir /B $(TESTDIR)\*.cpp))

# vpath %.cpp src
# vpath %.h include

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(shell if not exist "bin" mkdir bin)
	$(CC) $^ -g -o $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(shell if not exist "build" mkdir build)
	$(CC) $(CFLAGS) $(INCLUDE) -c -g -o $@ $<

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