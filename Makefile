CC		:=g++
CFLAGS	:=-Wall -Wpedantic -O3 -march=native -I. -D_POSIX_C_SOURCE=199309L -c
LDFLAGS	:=-O3 -lpthread -static-libstdc++ -static-libgcc

SRCDIR  :=src
BINDIR  :=bin
OBJDIR  :=obj

TARGET  := $(BINDIR)/PeriodicTimerDemo
SOURCES := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(SRCDIR)/*.hpp)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	mkdir -p $(@D)
	$(CC) $(LDFLAGS) $^ -o $@
	@echo "Linking complete!"


$(OBJECTS): $(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $<
	@echo "Compiled "$<" successfully!"

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJECTS)