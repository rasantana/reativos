#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef struct game_objects GameObjects;
typedef struct object Object;
void assertTextureNotNull(SDL_Texture * texture);
Object * newObject(SDL_Renderer * renderer, char * texture);