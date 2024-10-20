
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "interface.h"

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

// Compare function
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

// Επιστρέφει την μοναδική τιμή του set που είναι ισοδύναμη με value,
// ή αν δεν υπάρχει, την μικρότερη τιμή του set που είναι μεγαλύτερη
// από value. Αν δεν υπάρχει καμία τότε επιστρέφει NULL.

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
		Vector2 speed = vec2_from_polar(
			randf(ASTEROID_MIN_SPEED, ASTEROID_MAX_SPEED) * state->speed_factor,
			randf(0, 2*PI)
		);

		Object asteroid = create_object(
			ASTEROID,
			position,
			speed,
			(Vector2){0, 0},								// δεν χρησιμοποιείται για αστεροειδείς
			randf(ASTEROID_MIN_SIZE, ASTEROID_MAX_SIZE),	// τυχαίο μέγεθος
			0
		);
		set_insert(state->objects, asteroid);
	}
}


// Προσθέτει num εχθρους στην πίστα.

static void add_enemies(State state, int num) {
	for (int i = 0; i < num; i++) {

		// Τυχαία θέση σε απόσταση από το διστημόπλοιο.
		Vector2 position = vec2_add(
			state->info.spaceship->position,
			vec2_from_polar(
				randf(ENEMY_MIN_DIST, ENEMY_MAX_DIST),	// απόσταση
				randf(0, 2*PI)							// κατεύθυνση
			)
		);

		// Τυχαία ταχύτητα στο διάστημα με τυχαία κατεύθυνση.
		Vector2 speed = vec2_from_polar(
			ENEMY_SPEED * state->speed_factor,
			randf(0, 2*PI)
		);

		Object enemy = create_object(
			ENEMY,
			position,
			speed,
			(Vector2){0, 0},    // δεν χρησιμοποιείται για εχθρους
			ENEMY_SIZE,
			3                   // ζωη αντικειμενου
		);
		set_insert(state->objects, enemy);
	}
}


// Forward function declarations  

// Δημιουργια αστεροειδων 
static void asteroid_creation(State state);

// Δημιουργια εχθρων
static void enemy_creation(State state, int enemy_num);

// Δημιουργια Boss
static void spawn_boss(State state, ObjectType type, int boss_health, int boss_speed, int boss_size);

// Δημιουργια σφαιρών
static void bullet_creation(State state, KeyState keys);

// Δημιουργια σφαιρών του βοηθού (αναβαθμιση στο shop)
static void buddy_bullet_creation(State state);

// Συγκρουση αστεροειδη και σφαιρας
static void asteroid_bullet_collision(State state);

// Συγκρουση διαστημοπλοιου και αστεροειδη
static void spaceship_asteroid_collision(State state);

// Συγκρουση διαστημοπλοιου και εχθρου
static void spaceship_enemy_collision(State state);

// Συγκρουση διαστημοπλοιου και boss
static void spaceship_boss_collision(State state);

// Συγκρουση εχθρου και σφαιρας
static void enemy_bullet_collision(State state);

// Συγκρουση boss και σφαιρας
static void boss_bullet_collision(State state);

// Συγκρουση τελικου boss και σφαιρας
static void final_boss_bullet_collision(State state);

// Επιθεση (shockwave) τελικου boss
static void final_boss_shockwave(State state);

// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού
State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));

	// Γενικές πληροφορίες
	state->info.paused = false;				// Το παιχνίδι ξεκινάει χωρίς να είναι paused.
    state->info.shop_open = false;          // Το shop δεν ειναι ανοιχτο
    state->speed_factor = 1;				// Κανονική ταχύτητα
	state->next_bullet = 0;					// Σφαίρα επιτρέπεται αμέσως
	state->info.coins = 0;                  // Μηδεν αρχικα coins

	state->info.lost = false;       // Flag για να μπορεσει να εμφανιστει το game over screen
    state->info.game_won = false;   // Flag για να μπορεσει να εμφανιστει το win screen

	state->pauseTimer = 0; // Timer για το pause

    // Πληροφοριες για το text του astronaut
	state->text.textIndex = 0;
	state->text.delay = 0;
	state->text.index = 0;
	state->text.timer = 0;

    // Πληροφοριες για τον βοηθο (αναβαθμιση χαρακτηρα στο shop)
    state->info.buddy = NULL;
    state->buddy_next_bullet = 0;           // Σφαίρα buddy επιτρέπεται αμέσως

    // Πληροφοριες για το shop
    state->info.purchase_complete = false;
    state->info.not_enough_coins = false;
    state->purchaseTimer = 0;
    state->shop.more_bullets = 1;
    state->shop.buddy = false;

    // Πληροφοριες για boss 
    state->info.boss_spawned = false;
    state->info.boss_died = true;
    state->info.boss_health = 0;

    // Πληροφοριες τελικο για boss 
    state->info.final_boss_spawned= false;
    state->info.final_boss_died = true;
    state->info.final_boss_health = 0;
    state->info.final_boss_attacked = false;

    // Πληροφοριες για το καθε wave 
    state->wave.current_wave = 0;
    state->wave.time_until_next_wave = 0;
    state->wave.wave_delay = 1500; // 2000 ~30 sec 
    state->wave.enemies_per_wave = 10; 

	// Δημιουργούμε το vector των αντικειμένων, και προσθέτουμε αντικείμενα
	state->objects = set_create(compare_objects, NULL);

	// Δημιουργούμε το διαστημόπλοιο
	state->info.spaceship = create_object(
		SPACESHIP,
		(Vector2){0, 0},			// αρχική θέση στην αρχή των αξόνων
		(Vector2){0, 0},			// μηδενική αρχική ταχύτητα
		(Vector2){0, 1},			// κοιτάει προς τα πάνω
		SPACESHIP_SIZE,				// μέγεθος
		SPACESHIP_HEALTH            // Ζωη
	);

	return state;
}

// Function για την δημιουργεια waves

static void manage_waves(State state) {
    
    if (state->wave.time_until_next_wave > 0) {
        state->wave.time_until_next_wave--;
    } else {
        
        state->wave.current_wave += 1; // Αυξανει το wave κατα 1

        // Αφαιρει τους εχθρους του προηγουμενου wave

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

        state->wave.enemies_per_wave += state->wave.current_wave / 3 ;
        state->wave.time_until_next_wave = state->wave.wave_delay; // Οριζει το ποση ωρα θελει για το επομενο wave κατα wave_delay
    }

    // Εδω εμφανιζεται το πρωτο boss στο wave 5
    if (state->wave.current_wave == 5 && !state->info.boss_spawned) {
        spawn_boss(state,BOSS,BOSS_HEALTH, BOSS_SPEED, BOSS_SIZE);
        state->wave.time_until_next_wave = 2000000;
        state->info.boss_spawned = true;
        state->info.boss_died = false;
    } else if(state->wave.current_wave == 5 && state->info.boss_died){
        state->wave.time_until_next_wave = 0;
    
    // Εδω εμφανιζεται το τελικο boss στο wave 10
    } else if(state->wave.current_wave == 10 && !state->info.final_boss_spawned){
        spawn_boss(state,FINAL_BOSS ,FINAL_BOSS_HEALTH, FINAL_BOSS_SPEED, FINAL_BOSS_SIZE);
        state->wave.time_until_next_wave = 2000000;
        state->info.final_boss_spawned = true;
        state->info.final_boss_died = false;
    }else if(state->wave.current_wave == 10 && state->info.final_boss_died){
        state->wave.time_until_next_wave = 0;
    }
    
    enemy_creation(state, state->wave.enemies_per_wave);
  
    asteroid_creation(state); 
}

// Επιστρέφει τις βασικές πληροφορίες του παιχνιδιού στην κατάσταση state
StateInfo state_info(State state) {
	return &(state->info);
}
// Επιστρέφει πληοροφοριες για αυτα που λεει ο astronaut.
TextInfo state_text(State state) {
	return &(state->text);
}
// Επιστρέφει πληροφοριες για τα wave.
WaveInfo state_wave(State state) {
	return &(state->wave);
}
// Επιστρεφει την ζωη του αντικειμενου Object.
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

// Κανει το vector2 normalize
Vector2 vec2_normalize(Vector2 vec) {
    float length = sqrt(vec.x * vec.x + vec.y * vec.y);
	return (Vector2){vec.x / length, vec.y / length};
}

// Function για ελεγχο εχθρων αν κανουν overlap
static void check_overlap(Object a, Object b, float distance) {
    Rectangle a_hitbox = { a->position.x, a->position.y, a->size * 2, a->size * 2 };
    Rectangle b_hitbox = { b->position.x, b->position.y, b->size * 2, b->size * 2};

    if (CheckCollisionRecs(a_hitbox, b_hitbox)) {

        // Υπολογιζεται το διανυσμα μεταξυ των κεντρων τον object
        Vector2 a_center = { a->position.x + a->size, a->position.y + a->size };
        Vector2 b_center = { b->position.x + b->size, b->position.y + b->size };
        Vector2 direction = vec2_normalize(vec2_subtract(b_center, a_center));

        // Σπρωχνει τα object 
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

    // Ελεγχος Timer παυσης
    if (state->pauseTimer > 0) {
        state->pauseTimer--;
    }

    // Ενεργοποιηση ή Απενεργοποιηση παυσης του παιχνιδιου με το πληκτρο 'P'
    if (keys->p && state->pauseTimer == 0) {
        state->info.paused = !state->info.paused;
        state->pauseTimer = 20;
        printf("Game paused: %d\n", state->info.paused);
    }

    // Ενεργοποιηση ή Απενεργοποιηση shop με το πληκτρο 'S'
    if (keys->s && state->pauseTimer == 0) {
        state->info.shop_open = !state->info.shop_open;
        state->pauseTimer = 20;
        printf("Shop opened: %d\n", state->info.shop_open);
    }

    // Αγορες του shop
    if (state->info.shop_open) {
        if (state->purchaseTimer > 0) { // Timer ωστε να μην επιτρεπεται η αμεση αγορα μετα την προηγουμενη 
            state->purchaseTimer--;
        } else {
            state->info.purchase_complete = false;
            state->info.not_enough_coins = false;
        }

        // Αναβαθμιση (αγορα) σφαιρων (παραπανω σφαιρες)  
        if (keys->w && state->shop.more_bullets < 3 && state->purchaseTimer == 0) {
            if (state->info.coins >= 1000) { // Τιμη της αναβαθμισης
                state->shop.more_bullets++;
                state->info.coins -= 1000; 
                state->purchaseTimer = 120;
                state->info.purchase_complete = true;
            } else {
                state->info.not_enough_coins = true;
                state->purchaseTimer = 60;
            }
        } else if (keys->q && state->purchaseTimer == 0 && state->info.spaceship->health < 4) {
            if (state->info.coins >= 250) { // Τιμη της αγορας
                state->info.spaceship->health++;
                state->info.coins -= 250;
                state->purchaseTimer = 120;
                state->info.purchase_complete = true;
            } else {
                state->info.not_enough_coins = true;
                state->purchaseTimer = 60;

            }
        } else if(keys->e && state->purchaseTimer == 0 && !state->shop.buddy){
            if (state->info.coins >= 2000) { // Τιμη της αναβαθμισης
                state->shop.buddy = true;
                state->info.coins -= 2000;
                state->purchaseTimer = 120;
                state->info.purchase_complete = true;

                // Δημιουργια βοηθου 
               state->info.buddy = create_object(
                    BUDDY,
                    (Vector2){spaceship->position.x - SPACESHIP_SIZE, spaceship->position.y - SPACESHIP_SIZE}, 
                    (Vector2){0, 0},
                    (Vector2){0, 1},
                    SPACESHIP_SIZE / 2,
                    0                   // δεν εχει ζωη εφοσων δεν μπορει να πεθανει
                );
            } else {
                state->info.not_enough_coins = true;
                state->purchaseTimer = 60;

            }
        }
        return; 
    }

    // Οταν το shop ειναι κλειστο γινονται reset τα flags
    if (!state->info.shop_open) {
        state->info.purchase_complete = false;
        state->info.not_enough_coins = false;
        state->purchaseTimer = 0;
    }

    // Ελεγχος παυσης παιχνιδιου
    if (state->info.paused) {
        return;
    }

    // Επιθεση του τελικου boss
    static int shockwave_timer = 0;
    if (state->info.final_boss_spawned && !state->info.final_boss_died) {
        shockwave_timer++;
        if (shockwave_timer >= 300) {
            final_boss_shockwave(state);
            shockwave_timer = 0;
        }
    }

    if (state->info.final_boss_attacked) {
        state->info.shockwave_timer--;
        if (state->info.shockwave_timer <= 0) {
            state->info.final_boss_attacked = false;
        }
    }   

    // Διαχειριση waves
    manage_waves(state);


    // Οποιο Object προκειται να κουνηθει, πρεπει να βγει απο το set, να αλλαξει το position του και να ξαναμπει.

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
        } else if (obj->type == FINAL_BOSS) {
            Vector2 direction = vec2_from_to(obj->position, state->info.spaceship->position);
            direction = vec2_normalize(direction);
            Vector2 velocity = vec2_scale(direction, FINAL_BOSS_SPEED);
            set_remove(state->objects, obj);
            obj->position = vec2_add(obj->position, vec2_scale(velocity, state->speed_factor));
            set_insert(state->objects, obj);
        }
    }

    // Ελεγχος overlap εχθρων 
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

    // Ελεγχος ζωης παικτη
    if (spaceship->health <= 0) {
        state->info.lost = true;
        return;
    }

    // Ελεχγοι μετακινησης παικτη:

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

    // Ελεγος μετακινησης βοηθου

    if (state->info.buddy) {
        state->info.buddy->position = (Vector2){spaceship->position.x - SPACESHIP_SIZE * 2, spaceship->position.y - SPACESHIP_SIZE * 2};
    }


    asteroid_creation(state);
    bullet_creation(state, keys);
    buddy_bullet_creation(state);
    asteroid_bullet_collision(state);
    spaceship_asteroid_collision(state);
    enemy_bullet_collision(state);
    spaceship_enemy_collision(state);
    boss_bullet_collision(state);
    spaceship_boss_collision(state);
    final_boss_bullet_collision(state);
}


// Καταστρέφει την κατάσταση state ελευθερώνοντας τη δεσμευμένη μνήμη.
void state_destroy(State state) {
	free(state);
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
            play_sound(2); // player damaged sound
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
        
        play_sound(1); // laser sound

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
            buddy->position.x - 700, 
            buddy->position.y + 700
        };
        Vector2 bottom_right = {
            buddy->position.x + 700, 
            buddy->position.y - 700
        };

        List objects = state_objects(state, top_left, bottom_right);
        for(ListNode node = list_first(objects);
            node != LIST_EOF;
            node = list_next(objects, node)){

            Object obj = list_node_value(objects, node);
            if(obj->type == ENEMY || obj->type == BOSS ||obj->type == FINAL_BOSS){
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

                play_sound(1); // laser sound

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

void spawn_boss(State state, ObjectType type, int boss_health, int boss_speed, int boss_size) {
    Vector2 position = vec2_add(
			state->info.spaceship->position,
			vec2_from_polar(
				randf(ENEMY_MIN_DIST, ENEMY_MAX_DIST),	// απόσταση
				randf(0, 2*PI)							// κατεύθυνση
			)
		);

		Vector2 speed = vec2_from_polar(
			boss_speed * state->speed_factor,
			randf(0, 2*PI)
		);

		Object boss = create_object(
			type,
			position,
			speed,
			(Vector2){0, 0},
			boss_size,
			boss_health
		);
    state->info.boss_health = boss_health;
    set_insert(state->objects, boss);
    play_sound(6); // boss roar
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

                 play_sound(3); // enemy damaged sound

                if (enemy->health <= 0) {
                    set_remove(state->objects, enemy);
                    // free(enemy);
					state->info.coins += 20;
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
    if (boss == NULL) 
        return;
    
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
            play_sound(4); // boss damaged sound
            set_remove(state->objects, bullet);
            // free(bullet);

            if (boss->health <= 0) {
                set_remove(state->objects, boss);
                // free(boss);
                play_sound(5); // boss died sound
                state->info.coins += 1000; 
                state->info.boss_died = true;
            }
        }
    }
    list_destroy(bullets_list);
}       

static void final_boss_bullet_collision(State state) {
    Object boss = NULL;

    for (SetNode node = set_first(state->objects);
         node != SET_EOF;
         node = set_next(state->objects, node)) {

        Object obj = set_node_value(state->objects, node);
        if (obj->type == FINAL_BOSS) {
            boss = obj;
            break;
        }
    }
    if (boss == NULL) 
        return;
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
            boss->position.x - (FINAL_BOSS_SIZE * 2 / 2),
            boss->position.y - (FINAL_BOSS_SIZE * 2 / 2),
            FINAL_BOSS_SIZE * 2  ,
            FINAL_BOSS_SIZE * 2
        };  

        if (CheckCollisionCircleRec(bullet->position, bullet->size, boss_box)) {
            boss->health--;
            state->info.boss_health--;
            play_sound(4); // boss damaged sound
            set_remove(state->objects, bullet);
            // free(bullet);

            if (boss->health <= 0) {
                set_remove(state->objects, boss);
                // free(boss);
                play_sound(5); // boss died sound
                state->info.coins += 1000; 
                state->info.boss_died = true;
                state->info.game_won = true;
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

		// Ελεγχος συγκρουσης διαστημοπλοιο και εχθρου 
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
            play_sound(2); // player damaged sound

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
        if (boss == NULL || (boss->type != BOSS && boss->type != FINAL_BOSS)) 
            continue;

        if (spaceship == NULL || spaceship->type != SPACESHIP) 
            continue; 

        // Ελεγχος συγκρουσης διαστημοπλοιο και bosses 
        if (CheckCollisionCircles(
            spaceship->position,
            spaceship->size,
            boss->position,
            boss->size
        )) {
            if (boss->type == BOSS) {
                state->info.spaceship->health -= 4;
            } else if (boss->type == FINAL_BOSS) {
                state->info.spaceship->health -= 1;
            }
            play_sound(2); // player damaged sound
            printf("COLLIEDED WITH BOSS or FINAL_BOSS");
            break;
        }
    }

    list_destroy(boss_in_range);
}


static void final_boss_shockwave(State state) {
    Object boss = NULL;
    for (SetNode node = set_first(state->objects); node != SET_EOF; node = set_next(state->objects, node)) {
        Object obj = set_node_value(state->objects, node);
        if (obj->type == FINAL_BOSS) {
            boss = obj;
            break;
        }
    }

    float shockwave_radius = 300; 
    if (CheckCollisionCircles(boss->position, shockwave_radius, state->info.spaceship->position, state->info.spaceship->size)) {
        state->info.spaceship->health -= 1; 
        play_sound(2);
    }

    state->info.final_boss_attacked = true;
    state->info.shockwave_timer = 150;
}