#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

/* Type signatures */

typedef struct object Object;
typedef struct game_objects GameObjects;

/* Main functions */

GameObjects * setup(SDL_Renderer * renderer);
void update(GameObjects * objs);
void draw(SDL_Renderer * renderer, GameObjects * objs);