#include "util.h"
#include "constants.h"

/* Struct to wrap all game objects and it's quantity as n. */

typedef struct game_objects {
    int n;
    Object * objects;

} GameObjects;

/* Struct to represent a drawable game object. */

typedef struct object
{
	int x;
	int y;
    int w;
    int h;
	float dx;
	float dy;
	SDL_Texture *texture;

} Object;


void assertTextureNotNull(SDL_Texture * texture){
    if(texture == NULL) {
        printf("Error loading important texture. Aborting execution...");
        exit(1);
    }
}

int getWidth() {
    return W_WIDTH/R_SCALE_X;
}

int getHeight() {
    return W_HEIGHT/R_SCALE_Y;
}


Object * newObject(SDL_Renderer * renderer, char * texturePath) {
    Object * object = (Object *) malloc(sizeof(Object));

    SDL_Texture * texture = IMG_LoadTexture(renderer, texturePath);

    assertTextureNotNull(texture);

	object->texture = texture;
    object->x = 0;
    object->y = 0;
    object->dx = 0;
	object->dy = 0;

    SDL_QueryTexture(texture,NULL,NULL,&object->w,&object->h);
    
    return object;
}

int rectangularCollision(Object *a, Object *b) {
    int xa = a->x; int wa = a->w;
    int xb = b->x; int wb = b->w;
    int ya = a->y; int ha = a->h;
    int yb = b->y; int hb = b->h;
    int collidesXAxis = xb <= xa && xa <= xb+wb || xa+wa >= xb && xb+wb >= xa+wa;
    int collidesYAxis = yb <= ya && ya <= yb+hb || ya+ha >= yb && yb+hb >= ya+ha;
    return collidesXAxis && collidesYAxis;
}

int circularCollision(Object *a, Object *b) {
    double e = 1.1;
    int xa = a->x; int wa = a->w;
    int xb = b->x; int wb = b->w;
    int ya = a->y; int ha = a->h;
    int yb = b->y; int hb = b->h;

    int oxa = xa + wa/2; 
    int oxb = xb + wb/2;
    int oya = ya + ha/2;
    int oyb = yb + hb/2;
    double distBetweenOrigins = sqrt(pow(oxa-oxb,2) + pow(oya-oyb,2));
    return distBetweenOrigins*e <= wa/2 + wb/2;
}