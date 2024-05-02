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
    BeginDrawing();
    ClearBackground(BLACK);

    float x = state_info(state)->spaceship->position.x;
    float y = state_info(state)->spaceship->position.y;

    float radians = atan2(
						state_info(state)->spaceship->orientation.x,
						state_info(state)->spaceship->orientation.y
						);


	float rotation = radians * (-180.0 / PI);

    Rectangle source = {0, 0, spaceship_img.width, spaceship_img.height};
    Rectangle dest = {x, y, spaceship_img.width, spaceship_img.height};
    Vector2 origin = {dest.width / 2, dest.height / 2};

    // Draw the rotated texture
    DrawTexturePro(spaceship_img, source, dest, origin, rotation, WHITE);

    // Draw the score and the FPS counter
    DrawText(TextFormat("%04i", state_info(state)->score), 780, 20, 40, WHITE);
    DrawFPS(0, 0);

    EndDrawing();
}