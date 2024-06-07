#pragma once

#include "raylib.h"
#include "ADTList.h"

// Χαρακτηριστικά αντικειμένων.
#define ASTEROID_NUM 6
#define ASTEROID_MIN_SIZE 10
#define ASTEROID_MAX_SIZE 80
#define ASTEROID_MIN_SPEED 1
#define ASTEROID_MAX_SPEED 1.5
#define ASTEROID_MIN_DIST 300
#define ASTEROID_MAX_DIST 400
#define BULLET_SPEED 10
#define BULLET_SIZE 3
#define BULLET_DELAY 15
#define SPACESHIP_SIZE 20
#define SPACESHIP_ROTATION (PI/32)
#define SPACESHIP_ACCELERATION 0.1
#define SPACESHIP_SLOWDOWN 0.99
#define SPACESHIP_HEALTH 4 // Ζωη παικτη

// Χαρακτηριστικα Εχθρου.
#define ENEMY_NUM 4
#define ENEMY_SPEED 2
#define ENEMY_MIN_DIST 700
#define ENEMY_MAX_DIST 900
#define ENEMY_SIZE 32

// Χαρακτηριστικα Boss.
#define BOSS_SIZE 128
#define BOSS_HEALTH 44
#define BOSS_SPEED 4

// Χαρακτηριστικα τελικου Boss.
#define FINAL_BOSS_SIZE 128
#define FINAL_BOSS_HEALTH 132
#define FINAL_BOSS_SPEED 4


#define HEART_SIZE 64	// Μεγεθος εικονας καρδιας
#define HEART_COUNT 4

#define SCREEN_WIDTH 900	// Πλάτος της οθόνης
#define SCREEN_HEIGHT 700	// Υψος της οθόνης

typedef enum {
	SPACESHIP, ASTEROID, BULLET, PICKUP, ENEMY, BUDDY, BOSS, FINAL_BOSS
} ObjectType;

typedef enum {
	IDLE, JUMPING, FALLING, MOVING_UP, MOVING_DOWN
} VerticalMovement;

typedef struct game_state {
    bool start_menu;			// Αρχικη οθονη
    bool gameplay;				// Το παινχιδι εκει που πυροβολας
    bool introduction;			// Εκει που μιλαει ο αστροναυτης
    bool info_menu;				// Μενου πληροφοριων
    bool game_over;				
	bool game_won;
	bool quit;
} GameState;

// Πληροφορίες για κάθε αντικείμενο.
typedef struct object {
	ObjectType type;			// Τύπος (Διαστημόπλοιο, Αστεροειδής, Σφαίρα)
	Vector2 position;			// Θέση
	Vector2 speed;				// Ταχύτητα (pixels/frame)
	double size;				// Μέγεθος (pixels)
	Vector2 orientation;		// Κατεύθυνση (μόνο για διαστημόπλοιο)
	int health;					// Ζωη
}* Object;

typedef struct shop {
	int more_bullets;			// Item στο shop
	bool buddy;					// Item στο shop
}* Shop;

typedef struct text_info {
    int index;
    int textIndex;
    float timer;
    float delay;
}* TextInfo;

// Γενικές πληροφορίες για την κατάσταση του παιχνιδιού.
typedef struct state_info {
	Object spaceship;				// πληροφορίες για τη το διαστημόπλοιο
	Object buddy;					// πληροφορίες για τη τον βοηθο του διαστημοπλοιου 
	bool paused;					// true αν το παιχνίδι είναι paused
	
	bool lost;						// true αν οι καρδιες ειναι 0
	bool game_won;					// true αν νικησε ο παικτης το τελικο boss στο wave 10

	bool shop_open; 				// true αν το κουμπι "s" πατηθηκε 
	bool purchase_complete;			// true αν ο παικτης εχει αρκετα coins και καταφερε να αγορασει αντικειμενο στο shop
	bool not_enough_coins;			// true αν ο παικτης ειναι αφραγκος 
	int coins;						// τα coins

	bool boss_spawned; 				// Flag για το αν εχει εμφανιστει το boss (wave 5)
	bool boss_died;					// Flag για το αν ζει το boss
	int boss_health;				// Η ζωη του boss 

	bool final_boss_spawned; 		// Flag για το αν εχει εφανιστει το τελικο boss
	bool final_boss_died; 			// Flag για το αν ζει το τελικο boss
	int final_boss_health;			// Η ζωη του τελικου boss
	bool final_boss_attacked;		// Flag για αν εχει επιτεθει το τελικο boss
	int shockwave_timer;			// Timer που οριζει τον χρονο μεχρι την επομενη επιθεση του τελικου boss
}* StateInfo;

typedef struct wave_info {
	int current_wave;				// Το τρεχων wave
    int time_until_next_wave;		// Timer μεχρι το επομενο wave
    int wave_delay;  				// Ο χρονος που περνει το καθε wave
    int enemies_per_wave;			// Οι εχθροι που εχει το καθε wave
}* WaveInfo;

// Πληροφορίες για το ποια πλήκτρα είναι πατημένα.
typedef struct key_state {
	bool up;						// true αν το αντίστοιχο πλήκτρο είναι πατημένο:
	bool left;
	bool right;
	bool enter;
	bool space;
	bool n;
	bool p;
	bool s;
	bool q;
	bool w;
	bool e;
}* KeyState;

typedef struct menu_button{
	bool start;		// Κουμπια στην αρχικη οθονη:
	bool info;
	bool exit;
	int counter;	// Counter ωστε να ξερουμε πιο κουμπι ειναι επιλεγμενο
} MenuButton;

// Η κατάσταση του παιχνιδιού (handle)
typedef struct state* State;

// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού.

State state_create();

// Επιστρέφει τις βασικές πληροφορίες του παιχνιδιού στην κατάσταση state.

StateInfo state_info(State state);

// Επιστρέφει πληοροφοριες για αυτα που λεει ο astronaut.

TextInfo state_text(State state);

// Επιστρέφει πληροφοριες για τα wave.

WaveInfo state_wave(State state);

// Επιστρεφει την ζωη του αντικειμενου Object.

int object_health(Object object);

// Επιστρέφει μια λίστα με όλα τα αντικείμενα του παιχνιδιού στην κατάσταση state,
// των οποίων η θέση position βρίσκεται εντός του παραλληλογράμμου με πάνω αριστερή
// γωνία top_left και κάτω δεξιά bottom_right.

List state_objects(State state, Vector2 top_left, Vector2 bottom_right);

// Ενημερώνει την κατάσταση state του παιχνιδιού μετά την πάροδο 1 frame.
// Το keys περιέχει τα πλήκτρα τα οποία ήταν πατημένα κατά το frame αυτό.

void state_update(State state, KeyState keys);

// Καταστρέφει την κατάσταση state ελευθερώνοντας τη δεσμευμένη μνήμη.

void state_destroy(State state);
