#include "raylib.h"
#include <math.h>

#include "../include/state.h"
#include "../include/interface.h"

// Assets
Texture spaceship_img;
Texture asteroid_img;
Texture bullet_img;

// Αρχικοποιεί το interface του παιχνιδιού

void interface_init(){
    // Initialize the window
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "asteroids");
	SetTargetFPS(60);
    InitAudioDevice();
	
	// Load images
	spaceship_img = LoadTextureFromImage(LoadImage("assets/spaceship.png"));
    asteroid_img = LoadTextureFromImage(LoadImage("assets/asteroid.png"));
    bullet_img = LoadTextureFromImage(LoadImage("assets/bullet.png"));
    //background_img = LoadTextureFromImage(LoadImage("assets/space.png"));
    spaceship_img.height = spaceship_img.height * 3;
    spaceship_img.width = spaceship_img.width * 3;

}

// Κλείνει το interface του παιχνιδιού
void interface_close(){
	CloseAudioDevice();
	CloseWindow();
}

// Σχεδιάζει ένα frame με την τωρινή κατάσταση του παιχνδιού
void interface_draw_frame(State state) {
    int scale_factor = 5;

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

    float rotation = radians * (-180 / PI);
    
    rotation += 180; // add 180 for spaceship to look at correct angle
    
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
	};
	Vector2 bottom_right = {
		state_info(state)->spaceship->position.x  + ASTEROID_MAX_DIST, 
		state_info(state)->spaceship->position.y - ASTEROID_MAX_DIST
	};

    List objects_in_range = state_objects(state,top_left,bottom_right);
    
    for(ListNode node = list_first(objects_in_range); 
        node != LIST_EOF;
		node = list_next(objects_in_range, node)){

        Object object = list_node_value(objects_in_range, node);
        if (object->type == ASTEROID) {
            Rectangle source = { 0, 0, asteroid_img.width, asteroid_img.height };
            Rectangle dest = { object->position.x, object->position.y, object->size * scale_factor, object->size * scale_factor };
            Vector2 origin = { object->size * scale_factor / 2, object->size * scale_factor / 2 };
            DrawTexturePro(asteroid_img, source, dest, origin, 0, WHITE);

        }else if(object->type == BULLET){
            float radians = atan2(object->orientation.y, object->orientation.x);
            float rotation = radians * (180 / PI);
            Rectangle source = { 0, 0, bullet_img.width, bullet_img.height };
            Rectangle dest = { object->position.x, object->position.y, object->size * 30, object->size * 30 };
            Vector2 origin = { object->size * 30 / 2, object->size * 30 / 2 };
            DrawTexturePro(bullet_img, source, dest, origin, rotation, WHITE);
        }
    }
    
    EndMode2D();

    // Draw the score and the FPS counter
    DrawText(TextFormat("%04i", state_info(state)->score), 780, 20, 40, WHITE);
    DrawFPS(0, 0);

    EndDrawing();
}
