
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
