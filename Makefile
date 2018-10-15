CC := g++
CFLAGS := -std=c++17 -Wall -Wextra
SRCDIR := src
BUILDDIR := build
INCLUDE := -I include
LIB := -L lib
BINDIR := bin
TARGET := $(BINDIR)\main.exe

SOURCES := $(addprefix $(SRCDIR)/,$(shell dir /B $(SRCDIR)\*.cpp))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.cpp=.o))

# vpath %.cpp src
# vpath %.h include

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $^ -g -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c -g -o $@ $<

clean:
	del /s /q "$(TARGET)" $(BUILDDIR)

run: all
	./$(TARGET)

debug:
	echo $(OBJECTS)