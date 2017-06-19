#include "main.h"
#include "util.c"
#include "events.c"
#include <math.h>

int collidesWith(SDL_Rect rect, int x, int y)
{
	int rx = rect.x;
	int ry = rect.y;
	int rw = rect.w;
	int rh = rect.h;
	int collidesXAxis = rx <= x && x <= rx + rw;
	int collidesYAxis = ry <= y && y <= ry + rh;

	return collidesXAxis && collidesYAxis;
}

Object newEnemy(SDL_Renderer * renderer, int x, int y) {
    Object * obj = newObject(renderer, ASSET_ENEMY);
    obj->x = x - obj->w/2;
    obj->y = y - obj->h/2;
    obj->dx = 0;
    obj->dy = 3;
    return * obj;
}

Object newPlayer(SDL_Renderer * renderer) {
    Object * obj = newObject(renderer, ASSET_PLAYER);
    obj->x = getWidth()/2 - obj->w/2;
    obj->y = getHeight() - obj->h*2;
    obj->dx = 4;
    return * obj;
}

Object newBackgroundL0(SDL_Renderer * renderer) {
    Object * obj = newObject(renderer, ASSET_BACKGROUND_L0);
    return * obj;
}

Object newBackgroundL1(SDL_Renderer * renderer) {
    Object * obj = newObject(renderer, ASSET_BACKGROUND_L1);
    return * obj;
}

Object newBackgroundL2(SDL_Renderer * renderer) {
    Object * obj = newObject(renderer, ASSET_BACKGROUND_L2);
    return * obj;
}

GameObjects * setup(SDL_Renderer * renderer) {
    int numberOfObjects = 9;

    GameObjects * objs = (GameObjects * ) malloc(sizeof(GameObjects));
    
    objs->n = numberOfObjects;
    objs->objects = (Object *) malloc(sizeof(Object) * objs->n);

    objs->objects[0] = newBackgroundL0(renderer);
    objs->objects[1] = newBackgroundL1(renderer);
    objs->objects[2] = newBackgroundL2(renderer);
    objs->objects[3] = newPlayer(renderer);

    int x1,x2,x3,x4,x5,y1,y2,y3,y4,y5;

    x1 = 1*getWidth()/6;
    x2 = 2*getWidth()/6;
    x3 = 3*getWidth()/6;
    x4 = 4*getWidth()/6;
    x5 = 5*getWidth()/6;

    y1 = -2*getHeight()/10;
    y2 = -6*getHeight()/10;
    y3 = -10*getHeight()/10;
    y4 = -14*getHeight()/10;
    y5 = -18*getHeight()/10;

    objs->objects[4] = newEnemy(renderer,x1,y1);
    objs->objects[5] = newEnemy(renderer,x2,y2);
    objs->objects[6] = newEnemy(renderer,x3,y3);
    objs->objects[7] = newEnemy(renderer,x4,y4);
    objs->objects[8] = newEnemy(renderer,x5,y5);

    return objs;
}

void handleKeyboard(GameObjects * objs) {
    Object * player = &objs->objects[3];
    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    if(keystates[SDL_SCANCODE_RIGHT]) {
        player->x += player->dx; 
    } else if (keystates[SDL_SCANCODE_LEFT]) {
        player->x -= player->dx;
    }
    int bounds_r = getWidth() - player->w;
    int bounds_l = 0;
    if(player->x >= bounds_r) player->x = bounds_r;
    if(player->x <= bounds_l) player->x = bounds_l; 
}

void updateEnemies(GameObjects * objs) {
    for(int i = 4; i <= objs->n; i++) {
        Object * enemy = &objs->objects[i];
        enemy->y += enemy->dy;
        if(enemy->y >= getHeight() + enemy->h) {
            enemy->y = -enemy->h;
        }
    }
}

void finish() {
    SDL_Event sdlevent;
    sdlevent.type = SDL_QUIT;
    SDL_PushEvent(&sdlevent);
}

void checkForCollision(GameObjects * objs) {
    Object * player = &objs->objects[3];
    for(int i = 4; i <= objs->n; i++) {
        Object * enemy = &objs->objects[i];
        if(circularCollision(player,enemy)) {
            printf("Collision! Quitting game...");
            SDL_Delay(1000);
            finish();
        }
    }
}

void update(GameObjects * objs) {
    handleKeyboard(objs);
    updateEnemies(objs);
    checkForCollision(objs);
}

void draw(SDL_Renderer * renderer, GameObjects * objs) {

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0x00);
    SDL_RenderClear(renderer);

    for(int i = 0; i < objs->n; i++) {
        Object *object = &objs->objects[i];
        SDL_Rect destination;
        destination.x = object->x;
        destination.y = object->y;
        destination.w = object->w;
        destination.h = object->h;
        SDL_RenderCopy(renderer, object->texture, NULL, &destination);  
    }

    SDL_RenderPresent(renderer);
}