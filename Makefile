CC		:=g++
CFLAGS	:=-DKXVER=3 -Wall -Wpedantic -O3 -march=native -I. -D_POSIX_C_SOURCE=199309L -c
LDFLAGS	:=-O3 -flto -lpthread -static-libstdc++ -static-libgcc

SRCDIR  :=src
BINDIR  :=bin
OBJDIR  :=obj

TARGETS  := $(BINDIR)/PeriodicTimerDemo $(BINDIR)/PeriodicKDBPublisher
MAINS	 := $(TARGETS:$(BINDIR)/%=$(OBJDIR)/%.o)
SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(SRCDIR)/*.hpp)
OBJECTS  := $(filter-out $(MAINS),$(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o))

all: $(TARGETS)

c.o:
	wget -P $(OBJDIR) https://github.com/KxSystems/kdb/raw/master/l64/c.o

k.h:
	wget -P $(SRCDIR) https://raw.githubusercontent.com/kxcontrib/capi/master/k.h

$(TARGETS): $(BINDIR)/%: $(OBJECTS) $(OBJDIR)/%.o
	mkdir -p $(@D)
	$(CC) $(LDFLAGS) $^ $(OBJDIR)/c.o -o $@
	@echo "Linking complete!"	

$(OBJECTS) $(MAINS): $(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $<
	@echo "Compiled "$<" successfully!"


.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJECTS) $(MAINS)