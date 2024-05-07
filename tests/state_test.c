//////////////////////////////////////////////////////////////////
//
// Test για το state.h module
//
//////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing
#include <math.h>

#include "state.h"
#include "vec2.h"


///// Βοηθητικές συναρτήσεις ////////////////////////////////////////
//
// Ελέγχει την (προσεγγιστική) ισότητα δύο double
// (λόγω λαθών το a == b δεν είναι ακριβές όταν συγκρίνουμε double).
static bool double_equal(double a, double b) {
	return fabs(a-b) < 1e-6;
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
	// Το Object εχει Θεση (182.717728, 314.321869)

	// Set Cordinates for top_left
    Vector2 top_left; 
	top_left.x = 0;
	top_left.y = 10000;

	// Set Cordinates for bottom_right
    Vector2 bottom_right;
	bottom_right.x = 10000;
	bottom_right.y = 0;

	List list = state_objects(state, top_left, bottom_right); 
	TEST_ASSERT(list_size(list) != 0); // Ελεγχος αν η λιστα ειναι κενη

	ListNode node = list_first(list); 

	Object object = list_node_value(list, node);
	
    TEST_ASSERT(object->position.x >= top_left.x && 
				object->position.y <= top_left.y && 
				object->position.x <= bottom_right.x && 
				object->position.y >= bottom_right.y);

	// Δευτερη Κληση της state_object:
	// Το Object εχει θεση (286.382477, 182.218079)
	
	// Set Cordinates
	top_left.x = 0;
	top_left.y = 500;
	bottom_right.x = 350;
	bottom_right.y = 0;

	list = state_objects(state, top_left, bottom_right); 
	TEST_ASSERT(list_size(list) != 0);

	node = list_next(list, node);
	
	object = list_node_value(list, node);

    TEST_ASSERT(object->position.x >= top_left.x && 
				object->position.y <= top_left.y && 
				object->position.x <= bottom_right.x && 
				object->position.y >= bottom_right.y);

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

	TEST_ASSERT( vec2_equal( state_info(state)->spaceship->position, (Vector2){0,SPACESHIP_ACCELERATION}) );
	TEST_ASSERT( vec2_equal( state_info(state)->spaceship->speed,    (Vector2){0,SPACESHIP_ACCELERATION}) );

	// Προσθέστε επιπλέον ελέγχους
	keys.up = false;

	// Με πατημενο το RIGHT, το orientation του spaceship αλλαζει 

	state_info(state)->spaceship->orientation.x = 0; 
	state_info(state)->spaceship->orientation.y = 1; // Initial Orientation 

	Vector2 expected_right= vec2_rotate(state_info(state)->spaceship->orientation, SPACESHIP_ROTATION); // Rotates spaceship clockwise

	keys.right = true;
	state_update(state, &keys);

	TEST_ASSERT(vec2_equal(state_info(state)->spaceship->orientation, expected_right));

	keys.right = false;

	// Με πατημενο το LEFT, το orientation του spaceship αλλαζει 

	state_info(state)->spaceship->orientation.x = 0;
	state_info(state)->spaceship->orientation.y = 1; // Initial Orientation 

	Vector2 expected_left = vec2_rotate(state_info(state)->spaceship->orientation, -SPACESHIP_ROTATION); // Rotates spaceship counter-clockwise

	keys.left = true;
	state_update(state, &keys); 

	TEST_ASSERT(vec2_equal(state_info(state)->spaceship->orientation, expected_left));

	keys.left = false;

	// Με πατημενο το P

	keys.p = true;
	state_update(state, &keys); 

	TEST_ASSERT(state_info(state)->paused = true);

	keys.p = false;

	// Ασκηση 3 Tests 

	// Check for asteroid generation

	// Set Cordinates
	Vector2 top_left;
	Vector2 bottom_right;

	top_left.x = 0;
	top_left.y = 300;
	bottom_right.x = 300;
	bottom_right.y = 0;

	List asteroids = state_objects(state, top_left, bottom_right); // 2 asteroids existing at current top_left and bottom_right
	ListNode node;
	int counter = 0;													
	for(node = list_first(asteroids); 
		node != LIST_EOF; 												
		node = list_next(asteroids, node)){

		Object obj = list_node_value(asteroids, node);
		if(obj->type == ASTEROID){									
			counter++;
		}
	}

	int remaining = ASTEROID_NUM - counter;

	while (remaining > 0) {
		Object new_asteroid = malloc(sizeof(*new_asteroid));
		new_asteroid->type = ASTEROID;
		list_insert_next(asteroids, node, new_asteroid);
		remaining--;
	}
	state_update(state, &keys); 
	TEST_ASSERT(list_size(asteroids) == ASTEROID_NUM);

	// Check for Asteroid & Bullet collision

	Object asteroid = malloc(sizeof(*asteroid));
	asteroid->type = ASTEROID;
	asteroid->position = (Vector2){100, 100}; // Same postion for asteroid & bullet
	asteroid->speed = (Vector2){0, 0};
	asteroid->orientation = (Vector2){0, 0};
	asteroid->size = ASTEROID_MAX_SIZE;

	Object bullet = malloc(sizeof(*bullet));
	bullet->type = BULLET;
	bullet->position = (Vector2){100, 100}; // Same postion for asteroid & bullet
	bullet->speed = (Vector2){0, 0};
	bullet->orientation = (Vector2){0, 0};
	bullet->size = BULLET_SIZE;
	state_update(state, &keys); 

	TEST_ASSERT(CheckCollisionCircles(bullet->position, bullet->size, asteroid->position, asteroid->size));

	// Check for Asteroid & Spaceship collision
	
	Object spaceship = malloc(sizeof(*spaceship));
	spaceship->type = SPACESHIP;
	spaceship->position = (Vector2){100, 100}; // Same postion for asteroid & spaceship
	spaceship->speed = (Vector2){0, 0};
	spaceship->orientation = (Vector2){0, 1};
	spaceship->size = SPACESHIP_SIZE;
	state_update(state, &keys); 
	
	TEST_ASSERT(CheckCollisionCircles(spaceship->position, spaceship->size, asteroid->position, asteroid->size));
}
 

// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "test_state_create", test_state_create },
	{ "test_state_update", test_state_update },

	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};
