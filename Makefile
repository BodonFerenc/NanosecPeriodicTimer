CC		:=g++
CFLAGS	:=-DKXVER=3 -Wall -Wpedantic -O3 -march=native -I. -D_POSIX_C_SOURCE=199309L -c
LDFLAGS	:=-O3 -lpthread -static-libstdc++ -static-libgcc

SRCDIR  :=src
BINDIR  :=bin
OBJDIR  :=obj

TARGETS  := $(BINDIR)/PeriodicTimerDemo
MAINS	 := $(TARGETS:$(BINDIR)/%=$(OBJDIR)/%.o)
SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(SRCDIR)/*.hpp)
OBJECTS  := $(filter-out $(MAINS),$(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o))

all: $(TARGETS)

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