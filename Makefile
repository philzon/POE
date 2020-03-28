TITLE   := poe
VERSION := 0.1.0
COMMIT  := $(shell git rev-parse --short HEAD)
DATE    := $(shell date +"%Y-%m-%d %H:%M:%S")

CC      := g++
CFLAGS  := -std=c++11 \
           -O0 \
           -g \
           -DBUILD_VERSION='"$(VERSION)"' \
           -DBUILD_COMMIT='"$(COMMIT)"' \
           -DBUILD_DATE='"$(DATE)"'
LIBS    := -lncurses

BINDIR  := bin
SRCDIR  := src
INCDIR  := include
LIBDIR  := lib
OBJDIR  := obj
RESDIR  := resource
TESDIR  := test
SCRDIR  := script
INSDIR  := /usr/local/bin

SOURCES := $(shell find $(SRCDIR) -type f -name *.cpp)
OBJECTS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SOURCES:.cpp=.o))

.PHONY: all clean init lint build test install

all: init build

clean:
	@rm -rf $(OBJDIR)
	@rm -rf $(BINDIR)

init:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(BINDIR)

lint:
	@echo "Linting ..."

build: $(BINDIR)/$(TITLE)

$(BINDIR)/$(TITLE): $(OBJECTS)
	@$(CC) -L $(LIBDIR) -o $(BINDIR)/$(TITLE) $^ $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@$(CC) $(CFLAGS) -I $(INCDIR) -c -o $@ $<
	@echo "CC  $@"

install:
	@cp $(BINDIR)/$(TITLE) $(INSDIR)
	@echo "Installed $(INSDIR)/$(TITLE)"

uninstall:
	@rm $(BINDIR)/$(TITLE) $(INSDIR)/$(TITLE)
	@echo "Uninstalled $(INSDIR)/$(TITLE)"
