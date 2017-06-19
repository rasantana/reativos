#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

/* Function signatures. */

SDL_Texture * loadTexture(SDL_Renderer * renderer, char *name);
SDL_Window *createWindow();
SDL_Renderer *createRenderer(SDL_Window *window);
void batchDraw(SDL_Renderer *renderer);
void onError(int code);
void endProgram(SDL_Renderer *renderer, SDL_Window *window);
void mainLoop(SDL_Renderer *renderer, SDL_Window *window);
int heardQuitGameEvent();
int collidesWith(SDL_Rect rect, int x, int y);