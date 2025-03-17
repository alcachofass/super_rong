#include "definitions.h"

extern char *bmp_path;
extern SDL_Surface *surface;

void reduceRate(Uint64 *rate){
    *rate -= RATE_DECREMENT;

    if (*rate <= MIN_RATE){
        *rate = MAX_RATE;
    }
    
    return;
}

void safeIncrement(Uint8 *num, Uint8 max, Uint8 inc){
    
    if (*num != max){
        *num += inc;
    }

    return;
}

void safeDecrement(Uint8 *num, Uint8 min, Uint8 dec){
    if(*num != min){
        *num -= dec;
    }

    return;
}

void initializeGame(AppState *as){

    bmp_path = NULL;
    surface = NULL;

    as->score_points.totalCollisions = ZERO;
    as->score_points.totalMissed = ZERO;
    as->score_points.life = STARTING_LIFE;
    
    as->isGameOver = 0;

    as->playerBox.x = PLAYER_POSITION_X;
    as->playerBox.y = PLAYER_POSITION_Y;

    as->pShipBox.x = PLAYER_POSITION_X;          
    as->pShipBox.y = PLAYER_POSITION_Y + ENTITY_HEIGHT;

    as->score_points.hazard_1.collisions = ZERO;
    as->score_points.hazard_1.missed = ZERO;
    as->score_points.hazard_1.rate = STEP_RATE;
    as->score_points.hazard_1.previous = ZERO;

    as->score_points.hazard_2.collisions = ZERO;
    as->score_points.hazard_2.missed = ZERO;
    as->score_points.hazard_2.rate = STEP_RATE;
    as->score_points.hazard_2.previous = ZERO;

    as->score_points.hazard_3.collisions = ZERO;
    as->score_points.hazard_3.missed = ZERO;
    as->score_points.hazard_3.rate = STEP_RATE;
    as->score_points.hazard_3.previous = ZERO;


    as->hazardBox1.y = STARTING_Y_POSITION + ENTITY_HEIGHT;
    as->hazardBox2.y = STARTING_Y_POSITION + ENTITY_HEIGHT;
    as->hazardBox3.y = STARTING_Y_POSITION + ENTITY_HEIGHT;

    as->hazardBox1.x = (float)((rand() % (WINDOW_WIDTH/MOVEMENT_DISTANCE))) * MOVEMENT_DISTANCE;
    as->hazardBox2.x = (float)((rand() % (WINDOW_WIDTH/MOVEMENT_DISTANCE))) * MOVEMENT_DISTANCE;
    as->hazardBox3.x = (float)((rand() % (WINDOW_WIDTH/MOVEMENT_DISTANCE))) * MOVEMENT_DISTANCE;

    as->aShipBox1.y = STARTING_Y_POSITION;
    as->aShipBox1.x = as->hazardBox1.x;

    as->aShipBox2.y = STARTING_Y_POSITION;
    as->aShipBox2.x = as->hazardBox2.x;

    as->aShipBox3.y = STARTING_Y_POSITION;
    as->aShipBox3.x = as->hazardBox3.x;

    //Hazard Box Texture ///////////////////////////////
    SDL_asprintf(&bmp_path, "%sgfx/r_laser.bmp", SDL_GetBasePath());  /* allocate a string of the full file path */
    surface = SDL_LoadBMP(bmp_path);
    if (!surface) {
        SDL_Log("Couldn't load hazard texture: %s", SDL_GetError());
        return;
    }

    SDL_free(bmp_path);  /* done with this, the file is loaded. */

    as->media_artifacts.hazardBoxTexture1 = SDL_CreateTextureFromSurface(as->renderer, surface);
    as->media_artifacts.hazardBoxTexture2 = SDL_CreateTextureFromSurface(as->renderer, surface);
    as->media_artifacts.hazardBoxTexture3 = SDL_CreateTextureFromSurface(as->renderer, surface);
    if (!as->media_artifacts.hazardBoxTexture1 || !as->media_artifacts.hazardBoxTexture2 || !as->media_artifacts.hazardBoxTexture3 ) {
        SDL_Log("Couldn't create static texture: %s", SDL_GetError());
        return;
    }
    ////////////////////////////////////////////////////

    return;

}

void rotateColors(Uint8 *collisions, SDL_Texture **hazardBoxTexture, SDL_Renderer **renderer){

        switch ( *collisions % NUM_TEXTURES ){
            case 0:
                SDL_asprintf(&bmp_path, "%sgfx/r_laser.bmp", SDL_GetBasePath());  /* allocate a string of the full file path */
                break;
            case 1:
                SDL_asprintf(&bmp_path, "%sgfx/y_laser.bmp", SDL_GetBasePath());  /* allocate a string of the full file path */
                break;
            case 2:
                SDL_asprintf(&bmp_path, "%sgfx/b_laser.bmp", SDL_GetBasePath());  /* allocate a string of the full file path */
                break;
            default:
                break;
        }
            
        surface = SDL_LoadBMP(bmp_path);
        if (!surface) {
            SDL_Log("Couldn't load hazard texture: %s", SDL_GetError());
            return;
        }

        SDL_free(bmp_path);  /* done with this, the file is loaded. */

        *hazardBoxTexture = SDL_CreateTextureFromSurface(*renderer, surface);
        if ( !hazardBoxTexture ) {
            SDL_Log("Couldn't create static texture: %s", SDL_GetError());
            return;
        }


}

int checkCollision(AppState *as){

    if (as->playerBox.y == as->hazardBox1.y && as->playerBox.x == as->hazardBox1.x){
        #ifdef DEBUG
            printf("Collision detected\n");
        #endif

        safeIncrement(&as->score_points.hazard_1.collisions, MAX_UINT8, 1);
        reduceRate(&as->score_points.hazard_1.rate);

        as->hazardBox1.x = (float)((rand() % (WINDOW_WIDTH/MOVEMENT_DISTANCE))) * MOVEMENT_DISTANCE;
        as->hazardBox1.y = STARTING_Y_POSITION + ENTITY_HEIGHT;

        as->aShipBox1.x = as->hazardBox1.x;
        as->aShipBox1.y = as->hazardBox1.y - ENTITY_HEIGHT;

        return TRUE;
    }

    if (as->playerBox.y == as->hazardBox2.y && as->playerBox.x == as->hazardBox2.x){
        #ifdef DEBUG
            printf("Collision detected\n");
        #endif

        safeIncrement(&as->score_points.hazard_2.collisions, MAX_UINT8, 1);
        reduceRate(&as->score_points.hazard_2.rate);

        as->hazardBox2.x = (float)((rand() % (WINDOW_WIDTH/MOVEMENT_DISTANCE))) * MOVEMENT_DISTANCE;
        as->hazardBox2.y = STARTING_Y_POSITION + ENTITY_HEIGHT;

        as->aShipBox2.x = as->hazardBox2.x;
        as->aShipBox2.y = as->hazardBox2.y - ENTITY_HEIGHT;

        return TRUE;
    }

    if (as->playerBox.y == as->hazardBox3.y && as->playerBox.x == as->hazardBox3.x){
        #ifdef DEBUG
            printf("Collision detected\n");
        #endif

        safeIncrement(&as->score_points.hazard_3.collisions, MAX_UINT8, 1);
        reduceRate(&as->score_points.hazard_3.rate);

        as->hazardBox3.x = (float)((rand() % (WINDOW_WIDTH/MOVEMENT_DISTANCE))) * MOVEMENT_DISTANCE;
        as->hazardBox3.y = STARTING_Y_POSITION + ENTITY_HEIGHT;

        as->aShipBox3.x = as->hazardBox3.x;
        as->aShipBox3.y = as->hazardBox3.y - ENTITY_HEIGHT;

        return TRUE;
    }

    else
        return FALSE;
}

int handleKeyPress(SDL_Keycode key_code, AppState *as)
{
    switch (key_code) {

        case SDL_SCANCODE_UP:
            #ifdef DEBUG
                printf("^\n");
            #endif
            as->playerBox.y = as->playerBox.y - MOVEMENT_DISTANCE;
            if (as->playerBox.y <= ENTITY_HEIGHT * 3) { as->playerBox.y = ENTITY_HEIGHT * 3;} ;
            as->pShipBox.y = as->playerBox.y + ENTITY_HEIGHT;
            break;

        case SDL_SCANCODE_DOWN:
            #ifdef DEBUG
                printf("v\n");
            #endif
            as->playerBox.y = as->playerBox.y + MOVEMENT_DISTANCE;
            if (as->playerBox.y >= WINDOW_LENGTH - 2 * ENTITY_HEIGHT) { as->playerBox.y = WINDOW_LENGTH - 2 * ENTITY_HEIGHT;} ;
            break;

        case SDL_SCANCODE_LEFT:
            #ifdef DEBUG
                printf("<-\n");
            #endif
            as->playerBox.x = as->playerBox.x - MOVEMENT_DISTANCE;
            if (as->playerBox.x < ZERO) { as->playerBox.x = WINDOW_WIDTH - ENTITY_WIDTH;} ;
            break;

        case SDL_SCANCODE_RIGHT:
            #ifdef DEBUG
                printf("->\n");
            #endif
            as->playerBox.x = as->playerBox.x + MOVEMENT_DISTANCE;
            if (as->playerBox.x == WINDOW_WIDTH) { as->playerBox.x = ZERO;} ;
            break;
        case SDL_SCANCODE_S:
            as->isRunning = TRUE;
            initializeGame(as);
            break;
        case SDL_SCANCODE_R:
            initializeGame(as);
            break;
        case SDL_SCANCODE_Q:
            as->isQuit = TRUE;
            break;

        default:
            break;
    }

    // draw the plane directly below the player box
    as->pShipBox.y = as->playerBox.y + ENTITY_HEIGHT;
    as->pShipBox.x = as->playerBox.x;

    return 0;
}

