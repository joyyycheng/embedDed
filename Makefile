# Compiler
CC = g++

# Compiler flags
CFLAGS = -Isrc/include

# Linker flags
LDFLAGS = -Lsrc/lib -lmingw32 -lSDL2main -lSDL2

# Source files
SOURCES = main.c maze.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Executable
EXECUTABLE = main

# Rule to build the executable
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Rule to build object files
%.o: %.c maze.h
		$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
