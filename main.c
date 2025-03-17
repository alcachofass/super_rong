#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */

#include "definitions.h"
#include "functions.h"

#include <SDL3/SDL_main.h>

static const char *gameName      = "SUPER RONG";
static const char *gameVersion   = "1.0";
static const char *gameOver      = "GAME OVER";
static const char *pressToStart  = "Press 's' to start";
static const char *author        = "by alcachofass";
static const char *githubLink    = "https://github.com/alcachofass";
static const char *highScore     = "High Score:";

static Uint8 stream_i;           // variable used to toggle between the available audio streams
static Uint8 highestScore;

////////////These are also used in functions.c///////
char *bmp_path;
SDL_Surface *surface;
/////////////////////////////////////////////////////

//////////////////Variables for animation//////////////////
static int reference_offset = 0;
static int sprite_x = 0;
static int sprite_y = 0;
static Uint64 previous_ticks = 0;
///////////////////////////////////////////////////////////

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    int i;
    highestScore = ZERO;
    stream_i = MAX_AUDIO_STREAMS;

    surface = NULL; 
    bmp_path = NULL;

    //sound/////////////////
    SDL_AudioSpec spec;
    char *wav_path = NULL;
    ////////////////////////
    
    if (argc > 1){
        printf("This program does not accept arguments ->'%s'.\n", argv[1]);
        return SDL_APP_FAILURE;
    }

    AppState *as = SDL_calloc(1, sizeof(AppState));
    if (!as) {
        return SDL_APP_FAILURE;
    }

    *appstate = as;
    
    //sound
    for (i=0; i<MAX_AUDIO_STREAMS; i++){
        as->media_artifacts.stream[i] = NULL;
    }

    as->isQuit = FALSE;

    as->media_artifacts.collision_wav_data = NULL;
    as->media_artifacts.collision_wav_data_len = ZERO;

    as->media_artifacts.miss_wav_data = NULL;
    as->media_artifacts.miss_wav_data_len = ZERO;

    as->media_artifacts.BGMstream = NULL;
    as->media_artifacts.BGM_wav_data = NULL;
    as->media_artifacts.BGM_wav_data_len = ZERO;

    //video
    as->renderer = NULL;
    as->window = NULL;
    as->media_artifacts.playerBoxTexture = NULL;
    as->media_artifacts.hazardBoxTexture1 = NULL;
    as->media_artifacts.hazardBoxTexture2 = NULL;
    as->media_artifacts.hazardBoxTexture3 = NULL;
    as->media_artifacts.backgroundTexture = NULL;
    as->media_artifacts.aShipTexture = NULL;
    as->media_artifacts.pShipTexture = NULL;

    SDL_SetAppMetadata(gameName, gameVersion, gameName);

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer(gameName, WINDOW_WIDTH, WINDOW_LENGTH, 0, &as->window, &as->renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    initializeGame(as);

    srand((unsigned) time(NULL));

    as->score_points.totalCollisions = ZERO;
    as->score_points.totalMissed = ZERO;

    as->playerBox.w = ENTITY_WIDTH;
    as->playerBox.h = ENTITY_HEIGHT;

    as->hazardBox1.w = ENTITY_WIDTH;
    as->hazardBox1.h = ENTITY_HEIGHT;

    as->hazardBox2.w = ENTITY_WIDTH;
    as->hazardBox2.h = ENTITY_HEIGHT;

    as->hazardBox3.w = ENTITY_WIDTH;
    as->hazardBox3.h = ENTITY_HEIGHT;

    as->aShipBox1.w = ENTITY_WIDTH;
    as->aShipBox1.h = ENTITY_HEIGHT;

    as->aShipBox2.w = ENTITY_WIDTH;
    as->aShipBox2.h = ENTITY_HEIGHT;

    as->aShipBox3.w = ENTITY_WIDTH;
    as->aShipBox3.h = ENTITY_HEIGHT;

    as->pShipBox.w = ENTITY_WIDTH;
    as->pShipBox.h = ENTITY_WIDTH;

    //Background Texture//////////////////////////////////
    SDL_asprintf(&bmp_path, "%s/gfx/background.bmp", SDL_GetBasePath());  /* allocate a string of the full file path */
    surface = SDL_LoadBMP(bmp_path);
    if (!surface) {
        SDL_Log("Couldn't load player texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_free(bmp_path);  /* done with this, the file is loaded. */

    as->media_artifacts.backgroundTexture = SDL_CreateTextureFromSurface(as->renderer, surface);
    if (!as->media_artifacts.backgroundTexture) {
        SDL_Log("Couldn't create static texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    ////////////////////////////////////////////////////

    //Player Box Texture//////////////////////////////////
    SDL_asprintf(&bmp_path, "%sgfx/p_spritesheet.bmp", SDL_GetBasePath());  /* allocate a string of the full file path */
    surface = SDL_LoadBMP(bmp_path);
    if (!surface) {
        SDL_Log("Couldn't load player texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_free(bmp_path);  /* done with this, the file is loaded. */

    as->media_artifacts.playerBoxTexture = SDL_CreateTextureFromSurface(as->renderer, surface);
    if (!as->media_artifacts.playerBoxTexture) {
        SDL_Log("Couldn't create static texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    ////////////////////////////////////////////////////

    //Player Ship Texture//////////////////////////////////
    SDL_asprintf(&bmp_path, "%sgfx/p_ship_spritesheet.bmp", SDL_GetBasePath());  /* allocate a string of the full file path */
    surface = SDL_LoadBMP(bmp_path);
    if (!surface) {
        SDL_Log("Couldn't load player texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_free(bmp_path);  /* done with this, the file is loaded. */

    as->media_artifacts.pShipTexture = SDL_CreateTextureFromSurface(as->renderer, surface);
    if (!as->media_artifacts.pShipTexture) {
        SDL_Log("Couldn't create static texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    ////////////////////////////////////////////////////

    //Alien Ship Texture//////////////////////////////////
    SDL_asprintf(&bmp_path, "%sgfx/a_spritesheet.bmp", SDL_GetBasePath());  /* allocate a string of the full file path */
    surface = SDL_LoadBMP(bmp_path);
    if (!surface) {
        SDL_Log("Couldn't load player texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_free(bmp_path);  /* done with this, the file is loaded. */

    as->media_artifacts.aShipTexture = SDL_CreateTextureFromSurface(as->renderer, surface);
    if (!as->media_artifacts.aShipTexture) {
        SDL_Log("Couldn't create static texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    ////////////////////////////////////////////////////

    //Hazard Box Texture (Start with Red) ///////////////////////////////
    SDL_asprintf(&bmp_path, "%sgfx/r_laser.bmp", SDL_GetBasePath());  /* allocate a string of the full file path */
    surface = SDL_LoadBMP(bmp_path);
    if (!surface) {
        SDL_Log("Couldn't load hazard texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_free(bmp_path);  /* done with this, the file is loaded. */

    as->media_artifacts.hazardBoxTexture1 = SDL_CreateTextureFromSurface(as->renderer, surface);
    as->media_artifacts.hazardBoxTexture2 = SDL_CreateTextureFromSurface(as->renderer, surface);
    as->media_artifacts.hazardBoxTexture3 = SDL_CreateTextureFromSurface(as->renderer, surface);
    if (!as->media_artifacts.hazardBoxTexture1 || !as->media_artifacts.hazardBoxTexture2 || !as->media_artifacts.hazardBoxTexture1) {
        SDL_Log("Couldn't create static texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    ////////////////////////////////////////////////////

    SDL_DestroySurface(surface); 

    ///////////////////SOUND//////////////////
    /* Load the .wav file from wherever the app is being run from. */
    SDL_asprintf(&wav_path, "%ssfx/collision.wav", SDL_GetBasePath());  /* allocate a string of the full file path */
    if (!SDL_LoadWAV(wav_path, &spec, &as->media_artifacts.collision_wav_data, &as->media_artifacts.collision_wav_data_len)) {
        SDL_Log("Couldn't load .wav file: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_free(wav_path);  /* done with this string. */

    ///////////////////SOUND//////////////////
    /* Load the .wav file from wherever the app is being run from. */
    SDL_asprintf(&wav_path, "%ssfx/miss.wav", SDL_GetBasePath());  /* allocate a string of the full file path */
    if (!SDL_LoadWAV(wav_path, &spec, &as->media_artifacts.miss_wav_data, &as->media_artifacts.miss_wav_data_len)) {
        SDL_Log("Couldn't load .wav file: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_free(wav_path);  /* done with this string. */

    /* Create our audio stream in the same format as the .wav file. It'll convert to what the audio hardware wants. */
    for (i=0; i<MAX_AUDIO_STREAMS; i++){

        as->media_artifacts.stream[i] = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
        if (!as->media_artifacts.stream[i]) {
            SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
            return SDL_APP_FAILURE;
        }

        /* SDL_OpenAudioDeviceStream starts the device paused. You have to tell it to start! */
        SDL_ResumeAudioStreamDevice(as->media_artifacts.stream[i]);
    }
    //////////////////////////////////////

    ///////////////////BGM//////////////////
    /* Load the .wav file from wherever the app is being run from. */
    SDL_asprintf(&wav_path, "%sbgm/bgm.wav", SDL_GetBasePath());  /* allocate a string of the full file path */
    if (!SDL_LoadWAV(wav_path, &spec, &as->media_artifacts.BGM_wav_data, &as->media_artifacts.BGM_wav_data_len)) {
        SDL_Log("Couldn't load .wav file: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_free(wav_path);  /* done with this string. */

    /* Create our audio stream in the same format as the .wav file. It'll convert to what the audio hardware wants. */

    as->media_artifacts.BGMstream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    if (!as->media_artifacts.BGMstream) {
        SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    /* SDL_OpenAudioDeviceStream starts the device paused. You have to tell it to start! */
    SDL_ResumeAudioStreamDevice(as->media_artifacts.BGMstream);
    //////////////////////////////////////

    as->isRunning = 0;

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    AppState *as = (AppState *)appstate;

    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }

    switch (event->type) {
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS;
    case SDL_EVENT_KEY_DOWN:
        #ifdef DEBUG
            printf("Key went down\n");
            printf("Code is %d\n", event->key.scancode);
        #endif
        handleKeyPress(event->key.scancode, as);        
        break;
    case SDL_EVENT_KEY_UP:
        #ifdef DEBUG
            printf("Key went up\n");
        #endif
        break;
    }

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}


/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    SDL_FRect srcRect = { sprite_x, sprite_y, SPRITE_WIDTH, SPRITE_HEIGHT }; // Selecting a specific sprite from x and y offset
    const Uint64 now = SDL_GetTicks();

    if ((now - previous_ticks) > ANIMATION_MS){
        sprite_x = (reference_offset % ANIMATION_FRAMES) * SPRITE_WIDTH;
        reference_offset++;
        previous_ticks = now; 
    }

    AppState *as = (AppState *)appstate;

    if (as->isQuit){
        return SDL_APP_SUCCESS;
    }

    as->score_points.totalMissed = as->score_points.hazard_1.missed + as->score_points.hazard_2.missed + as->score_points.hazard_3.missed;

    if (as->score_points.life <= MIN_LIFE){
        as->isGameOver = TRUE;
    }
    
    if ((checkCollision(as)) && !as->isGameOver) {
        //play the collision noise////        
        SDL_PutAudioStreamData(as->media_artifacts.stream[stream_i%MAX_AUDIO_STREAMS], as->media_artifacts.collision_wav_data, as->media_artifacts.collision_wav_data_len);
        stream_i++;

        #ifdef DEBUG
            printf("Playing collision sound on stream %d\n", stream_i%MAX_AUDIO_STREAMS);
        #endif

          
        //////////////////
        as->score_points.totalCollisions = as->score_points.hazard_1.collisions + as->score_points.hazard_2.collisions + as->score_points.hazard_3.collisions;
        safeIncrement(&as->score_points.life, MAX_LIFE, LIFE_INCREMENT);
        
        #ifdef DEBUG
            printf("Collisons %d\n", as->score_points.totalCollisions);
            printf("Rates %ld, %ld, %ld\n", as->score_points.hazard_1.rate, as->score_points.hazard_2.rate, as->score_points.hazard_3.rate);
        #endif

        //////////////////change colors if there was a collision///////
        rotateColors(&as->score_points.hazard_1.collisions, &as->media_artifacts.hazardBoxTexture1, &as->renderer);
        rotateColors(&as->score_points.hazard_2.collisions, &as->media_artifacts.hazardBoxTexture2, &as->renderer);
        rotateColors(&as->score_points.hazard_3.collisions, &as->media_artifacts.hazardBoxTexture3, &as->renderer);

    }

    if (!as->isGameOver){
        while(now - as->score_points.hazard_1.previous >= as->score_points.hazard_1.rate ){
            as->hazardBox1.y = as->hazardBox1.y + MOVEMENT_DISTANCE;
            if (as->hazardBox1.y >= WINDOW_LENGTH){
                as->hazardBox1.y = 3 * ENTITY_HEIGHT; 
                safeIncrement(&as->score_points.hazard_1.missed, MAX_UINT8, 1);
                safeDecrement(&as->score_points.life, MIN_LIFE, LIFE_DECREMENT);
                SDL_PutAudioStreamData(as->media_artifacts.stream[stream_i%MAX_AUDIO_STREAMS], as->media_artifacts.miss_wav_data, as->media_artifacts.miss_wav_data_len);

            } 
            as->score_points.hazard_1.previous = now;
        }
    }

    if (as->score_points.totalCollisions >= HAZARD_2_START_THRESHOLD && !as->isGameOver){
        while(now - as->score_points.hazard_1.previous >= as->score_points.hazard_1.rate ){
            as->hazardBox1.y = as->hazardBox1.y + MOVEMENT_DISTANCE;
            if (as->hazardBox1.y >= WINDOW_LENGTH){
                as->hazardBox1.y = 3 * ENTITY_HEIGHT; 
                safeIncrement(&as->score_points.hazard_1.missed, MAX_UINT8, 1);
                safeDecrement(&as->score_points.life, MIN_LIFE, LIFE_DECREMENT);
                SDL_PutAudioStreamData(as->media_artifacts.stream[stream_i%MAX_AUDIO_STREAMS], as->media_artifacts.miss_wav_data, as->media_artifacts.miss_wav_data_len);

            } 
            as->score_points.hazard_1.previous = now;
        }

        while(now - as->score_points.hazard_2.previous >= as->score_points.hazard_2.rate ){
            as->hazardBox2.y = as->hazardBox2.y + MOVEMENT_DISTANCE;
            if (as->hazardBox2.y >= WINDOW_LENGTH){
                as->hazardBox2.y = 3 * ENTITY_HEIGHT; 
                safeIncrement(&as->score_points.hazard_2.missed, MAX_UINT8, 1);
                safeDecrement(&as->score_points.life, MIN_LIFE, LIFE_DECREMENT);
                SDL_PutAudioStreamData(as->media_artifacts.stream[stream_i%MAX_AUDIO_STREAMS], as->media_artifacts.miss_wav_data, as->media_artifacts.miss_wav_data_len);

            } 
            as->score_points.hazard_2.previous = now;
        }
    } 

    if (as->score_points.totalCollisions >= HAZARD_3_START_THRESHOLD && !as->isGameOver){
        while(now - as->score_points.hazard_1.previous >= as->score_points.hazard_1.rate ){
            as->hazardBox1.y = as->hazardBox1.y + MOVEMENT_DISTANCE;
            if (as->hazardBox1.y >= WINDOW_LENGTH){
                as->hazardBox1.y = 3 * ENTITY_HEIGHT; 
                safeIncrement(&as->score_points.hazard_1.missed, MAX_UINT8, 1);
                safeDecrement(&as->score_points.life, MIN_LIFE, LIFE_DECREMENT);
                SDL_PutAudioStreamData(as->media_artifacts.stream[stream_i%MAX_AUDIO_STREAMS], as->media_artifacts.miss_wav_data, as->media_artifacts.miss_wav_data_len);

            } 
            as->score_points.hazard_1.previous = now;
        }

        while(now - as->score_points.hazard_2.previous >= as->score_points.hazard_2.rate ){
            as->hazardBox2.y = as->hazardBox2.y + MOVEMENT_DISTANCE;
            if (as->hazardBox2.y >= WINDOW_LENGTH){
                as->hazardBox2.y = 3 * ENTITY_HEIGHT; 
                safeIncrement(&as->score_points.hazard_2.missed, MAX_UINT8, 1);
                safeDecrement(&as->score_points.life, MIN_LIFE, LIFE_DECREMENT);
                SDL_PutAudioStreamData(as->media_artifacts.stream[stream_i%MAX_AUDIO_STREAMS], as->media_artifacts.miss_wav_data, as->media_artifacts.miss_wav_data_len);

            } 
            as->score_points.hazard_2.previous = now;
        }

        while(now - as->score_points.hazard_3.previous >= as->score_points.hazard_3.rate ){
            as->hazardBox3.y = as->hazardBox3.y + MOVEMENT_DISTANCE;
            if (as->hazardBox3.y >= WINDOW_LENGTH){
                as->hazardBox3.y = 3 * ENTITY_HEIGHT; 
                safeIncrement(&as->score_points.hazard_3.missed, MAX_UINT8, 1);
                safeDecrement(&as->score_points.life, MIN_LIFE, LIFE_DECREMENT);
                SDL_PutAudioStreamData(as->media_artifacts.stream[stream_i%MAX_AUDIO_STREAMS], as->media_artifacts.miss_wav_data, as->media_artifacts.miss_wav_data_len);

            } 
            as->score_points.hazard_3.previous = now;
        }        
    } 

    SDL_SetRenderDrawColor(as->renderer, 10, 10, 10, 1); 

    /* clear the window to the draw color. */
    SDL_RenderClear(as->renderer);

    //background image
    SDL_RenderTexture(as->renderer, as->media_artifacts.backgroundTexture, NULL, NULL);

    //player box
    #ifdef DEBUG
        SDL_SetRenderDrawColor(as->renderer, 255,255,255,255);
        SDL_RenderFillRects(as->renderer, &as->playerBox, 1);
    #endif
    //Render textures. &srcRect is used here for spritesheet references
    SDL_RenderTexture(as->renderer, as->media_artifacts.playerBoxTexture, &srcRect, &as->playerBox);
    SDL_RenderTexture(as->renderer, as->media_artifacts.pShipTexture, &srcRect, &as->pShipBox);
    SDL_RenderTexture(as->renderer, as->media_artifacts.aShipTexture, &srcRect, &as->aShipBox1);
    SDL_RenderTexture(as->renderer, as->media_artifacts.aShipTexture, &srcRect, &as->aShipBox2);
    SDL_RenderTexture(as->renderer, as->media_artifacts.aShipTexture, &srcRect, &as->aShipBox3);
    
    #ifdef DEBUG
        SDL_RenderRect(as->renderer, &as->playerBox);
    #endif

    //hazard box 1
    #ifdef DEBUG
        SDL_SetRenderDrawColor(as->renderer, 0,255,0,255);
        SDL_RenderFillRects(as->renderer, &as->hazardBox1, 1);
    #endif
    //textures
    SDL_RenderTexture(as->renderer, as->media_artifacts.hazardBoxTexture1, NULL, &as->hazardBox1);

    #ifdef DEBUG
        SDL_RenderRect(as->renderer, &as->hazardBox1);
    #endif

    //hazard box2
    if (as->score_points.totalCollisions >= HAZARD_2_START_THRESHOLD) { 
        #ifdef DEBUG
            SDL_SetRenderDrawColor(as->renderer, 0,255,0,255); 
            SDL_RenderFillRects(as->renderer, &as->hazardBox2, 1);
        #endif
        SDL_RenderTexture(as->renderer, as->media_artifacts.hazardBoxTexture2, NULL, &as->hazardBox2);

        #ifdef DEBUG
            SDL_RenderRect(as->renderer, &as->hazardBox2);
        #endif
    }

    //hazard box3
    if (as->score_points.totalCollisions >= HAZARD_3_START_THRESHOLD) { 
        #ifdef DEBUG
            SDL_SetRenderDrawColor(as->renderer, 0,255,0,255);
            SDL_RenderFillRects(as->renderer, &as->hazardBox3, 1);
        #endif
        SDL_RenderTexture(as->renderer, as->media_artifacts.hazardBoxTexture3, NULL, &as->hazardBox3);

        #ifdef DEBUG
            SDL_RenderRect(as->renderer, &as->hazardBox3);
        #endif
    }

    //scores
    char buffer[BUFFER_SIZE];
    SDL_SetRenderDrawColor(as->renderer, 0,255,0,255);

    if (as->isRunning){
        SDL_RenderDebugText(as->renderer, 0, 0, "Captured:");
        #ifdef _WIN64
            sprintf_s(buffer, 4, "%3d", as->score_points.totalCollisions);
        #endif
        #ifdef __linux__
            snprintf(buffer, 4, "%3d", as->score_points.totalCollisions);
        #endif
        SDL_RenderDebugText(as->renderer, 80, 0, buffer);

        SDL_RenderDebugText(as->renderer, 0, 10, "Missed:");
        #ifdef _WIN64
            sprintf_s(buffer, 4, "%3d", as->score_points.totalMissed);
        #endif
        #ifdef __linux__
            snprintf(buffer, 4, "%3d", as->score_points.totalMissed);
        #endif
        SDL_RenderDebugText(as->renderer, 80, 10, buffer);

        SDL_RenderDebugText(as->renderer, 0, 20, "Life:");
        #ifdef _WIN64
            sprintf_s(buffer, 4, "%3d", as->score_points.life);
        #endif
        #ifdef __linux__
            snprintf(buffer, 4, "%3d", as->score_points.life);
        #endif
        SDL_RenderDebugText(as->renderer, 80, 20, buffer);

        SDL_RenderDebugText(as->renderer, 0, 710, "Press 'r' to reset or 'q' to quit.");
    }

    if (!as->isRunning){

        SDL_SetRenderDrawColor(as->renderer, 255,0,0,255);  
        SDL_SetRenderScale(as->renderer, 1.3f, 1.3f);
        SDL_RenderDebugText(as->renderer, 210, 140, gameName);

        SDL_SetRenderScale(as->renderer, 1.0f, 1.0f);
        SDL_RenderDebugText(as->renderer, 270, 220, author);

        SDL_SetRenderDrawColor(as->renderer, 0,255,0,255);
        SDL_RenderDebugText(as->renderer, 260, 300, pressToStart);

        SDL_SetRenderDrawColor(as->renderer, 255,0,0,255);  
        SDL_RenderDebugText(as->renderer, 200, 320, githubLink);

        SDL_SetRenderDrawColor(as->renderer, 0,255,0,255);
    }

    if (as->isGameOver && as->isRunning){
        highestScore = (as->score_points.totalCollisions > highestScore) ? as->score_points.totalCollisions : highestScore;

        #ifdef DEBUG
            printf("Number of collisions: %d\n", as->score_points.totalCollisions);            
            printf("Highest score: %d\n", highestScore);
        #endif

        SDL_SetRenderDrawColor(as->renderer, 255,0,0,255);
        SDL_RenderDebugText(as->renderer, 280, 120, gameOver);

        SDL_SetRenderDrawColor(as->renderer, 0,255,0,255);
        SDL_RenderDebugText(as->renderer, 280, 140, highScore);

        #ifdef _WIN64
            sprintf_s(buffer, 4, "%3d", highestScore);
        #endif
        #ifdef __linux__
            snprintf(buffer, 4, "%3d", highestScore);
        #endif

        SDL_RenderDebugText(as->renderer, 380, 140, buffer);

        SDL_SetRenderDrawColor(as->renderer, 0,255,0,255);
    }

    //Draw it
    SDL_RenderPresent(as->renderer);

    //Play BGM on loop
    if (SDL_GetAudioStreamAvailable(as->media_artifacts.BGMstream) < (int)as->media_artifacts.BGM_wav_data_len) {
        SDL_PutAudioStreamData(as->media_artifacts.BGMstream, as->media_artifacts.BGM_wav_data, as->media_artifacts.BGM_wav_data_len);
    }

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    /* SDL will clean up the window/renderer for us. */
    if (appstate != NULL) {
        AppState *as = (AppState *)appstate;
        SDL_DestroyRenderer(as->renderer);
        SDL_DestroyWindow(as->window);
        SDL_free(as);
    }

    #ifdef DEBUG
        printf("SDL_AppQuit executed. SDL_AppResult = %d\n", result);
    #endif

    if (!result)
        printf("%d", result);

    return;
}


