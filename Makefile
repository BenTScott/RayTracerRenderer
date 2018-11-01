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
	$(CC) $^ -g -o $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c -g -o $@ $<

clean:
	del /s /q "$(TARGET)" "$(TESTSTARGET)" $(BUILDDIR)

run: all
	./$(TARGET)

.PHONY: tests
tests: $(filter-out $(BUILDDIR)/main.o, $(OBJECTS))
	$(CC) $(TESTDIR)/catch2/tests-main.o $^ $(TESTS) $(INCLUDE) -o $(TESTSTARGET)

debug:
	echo $(OBJECTS)