#ifndef DEFINITIONS_H
#define DEFINITIONS_H


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#ifdef __linux__
    #include <unistd.h>
#endif

#ifdef _WIN64
    #include <io.h>
#endif

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>

#define MAX_UINT8   250

#define WINDOW_WIDTH   640
#define WINDOW_LENGTH  720

#define PLAYER_POSITION_X 320
#define PLAYER_POSITION_Y 640

#define ENTITY_WIDTH  40
#define ENTITY_HEIGHT 40

#define MOVEMENT_DISTANCE 40
#define ZERO 0

#define STARTING_Y_POSITION 40

#define STEP_RATE 100
#define RATE_DECREMENT 20

#define MIN_RATE 20
#define MAX_RATE 100

#define STARTING_LIFE 100
#define LIFE_DECREMENT 10
#define LIFE_INCREMENT 10
#define MAX_LIFE 200
#define MIN_LIFE 0

#define TRUE  1
#define FALSE 0

#define BUFFER_SIZE 10
#define MAX_AUDIO_STREAMS 10

#define NUM_TEXTURES 3

#define HAZARD_2_START_THRESHOLD 6
#define HAZARD_3_START_THRESHOLD 18

#define SPRITE_WIDTH 58
#define SPRITE_HEIGHT 58
#define ANIMATION_FRAMES 10
#define ANIMATION_MS 100

typedef struct
{
    Uint8 collisions;
    Uint8 missed;
    Uint64 previous;
    Uint64 rate;
} Hazard;

typedef struct
{
    Hazard hazard_1;
    Hazard hazard_2;
    Hazard hazard_3;
    Uint16 totalCollisions;
    Uint8  totalMissed;
    Uint8  life; 
} ScorePoints;

typedef struct{
    SDL_Texture *playerBoxTexture;
    SDL_Texture *hazardBoxTexture1;
    SDL_Texture *hazardBoxTexture2;
    SDL_Texture *hazardBoxTexture3;
    SDL_Texture *backgroundTexture;
    SDL_Texture *pShipTexture;
    SDL_Texture *aShipTexture;
    SDL_AudioStream *stream[MAX_AUDIO_STREAMS];
    Uint8 *collision_wav_data;
    Uint32 collision_wav_data_len;
    Uint8 *miss_wav_data;
    Uint32 miss_wav_data_len;
    SDL_AudioStream* BGMstream;
    Uint8 *BGM_wav_data;
    Uint32 BGM_wav_data_len;
} Media;

typedef struct
{
    ScorePoints score_points;
    Media media_artifacts;
    Uint8 isGameOver;
    Uint8 isRunning;
    Uint8 isQuit;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_FRect playerBox;
    SDL_FRect hazardBox1;
    SDL_FRect hazardBox2;
    SDL_FRect hazardBox3;
    SDL_FRect pShipBox;
    SDL_FRect aShipBox1;
    SDL_FRect aShipBox2;
    SDL_FRect aShipBox3;
} AppState;

#endif
