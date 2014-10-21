#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "SDL.h"
#include "graphics.h"
#include "entity.h"
#include "player.h"
#include "projectiles.h"


#define MAXSTATE 1

extern SDL_Surface *screen;
extern SDL_Surface *bgimage;
extern SDL_Surface *background;
extern SDL_Rect Camera;
extern Entity *ThePlayer;
extern SDL_Surface *clipmask;
int drawents = 1,drawboxes = 0;
int windowed = 0;
int mapeditmode = 0;
int bucketdraw = 0;

SDL_Joystick *joy;
Uint8 *keys;


void Init_All();
void Update_ALL();
int Think_ALL();
void Draw_ALL();
void DrawSplashScreen();
void GiveInfo();
void PopUpWindow(Sprite *sprite,char *text,Uint32 Color,int pwait);
void UpdateCamera();
void UpdateMapCamera();

int main(int argc, char *argv[])
{
  FILE *file = NULL;
  int done = 0;
  int i;
  char string[40];
  SDLMod mod;
  
  Init_All();
  GiveInfo();
  
  if(ThePlayer == NULL)SpawnPlayer(128,128);
  do
  {
    ResetBuffer();
    SDL_PumpEvents();
    keys = SDL_GetKeyState(NULL);    
    mod = SDL_GetModState();
    Draw_ALL();
    if(keys[SDLK_ESCAPE] == 1)done = 1;
    Update_ALL();
    NextFrame();
  }while(!done);
    
  exit(0);
  return 0;
}



void CleanUpAll()
{
  CloseSprites();
  ClearEntities();
  ClearRegionMask();
  if(SDL_JoystickOpened(0))
    SDL_JoystickClose(joy);
  SDL_FreeSurface(clipmask);
  /*any other cleanup functions can be added here*/ 
}

void Init_All()
{
  Init_Graphics(windowed);
  InitSpriteList();
  
  SDL_InitSubSystem(SDL_INIT_JOYSTICK);
  atexit(CleanUpAll);
  LoadFonts();
  InitMessages();
  InitEntityList();
  initTree();
  LoadHUD();

}

void GiveInfo()
{
  NewMessage("Press Esc to Quit",IndexColor(White));
}

void PopUpWindow(Sprite *splash,char *text,Uint32 Color,int frame)
{
  Sprite *popup;
  int len = strlen(text);
  popup = LoadSprite("images/popup.png",400,300);
  DrawSprite(popup,screen,(screen->w - 400)/2,(screen->h - 300)/2,0);
  if(splash != NULL)
    DrawSprite(splash,screen,(screen->w - splash->w)/2,(screen->h - splash->h)/2,frame);
  if(text != NULL)
    DrawText(text,screen,(screen->w - (len *11))/2 ,(screen->h - 300)/2 + 50,Color,F_Large);
  DrawText("Hit Spacebar to continue",screen,(screen->w)/2 - 136 ,(screen->h/2) + 100,Color,F_Large);
  NextFrame();
  FreeSprite(splash);
  FreeSprite(popup);
  do
  {
    SDL_PumpEvents();
    keys = SDL_GetKeyState(NULL);
  }
  while(keys[SDLK_SPACE] != 1);
  
}





/*calls all of the update functions for everything*/
void Update_ALL()
{
  int i;
  Uint8 *keys;
  if(!mapeditmode)
  {
    UpdateEntities();
    UpdateCamera();
  }
  else
  {
    UpdateMapCamera();
  }
}

/*calls all of the think function for everything*/
int Think_ALL()
{
  int done = 0;
  ThinkEntities();
  return done;
}

/*calls all of the draw functions for everything*/
void Draw_ALL()
{
  
  if(drawents)
  {
    DrawEntities();
    if((ThePlayer != NULL))DrawEntity(ThePlayer);
  }
  if(drawboxes)DrawBBoxEntities();
  DrawMessages();
  DrawHUD(ThePlayer);

  if(bucketdraw)DrawBuckets();
}

void UpdateMapCamera()
{
  int mx,my;
  SDL_GetMouseState(&mx,&my);
  if((mx < 10)&&(Camera.x >= 10))  Camera.x -= 10;
  if((mx > screen->w - 10)&&(Camera.x <= (background->w - screen->w) - 10))Camera.x += 10;
  if((my < 10)&&(Camera.y >= 10))  Camera.y -= 10;
  if((my > screen->h - 10)&&(Camera.y <= (background->h - (screen->h - 100)) - 10))Camera.y += 10;
}

void UpdateCamera()
{
  Camera.x = (int)ThePlayer->s.x - (Camera.w >> 1);
  Camera.y = (int)ThePlayer->s.y - (Camera.h >> 1);
  if(Camera.x > background->w - Camera.w)Camera.x = background->w - Camera.w;
  if(Camera.x < 0)Camera.x = 0;
  if(Camera.y > background->h - Camera.h)Camera.y = background->h - Camera.h;
  if(Camera.y < 0)Camera.y = 0;
  
}
