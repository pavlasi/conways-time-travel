
# Target

TARGET = t1

# Directories

SRCDIR := src
INCDIR := src
OBJDIR := obj

# Extensions

SRCEXT := cpp
OBJEXT := o

# Files

SRCFILES := $(foreach D, $(SRCDIR), $(wildcard $(D)/*.$(SRCEXT)))
OBJFILES := $(patsubst %.$(SRCEXT), $(OBJDIR)/%.$(OBJEXT), $(SRCFILES))

# Compiler

CC := g++

# Flags

CFLAGS 	:= -Wall -Wextra -pedantic -std=c++20
LDFLAGS := $(foreach $D, $(INCDIR), $(wildcard -I$(D)))
LDLIBS	:= -lz3

#
# Build Rules
#

.PHONY: all buildmsg build done

all: buildmsg build done

buildmsg:
	@echo "compiling..."

build: $(TARGET)

$(TARGET): $(OBJFILES)
	@mkdir -p '$(@D)'
	@$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

$(OBJDIR)/%.$(OBJEXT): %.$(SRCEXT)
	@mkdir -p '$(@D)'
	@$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)


#
# Clean Rules
#

.PHONY: clean cleanmsg cleanfonts done

clean: cleanmsg cleanfonts done

cleanmsg:
	@echo "cleaning..."

cleanfonts:
	@rm -rf $(OBJDIR) $(TARGET)

done:
	@echo "done"

