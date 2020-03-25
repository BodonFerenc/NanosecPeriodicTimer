ifeq ($(shell uname),Darwin)
	PLATFORM=m64
else ifeq ($(shell uname),Linux)
	PLATFORM=l64
	LDFLAGSEXTRA := -static-libstdc++ -static-libgcc
endif

SRCDIR  :=src
BINDIR  :=bin
OBJDIR  :=obj
LIBDIR  :=lib


CC		:=g++
CFLAGS	:=-DKXVER=3 -std=c++14 -Wall -Wpedantic -O3 -march=native -I $(LIBDIR) -c
LDFLAGS	:=-O3 -flto -lpthread $(LDFLAGSEXTRA)


TARGETS  := $(BINDIR)/PeriodicTimerDemo $(BINDIR)/PeriodicKDBPublisher $(BINDIR)/KDBPublishLatencyTester
MAINS	 := $(TARGETS:$(BINDIR)/%=$(OBJDIR)/%.o)
SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(SRCDIR)/*.hpp)
OBJECTS  := $(filter-out $(MAINS),$(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o))

all: $(TARGETS)

$(LIBDIR)/c.o:
	@mkdir -p $(@D)
	wget -P $(LIBDIR) https://github.com/KxSystems/kdb/raw/master/$(PLATFORM)/c.o

$(LIBDIR)/k.h:
	@mkdir -p $(@D)
	wget -P $(LIBDIR) https://raw.githubusercontent.com/kxcontrib/capi/master/k.h

$(TARGETS): $(BINDIR)/%: $(OBJECTS) $(OBJDIR)/%.o $(LIBDIR)/c.o
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) $^ -o $@
	@echo "Linking complete!"	

$(OBJECTS) $(MAINS): $(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(LIBDIR)/k.h
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $<
	@echo "Compiled "$<" successfully!"


.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJECTS) $(MAINS)