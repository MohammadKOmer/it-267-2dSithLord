#include <stdlib.h>
#include <string.h>
#include "level.h"
#include "entity.h"


extern ScreenData  S_Data;
extern Uint32 rmask,gmask,bmask,amask;
extern SDL_Rect Camera;
extern SDL_Surface *bgimage;
extern SDL_Surface *background;
SDL_Surface *clipmask;
Level level;



void CloseLevel()
{
	if(bgimage != NULL)
	{
		SDL_FreeSurface(bgimage);
	}
	if(clipmask != NULL)
	{
		SDL_FreeSurface(clipmask);
	}
}


void GenerateLevel(int x, int y)
{
	int i,j;
	if(x > SPACE_W)x = SPACE_W;
	if(y > SPACE_H)y = SPACE_H;
	level.width = x;
	level.height = y;
	level.tileset = 1;
	
	level.spawncount = 0;
	strcpy(level.levelname,"Test Level");
	strcpy(level.bgimage,"images/bgswimage.png");
	memset(level.tilemap,0,sizeof(level.tilemap));
	for(j = 0;j < y;j++)
	{
		level.tilemap[j][0] = 1;
		level.tilemap[j][x - 1] = 1;    
	}
	for(i = 0;i < x;i++)
	{
		level.tilemap[0][i] = 1;
		level.tilemap[y - 1][i] = 1;    
	}
}

/*load the graphics and draw the graphics to the background buffer*/
void DrawLevel()
{
	Sprite *tiles = NULL;
	SDL_Surface *temp;
	int i,j;
	/*first we load the sprite sheet for the background tiles*/

	tiles = LoadSprite("images/FloorTile.png",256,256);
	SDL_SetColorKey(tiles->image, SDL_SRCCOLORKEY , SDL_MapRGB(tiles->image->format, 255,255,255));

	
	/*And we load the background image*/
	if(bgimage != NULL)SDL_FreeSurface(bgimage);
	temp = IMG_Load("images/bgswimage.png");
	if(temp == NULL)
	{
		printf( "Unable to load background image: %s \n",SDL_GetError());
		exit(0);
	}
	bgimage = SDL_DisplayFormat(temp);
	SDL_SetColorKey(bgimage, SDL_RLEACCEL, 0);
	SDL_FreeSurface(temp);

	/*then we check to see if the background image is already in memory and if so, then we shall destroy it*/
	if(background != NULL)SDL_FreeSurface(background);
	temp = SDL_CreateRGBSurface(SDL_HWSURFACE, level.width * 256, level.height * 256, S_Data.depth,rmask, gmask,bmask,amask);
	if(temp == NULL)
	{
		printf("Couldn't initialize background buffer: %s\n", SDL_GetError());
		exit(1);
	}
	/* Just to make sure that the surface we create is compatible with the screen*/
	background = SDL_DisplayFormatAlpha(temp);
	SDL_FreeSurface(temp);

	if(clipmask != NULL)SDL_FreeSurface(clipmask);
	temp = SDL_CreateRGBSurface(SDL_HWSURFACE, level.width * 64, level.height * 64, S_Data.depth,rmask, gmask,bmask,amask);
	if(temp == NULL)
	{
		printf( "Couldn't initialize background buffer: %s\n", SDL_GetError());
		exit(1);
	}
	/* Just to make sure that the surface we create is compatible with the screen*/
	clipmask = SDL_DisplayFormatAlpha(temp);
	SDL_FreeSurface(temp);
	DrawFilledRect(0,0, background->w,background->h, SDL_MapRGB(background->format,0,0,0), background);
	for(j = 0;j < level.height;j++)
	{
		for(i = 0;i < level.width;i++)
		{
			DrawSprite(tiles,background,i * tiles->w,j * tiles->h,level.tilemap[j][i] - 1,0);
		}
	}
	SDL_BlitSurface(background,NULL,clipmask,NULL);
	SDL_SetColorKey(background,SDL_RLEACCEL, SDL_MapRGB(background->format,0,0,0));
	FreeSprite(tiles);

}

void SaveLevel(char *filename)
{
	FILE *file;
	file = fopen(filename, "wb");
	if(file == NULL)
	{
		printf( "Unable to open file for writing: %s",SDL_GetError());
		exit(0);
	}
	if(fwrite(&level, sizeof(level), 1, file)!= 1)
	{
		printf( "Unable to write to file: %s",SDL_GetError());
		exit(0);
	}
	fclose(file);
}

void LoadLevel(char *filename)
{
	FILE *file;
	file = fopen(filename, "rb");
	if(file == NULL)
	{
		printf( "Unable to open file for reading: %s",SDL_GetError());
		exit(0);
	}
	if(fread(&level, sizeof(level), 1, file)!= 1)
	{
		printf( "Unable to read from file: %s",SDL_GetError());
		exit(0);    
	}
	fclose(file);
}