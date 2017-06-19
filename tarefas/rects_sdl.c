#include <SDL2/SDL.h>
#include <stdlib.h>
#include <math.h>

#define true 1
#define false 0

#define W_WIDTH 640		//Window width.
#define W_HEIGHT 480	//Window height.
#define MAX_DISTANCE 50 //The max distance a rectangle will walk before changing it's direction.
#define NR 2			//Numer of rectangles.

#define UP 0	//Facing up.
#define RIGHT 1 //Facing right.
#define DOWN 2  //Facing down.
#define LEFT 3  //Facing left.

/* Type signatures */

typedef struct rect Rect;
typedef struct color Color;

/* Function signatures. */

SDL_Window *createWindow();
SDL_Renderer *createRenderer(SDL_Window *window);
SDL_Rect buildRectangle(int x1, int x2, int y1, int y2);
Rect *getInitialRectangles();
void moveRectangle(Rect *r, int dx, int dy);
void drawRectangle(SDL_Renderer *renderer, SDL_Rect rect, Color color);
void batchDraw(SDL_Renderer *renderer, Rect *rects);
void onError(int code);
void endProgram(SDL_Renderer *renderer, SDL_Window *window, Rect * rects);
void mainLoop(SDL_Renderer *renderer, SDL_Window *window);
int heardQuitGameEvent();
int clickedAnyRect(Rect *rects);
int collidesWith(Rect rect, int x, int y);

/* Struct to represent a color. */

typedef struct color
{
	int r;
	int g;
	int b;
	int a;
} Color;

/* Struct to wrap the rectangle and it's facing direction. */

typedef struct rect
{
	SDL_Rect r;
	float speed;
	int facing;
	int distance;
	Color color;
} Rect;


/**
 * @function 		createWindow
 * @description 	Create a SDL_Window.
 * @return 			{SDL_Window} 		Returns a SDL_Window struct.
 */

SDL_Window *createWindow()
{
	return SDL_CreateWindow("Rectangles!",
							SDL_WINDOWPOS_UNDEFINED,
							SDL_WINDOWPOS_UNDEFINED,
							W_WIDTH, W_HEIGHT, SDL_WINDOW_SHOWN);
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
 * @function 		buildRectangle
 * @description 	Create a SDL_Rect.
 * @param	{int}	x	X-Axis origin coordinate.
 * @param	{int}	y	Y-Axis origin coordinate.
 * @param	{int}	w	Rectangle width.
 * @param	{int}	y	Rectangle height.
 * @return 	{SDL_Rect} 	Returns a SDL_Rect struct.
 */

SDL_Rect buildRectangle(int x, int y, int w, int h)
{
	SDL_Rect rect = {x, y, w, h};
	return rect;
}

/**
 * @function 			moveRectangle
 * @description 		Move a single rectangle with offset x and y, and turn around if a maximum distance is reached.
 * @param	{Rect *}	rectangle	Rectangle to move.
 * @param	{int}		offsetX		X offset.
 * @param	{int}		offsetY		Y offset.
 */

void moveRectangle(Rect *rect, int offsetX, int offsetY)
{
	rect->r.x += offsetX;
	rect->r.y += offsetY;
	rect->distance += abs(offsetX);
	rect->distance += abs(offsetY);
	//Reset walked distance, turn around.
	if (rect->distance >= MAX_DISTANCE)
	{
		rect->distance = 0;
		rect->facing = (rect->facing + 1) % 4;
	}
}

/**
 * @function 			drawRectangle
 * @description 		Render a single rectangle, with an specific color.
 * @param	{SDL_Renderer *}	renderer	Current renderer.
 * @param	{SDL_Rect}			rect		Rectangle to draw.
 * @param	{Color}				color		Desired rectangle color.
 */

void drawRectangle(SDL_Renderer *renderer, SDL_Rect rect, Color color)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer, &rect);
}

/**
 * @function 			batchDraw
 * @description 		Batch draws all rectangles to avoid performance problems.
 * @param	{SDL_Renderer *}	renderer	Current renderer.
 * @param	{SDL_Window *}		window		Current window.
 */

void batchDraw(SDL_Renderer *renderer, Rect *rects)
{
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0x00);
	SDL_RenderFillRect(renderer, NULL);
	//Render all rectangles.
	for (int i = 0; i < NR; i++)
		drawRectangle(renderer, rects[i].r, rects[i].color);
	//Batch render all the previous rendering calls.
	SDL_RenderPresent(renderer);
}

/**
 * @function 			batchUpdate
 * @description 		Batch update all rectangles.
 * @param	{Rect *}	rects		The rectangles to update.
 */

void batchUpdate(Rect *rects)
{
	for (int i = 0; i < NR; i++)
	{
		switch (rects[i].facing)
		{
		case UP:
			moveRectangle(&rects[i], 0, 1);
			break;
		case RIGHT:
			moveRectangle(&rects[i], 1, 0);
			break;
		case DOWN:
			moveRectangle(&rects[i], 0, -1);
			break;
		case LEFT:
			moveRectangle(&rects[i], -1, 0);
			break;
		}
	}
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
 * @function 		getInitialRectangles
 * @description 	Get initial rects.
 * @return		{Rect *}	the rectangles.
 */

Rect *getInitialRectangles()
{
	SDL_Rect r1 = {300, 100, 50, 50};
	SDL_Rect r2 = {200, 200, 50, 50};
	
	Color c1, c2;

	c1.r = 0x00; c1.g = 0x00; c1.b = 0xFF; c1.a = 0x00;
	c2.r = 0xFF; c2.g = 0x00; c2.b = 0x00; c2.a = 0x00;
	
	Rect *rects = (Rect *)malloc(sizeof(Rect) * NR);

	rects[0].r = r1;
	rects[0].speed = 1;
	rects[0].facing = UP;
	rects[0].distance = 0;
	rects[0].color = c1;

	rects[1].r = r2;
	rects[1].speed = 1;
	rects[1].facing = DOWN;
	rects[1].distance = 0;
	rects[1].color = c2;

	return rects;
}

/**
 * @function 	 endProgram
 * @description  Finalization method to properly end the program.
 * @param 		 {SDL_Renderer *}	renderer	Current renderer.
 * @param 		 {SDL_Window *}		window		Current window.            
 */

void endProgram(SDL_Renderer *renderer, SDL_Window *window, Rect * rects)
{
	free(rects);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

/**
 * @function 		heardQuitGameEvent     
 * @description 	This function listen for SDL_Quit events.
 * @return 			{int} 	Returns wether or not a SDL_Quit event was listened.
 */

int heardQuitGameEvent()
{
	static SDL_Event event;
	SDL_PollEvent(&event);
	return event.type == SDL_QUIT;
}

/**
 * @function collidesWith
 * @param 	{Rect}		rect	Target rectangle.
 * @param 	{int}		x		X-Axis coordinate.
 * @param 	{int}		y		Y-Axis coordinate.               
 * @return 	{int} 		Returns wether or not the rectangle contains that coordinate.
 */

int collidesWith(Rect rect, int x, int y)
{
	int rx = rect.r.x;
	int ry = rect.r.y;
	int rw = rect.r.w;
	int rh = rect.r.h;
	int collidesXAxis = rx <= x && x <= rx + rw;
	int collidesYAxis = ry <= y && y <= ry + rh;
	return collidesXAxis && collidesYAxis;
}

/**
 * @function 	clickedAnyRect
 * @description Listen for click events on any rectangle area.
 * @param 		{Rect}	rects	Current rectangles.
 * @return 		{int} 	Returns wether or not there was a click in a coordinate that is contained by a rectangle.
 */

int clickedAnyRect(Rect *rects)
{
	static SDL_Event event;
	SDL_PollEvent(&event);
	if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		SDL_MouseButtonEvent *buttonEvent = (SDL_MouseButtonEvent *)&event;
		for (int i = 0; i < NR; i++)
			if (collidesWith(rects[i], buttonEvent->x, buttonEvent->y))
				return true;
	}
	return false;
}

/**
 * @function mainLoop
 * @param 	{SDL_Renderer *} 	renderer	Instantiated renderer.
 * @param 	{SDL_Window *} 		window		Instantiated window.
 * @description Runs the game loop.
 */

void mainLoop(SDL_Renderer *renderer, SDL_Window *window)
{
	Rect *rects = getInitialRectangles();
	int running = true;
	while (running)
	{
		batchUpdate(rects);
		batchDraw(renderer, rects);
		
		if (clickedAnyRect(rects) || heardQuitGameEvent())
		{
			endProgram(renderer, window, rects);
			break;
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
	//Main routine.
	mainLoop(renderer, window);
	return 0;
}
