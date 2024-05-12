
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "ADTVector.h"
#include "ADTList.h"
#include "ADTSet.h"
#include "state.h"
#include "vec2.h"


// Οι ολοκληρωμένες πληροφορίες της κατάστασης του παιχνιδιού.
// Ο τύπος State είναι pointer σε αυτό το struct, αλλά το ίδιο το struct
// δεν είναι ορατό στον χρήστη.

struct state {
	Set objects;			// περιέχει στοιχεία Object (αστεροειδείς, σφαίρες)
	struct state_info info;	// Γενικές πληροφορίες για την κατάσταση του παιχνιδιού
	int next_bullet;		// Αριθμός frames μέχρι να επιτραπεί ξανά σφαίρα
	float speed_factor;		// Πολλαπλασιαστής ταχύτητς (1 = κανονική ταχύτητα, 2 = διπλάσια, κλπ)
	int pickupTimer;
	};


// Δημιουργεί και επιστρέφει ένα αντικείμενο

static Object create_object(ObjectType type, Vector2 position, Vector2 speed, Vector2 orientation, double size, int health) {
	Object obj = malloc(sizeof(*obj));
	obj->type = type;
	obj->position = position;
	obj->speed = speed;
	obj->orientation = orientation;
	obj->size = size;
	obj->health = health;
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

static Pointer set_find_eq_or_greater(Set set, Pointer value){
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


static Rectangle enemyToRectangle(Object obj) {
    Rectangle rect;
    rect.x = obj->position.x - (obj->size / 2); // Offset by half the width
    rect.y = obj->position.y - (obj->size / 2); // Offset by half the height
    rect.width = obj->size * 1.5;
    rect.height = obj->size * 1.5;
    return rect;
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
			randf(ASTEROID_MIN_SIZE, ASTEROID_MAX_SIZE),		// τυχαίο μέγεθος
			0
		);
		set_insert(state->objects, asteroid);
	}
}

static void add_enemies(State state, int num) {
	for (int i = 0; i < num; i++) {
		// Τυχαία θέση σε απόσταση [ASTEROID_MIN_DIST, ASTEROID_MAX_DIST]
		// από το διστημόπλοιο.
		//
		Vector2 position = vec2_add(
			state->info.spaceship->position,
			vec2_from_polar(
				randf(ENEMY_MIN_DIST, ENEMY_MAX_DIST),	// απόσταση
				randf(0, 2*PI)							// κατεύθυνση
			)
		);

		// Τυχαία ταχύτητα στο διάστημα [ASTEROID_MIN_SPEED, ASTEROID_MAX_SPEED]
		// με τυχαία κατεύθυνση.
		//
		Vector2 speed = vec2_from_polar(
			ENEMY_SPEED * state->speed_factor,
			randf(0, 2*PI)
		);

		Object enemy = create_object(
			ENEMY,
			position,
			speed,
			(Vector2){0, 0},								// δεν χρησιμοποιείται για αστεροειδείς
			ENEMY_SIZE,
			3
		);
		set_insert(state->objects, enemy);
	}
}

// Forward function declarations  
static void spaceship_pickup_collision(State state);

static void asteroid_creation(State state);

static void enemy_creation(State state);

static void bullet_creation(State state, KeyState keys);

static void spaceship_pickup_collision(State state);

static void asteroid_bullet_collision(State state);

static void spaceship_asteroid_collision(State state);

static void spaceship_enemy_collision(State state);

static void enemy_bullet_collision(State state);

// Fix enemy_bullet_collision, maybe add one collision for everything

// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού

State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));

	// Γενικές πληροφορίες
	state->info.paused = false;				// Το παιχνίδι ξεκινάει χωρίς να είναι paused.
	state->speed_factor = 1;				// Κανονική ταχύτητα
	state->next_bullet = 0;					// Σφαίρα επιτρέπεται αμέσως
	state->info.score = 0;					// Αρχικό σκορ 0
	state->pickupTimer = 0;
	state->info.lost = false;

	// Δημιουργούμε το vector των αντικειμένων, και προσθέτουμε αντικείμενα
	state->objects = set_create(compare_objects, NULL);

	// Δημιουργούμε το διαστημόπλοιο
	state->info.spaceship = create_object(
		SPACESHIP,
		(Vector2){0, 0},			// αρχική θέση στην αρχή των αξόνων
		(Vector2){0, 0},			// μηδενική αρχική ταχύτητα
		(Vector2){0, 1},			// κοιτάει προς τα πάνω
		SPACESHIP_SIZE,				// μέγεθος
		SPACESHIP_HEALTH
	);

	// Προσθήκη αρχικών αστεροειδών
	add_asteroids(state, ASTEROID_NUM);

	add_enemies(state, ENEMY_NUM);
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
									0,
									0);
    Object start_obj = set_find_eq_or_greater(state->objects, search_obj);
    
    SetNode node = set_find_node(state->objects, start_obj);
	
    while (node != SET_EOF) {
        Object object_to_add = set_node_value(state->objects, node);
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
        
        node = set_next(state->objects, node);
    }
    return result_list;
}

Vector2 vec2_normalize(Vector2 v) {
    float length = sqrt(v.x * v.x + v.y * v.y);
    if (length != 0) {
        return (Vector2){v.x / length, v.y / length};
    }
    return (Vector2){0, 0};  // Return zero vector if input has zero length
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
			set_remove(state->objects,obj);
            obj->position = vec2_add(obj->position, vec2_scale(obj->speed, state->speed_factor));
			set_insert(state->objects,obj);
		} else if (obj->type == ENEMY) {

        Vector2 direction = vec2_from_to(obj->position, state->info.spaceship->position);
        direction = vec2_normalize(direction); 

        Vector2 velocity = vec2_scale(direction, ENEMY_SPEED);

        set_remove(state->objects, obj);
        obj->position = vec2_add(obj->position, velocity);
        set_insert(state->objects, obj);
    }
	}

	// Health handle
	if(spaceship->health <= 0){
		state->info.lost = true;
		printf("YOU HAVE LOST THE GAME TRY AGAIN FOO\n");
		return;
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


	static int previous_score = 0;

	if (state->info.score / 100 > previous_score / 100) {
		state->speed_factor *= 1.10;   
		previous_score = state->info.score;
		printf("SPEED FACTOR CHANGED\n");
	}
	if(state->info.score < 0)
		state->info.score = 0;


	asteroid_creation(state);

	enemy_creation(state);

	bullet_creation(state, keys);

	spaceship_pickup_collision(state);

	asteroid_bullet_collision(state);

	spaceship_asteroid_collision(state);

	enemy_bullet_collision(state);

	spaceship_enemy_collision(state);

}

// INCLUDE THE SPEED FACTOR!!!!!!!!!!!  ⬆️⬆️⬆️⬆️⬆️⬆️⬆️⬆️⬆️⬆️⬆️⬆️


// Καταστρέφει την κατάσταση state ελευθερώνοντας τη δεσμευμένη μνήμη.

void state_destroy(State state) {
	// Προς υλοποίηση
}


static void spaceship_asteroid_collision(State state){

    Object spaceship = state->info.spaceship;

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
			set_remove(state->objects,asteroid);
			free(asteroid);
			state->info.spaceship->health --;
			if(state->info.score > 0)
				state->info.score = state->info.score / 2;
			break;
		}
	}
}

static void asteroid_bullet_collision(State state){

	// Συγκρούσεις Αστεροειδή και Σφαίρας
	
	List asteroid_list = list_create(NULL);
	List bullets_list = list_create(NULL);

	for (SetNode node = set_first(state->objects);
		node != SET_EOF;
		node = set_next(state->objects, node)) {

		Object obj = set_node_value(state->objects, node);
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
							asteroid->size / 2, // Μισό μέγεθος από τον αρχικό αστεροειδη
							0
						);

						// Προστίθενται δύο νέοι αστεροειδείς
						set_insert(state->objects, new_asteroid);

					}

				}

				const int pickup_probability_percent = 20;

				int random_value = rand() % 100 + 1;
				if(random_value <= pickup_probability_percent){
					// Creating a new PICKUP object where the asteroid was broken
					Object new_pickup = create_object(
						PICKUP,              // Object type
						asteroid->position,  // Position at the location of the broken asteroid
						(Vector2){0, 0},     // Zero velocity for the pickup object
						(Vector2){0, 0},     // Zero acceleration
						PICKUP_SIZE,    // Set size of pickup, adjust based on game design
						0
					);
					set_insert(state->objects, new_pickup); // Add the pickup to the game state
					printf("PICKUP CREATED LEGOOO\n");
				}


				set_remove(state->objects, asteroid);
				free(asteroid);

				set_remove(state->objects, bullet);
				free(bullet);

				if (state->info.score > 0) 
					state->info.score -= 10;
				break;
			}
		}
	}

	list_destroy(asteroid_list);
	list_destroy(bullets_list);
}

static void spaceship_pickup_collision(State state){
	Object spaceship = state->info.spaceship;

	float search_radius = 2 * ASTEROID_MAX_DIST;

	List pickups_in_range = state_objects(state,
											(Vector2){spaceship->position.x - search_radius,spaceship->position.y + search_radius}, 
											(Vector2){spaceship->position.x + search_radius,spaceship->position.y - search_radius}
											);

	for(ListNode node = list_first(pickups_in_range); 
		node != LIST_EOF; 						
		node = list_next(pickups_in_range, node)){

		Object pickup = list_node_value(pickups_in_range, node);
		if (pickup == NULL || pickup->type != PICKUP) 
			continue;

		if (spaceship == NULL || spaceship->type != SPACESHIP) 
			continue; 

		// Ελεγχος συγκρουσης διαστημοπλοιο και αστεροειδης 
		if (CheckCollisionCircles(
			spaceship->position,
			spaceship->size,
			pickup->position,
			pickup->size
		)) {
			state->pickupTimer = 250;  
			set_remove(state->objects,pickup);
			free(pickup);
			printf("PICKUP ACTUIALLY PICKED UP OLOLOLOL");
			break;
		}
	}
	state->pickupTimer--;
}

static void bullet_creation(State state, KeyState keys) {

    if (state->next_bullet > 0) {
        state->next_bullet--;
    }

    if (keys->space && state->next_bullet <= 0) {
        Object spaceship = state->info.spaceship;
        Vector2 perpendicular = {-spaceship->orientation.y, spaceship->orientation.x};
        Vector2 offset = vec2_scale(perpendicular, 20);

        // Determine if special bullet mode is active
        if (state->pickupTimer > 0) {
            // Create three bullets in perpendicular directions
            Object bullets[3];
            Vector2 positions[3] = {
                vec2_add(spaceship->position, offset),
                spaceship->position,
                vec2_subtract(spaceship->position, offset)
            };

            for (int i = 0; i < 3; i++) {
                bullets[i] = create_object(
                    BULLET,
                    positions[i],
            		vec2_add(spaceship->speed, vec2_scale(spaceship->orientation, BULLET_SPEED * state->speed_factor)),
                    spaceship->orientation,
                    BULLET_SIZE,
                    0
                );
                if (bullets[i] == NULL) {
                    printf("Failed to create bullet\n");
                    continue;
                }
				set_insert(state->objects, bullets[i]);
            }
        } else {
            // Create a single bullet
            Object bullet = create_object(
                BULLET,
                spaceship->position,
                vec2_add(spaceship->speed, vec2_scale(spaceship->orientation, BULLET_SPEED)),
                spaceship->orientation,
                BULLET_SIZE,
                0
            );
            if (bullet == NULL) {
                printf("Failed to create bullet\n");
                return;
            }
			set_insert(state->objects, bullet);
        }

        // Reset bullet firing delay
        state->next_bullet = BULLET_DELAY;
        printf("Created bullet(s)\n");
    }
}


static void asteroid_creation(State state){
	Object spaceship = state->info.spaceship;

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
		printf("Created %d new asteroids\n", remaining);
	}
}

static void enemy_creation(State state){
	// Enemy creation
	Object spaceship = state->info.spaceship;

	Vector2 top_left = {
		spaceship->position.x - ENEMY_MAX_DIST, 
		spaceship->position.y + ENEMY_MAX_DIST
	};
	Vector2 bottom_right = {
		spaceship->position.x + ENEMY_MAX_DIST, 
		spaceship->position.y - ENEMY_MAX_DIST
	};

	List objects = state_objects(state, top_left, bottom_right);
	int enemies = 0;												
	for(ListNode node = list_first(objects); 
		node != LIST_EOF; 												
		node = list_next(objects, node)){

		Object obj = list_node_value(objects, node);
		if(obj->type == ENEMY){										
			enemies++;
		}
	}
	int remaining = ENEMY_NUM - enemies;
	if (remaining > 0) {
		add_enemies(state, remaining);
		printf("Created %d new enemies\n", remaining);
	}
}

static void enemy_bullet_collision(State state){

	// Συγκρούσεις Αστεροειδή και Σφαίρας
	
	List enemy_list = list_create(NULL);
	List bullets_list = list_create(NULL);

	for (SetNode node = set_first(state->objects);
		node != SET_EOF;
		node = set_next(state->objects, node)) {

		Object obj = set_node_value(state->objects, node);
		if (obj->type == ENEMY) {
			list_insert_next(enemy_list, LIST_BOF, obj);
		} else if (obj->type == BULLET) {
			list_insert_next(bullets_list, LIST_BOF, obj);
		}
	}

	// Συγκρουσεις Εχθρου και Σφαιρας
	for (ListNode enemy_node = list_first(enemy_list);
		enemy_node != LIST_EOF;
		enemy_node = list_next(enemy_list, enemy_node)) {

		Object enemy = list_node_value(enemy_list, enemy_node);

		for (ListNode bullet_node = list_first(bullets_list);
			bullet_node != LIST_EOF;
			bullet_node = list_next(bullets_list, bullet_node)) {

			Object bullet = list_node_value(bullets_list, bullet_node);

			Rectangle enemyRect = enemyToRectangle(enemy);

			if (CheckCollisionCircleRec(
					bullet->position,
					bullet->size,
					enemyRect)) {
	
				enemy->health--;
				printf(" ENEMY HIT \n");
				set_remove(state->objects, bullet);
				free(bullet);

				if(enemy->health <= 0){
					set_remove(state->objects, enemy);
					free(enemy);
				}
			}
		}
	}

	list_destroy(enemy_list);
	list_destroy(bullets_list);
}

static void spaceship_enemy_collision(State state){
	Object spaceship = state->info.spaceship;

	float search_radius = 2 * ENEMY_MAX_DIST;

	List enemies_in_range = state_objects(state,
											(Vector2){spaceship->position.x - search_radius,spaceship->position.y + search_radius}, 
											(Vector2){spaceship->position.x + search_radius,spaceship->position.y - search_radius}
											);

	for(ListNode node = list_first(enemies_in_range); 
		node != LIST_EOF; 						
		node = list_next(enemies_in_range, node)){

		Object enemy = list_node_value(enemies_in_range, node);
		if (enemy == NULL || enemy->type != ENEMY) 
			continue;

		if (spaceship == NULL || spaceship->type != SPACESHIP) 
			continue; 

		// Ελεγχος συγκρουσης διαστημοπλοιο και αστεροειδης 
		if (CheckCollisionCircles(
			spaceship->position,
			spaceship->size,
			enemy->position,
			enemy->size
		)) {
			set_remove(state->objects,enemy);
			free(enemy);
			printf("COLLIEDED WITH ENEMY");
			state->info.spaceship->health--;
			break;
		}
	}
	state->pickupTimer--;
}