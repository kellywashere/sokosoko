# source: https://stackoverflow.com/questions/7004702/how-can-i-create-a-makefile-for-c-projects-with-src-obj-and-bin-subdirectories
# added run task

CC       = gcc
# compiling flags here

# Valgrind//debug:
CFLAGS   := -Wall -I. -g -DDEBUG
## CFLAGS   := -Wall -I.

LINKER   = gcc
# linking flags here
LFLAGS   := -I.

# Windows portion is outdated, still refers to SDL2
ifeq ($(OS),Windows_NT)
	CFLAGS += -Dmain=SDL_main -I/mingw64/include/SDL2
	LFLAGS += -L/mingw64/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image
else
	CFLAGS += `pkg-config --cflags sdl3 sdl3-image libxml-2.0`
	LFLAGS += `pkg-config --libs sdl3 sdl3-image libxml-2.0`
endif

# change these to proper directories where each file should be
SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin

# output executable
TARGET   = main

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
rm       = rm -f

$(BINDIR)/$(TARGET): $(OBJECTS)
	$(LINKER) $(OBJECTS) $(LFLAGS) -o $@
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONY: run
run: $(BINDIR)/$(TARGET)
	@./$(BINDIR)/$(TARGET)

.PHONY: memcheck
memcheck:
	valgrind --leak-check=yes ./$(BINDIR)/$(TARGET)

.PHONY: memcheckfull
memcheckfull:
	valgrind --leak-check=full --track-origins=yes --show-reachable=yes ./$(BINDIR)/$(TARGET)

.PHONY: clean
clean:
	@$(rm) $(OBJECTS)
	@echo "Cleanup complete!"

.PHONY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"


