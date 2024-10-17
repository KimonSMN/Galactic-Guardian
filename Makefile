# This is the main Makefile at the root of the repository

# Define the subdirectory where the actual Makefile is located
SUBDIR = programs

# Default target, which delegates the task to the subdirectory Makefile
all:
	$(MAKE) -C $(SUBDIR)

# Clean target to remove compiled objects and executables, delegated to the subdirectory
clean:
	$(MAKE) -C $(SUBDIR) clean

# Run the program from the subdirectory
run:
	$(MAKE) -C $(SUBDIR) run

# Valgrind check from the subdirectory
valgrind:
	$(MAKE) -C $(SUBDIR) valgrind
