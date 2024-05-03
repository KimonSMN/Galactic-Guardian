#include "raylib.h"
#include <math.h>

#include "../include/state.h"
#include "../include/interface.h"


// Assets
Texture spaceship_img;


// Αρχικοποιεί το interface του παιχνιδιού
void interface_init(){
    // Αρχικοποίηση του παραθύρου
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "asteroids");
	SetTargetFPS(60);
    InitAudioDevice();
	

	// Φόρτωση εικόνων
	spaceship_img = LoadTextureFromImage(LoadImage("assets/spaceship.png"));
}

// Κλείνει το interface του παιχνιδιού
void interface_close(){
	CloseAudioDevice();
	CloseWindow();
}

// Σχεδιάζει ένα frame με την τωρινή κατάσταση του παιχνδιού
void interface_draw_frame(State state) {

	Camera2D camera; // Αρχικοποιηση camera

	camera.offset = (Vector2){SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}; // window origin
	camera.target = (Vector2){0, 0}; 
	camera.rotation = 0;
	camera.zoom = 1;
	
	// Καμερα ακολουθει διαστημοπλοιο 
    camera.target.x = state_info(state)->spaceship->position.x;
    camera.target.y = state_info(state)->spaceship->position.y;

    BeginDrawing();
	BeginMode2D(camera); 
    ClearBackground(BLACK);

    float radians = atan2(
						state_info(state)->spaceship->orientation.x,
						state_info(state)->spaceship->orientation.y
						);

    float rotation = radians * (-180.0 / PI);

    Rectangle source = {0, 0, spaceship_img.width, spaceship_img.height};
    Rectangle dest = {
                    state_info(state)->spaceship->position.x,
                    state_info(state)->spaceship->position.y, 
                    spaceship_img.width, 
                    spaceship_img.height
                    };
                    
    Vector2 origin = {dest.width / 2, dest.height / 2};

    DrawTexturePro(spaceship_img, source, dest, origin, rotation, WHITE);

 
	Vector2 top_left = {
		state_info(state)->spaceship->position.x - ASTEROID_MAX_DIST, 
		state_info(state)->spaceship->position.y + ASTEROID_MAX_DIST
	}; // Set top_left
	Vector2 bottom_right = {
		state_info(state)->spaceship->position.x  + ASTEROID_MAX_DIST, 
		state_info(state)->spaceship->position.y - ASTEROID_MAX_DIST
	};	// Set bottom_right

    List objects_in_range = state_objects(state,top_left,bottom_right);
    
    for(ListNode node = list_first(objects_in_range); 
        node != LIST_EOF;
		node = list_next(objects_in_range, node)){

        Object object = list_node_value(objects_in_range, node);
        if (object->type == ASTEROID) {
            DrawRectangle(object->position.x, object->position.y, object->size, object->size, RED);
        }
        else
        {
            DrawCircle(object->position.x, object->position.y, BULLET_SIZE, BLUE);
        }
            
    }
    EndMode2D();

    // Draw the score and the FPS counter
    DrawText(TextFormat("%04i", state_info(state)->score), 780, 20, 40, WHITE);
    DrawFPS(0, 0);


     EndDrawing();
}
