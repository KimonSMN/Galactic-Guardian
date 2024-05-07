
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "ADTVector.h"
#include "ADTList.h"
#include "state.h"
#include "vec2.h"


// Οι ολοκληρωμένες πληροφορίες της κατάστασης του παιχνιδιού.
// Ο τύπος State είναι pointer σε αυτό το struct, αλλά το ίδιο το struct
// δεν είναι ορατό στον χρήστη.

struct state {
	Vector objects;			// περιέχει στοιχεία Object (αστεροειδείς, σφαίρες)
	struct state_info info;	// Γενικές πληροφορίες για την κατάσταση του παιχνιδιού
	int next_bullet;		// Αριθμός frames μέχρι να επιτραπεί ξανά σφαίρα
	float speed_factor;		// Πολλαπλασιαστής ταχύτητς (1 = κανονική ταχύτητα, 2 = διπλάσια, κλπ)
};


// Δημιουργεί και επιστρέφει ένα αντικείμενο

static Object create_object(ObjectType type, Vector2 position, Vector2 speed, Vector2 orientation, double size) {
	Object obj = malloc(sizeof(*obj));
	obj->type = type;
	obj->position = position;
	obj->speed = speed;
	obj->orientation = orientation;
	obj->size = size;
	return obj;
}

// Επιστρέφει έναν τυχαίο πραγματικό αριθμό στο διάστημα [min,max]

static double randf(double min, double max) {
	return min + (double)rand() / RAND_MAX * (max - min);
}

// Προσθέτει num αστεροειδείς στην πίστα (η οποία μπορεί να περιέχει ήδη αντικείμενα).
//
// ΠΡΟΣΟΧΗ: όλα τα αντικείμενα έχουν συντεταγμένες x,y σε ένα καρτεσιανό επίπεδο.
// - Η αρχή των αξόνων είναι η θέση του διαστημόπλοιου στην αρχή του παιχνιδιού
// - Στο άξονα x οι συντεταγμένες μεγαλώνουν προς τα δεξιά.
// - Στον άξονα y οι συντεταγμένες μεγαλώνουν προς τα πάνω.

static void add_asteroids(State state, int num) {
	for (int i = 0; i < num; i++) {
		// Τυχαία θέση σε απόσταση [ASTEROID_MIN_DIST, ASTEROID_MAX_DIST]
		// από το διστημόπλοιο.
		//
		Vector2 position = vec2_add(
			state->info.spaceship->position,
			vec2_from_polar(
				randf(ASTEROID_MIN_DIST, ASTEROID_MAX_DIST),	// απόσταση
				randf(0, 2*PI)									// κατεύθυνση
			)
		);

		// Τυχαία ταχύτητα στο διάστημα [ASTEROID_MIN_SPEED, ASTEROID_MAX_SPEED]
		// με τυχαία κατεύθυνση.
		//
		Vector2 speed = vec2_from_polar(
			randf(ASTEROID_MIN_SPEED, ASTEROID_MAX_SPEED) * state->speed_factor,
			randf(0, 2*PI)
		);

		Object asteroid = create_object(
			ASTEROID,
			position,
			speed,
			(Vector2){0, 0},								// δεν χρησιμοποιείται για αστεροειδείς
			randf(ASTEROID_MIN_SIZE, ASTEROID_MAX_SIZE)		// τυχαίο μέγεθος
		);
		vector_insert_last(state->objects, asteroid);
	}
}

// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού

State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));

	// Γενικές πληροφορίες
	state->info.paused = false;				// Το παιχνίδι ξεκινάει χωρίς να είναι paused.
	state->speed_factor = 1;				// Κανονική ταχύτητα
	state->next_bullet = 0;					// Σφαίρα επιτρέπεται αμέσως
	state->info.score = 0;				// Αρχικό σκορ 0

	// Δημιουργούμε το vector των αντικειμένων, και προσθέτουμε αντικείμενα
	state->objects = vector_create(0, NULL);

	// Δημιουργούμε το διαστημόπλοιο
	state->info.spaceship = create_object(
		SPACESHIP,
		(Vector2){0, 0},			// αρχική θέση στην αρχή των αξόνων
		(Vector2){0, 0},			// μηδενική αρχική ταχύτητα
		(Vector2){0, 1},			// κοιτάει προς τα πάνω
		SPACESHIP_SIZE				// μέγεθος
	);

	// Προσθήκη αρχικών αστεροειδών
	add_asteroids(state, ASTEROID_NUM);

	return state;
}

// Επιστρέφει τις βασικές πληροφορίες του παιχνιδιού στην κατάσταση state

StateInfo state_info(State state) {
	// Προς υλοποίηση checked
	return &(state->info);
}

// Επιστρέφει μια λίστα με όλα τα αντικείμενα του παιχνιδιού στην κατάσταση state,
// των οποίων η θέση position βρίσκεται εντός του παραλληλογράμμου με πάνω αριστερή
// γωνία top_left και κάτω δεξιά bottom_right.


List state_objects(State state, Vector2 top_left, Vector2 bottom_right) {

	List list = list_create(free); // Creates the List to store the data and eventually return them
    
	for (int i = 0; i < vector_size(state->objects); i++) {
        Object object = vector_get_at(state->objects, i);
		if (object->position.x >= top_left.x && object->position.y <= top_left.y && 
			object->position.x <= bottom_right.x && object->position.y >= bottom_right.y) {
			list_insert_next(list, LIST_BOF, object);
		}
    }
    return list;
}

// Ενημερώνει την κατάσταση state του παιχνιδιού μετά την πάροδο 1 frame.
// Το keys περιέχει τα πλήκτρα τα οποία ήταν πατημένα κατά το frame αυτό.

void state_update(State state, KeyState keys) {

	Object spaceship = state->info.spaceship;

	// Κινηση Αντικειμένων

	for (int i = 0; i < vector_size(state->objects); i++) {
		Object obj = vector_get_at(state->objects, i);
		if (obj->type == BULLET || obj->type == ASTEROID) {
            obj->position = vec2_add(obj->position, vec2_scale(obj->speed, state->speed_factor));
		}
	}

	// Περιστροφή διαστημοπλοίου

	if(keys->right){
		spaceship->orientation = vec2_rotate(spaceship->orientation, SPACESHIP_ROTATION);
	}
	if(keys->left){
		spaceship->orientation = vec2_rotate(spaceship->orientation, -SPACESHIP_ROTATION);
	}

	// Επιτάχυνση διαστημοπλοίου

	if (keys->up){
		spaceship->speed = vec2_add(spaceship->speed, vec2_scale(spaceship->orientation, SPACESHIP_ACCELERATION));
	}else{
		// Επιβράδυνση διαστημοπλοίου
    	spaceship->speed = vec2_scale(spaceship->speed, SPACESHIP_SLOWDOWN);
	}

	// Παύση και διακοπή
	if(state->info.paused && !keys->n) //Αν το παιχνίδι είναι σε pause και πατηθεί N τότε ενημερώνεται για μόνο 1 frame
		return;

	if (keys->p) { // Αν πατηθεί P το παιχνίδι μπαίνει σε pause και δεν ενημερώνεται πλέον
        state->info.paused = true;
	}

	// Θεση διαστημοπλοίου

    spaceship->position = vec2_add(spaceship->position, spaceship->speed);

	// Δημιουργία αστεροειδών 

	Vector2 top_left = {
		spaceship->position.x - ASTEROID_MAX_DIST, 
		spaceship->position.y + ASTEROID_MAX_DIST
	};
	Vector2 bottom_right = {
		spaceship->position.x + ASTEROID_MAX_DIST, 
		spaceship->position.y - ASTEROID_MAX_DIST
	};

	List objects = state_objects(state, top_left, bottom_right);
	int asteroids = 0;													// Set asteroid counter to 0
	for(ListNode node = list_first(objects); 
		node != LIST_EOF; 												// Loop through the list
		node = list_next(objects, node)){

		Object obj = list_node_value(objects, node);
		if(obj->type == ASTEROID){										// If list object is asteroid ++ the counter
			asteroids++;
		}
	}
	int remaining = ASTEROID_NUM - asteroids;
	if (remaining > 0) {
		state->info.score += remaining; // Για κάθε νέο αστεροειδή που δημιουργείται το σκορ αυξάνεται κατά 1
		add_asteroids(state, remaining);
	}
	

	// Σφαιρες

	if(keys->space && state->next_bullet <= 0){

		Object bullet = create_object( // Δημιουργεια καινουριας Σφαιρας
			BULLET,
			spaceship->position,
            vec2_add(spaceship->speed, vec2_scale(spaceship->orientation, BULLET_SPEED * state->speed_factor)),
			spaceship->orientation,
			BULLET_SIZE
			);

		if (bullet == NULL) {
        	return;
    	}
	
		vector_insert_last(state->objects, bullet);

		state->next_bullet = BULLET_DELAY; // Οριζεται το delay της σφαιρας σε BULLET_DELAY
	}
	if (state->next_bullet > 0 ) {
    	state->next_bullet--; 
	}

	// Συγκρουσεις Αστεροιδη και Σφαιρας

	for (int i = 0; i < vector_size(state->objects); i++) {
		Object asteroid = vector_get_at(state->objects, i);
		if (asteroid == NULL || asteroid->type != ASTEROID)
			continue;

		for (int j = 0; j < vector_size(state->objects); j++) {
			Object bullet = vector_get_at(state->objects, j);
			if(bullet == NULL || bullet->type != BULLET) 
				continue;
				
			if (CheckCollisionCircles( 
			bullet->position,
			bullet->size,
			asteroid->position,
			asteroid->size
			)){
				if (asteroid->size / 2 >= ASTEROID_MIN_SIZE) { // Πρέπει να είναι τουλάχιστον ASTEROID_MIN_SIZE σε μέγεθος
					for (int k = 0; k < 2; k++) {
						// Υπολογισμος Length
						double length = sqrt(asteroid->speed.x * asteroid->speed.x + asteroid->speed.y * asteroid->speed.y);
						Vector2 asteroid_speed = vec2_from_polar(
							length * state->speed_factor, 
							randf(0, 2*PI) // Τυχαιο angle
						);

						Object new_asteroid = create_object(
							ASTEROID,
							asteroid->position,
							asteroid_speed,
							(Vector2){0, 0},
							asteroid->size/2 // Μισό μέγεθος από τον αρχικό αστεροειδη
							);

						// Προστίθενται δύο νέεοι αστεροειδείς
						vector_insert_last(state->objects, new_asteroid);
					}
				}
				free(asteroid); // Ο αστεροειδής αφαιρείται 
				free(bullet); 	// Η σφαιρα αφαιρείται 
				if (state->info.score > 0) 
					state->info.score -= 10; // Το σκορ μειώνεται κατά 10
				break;	
			}
		}
	}

	// Συγκρουσεις Αστεροιδη και Διαστημοπλοιου

	for (int i = 0; i < vector_size(state->objects); i++) {
		Object asteroid = vector_get_at(state->objects, i);
		if (asteroid == NULL || asteroid->type != ASTEROID) 
			continue;
			
		if (spaceship == NULL || spaceship->type != SPACESHIP) 
			continue; 
		
		// Ελεγχος συγκρουσης διαστημοπλοιο και αστεροειδης 
		if (CheckCollisionCircles(
			spaceship->position,
			spaceship->size,
			asteroid->position,
			asteroid->size
		)) {
			free(asteroid);
			if(state->info.score > 0)
				state->info.score = state->info.score / 2;
			break;
		}
	}

	static int previous_score = 0;

	if (state->info.score / 100 > previous_score / 100) {
		state->speed_factor *= 1.10;   
		previous_score = state->info.score;
	}
		if(state->info.score < 0)
			state->info.score = 0;

}

// Καταστρέφει την κατάσταση state ελευθερώνοντας τη δεσμευμένη μνήμη.

void state_destroy(State state) {

    for (int i = 0; i < vector_size(state->objects); i++) {
        Object obj = vector_get_at(state->objects, i);
        free(obj);
    }
    vector_destroy(state->objects); 

    free(state);
}
