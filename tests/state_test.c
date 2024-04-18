//////////////////////////////////////////////////////////////////
//
// Test για το state.h module
//
//////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing

#include "state.h"


///// Βοηθητικές συναρτήσεις ////////////////////////////////////////
//
// Ελέγχει την (προσεγγιστική) ισότητα δύο double
// (λόγω λαθών το a == b δεν είναι ακριβές όταν συγκρίνουμε double).
static bool double_equal(double a, double b) {
	return abs(a-b) < 1e-6;
}

// Ελέγχει την ισότητα δύο διανυσμάτων
static bool vec2_equal(Vector2 a, Vector2 b) {
	return double_equal(a.x, b.x) && double_equal(a.y, b.y);
}
/////////////////////////////////////////////////////////////////////


void test_state_create() {

	State state = state_create();
	TEST_ASSERT(state != NULL);

	StateInfo info = state_info(state);
	TEST_ASSERT(info != NULL);

	TEST_ASSERT(!info->paused);
	TEST_ASSERT(info->score == 0);

	// Προσθέστε επιπλέον ελέγχους

	TEST_ASSERT(info->spaceship != NULL); 
	TEST_ASSERT(info->spaceship->type == SPACESHIP); 

	// Tests για state_objects 
	// Πρωτη Κληση της state_object:
	// First Testing, Object has Position (182.717728, 314.321869)

	// Set Cordinates for top_left
    Vector2 top_left; 
	top_left.x = 100;
	top_left.y = 100;

	// Set Cordinates for bottom_right
    Vector2 bottom_right;
	bottom_right.x = 400;
	bottom_right.y = 400;

	List list = state_objects(state, top_left, bottom_right); 
	TEST_ASSERT(list_size(list) != 0); // Check if list isn't empty

	ListNode node = list_first(list); // Set node to first element of list

	Object object = list_node_value(list, node); // Save the value of the node to the object
	
    TEST_ASSERT(object->position.x >= top_left.x && 
				object->position.y >= top_left.y && 
				object->position.x <= bottom_right.x && 
				object->position.y <= bottom_right.y);

	// Δευτερη Κληση της state_object:
	// Second Testing, Object has Position (286.382477, 182.218079)
	
	// Set Cordinates
	top_left.x = 50;
	top_left.y = 50;
	bottom_right.x = 350;
	bottom_right.y = 350;

	list = state_objects(state, top_left, bottom_right); 
	TEST_ASSERT(list_size(list) != 0); // Check if list isn't empty

	node = list_next(list, node); // Set node to second element of list
	
	object = list_node_value(list, node);

    TEST_ASSERT(object->position.x >= top_left.x && 
				object->position.y >= top_left.y && 
				object->position.x <= bottom_right.x && 
				object->position.y <= bottom_right.y);
}

void test_state_update() {
	State state = state_create();
	TEST_ASSERT(state != NULL && state_info(state) != NULL);

	// Πληροφορίες για τα πλήκτρα (αρχικά κανένα δεν είναι πατημένο)
	struct key_state keys = { false, false, false, false, false, false, false };
	
	// Χωρίς κανένα πλήκτρο, το διαστημόπλοιο παραμένει σταθερό με μηδενική ταχύτητα
	state_update(state, &keys);

	TEST_ASSERT( vec2_equal( state_info(state)->spaceship->position, (Vector2){0,0}) );
	TEST_ASSERT( vec2_equal( state_info(state)->spaceship->speed,    (Vector2){0,0}) );

	// Με πατημένο το πάνω βέλος, η ταχήτητα αυξάνεται ενώ το διαστημόπλοιο παραμένει για την ώρα ακίνητο
	keys.up = true;
	state_update(state, &keys);

	TEST_ASSERT( vec2_equal( state_info(state)->spaceship->position, (Vector2){0,0}) );
	TEST_ASSERT( vec2_equal( state_info(state)->spaceship->speed,    (Vector2){0,SPACESHIP_ACCELERATION}) );

	// Προσθέστε επιπλέον ελέγχους
	keys.up = false;

	// Με πατημενο το p, το παιχνιδι σταματαει
	keys.p = true;	
	state_update(state, &keys);
	
	TEST_ASSERT(state_info(state)->paused == true);
	keys.p = false;	

	keys.right = true;
	state_update(state, &keys);

	TEST_ASSERT( !vec2_equal( state_info(state)->spaceship->orientation, (Vector2){0,0}) );
	keys.right = false;

	keys.left = true;
	state_update(state, &keys);
	
	TEST_ASSERT( !vec2_equal( state_info(state)->spaceship->orientation, (Vector2){0,0}) );
	keys.left = false;

}


// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "test_state_create", test_state_create },
	{ "test_state_update", test_state_update },

	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};
