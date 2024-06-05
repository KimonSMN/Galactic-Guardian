
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "ADTVector.h"
#include "ADTList.h"
#include "ADTSet.h"
#include "state.h"
#include "vec2.h"
#include <string.h>


// Οι ολοκληρωμένες πληροφορίες της κατάστασης του παιχνιδιού.
// Ο τύπος State είναι pointer σε αυτό το struct, αλλά το ίδιο το struct
// δεν είναι ορατό στον χρήστη.

struct state {
	Set objects;			// περιέχει στοιχεία Object (αστεροειδείς, σφαίρες)
	struct state_info info;	// Γενικές πληροφορίες για την κατάσταση του παιχνιδιού
	int next_bullet;		// Αριθμός frames μέχρι να επιτραπεί ξανά σφαίρα
    int buddy_next_bullet;
	float speed_factor;		// Πολλαπλασιαστής ταχύτητς (1 = κανονική ταχύτητα, 2 = διπλάσια, κλπ)
	int pickupTimer;
	int pauseTimer; 
    int purchaseTimer;
	struct text_info text;
	struct wave_info wave;
    struct shop shop;
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

static void enemy_creation(State state, int enemy_num);

static void spawn_boss(State state);

static void bullet_creation(State state, KeyState keys);

static void buddy_bullet_creation(State state);

static void spaceship_pickup_collision(State state);

static void asteroid_bullet_collision(State state);

static void spaceship_asteroid_collision(State state);

static void spaceship_enemy_collision(State state);

static void spaceship_boss_collision(State state);

static void enemy_bullet_collision(State state);

static void boss_bullet_collision(State state);


// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού

State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));

	// Γενικές πληροφορίες
	state->info.paused = false;				// Το παιχνίδι ξεκινάει χωρίς να είναι paused.
    state->info.shop_open = false;
    state->speed_factor = 1;				// Κανονική ταχύτητα
	state->next_bullet = 0;					// Σφαίρα επιτρέπεται αμέσως
    state->buddy_next_bullet = 0;
	state->info.coins = 0;
	state->pickupTimer = 0;
	state->info.lost = false;
	state->pauseTimer = 0; 
	state->text.textIndex = 0;
	state->text.delay = 0;
	state->text.index = 0;
	state->text.timer = 0;
    
    state->info.purchase_complete = false;
    state->info.not_enough_coins = false;

    state->info.boss_spawned = false;
    state->info.boss_died = true;
    state->info.boss_health = 0;

    state->purchaseTimer = 0;

    state->wave.current_wave = 0;
    state->wave.time_until_next_wave = 0;
    state->wave.wave_delay = 1000; // 2000 ~30 sec 
    state->wave.enemies_per_wave = 10;   // Initial number of enemies per wave

    state->shop.more_bullets = 1;
    state->shop.buddy = false;

    state->info.buddy = NULL;

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

	return state;
}

static void manage_waves(State state) {
    
    if (state->wave.time_until_next_wave > 0) {
        state->wave.time_until_next_wave--;
    } else {
        
        state->wave.current_wave += 1;

        // removes enemies from last wave

        SetNode node = set_first(state->objects);
        while (node != SET_EOF) {
            Object obj = set_node_value(state->objects, node);
            if (obj->type == ENEMY) {
                node = set_next(state->objects, node);
                set_remove(state->objects, obj);
                free(obj);
            } else {
                node = set_next(state->objects, node);
            }
        }

        printf("NEW WAVE CREATED: %d\n", state->wave.current_wave);
        state->wave.enemies_per_wave += state->wave.current_wave / 3 ;
        state->wave.time_until_next_wave = state->wave.wave_delay;
    }

    if (state->wave.current_wave == 5 && !state->info.boss_spawned) {
        spawn_boss(state);
        state->wave.time_until_next_wave = 2000000;
        state->info.boss_spawned = true;
        state->info.boss_died = false;
    } else if(state->wave.current_wave == 5 && state->info.boss_died){
        state->wave.time_until_next_wave = 0;
    }
    
    enemy_creation(state, state->wave.enemies_per_wave);
  
    asteroid_creation(state); 
}

// Επιστρέφει τις βασικές πληροφορίες του παιχνιδιού στην κατάσταση state

StateInfo state_info(State state) {
	// Προς υλοποίηση checked
	return &(state->info);
}

TextInfo state_text(State state) {
	return &(state->text);
}

WaveInfo state_wave(State state) {
	return &(state->wave);
}
int object_health(Object object) {
    return object->health;
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

Vector2 vec2_normalize(Vector2 vec) {
    float length = sqrt(vec.x * vec.x + vec.y * vec.y);
	return (Vector2){vec.x / length, vec.y / length};
}
static void check_overlap(Object a, Object b, float distance) {
    Rectangle a_hitbox = { a->position.x, a->position.y, a->size * 2, a->size * 2 };
    Rectangle b_hitbox = { b->position.x, b->position.y, b->size * 2, b->size * 2};

    if (CheckCollisionRecs(a_hitbox, b_hitbox)) {

        // Calculate the vector between the centers of the two objects
        Vector2 a_center = { a->position.x + a->size, a->position.y + a->size };
        Vector2 b_center = { b->position.x + b->size, b->position.y + b->size };
        Vector2 direction = vec2_normalize(vec2_subtract(b_center, a_center));

        // Gradually push objects away from each other by the specified distance
        Vector2 adjustment = vec2_scale(direction, distance);
        a->position = vec2_subtract(a->position, adjustment);
        b->position = vec2_add(b->position, adjustment);
    }
}


// Ενημερώνει την κατάσταση state του παιχνιδιού μετά την πάροδο 1 frame.
// Το keys περιέχει τα πλήκτρα τα οποία ήταν πατημένα κατά το frame αυτό.
void state_update(State state, KeyState keys) {
    Object spaceship = state->info.spaceship;
    Vector2 top_left = {spaceship->position.x - 2 * SCREEN_HEIGHT, spaceship->position.y + 2 * SCREEN_HEIGHT};
    Vector2 bottom_right = {spaceship->position.x + 2 * SCREEN_HEIGHT, spaceship->position.y - 2 * SCREEN_HEIGHT};

    if (state->pauseTimer > 0) {
        state->pauseTimer--;
    }

    if (keys->p && state->pauseTimer == 0) {
        state->info.paused = !state->info.paused;
        state->pauseTimer = 20;
        printf("Game paused: %d\n", state->info.paused);
    }

    if (keys->s && state->pauseTimer == 0) {
        state->info.shop_open = !state->info.shop_open;
        state->pauseTimer = 20;
        printf("Shop opened: %d\n", state->info.shop_open);
    }

    if (state->info.shop_open) {
        if (state->purchaseTimer > 0) {
            state->purchaseTimer--;
        } else {
            state->info.purchase_complete = false;
            state->info.not_enough_coins = false;
        }

        if (keys->w && state->shop.more_bullets < 3 && state->purchaseTimer == 0) {
            if (state->info.coins >= 1000) {
                state->shop.more_bullets++;
                state->info.coins -= 1000;
                state->purchaseTimer = 120;
                printf("BOUGHT ITEM\n");
                state->info.purchase_complete = true;
            } else {
                printf("NOT ENOUGH COINS\n");
                state->info.not_enough_coins = true;
                state->purchaseTimer = 60;
            }
        } else if (keys->q && state->purchaseTimer == 0 && state->info.spaceship->health < 4) {
            if (state->info.coins >= 250) {
                state->info.spaceship->health++;
                state->info.coins -= 250;
                state->purchaseTimer = 120;
                state->info.purchase_complete = true;
                printf("BOUGHT ITEM\n");
            } else {
                printf("NOT ENOUGH COINS\n");
                state->info.not_enough_coins = true;
                state->purchaseTimer = 60;

            }
        } else if(keys->e && state->purchaseTimer == 0 && !state->shop.buddy){
            if (state->info.coins >= 2000) {
                state->shop.buddy = true;
                state->info.coins -= 2000;
                state->purchaseTimer = 120;
                state->info.purchase_complete = true;
                printf("BOUGHT ITEM\n");

               state->info.buddy = create_object(
                    BUDDY,
                    (Vector2){spaceship->position.x - SPACESHIP_SIZE, spaceship->position.y - SPACESHIP_SIZE}, 
                    (Vector2){0, 0},
                    (Vector2){0, 1},
                    SPACESHIP_SIZE / 2,
                    0
                );
            } else {
                printf("NOT ENOUGH COINS\n");
                state->info.not_enough_coins = true;
                state->purchaseTimer = 60;

            }
        }
        return; 
    }


    if (!state->info.shop_open) {
        state->info.purchase_complete = false;
        state->info.not_enough_coins = false;
        state->purchaseTimer = 0;
    }

    if (state->info.paused) {
        return;
    }

    manage_waves(state);

    List objects_to_update = state_objects(state, top_left, bottom_right);

    for (ListNode node = list_first(objects_to_update);
        node != LIST_EOF;
        node = list_next(objects_to_update, node)) {

        Object obj = list_node_value(objects_to_update, node);
        if (obj->type == BULLET || obj->type == ASTEROID ) {
            set_remove(state->objects, obj);
            obj->position = vec2_add(obj->position, vec2_scale(obj->speed, state->speed_factor));
            set_insert(state->objects, obj);
        } else if (obj->type == ENEMY) {
            Vector2 direction = vec2_from_to(obj->position, state->info.spaceship->position);
            direction = vec2_normalize(direction);
            Vector2 velocity = vec2_scale(direction, ENEMY_SPEED);
            set_remove(state->objects, obj);
            obj->position = vec2_add(obj->position, vec2_scale(velocity, state->speed_factor));
            set_insert(state->objects, obj);
        } else if (obj->type == BOSS) {
            Vector2 direction = vec2_from_to(obj->position, state->info.spaceship->position);
            direction = vec2_normalize(direction);
            Vector2 velocity = vec2_scale(direction, BOSS_SPEED);
            set_remove(state->objects, obj);
            obj->position = vec2_add(obj->position, vec2_scale(velocity, state->speed_factor));
            set_insert(state->objects, obj);
        }
    }

    // check for overlaps between enemies
    for (ListNode nodeA = list_first(objects_to_update);
        nodeA != LIST_EOF;
        nodeA = list_next(objects_to_update, nodeA)) {

        Object enemy_a = list_node_value(objects_to_update, nodeA);
        if (enemy_a->type != ENEMY) {
            continue;
        }

        for (ListNode nodeB = list_next(objects_to_update, nodeA); // Start from the next node to avoid self-comparison
            nodeB != LIST_EOF;
            nodeB = list_next(objects_to_update, nodeB)) {

            Object enemy_b = list_node_value(objects_to_update, nodeB);
            if (enemy_b->type == ENEMY) {
                float distance = 0.5; 
                set_remove(state->objects, enemy_a);
                set_remove(state->objects, enemy_b);
                check_overlap(enemy_a, enemy_b, distance);
                set_insert(state->objects, enemy_a);
                set_insert(state->objects, enemy_b);
            }
        }
    }

    if (spaceship->health <= 0) {
        state->info.lost = true;
        printf("YOU HAVE LOST THE GAME TRY AGAIN FOO\n");
        return;
    }
    if (keys->right) {
        spaceship->orientation = vec2_rotate(spaceship->orientation, SPACESHIP_ROTATION);
    }
    if (keys->left) {
        spaceship->orientation = vec2_rotate(spaceship->orientation, -SPACESHIP_ROTATION);
    }
    if (keys->up) {
        spaceship->speed = vec2_add(spaceship->speed, vec2_scale(spaceship->orientation, SPACESHIP_ACCELERATION));
    } 
    else {
        spaceship->speed = vec2_scale(spaceship->speed, SPACESHIP_SLOWDOWN);
    }

    spaceship->position = vec2_add(spaceship->position, spaceship->speed);

    if (state->info.buddy) {
        state->info.buddy->position = (Vector2){spaceship->position.x - SPACESHIP_SIZE * 2, spaceship->position.y - SPACESHIP_SIZE * 2};
    }


    asteroid_creation(state);
    bullet_creation(state, keys);
    buddy_bullet_creation(state);
    spaceship_pickup_collision(state);
    asteroid_bullet_collision(state);
    spaceship_asteroid_collision(state);
    enemy_bullet_collision(state);
    boss_bullet_collision(state);
    spaceship_enemy_collision(state);
    spaceship_boss_collision(state);
}


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
			// free(asteroid);
			state->info.spaceship->health --;
			break;
		}
	}
}

static void asteroid_bullet_collision(State state) {
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
        bool asteroid_removed = false;

        for (ListNode bullet_node = list_first(bullets_list);
            bullet_node != LIST_EOF;
            bullet_node = list_next(bullets_list, bullet_node)) {

            Object bullet = list_node_value(bullets_list, bullet_node);
            if (bullet == NULL)
				continue;

            if (CheckCollisionCircles(
                    bullet->position,
                    bullet->size,
                    asteroid->position,
                    asteroid->size)) {

                if (asteroid->size / 2 >= ASTEROID_MIN_SIZE) {
                    for (int k = 0; k < 2; k++) {
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
                            asteroid->size / 2, // Μισό μέγεθος από τον αρχικό αστεροειδή
                            0
                        );

                        set_insert(state->objects, new_asteroid);
                    }
                }

                const int pickup_probability_percent = 5;
                int random_value = rand() % 100 + 1;
                if (random_value <= pickup_probability_percent) {
                    Object new_pickup = create_object(
                        PICKUP,
                        asteroid->position,
                        (Vector2){0, 0},
                        (Vector2){0, 0},
                        PICKUP_SIZE,
                        0
                    );
                    set_insert(state->objects, new_pickup);
                }

                set_remove(state->objects, asteroid);
                asteroid_removed = true;
                // free(asteroid);

                set_remove(state->objects, bullet);
                // free(bullet);

				state->info.coins += 10;
                break;
            }
        }

        if (asteroid_removed)
            break;
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
			state->pickupTimer = 300;  
			set_remove(state->objects,pickup);
			// free(pickup);
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

        Object bullets[3];
        Vector2 positions[3] = {
            spaceship->position,
            vec2_add(spaceship->position, offset),
            vec2_subtract(spaceship->position, offset)
        };

        for (int i = 0; i < state->shop.more_bullets; i++) {
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
        

        // Reset bullet firing delay
        state->next_bullet = BULLET_DELAY;
        printf("Created bullets\n");
    }
}

static void buddy_bullet_creation(State state) {


    if (state->buddy_next_bullet > 0) {
        state->buddy_next_bullet--;
    }

    if (state->buddy_next_bullet <= 0 && state->info.buddy) {
        Object buddy = state->info.buddy;

        Vector2 top_left = {
            buddy->position.x - 400, 
            buddy->position.y + 400
        };
        Vector2 bottom_right = {
            buddy->position.x + 400, 
            buddy->position.y - 400
        };

        List objects = state_objects(state, top_left, bottom_right);
        for(ListNode node = list_first(objects);
            node != LIST_EOF;
            node = list_next(objects, node)){

            Object obj = list_node_value(objects, node);
            if(obj->type == ENEMY){
                Vector2 direction_to_enemy = vec2_normalize(vec2_from_to(buddy->position, obj->position));

                buddy->orientation = direction_to_enemy; // note that if i change this to . position i can create something like a bomb

                Object bullet = create_object(
                    BULLET,
                    buddy->position,
                    vec2_add(buddy->speed, vec2_scale(direction_to_enemy, BULLET_SPEED * state->speed_factor)),
                    direction_to_enemy,
                    BULLET_SIZE,
                    0
                );
            
                set_insert(state->objects, bullet);
                
                // Reset bullet firing delay
                state->buddy_next_bullet = 200;
                printf("Created bullets\n");
            }
        }
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
		add_asteroids(state, remaining);
	}
}

static void enemy_creation(State state, int enemy_num){
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
	int enemies = 0;													// Set asteroid counter to 0
	for(ListNode node = list_first(objects); 
		node != LIST_EOF; 												// Loop through the list
		node = list_next(objects, node)){

		Object obj = list_node_value(objects, node);
		if(obj->type == ENEMY){										// If list object is asteroid ++ the counter
			enemies++;
		}
	}
	int remaining = enemy_num - enemies;
	if (remaining > 0) {
		add_enemies(state, remaining);
	}
}

void spawn_boss(State state) {
    Vector2 position = vec2_add(
			state->info.spaceship->position,
			vec2_from_polar(
				randf(ENEMY_MIN_DIST, ENEMY_MAX_DIST),	// απόσταση
				randf(0, 2*PI)							// κατεύθυνση
			)
		);

		Vector2 speed = vec2_from_polar(
			BOSS_SPEED * state->speed_factor,
			randf(0, 2*PI)
		);

		Object boss = create_object(
			BOSS,
			position,
			speed,
			(Vector2){0, 0},
			BOSS_SIZE,
			BOSS_HEALTH
		);
    state->info.boss_health = BOSS_HEALTH;
    set_insert(state->objects, boss);
}


static void enemy_bullet_collision(State state) {

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
    for (ListNode enemy_node = list_first(enemy_list);
         enemy_node != LIST_EOF;
         enemy_node = list_next(enemy_list, enemy_node)) {

        Object enemy = list_node_value(enemy_list, enemy_node);

        for (ListNode bullet_node = list_first(bullets_list);
             bullet_node != LIST_EOF;
             bullet_node = list_next(bullets_list, bullet_node)) {

            Object bullet = list_node_value(bullets_list, bullet_node);

            Rectangle enemy_box = {
                enemy->position.x - (ENEMY_SIZE * 3 / 2),
                enemy->position.y - (ENEMY_SIZE * 3 / 2),
                ENEMY_SIZE * 3,
                ENEMY_SIZE * 3 
            };

            if (CheckCollisionCircleRec(bullet->position, bullet->size, enemy_box)) {
                enemy->health--;
                set_remove(state->objects, bullet);
                // free(bullet);

                if (enemy->health <= 0) {
                    set_remove(state->objects, enemy);
                    // free(enemy);
					state->info.coins += 40;
                }
            }
        }
    }
    list_destroy(enemy_list);
    list_destroy(bullets_list);
}

static void boss_bullet_collision(State state) {
    Object boss = NULL;

    for (SetNode node = set_first(state->objects);
         node != SET_EOF;
         node = set_next(state->objects, node)) {

        Object obj = set_node_value(state->objects, node);
        if (obj->type == BOSS) {
            boss = obj;
            break;
        }
    }

    if (boss == NULL) return;

    List bullets_list = list_create(NULL);

    for (SetNode node = set_first(state->objects);
         node != SET_EOF;
         node = set_next(state->objects, node)) {

        Object obj = set_node_value(state->objects, node);
        if (obj->type == BULLET) {
            list_insert_next(bullets_list, LIST_BOF, obj);
        }
    }

    for (ListNode bullet_node = list_first(bullets_list);
         bullet_node != LIST_EOF;
         bullet_node = list_next(bullets_list, bullet_node)) {

        Object bullet = list_node_value(bullets_list, bullet_node);

        Rectangle boss_box = {
            boss->position.x - (BOSS_SIZE * 3 / 2),
            boss->position.y - (BOSS_SIZE * 3 / 2),
            BOSS_SIZE * 2  ,
            BOSS_SIZE * 2
        };  

        if (CheckCollisionCircleRec(bullet->position, bullet->size, boss_box)) {
            boss->health--;
            state->info.boss_health--;
            set_remove(state->objects, bullet);
            // free(bullet);

            if (boss->health <= 0) {
                set_remove(state->objects, boss);
                // free(boss);
                state->info.coins += 1000; 
                state->info.boss_died = true;
            }
        }
    }
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
			// free(enemy);
			printf("COLLIEDED WITH ENEMY");
			state->info.spaceship->health--;
			break;
		}
	}
}

static void spaceship_boss_collision(State state){
	Object spaceship = state->info.spaceship;

	float search_radius = 2 * ENEMY_MAX_DIST;

	List boss_in_range = state_objects(state,
											(Vector2){spaceship->position.x - search_radius,spaceship->position.y + search_radius}, 
											(Vector2){spaceship->position.x + search_radius,spaceship->position.y - search_radius}
											);

	for(ListNode node = list_first(boss_in_range); 
		node != LIST_EOF; 						
		node = list_next(boss_in_range, node)){

		Object boss = list_node_value(boss_in_range, node);
		if (boss == NULL || boss->type != BOSS) 
			continue;

		if (spaceship == NULL || spaceship->type != SPACESHIP) 
			continue; 

		// Ελεγχος συγκρουσης διαστημοπλοιο και αστεροειδης 
		if (CheckCollisionCircles(
			spaceship->position,
			spaceship->size,
			boss->position,
			boss->size
		)) {
			set_remove(state->objects,boss);
			// free(boss);
			printf("COLLIEDED WITH BOSS");
			state->info.spaceship->health-= 4;
			break;
		}
	}
}