
- To run state_test.c (change $(MODULES)/state.o or $(MODULES)/state_alt.o) depending on what state you would like to use

# Αρχεία .o
OBJS = state_test.o $(MODULES)/set_utils.o $(MODULES)/state.o $(MODULES)/vec2.o $(LIB)/k08.a $(LIB)/libraylib.a

# Το εκτελέσιμο πρόγραμμα
EXEC = state_test


- To run set_utils_test.c

# Αρχεία .o
OBJS = set_utils_test.o $(MODULES)/set_utils.o $(MODULES)/state.o $(MODULES)/vec2.o $(LIB)/k08.a $(LIB)/libraylib.a

# Το εκτελέσιμο πρόγραμμα
EXEC = set_utils_test
