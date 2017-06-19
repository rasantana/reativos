#include "constants.h"
#include "main.c"
#include "sdl_engine.h"
#include "events.c"

#define MAX_FPS 60
#define TICKS_PER_FRAME 1000/MAX_FPS

/**
 * @function 		createWindow
 * @description 	Create a SDL_Window.
 * @return 			{SDL_Window} 		Returns a SDL_Window struct.
 */

SDL_Window *createWindow()
{
	return SDL_CreateWindow(W_NAME, SDL_WINDOWPOS_UNDEFINED,
					SDL_WINDOWPOS_UNDEFINED,
					W_WIDTH, W_HEIGHT, 
					SDL_WINDOW_SHOWN);
}

/**
 * @function 		createRenderer
 * @description 	Create a SDL_Renderer with additional customization (V-Sync).
 * @param			{SDL_Window *}		window
 * @return 			{SDL_Renderer} 		Returns a SDL_Renderer struct.
 */


SDL_Renderer *createRenderer(SDL_Window *window)
{
	return SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

/**
 * @function 			onError
 * @description 		Handle errors by code.
 * @param	{int}		code		Error code.
 */

void onError(int code)
{
	switch (code)
	{
	default:
		printf("Unexpected error ocurred. Aborting execution... Code: %d", code);
		exit(code);
	}
}

/**
 * @function 	 endProgram
 * @description  Finalization method to properly end the program.
 * @param 		 {SDL_Renderer *}	renderer	Current renderer.
 * @param 		 {SDL_Window *}		window		Current window.            
 */

void endProgram(SDL_Renderer *renderer, SDL_Window *window)
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int heardQuitGameEvent()
{
	while(SDL_PollEvent(&currentEvent)){
        switch(currentEvent.type) {
            case SDL_QUIT:
				return true;
			default:
				return false;
        }
	}
}

/**
 * @function mainLoop
 * @param 	renderer	Instantiated renderer.
 * @param 	window		Instantiated window.
 * @description Runs the game loop.
 */

void mainLoop(SDL_Renderer *renderer, SDL_Window *window)
{
	GameObjects * gameObjects = setup(renderer);
	Uint32 lastTick;
	Uint32 startTick = SDL_GetTicks();
	Uint32 frameCount = 0;
	int running = true;
	while (running)
	{
		lastTick = SDL_GetTicks();
		update(gameObjects);
		draw(renderer, gameObjects);
		frameCount++;
		printf("AVG FPS: %f\n", frameCount/ (SDL_GetTicks() / 1000.0) );

		if (heardQuitGameEvent())
		{
			running = false;
			endProgram(renderer, window);
			//Give memory back to the OS.
			free(gameObjects);
		}

		if(SDL_GetTicks() - lastTick < TICKS_PER_FRAME) {
			SDL_Delay(TICKS_PER_FRAME - SDL_GetTicks() + lastTick);
		}
	}
}

int main(int argc, char *argv[])
{
	//Initialize stuff and abort in case of any error.
	if (SDL_Init(SDL_INIT_EVERYTHING))
	{
		onError(0);
	}
	//Try to initialize the window.
	SDL_Window *window = createWindow();
	if (window == NULL)
	{
		onError(1);
	}
	//Try to initialize renderer.
	SDL_Renderer *renderer = createRenderer(window);
	if (renderer == NULL)
	{
		onError(2);
	}
	int sdlImageFlags = IMG_INIT_PNG;
	if(!IMG_Init(sdlImageFlags)) {
		printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
		onError(3);
	}
	//Scale rendering.
	SDL_RenderSetScale(renderer, R_SCALE_X, R_SCALE_Y);
	SDL_GL_SetSwapInterval(1);
	//Main routine.
	mainLoop(renderer, window);
	return 0;
}