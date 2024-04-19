
#include <stdlib.h>
#include <stdio.h>

#include "set_utils.h"

Pointer set_find_eq_or_greater(Set set, Pointer value){
    Pointer found = set_find(set, value);
    if (found != NULL)                          // Πρώτα ελέγχετε αν το value υπάρχει ήδη
        return found; 
    
    set_insert(set, value);                     // Αν όχι, τότε το εισάγετε 
    SetNode node = set_find_node(set, value);   // Χρησιμοποιείτε το νέο στοιχείο 
    node = set_next(set, node);                 // για να βρείτε το αμέσως επόμενο
    Pointer result = NULL; 

    if (node != SET_EOF) 
        result = set_node_value(set, node);
    
    set_remove(set, value);                     // Τέλος το αφαιρείτε
    return result;
}

Pointer set_find_eq_or_smaller(Set set, Pointer value){
    Pointer found = set_find(set, value);
    if (found != NULL)                          // Πρώτα ελέγχετε αν το value υπάρχει ήδη
        return found; 
    
    set_insert(set, value);                     // Αν όχι, τότε το εισάγετε 
    SetNode node = set_find_node(set, value);   // Χρησιμοποιείτε το νέο στοιχείο 
    node = set_previous(set, node);                 // για να βρείτε το αμέσως προηγούμενο
    Pointer result = NULL; 

    if (node != SET_BOF) 
        result = set_node_value(set, node);
    
    set_remove(set, value);                     // Τέλος το αφαιρείτε
    return result;
}

int* create_int(int value){
    int* p = malloc(sizeof(int));
    *p = value;
    return p;
}

int compare_ints(Pointer a, Pointer b) {
    return *(int*)a - *(int*)b;
}

int main(){
    Set set = set_create(compare_ints, NULL);  
    set_insert(set, create_int(7));   // Dynamically allocate integers for insertion
    set_insert(set, create_int(15));
    set_insert(set, create_int(12));
    set_insert(set, create_int(4)); 
    set_insert(set, create_int(11)); 

    Pointer value = create_int(10);  
    int* result = set_find_eq_or_greater(set, value); 

    if (result != NULL) {
        printf("Test Value: %d, Result: %d\n", *(int*)value, *result); 
    } else {
        printf("Test Value: %d, Result: None\n", *(int*)value);
    }

    result = set_find_eq_or_smaller(set, value); 

    if (result != NULL) {
        printf("Test Value: %d, Result: %d\n", *(int*)value, *result); 
    } else {
        printf("Test Value: %d, Result: None\n", *(int*)value);
    }
}