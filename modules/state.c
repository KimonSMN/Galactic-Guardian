
#include <stdlib.h>
#include <stdio.h>

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
	// Προς υλοποίηση
	// if(keys->enter )

	// Press N //

    if (state->info.paused && !keys->n) {
        return;
    }

    if (state->info.paused && keys->n) {
		state->info.paused = false;
    }

    Object spaceship = state->info.spaceship;
	Object bullet = malloc(sizeof(struct object));

	// Orientation of Spaceship //

	if(keys->right){
		spaceship->orientation = vec2_rotate(spaceship->orientation, -SPACESHIP_ROTATION);
	}
	if(keys->left){
		spaceship->orientation = vec2_rotate(spaceship->orientation, SPACESHIP_ROTATION);
	}

	// Speed of Spaceship //

	if (keys->up){
		spaceship->speed = vec2_add(spaceship->speed, vec2_scale(spaceship->orientation, SPACESHIP_ACCELERATION)); // Handle Acceleration
	}else{
		// Check if speed is greater than 0 (speed >= 0)
    	spaceship->speed = vec2_scale(spaceship->speed, SPACESHIP_SLOWDOWN); // Handle Slowdown
	}

	// Position of Spaceship //

	spaceship->position = vec2_add(spaceship->position, spaceship->speed);


	// Δημιουργία αστεροειδών //

	Vector2 top_left = {spaceship->position.x,ASTEROID_MAX_DIST}; 		// Set top_left
	Vector2 bottom_left = {ASTEROID_MAX_DIST,spaceship->position.y};	// Set bottom_right

	List objects = state_objects(state, top_left, bottom_left);
	int asteroids = 0;													// Set asteroid counter to 0
	for(ListNode node = list_first(objects); 
		node != LIST_EOF; 												// Loop through the list
		node = list_next(objects, node)){

		Object obj = list_node_value(objects, node);
		if(obj->type == ASTEROID){										// If object of list is asteroid ++ the counter
			asteroids++;
		}
	}
	if (asteroids != ASTEROID_NUM){										// If asteroids != ASTEROID_NUM create more asteroids
		int remaining = ASTEROID_NUM - asteroids;
		if (remaining > 0) {
			state->info.score += remaining; // Για κάθε νέο αστεροειδή που δημιουργείται το σκορ αυξάνεται κατά 1
        	add_asteroids(state, remaining);
    	}
	}

	// Bullets

	if(keys->space && state->next_bullet <= 0){
		Object bullet = malloc(sizeof(Object));
		if (bullet == NULL) {
        	return;
    }
  		bullet->type = BULLET;			// Set the type to BULLET
    	bullet->size = BULLET_SIZE;		// Set the size of the bullet to BULLET_SIZE

    	bullet->speed = vec2_add(spaceship->speed, vec2_scale(spaceship->orientation, BULLET_SPEED));
		
		bullet->position = spaceship->position;

		vector_insert_last(state->objects, bullet);

		state->next_bullet = BULLET_DELAY;
	}
	if (state->next_bullet > 0 ) {
    	state->next_bullet--;
	}

	// Συγκρουσεις

	for (int i = 0; i < vector_size(state->objects); i++) {
		Object obj = vector_get_at(state->objects, i);
		if(obj == NULL)
			continue;
		
		if(obj->type == ASTEROID &&
		CheckCollisionCircles(obj->position,
		obj->size,
		bullet->position,
		bullet->size
		)){
			
			vector_set_at(state->objects, i , NULL);

			if (obj->size/2 >= ASTEROID_MIN_SIZE ){			// Πρέπει να είναι τουλάχιστον ASTEROID_MIN_SIZE σε μέγεθος
				for (int i = 0; i < 2; i++) {

					Vector2 position = vec2_add(
						state->info.spaceship->position,
						vec2_from_polar(
							randf(ASTEROID_MIN_DIST, ASTEROID_MAX_DIST),	// απόσταση
							randf(0, 2*PI)									// κατεύθυνση
						)
					);

					Vector2 speed = vec2_from_polar(
						obj->speed.x * 1.5 * state->speed_factor, 	// Μήκος 1,5 φορά μεγαλύτερο της ταχύτητας του αρχικού
						randf(0, 2*PI)
					);

					Object asteroid = create_object(
						ASTEROID,
						position,
						speed,
						(Vector2){0, 0},		
						obj->size/2		// Το μισό μέγεθος από τον αρχικό
					);
					vector_insert_last(state->objects, asteroid);
				}
			}
			
			state->info.score -= 10;	//Για κάθε αστεροειδή που συγκρούεται με σφαίρα το σκορ μειώνεται κατά 10
		}

	}


	for (int i = 0; i < vector_size(state->objects); i++) {
		Object obj = vector_get_at(state->objects, i);
		if(obj == NULL)
			continue;
		if (obj->type == ASTEROID && 
			CheckCollisionCircles( // Check for collision διαστημόπλοιο με αστεροειδή
			obj->position,
			obj->size,
			spaceship->position,
			spaceship->size
			)){ 
			state->info.score = state->info.score / 2; // Αν το διαστημόπλοιο συγκρουστεί με αστεροειδή χάνεται το μισό σκορ
		}
	}

	if (state->info.score % 100 == 0){
		state->speed_factor *= 1.10;	// Η ταχύτητα του παιχνιδιού γίνεται 10% μεγαλύτερη
	}

	// Pause
	if (keys->p){
		state->info.paused = true;
		return;
	}else{
		state->info.paused = false;
	}
}

// Καταστρέφει την κατάσταση state ελευθερώνοντας τη δεσμευμένη μνήμη.

void state_destroy(State state) {
	// Προς υλοποίηση
}
