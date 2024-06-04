#include "raylib.h"
#include <math.h>

#include "state.h"
#include "interface.h"
#include <string.h>
#include <stdio.h>

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
Texture2D astronaut;
Texture2D skip_text_button;
Texture2D space_background;
Texture2D wave;
// Sound bullet_sound;
//Texture background_img;
// Sound damage_sound;

int pickupIndex = 0;
float pickupTimer = PICKUP_TIME;

int heartIndex = 0;

int waveIndex = 0; 

int skipTextIndex = 0;
float skipTextTimer = 8;


int buttonCounter = 1;
float buttonTimer = 30;

int startButtonIndex = 0;
int infoButtonIndex = 0;
int exitButtonIndex = 0;

// Start menu background color
Color menu_color = {1,0,20,0};

// Αρχικοποιεί το interface του παιχνιδιού

void interface_init(){
    // Initialize the window
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "asteroids");
	SetTargetFPS(60);
	
	// Load images

    game_name = LoadTextureFromImage(LoadImage("assets/game_name_v2.png"));

    start_button = LoadTextureFromImage(LoadImage("assets/start_button.png"));
    info_button = LoadTextureFromImage(LoadImage("assets/info_button.png"));
    exit_button = LoadTextureFromImage(LoadImage("assets/exit_button.png"));

    skip_text_button = LoadTextureFromImage(LoadImage("assets/skip_text_button.png"));

    space_background = LoadTextureFromImage(LoadImage("assets/space.png"));

    wave = LoadTextureFromImage(LoadImage("assets/wave.png"));
    wave.height = wave.height * 0.5;
    wave.width = wave.width * 0.5;
    astronaut = LoadTextureFromImage(LoadImage("assets/astronaut.png"));
    astronaut.height = astronaut.height * 10;
    astronaut.width = astronaut.width * 10;
    enemy_scout = LoadTextureFromImage(LoadImage("assets/enemy_scout.png"));
    pickup = LoadTextureFromImage(LoadImage("assets/rocket_pickup.png"));
    heart = LoadTextureFromImage(LoadImage("assets/hearts.png"));
	spaceship_img = LoadTextureFromImage(LoadImage("assets/spaceship.png"));
    asteroid_img = LoadTextureFromImage(LoadImage("assets/asteroid.png"));
    bullet_img = LoadTextureFromImage(LoadImage("assets/bullet.png"));
    explosion = LoadTextureFromImage(LoadImage("assets/explosion.png"));
    spaceship_img.height = spaceship_img.height * 3;
    spaceship_img.width = spaceship_img.width * 3;
	start_button.height = start_button.height *1.2;
    start_button.width = start_button.width *1.2;
	info_button.height = info_button.height *1.2;
    info_button.width = info_button.width *1.2;
    exit_button.height = exit_button.height *1.2;
    exit_button.width = exit_button.width *1.2;
    // Load sounds
    // bullet_sound = LoadSound("assets/sound.wav");
    
    // damage_sound = LoadSound("assets/hurt.wav");
}

// Κλείνει το interface του παιχνιδιού
void interface_close(){
	CloseWindow();
}

float fadeInOpacity = 1;
bool fadingIn = true;

void interface_fade_in() {
    if (fadingIn) {
        fadeInOpacity -= 0.015;
        if (fadeInOpacity <= 0) {
            fadeInOpacity = 0;
            fadingIn = false;
        }
    }
}

// Αρχικοποιεί το start menu του παιχνιδιού
void interface_draw_menu() {
        // if (IsKeyPressed(KEY_SPACE)) PlaySound(bullet_sound);      // Play WAV sound

    
    buttonTimer --;
    if (buttonTimer < 0 && buttonCounter == 1){
        buttonTimer = 30;
        startButtonIndex++;
        if(startButtonIndex > 1){
            startButtonIndex = 0;
        }
    } else if (buttonTimer < 0 && buttonCounter == 2){
        buttonTimer = 30;
        infoButtonIndex++;
        if(infoButtonIndex > 1){
            infoButtonIndex = 0;
        }
    } else if (buttonTimer < 0 && buttonCounter == 3){
        buttonTimer = 30;
        exitButtonIndex++;
        if(exitButtonIndex > 1){
            exitButtonIndex = 0;
        }
    }

    if(IsKeyPressed(KEY_UP)){
        startButtonIndex = 0;
        infoButtonIndex = 0;
        exitButtonIndex = 0;
        buttonCounter--;
        if(buttonCounter < 1)
            buttonCounter = 3;
        else if(buttonCounter > 3)
            buttonCounter = 1;
    } else if (IsKeyPressed(KEY_DOWN)){
        startButtonIndex = 0;
        infoButtonIndex = 0;
        exitButtonIndex = 0;
        buttonCounter++;
        if(buttonCounter < 1)
            buttonCounter = 3;
        else if(buttonCounter > 3)
            buttonCounter = 1;
    }

    Vector2 gameNamePos = { SCREEN_WIDTH / 2 - game_name.width / 2, 60 };
    Vector2 startButtonPos = { SCREEN_WIDTH / 2 - start_button.width / 2, 350 };
    Vector2 infoButtonPos = { SCREEN_WIDTH / 2 - info_button.width / 2, 450 };
    Vector2 exitButtonPos = { SCREEN_WIDTH / 2 - exit_button.width / 2, 550 };

    BeginDrawing();
    ClearBackground(menu_color);
    DrawTexture(space_background, 0, 0, WHITE);

    DrawTexture(game_name, gameNamePos.x, gameNamePos.y, WHITE);

    Rectangle start_source = (Rectangle){192 * startButtonIndex ,0,192,48};
    Rectangle start_dest = (Rectangle){startButtonPos.x, startButtonPos.y, start_source.width,start_source.height};
    DrawTexturePro(start_button, start_source, start_dest, (Vector2){-95,0}, 0, WHITE);

    Rectangle info_source = (Rectangle){192 * infoButtonIndex ,0,192,48};
    Rectangle info_dest = (Rectangle){infoButtonPos.x, infoButtonPos.y, info_source.width,info_source.height};
    DrawTexturePro(info_button, info_source, info_dest, (Vector2){-95,0}, 0, WHITE);

    Rectangle exit_source = (Rectangle){192 * exitButtonIndex ,0,192,48};
    Rectangle exit_dest = (Rectangle){exitButtonPos.x, exitButtonPos.y, exit_source.width,exit_source.height};
    DrawTexturePro(exit_button, exit_source, exit_dest, (Vector2){-95,0}, 0, WHITE);

    if (fadingIn) {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, fadeInOpacity));
    }

    EndDrawing();
}


void interface_draw_info(State state){

    BeginDrawing();
    ClearBackground(menu_color);
    
    // Game Controls: Provide a list of all the controls used in the game
    // Gameplay Instructions: Explain the mechanics of the game
    // Scoring System: Describe how the scoring works.
    // Power-Ups and Items: Explain their effects.

    EndDrawing();

};

const char *introTexts[] = {
    "Hello traveler!\nI am BRUNO.",
    "You have to help me.\nMy galaxy is under attack!",
    "You must defend yourself from\n 10 grueling waves of enemies\n.",
    "Only then will I be able to escape."
};
const int numTexts = sizeof(introTexts) / sizeof(introTexts[0]);

void interface_draw_intro(State state, GameState *gameState) {
    
    BeginDrawing();
    ClearBackground(menu_color);

    // Draw spaceman (rectangle for now)
    // DrawRectangle(SCREEN_WIDTH / 2 - 75, 150, 150, 300, RED);
    DrawTexture(astronaut,SCREEN_WIDTH / 2 - astronaut.width /2 ,150,WHITE);

    // Draw text box
    DrawRectangle(0, 480, SCREEN_WIDTH, 150, LIGHTGRAY);

    if (state_text(state)->textIndex >= 0 && state_text(state)->textIndex < numTexts) {
        const char *currentText = introTexts[state_text(state)->textIndex];
        int length = strlen(currentText);

        // Draw text letter by letter
        for (int i = 0; i < state_text(state)->index; i++) {
            DrawText(TextSubtext(currentText, 0, i + 1), 20, 500, 40, BLACK);
        }

        state_text(state)->timer += 1;
        if (state_text(state)->timer >= state_text(state)->delay && state_text(state)->index < length) {
            state_text(state)->index++;
            state_text(state)->timer = 0;
        }
        
        if (state_text(state)->index >= length) {
            skipTextTimer++;
            if (skipTextTimer >= 7) { 
                skipTextIndex++;
                if (skipTextIndex >= 8) 
                    skipTextIndex = 0;
                skipTextTimer = 0;
            }

            Rectangle skipTextSource = { 45 * skipTextIndex, 0, 45, 38 };
            DrawTextureRec(skip_text_button, skipTextSource, (Vector2){SCREEN_WIDTH - 70, 560}, WHITE);
            if (IsKeyPressed(KEY_ENTER)) {
                state_text(state)->textIndex++;
                if (state_text(state)->textIndex < numTexts) {
                    state_text(state)->index = 0; 
                } else {
                    gameState->introduction = false;
                    gameState->gameplay = true;
                }
            }
        }
    }

    EndDrawing();
}

// Σχεδιάζει ένα frame με την τωρινή κατάσταση του παιχνδιού
void interface_draw_frame(State state) {
    int scale_factor = 5;

    pickupTimer--;
    if (pickupTimer < 0) {
        pickupTimer = PICKUP_TIME;
        pickupIndex++;
        if (pickupIndex >= PICKUP_COUNT) {
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
                    
    DrawTexturePro(spaceship_img, spaceship_source, spaceship_dest, (Vector2){spaceship_dest.width / 2, spaceship_dest.height / 2}, rotation, WHITE);

    Vector2 top_left = {
        state_info(state)->spaceship->position.x - ASTEROID_MAX_DIST, 
        state_info(state)->spaceship->position.y + ASTEROID_MAX_DIST
    }; // Set top_left
    Vector2 bottom_right = {
        state_info(state)->spaceship->position.x + ASTEROID_MAX_DIST, 
        state_info(state)->spaceship->position.y - ASTEROID_MAX_DIST
    };  // Set bottom_right

    List objects_in_range = state_objects(state, top_left, bottom_right);
    
    for (ListNode node = list_first(objects_in_range); 
        node != LIST_EOF;
        node = list_next(objects_in_range, node)) {

        Object object = list_node_value(objects_in_range, node);
        if (object->type == ASTEROID) {
            Rectangle source = {0, 0, asteroid_img.width, asteroid_img.height};
            Rectangle dest = {object->position.x, object->position.y, object->size * scale_factor, object->size * scale_factor};
            Vector2 origin = {object->size * scale_factor / 2, object->size * scale_factor / 2};
            DrawTexturePro(asteroid_img, source, dest, origin, 0, WHITE);
        } else if (object->type == BULLET) {
            float radians = atan2(object->orientation.y, object->orientation.x);
            float rotation = radians * (180 / PI);
            Rectangle source = {0, 0, bullet_img.width, bullet_img.height};
            Rectangle dest = {object->position.x, object->position.y, object->size * 30, object->size * 30};
            Vector2 origin = {object->size * 30 / 2, object->size * 30 / 2};
            DrawTexturePro(bullet_img, source, dest, origin, rotation, WHITE);
        } else if (object->type == PICKUP) {
            Rectangle source = (Rectangle){PICKUP_SIZE * pickupIndex, 0, PICKUP_SIZE, PICKUP_SIZE};
            Rectangle dest = (Rectangle){object->position.x, object->position.y, source.width, source.height};
            DrawTexturePro(pickup, source, dest, (Vector2){0, 0}, 0, WHITE);
        } else if (object->type == ENEMY) {
            Vector2 directionToSpaceship = {
                state_info(state)->spaceship->position.x - object->position.x,
                state_info(state)->spaceship->position.y - object->position.y
            };
            float enemy_radians = atan2(directionToSpaceship.y, directionToSpaceship.x);
            float enemy_rotation = enemy_radians * (180 / PI) + 90;

            int enemy_health = object_health(object);

            int enemyHealthIndex = 3 - enemy_health;
            Rectangle source = (Rectangle){enemyHealthIndex * ENEMY_SIZE, 0, ENEMY_SIZE, ENEMY_SIZE};
            Rectangle dest = {object->position.x, object->position.y, object->size * 3, object->size * 3};
            Vector2 origin = {dest.width / 2, dest.height / 2};

            DrawTexturePro(enemy_scout, source, dest, origin, enemy_rotation, WHITE);
        }
    }
    
    EndMode2D();

    heartIndex = state_info(state)->spaceship->health;

    Rectangle heart_source = (Rectangle){0, 0, HEART_SIZE * heartIndex, HEART_SIZE};
    Rectangle heart_dest = (Rectangle){0, 10, heart_source.width, heart_source.height};
    DrawTexturePro(heart, heart_source, heart_dest, (Vector2){0, 0}, 0, WHITE);

    waveIndex = state_wave(state)->current_wave - 1;

    Rectangle wave_source = (Rectangle){waveIndex * 500/2, 0, 500/2, 80/2};
    Rectangle wave_dest = (Rectangle){0, 0, wave_source.width, wave_source.height};
    DrawTexturePro(wave, wave_source, wave_dest, (Vector2){ - SCREEN_WIDTH /2 + 250/2, -20}, 0, WHITE);


    // Draw the coins and the FPS counter
    DrawText(TextFormat("%04d", state_info(state)->coins), 780, 20, 40, WHITE);
    DrawFPS(0, 0);

    EndDrawing();
}

