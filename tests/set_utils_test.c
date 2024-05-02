//////////////////////////////////////////////////////////////////
//
// Test για το set_utils.h module
//
//////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "acutest.h"

#include "set_utils.h"

int* create_int(int value){
    int* p = malloc(sizeof(int));
    *p = value;
    return p;
}

int compare_ints(Pointer a, Pointer b) {
    return *(int*)a - *(int*)b;
}

void test_set_find_eq_or_greater(){
    Set set = set_create(compare_ints, free);  
    set_insert(set, create_int(7));  
    set_insert(set, create_int(15));
    set_insert(set, create_int(12));
    set_insert(set, create_int(4)); 
    set_insert(set, create_int(11)); 

    Pointer value = create_int(10); 
    int* result = set_find_eq_or_greater(set, value); 


    TEST_ASSERT(result != NULL && *result == 11);

    set_destroy(set);
}

void test_set_find_eq_or_smaller(){
  Set set = set_create(compare_ints, free);  
    set_insert(set, create_int(7));  
    set_insert(set, create_int(15));
    set_insert(set, create_int(12));
    set_insert(set, create_int(4)); 
    set_insert(set, create_int(11)); 

    Pointer value = create_int(10); 
    int* result = set_find_eq_or_smaller(set, value); 


    TEST_ASSERT(result != NULL && *result == 7);

    set_destroy(set);
}


// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "test_set_find_eq_or_greater", test_set_find_eq_or_greater },
	{ "test_set_find_eq_or_smaller", test_set_find_eq_or_smaller },

	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};
