
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "ADTVector.h"
#include "ADTList.h"
#include "ADTSet.h"
#include "set_utils.h"
#include "state.h"
#include "vec2.h"


// Οι ολοκληρωμένες πληροφορίες της κατάστασης του παιχνιδιού.
// Ο τύπος State είναι pointer σε αυτό το struct, αλλά το ίδιο το struct
// δεν είναι ορατό στον χρήστη.

struct state {
	Set objects_set;		// περιέχει στοιχεία Object (αστεροειδείς, σφαίρες)
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

static int compare_objects(Pointer a_ptr, Pointer b_ptr) {
    Object a = a_ptr;
    Object b = b_ptr;

    if (a->position.x < b->position.x)
        return -1;
    else if (a->position.x > b->position.x)
        return 1;

    if (a->position.y < b->position.y)
        return -1;
    else if (a->position.y > b->position.y)
        return 1;

    if (a < b)
        return -1;
    else if (a > b)
        return 1;

    return 0;
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

		set_insert(state->objects_set, asteroid);
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

	// Δημιουργούμε το set των αντικειμένων, και προσθέτουμε αντικείμενα
	state->objects_set = set_create(compare_objects, NULL);

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
    List result_list = list_create(NULL);

    Object search_obj = create_object(ASTEROID,
									(Vector2){top_left.x, bottom_right.y}, 
									(Vector2){0, 0},
									(Vector2){0, 0},
									0);
    Object start_obj = set_find_eq_or_greater(state->objects_set, search_obj);
    
    SetNode node = set_find_node(state->objects_set, start_obj);
	
    while (node != SET_EOF) {
        Object object_to_add = set_node_value(state->objects_set, node);
        if (object_to_add == NULL) {
            break;
        }
        if (object_to_add->position.x >= top_left.x && 
            object_to_add->position.y <= top_left.y && 
            object_to_add->position.x <= bottom_right.x && 
            object_to_add->position.y >= bottom_right.y) {
            list_insert_next(result_list, LIST_BOF, object_to_add);
        }

        if (object_to_add->position.x > bottom_right.x) {
            break;
        }
        
        node = set_next(state->objects_set, node);
    }
    return result_list;
}

// Ενημερώνει την κατάσταση state του παιχνιδιού μετά την πάροδο 1 frame.
// Το keys περιέχει τα πλήκτρα τα οποία ήταν πατημένα κατά το frame αυτό.

void state_update(State state, KeyState keys) {

	Object spaceship = state->info.spaceship;

	Vector2 top_left = {spaceship->position.x - 2 * SCREEN_HEIGHT, spaceship->position.y + 2 * SCREEN_HEIGHT};
	Vector2 bottom_right = {spaceship->position.x + 2 * SCREEN_HEIGHT, spaceship->position.y - 2 * SCREEN_HEIGHT};

	// Κινηση Αντικειμένων

	List objects_to_update = state_objects(state, top_left, bottom_right);

	for(ListNode node = list_first(objects_to_update); 
		node != LIST_EOF; 						
		node = list_next(objects_to_update, node)){

		Object obj = list_node_value(objects_to_update, node);
		if (obj->type == BULLET || obj->type == ASTEROID ) {
			set_remove(state->objects_set,obj);
            obj->position = vec2_add(obj->position, vec2_scale(obj->speed, state->speed_factor));
			set_insert(state->objects_set,obj);
		}
	}
	// Παύση και διακοπή
	if(state->info.paused && !keys->n)
		return;

	if (keys->p) {
        state->info.paused = true;
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

	// Θεση διαστημοπλοίου

	spaceship->position = vec2_add(spaceship->position, spaceship->speed);

	// Δημιουργία αστεροειδών 

	Vector2 top_left_asteroids = {
		spaceship->position.x - ASTEROID_MAX_DIST, 
		spaceship->position.y + ASTEROID_MAX_DIST
	};
	Vector2 bottom_right_asteroids = {
		spaceship->position.x + ASTEROID_MAX_DIST, 
		spaceship->position.y - ASTEROID_MAX_DIST
	};

	List objects = state_objects(state, top_left_asteroids, bottom_right_asteroids);
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
	
		set_insert(state->objects_set, bullet);

		state->next_bullet = BULLET_DELAY; // Οριζεται το delay της σφαιρας σε BULLET_DELAY
	}
	if (state->next_bullet > 0 ) {
    	state->next_bullet--; 
	}
	
	// Συγκρούσεις Αστεροειδή και Σφαίρας
	
	List asteroid_list = list_create(NULL);
	List bullets_list = list_create(NULL);

	for (SetNode node = set_first(state->objects_set);
		node != SET_EOF;
		node = set_next(state->objects_set, node)) {

		Object obj = set_node_value(state->objects_set, node);
		if (obj->type == ASTEROID) {
			list_insert_next(asteroid_list, LIST_BOF, obj);
		} else if (obj->type == BULLET) {
			list_insert_next(bullets_list, LIST_BOF, obj);
		}
	}

	// Συγκρουσεις Αστεροιδη και Σφαιρας
	for (ListNode asteroid_node = list_first(asteroid_list);
		asteroid_node != LIST_EOF;
		asteroid_node = list_next(asteroid_list, asteroid_node)) {

		Object asteroid = list_node_value(asteroid_list, asteroid_node);

		for (ListNode bullet_node = list_first(bullets_list);
			bullet_node != LIST_EOF;
			bullet_node = list_next(bullets_list, bullet_node)) {

			Object bullet = list_node_value(bullets_list, bullet_node);

			if (CheckCollisionCircles(
					bullet->position,
					bullet->size,
					asteroid->position,
					asteroid->size)) {

				if (asteroid->size / 2 >= ASTEROID_MIN_SIZE) {
					for (int k = 0; k < 2; k++) {
						// Υπολογισμός Length
						double length = sqrt(asteroid->speed.x * asteroid->speed.x + asteroid->speed.y * asteroid->speed.y);
						Vector2 asteroid_speed = vec2_from_polar(
							length * state->speed_factor,
							randf(0, 2 * PI) // Τυχαιο angle
						);

						Object new_asteroid = create_object(
							ASTEROID,
							asteroid->position,
							asteroid_speed,
							(Vector2){0, 0},
							asteroid->size / 2 // Μισό μέγεθος από τον αρχικό αστεροειδη
						);

						// Προστίθενται δύο νέοι αστεροειδείς
						set_insert(state->objects_set, new_asteroid);
					}
				}

				set_remove(state->objects_set, asteroid);
				free(asteroid);

				set_remove(state->objects_set, bullet);
				free(bullet);

				if (state->info.score > 0) 
					state->info.score -= 10;
				break;
			}
		}
	}

	list_destroy(asteroid_list);
	list_destroy(bullets_list);

	// Συγκρουσεις Αστεροιδη και Διαστημοπλοιου

	float search_radius = 2 * ASTEROID_MAX_DIST;

	List asteroids_in_range = state_objects(state,
											(Vector2){spaceship->position.x - search_radius,spaceship->position.y + search_radius}, 
											(Vector2){spaceship->position.x + search_radius,spaceship->position.y - search_radius}
											);

	for(ListNode node = list_first(asteroids_in_range); 
		node != LIST_EOF; 						
		node = list_next(asteroids_in_range, node)){

		Object asteroid = list_node_value(asteroids_in_range, node);
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
			set_remove(state->objects_set,asteroid);
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
	// Προς υλοποίηση
}
