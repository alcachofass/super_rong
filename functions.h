#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void reduceRate(Uint64 *rate);
void safeIncrement(Uint8 *num, Uint8 max, Uint8 inc);
void safeDecrement(Uint8 *num, Uint8 min, Uint8 dec);
void initializeGame(AppState *as);
void rotateColors(Uint8 *collisions, SDL_Texture **hazardBoxTexture, SDL_Renderer **renderer);
int  checkCollision(AppState *as);
int  handleKeyPress(SDL_Keycode key_code, AppState *as);

#endif
