#include "raylib.h"
#include <math.h>

#include "state.h"
#include "interface.h"


// Assets
Texture2D spaceship_img;
Texture2D asteroid_img;
Texture2D bullet_img;
Texture2D pickup;
Texture2D heart;
Texture2D explosion;
Texture2D enemy_scout;
Texture2D game_name; 
Texture2D start_button;
Texture2D info_button;
Texture2D exit_button;
//Texture background_img;

int pickupIndex = 0;
float pickupTimer = PICKUP_TIME;

int heartIndex = 0;
int testIndex = 0;

// Start menu background color
Color menu_color = {1,0,20,0};

// Αρχικοποιεί το interface του παιχνιδιού

void interface_init(){
    // Initialize the window
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "asteroids");
	SetTargetFPS(60);
    InitAudioDevice();
	
	// Load images

    game_name = LoadTextureFromImage(LoadImage("assets/game_name.png"));
    start_button = LoadTextureFromImage(LoadImage("assets/start_button.png"));
    info_button = LoadTextureFromImage(LoadImage("assets/info_button.png"));
    exit_button = LoadTextureFromImage(LoadImage("assets/exit_button.png"));

    enemy_scout = LoadTextureFromImage(LoadImage("assets/enemy_scout.png"));
    pickup = LoadTextureFromImage(LoadImage("assets/rocket_pickup.png"));
    heart = LoadTextureFromImage(LoadImage("assets/hearts.png"));
	spaceship_img = LoadTextureFromImage(LoadImage("assets/spaceship.png"));
    asteroid_img = LoadTextureFromImage(LoadImage("assets/asteroid.png"));
    bullet_img = LoadTextureFromImage(LoadImage("assets/bullet.png"));
    explosion = LoadTextureFromImage(LoadImage("assets/explosion.png"));
    spaceship_img.height = spaceship_img.height * 3;
    spaceship_img.width = spaceship_img.width * 3;

}

// Κλείνει το interface του παιχνιδιού
void interface_close(){
	CloseAudioDevice();
	CloseWindow();
}
void interface_draw_menu() {
    Vector2 gameNamePos = { SCREEN_WIDTH / 2 - game_name.width / 2, 20 };
    Vector2 startButtonPos = { SCREEN_WIDTH / 2 - start_button.width / 2, 350 };
    Vector2 infoButtonPos = { SCREEN_WIDTH / 2 - info_button.width / 2, 450 };
    Vector2 exitButtonPos = { SCREEN_WIDTH / 2 - exit_button.width / 2, 550 };

    BeginDrawing();
    ClearBackground(menu_color);
    
    DrawTexture(game_name, gameNamePos.x, gameNamePos.y, WHITE);

    DrawTexture(start_button, startButtonPos.x, startButtonPos.y, WHITE);
    DrawTexture(info_button, infoButtonPos.x, infoButtonPos.y, WHITE);
    DrawTexture(exit_button, exitButtonPos.x, exitButtonPos.y, WHITE);


    EndDrawing();
}

// Σχεδιάζει ένα frame με την τωρινή κατάσταση του παιχνδιού
void interface_draw_frame(State state) {
    int scale_factor = 5;

    pickupTimer --;
    if (pickupTimer < 0){
        pickupTimer = PICKUP_TIME;
        pickupIndex++;
        if(pickupIndex >= PICKUP_COUNT){
            pickupIndex = 0;
        }
    }

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
    ClearBackground(menu_color);

    float radians = atan2(
						state_info(state)->spaceship->orientation.x,
						state_info(state)->spaceship->orientation.y
						);

    float rotation = radians * (-180 / PI);
    
    rotation += 180; // add 180 for spaceship to look at correct angle
    
    Rectangle spaceship_source = {-heartIndex * 90, 0, 90, 78};
    Rectangle spaceship_dest = {
                    state_info(state)->spaceship->position.x,
                    state_info(state)->spaceship->position.y, 
                    spaceship_source.width, 
                    spaceship_source.height
                    };
                    
    DrawTexturePro(spaceship_img, spaceship_source, spaceship_dest, (Vector2){spaceship_dest.width/2, spaceship_dest.height/2}, rotation, WHITE);

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
        }else if(object->type == PICKUP){
            Rectangle source = (Rectangle){PICKUP_SIZE * pickupIndex,0,PICKUP_SIZE,PICKUP_SIZE};
            Rectangle dest = (Rectangle){object->position.x, object->position.y, source.width,source.height};
            DrawTexturePro(pickup, source, dest, (Vector2){0, 0}, 0, WHITE);
        }else if (object->type == ENEMY){
            Vector2 directionToSpaceship = {
            state_info(state)->spaceship->position.x - object->position.x,
            state_info(state)->spaceship->position.y - object->position.y
            };
            float enemy_radians = atan2(directionToSpaceship.y, directionToSpaceship.x);
            float enemy_rotation = enemy_radians * (180 / PI) + 90;

            int enemy_health = object_health(object);

            int enemyHealthIndex = 3 - enemy_health; 
            Rectangle source = (Rectangle){enemyHealthIndex * ENEMY_SIZE,0, ENEMY_SIZE, ENEMY_SIZE};
            Rectangle dest = { object->position.x, object->position.y, object->size * 3, object->size * 3};
            Vector2 pivot = {object->size , object->size };
            DrawTexturePro(enemy_scout, source, dest, pivot, enemy_rotation, WHITE);
            
        }
    }
    
    EndMode2D();

    heartIndex = state_info(state)->spaceship->health;

    Rectangle heart_source = (Rectangle){0,0,HEART_SIZE * heartIndex,HEART_SIZE};
    Rectangle heart_dest = (Rectangle){0,10,heart_source.width,heart_source.height};
    DrawTexturePro(heart, heart_source, heart_dest, (Vector2){0,0}, 0, WHITE);

    // Draw the score and the FPS counter
    DrawText(TextFormat("%04i", state_info(state)->score), 780, 20, 40, WHITE);
    DrawFPS(0, 0);

    EndDrawing();
}
