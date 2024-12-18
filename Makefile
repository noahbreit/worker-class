# Makefile for Worker Class + Doxygen 

# Compiler settings
CXX = g++
CXXFLAGS = -Wall -g

# Directories
SRCDIR = src
BUILDDIR = build
DOCDIR = docs

# Target executable
TARGET = $(BUILDDIR)/worker

# Source files
SRCS = $(wildcard $(SRCDIR)/*.cpp)

# Object files
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRCS))

# Header files
HEADERS = $(wildcard $(SRCDIR)/*.hpp)

# Default target
all: $(TARGET)

# Create build directory
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Link object files to create the executable
$(TARGET): $(OBJS) | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files into object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS) | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Doxygen documentation target
doc: Doxyfile $(SRCS) $(HEADERS)
	doxygen Doxyfile

# Create Doxyfile if it doesn't exist
Doxyfile:
	doxygen -g
	@echo "Doxyfile created. Please edit it to suit your project needs."
	@echo "Then run 'make doc' again to generate the documentation."
	@exit 1

# Clean up build files and documentation
clean:
	rm -rf $(BUILDDIR)
	rm -rf $(DOCDIR)

# Phony targets
.PHONY: all clean doc
