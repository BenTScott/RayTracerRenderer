CC := g++
CFLAGS := -std=c++17 -Wall -Wextra
SRCDIR := src
BUILDDIR := build
INCLUDE := -I include
LIB := -L lib
BINDIR := bin
TARGET := $(BINDIR)/main.exe
SOURCES := $(addprefix $(SRCDIR)/,$(shell dir /B $(SRCDIR)\*.cpp))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.cpp=.o))

# vpath %.cpp src
# vpath %.h include

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $<

clean:
	del /s /q $(BINDIR) $(BUILDDIR)

run: all
	./$(TARGET

debug:
	echo $(SOURCES)
	echo $(OBJECTS)