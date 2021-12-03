LIBS = -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lgdi32 -lfreetype -lwinmm -lopengl32 -lglu32

BUILDDIR = build
SOURCEDIR = src
HEADERDIR = include
LIBDIR = libs
NAME = main





BINARY = $(NAME).exe
CC = gcc
CPP = g++



SOURCES = $(wildcard $(SOURCEDIR)/*.c)
SOURCES += $(wildcard $(SOURCEDIR)/*.cpp)
OBJECTS = $(patsubst $(SOURCEDIR)/%.cpp,$(BUILDDIR)/%.o,\
	$(patsubst $(SOURCEDIR)/%.c,$(BUILDDIR)/%.o,$(SOURCES)))

CPPFLAGS = -std=c++17 -g -Wall -D SFML_STATIC
CFLAGS = -std=c11 -g -Wall -D SFML_STATIC


CC = gcc
CPP = g++

.PHONY: all clean init

all: $(BINARY)
	@echo.


clean:
	@del $(BUILDDIR)\*.o


# !!! CPP COMPILER 
$(BINARY): $(OBJECTS)
	@echo building $(BINARY)...
	@$(CPP) $(CPPLAGS) -I$(HEADERDIR) -I$(SOURCEDIR) $(OBJECTS) -o $(BINARY) -L$(LIBDIR) $(LIBS)



$(BUILDDIR)/%.o: $(SOURCEDIR)/%.c
	@echo building (c) $@ ...
	@$(CC) $(CFLAGS) -I$(HEADERDIR) -I$(SOURCEDIR) -c $< -o $@ -L$(LIBDIR) $(LIBS)


$(BUILDDIR)/%.o: $(SOURCEDIR)/%.cpp
	@echo building (c++) $@ ...
	@$(CPP) $(CPPFLAGS) -I$(HEADERDIR) -I$(SOURCEDIR) -c $< -o $@ -L$(LIBDIR) $(LIBS)


init:
	@mkdir $(BUILDDIR)
	@mkdir $(SOURCEDIR)
	@mkdir $(HEADERDIR)
	@mkdir $(LIBDIR)
