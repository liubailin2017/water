#include<iostream>
#include<SDL2/SDL.h>
#include<stdio.h>
#include<stdlib.h>
using namespace std;

SDL_Surface * img = SDL_LoadBMP("test.bmp");
int WIDTH = img->w ,HEIGHT = img->h ;
short *buf1 = new short[WIDTH * HEIGHT];
short *buf2 = new short[WIDTH * HEIGHT];


void init(){
        for(int i = 0; i < WIDTH*HEIGHT; i++){
            buf1[i] = 0;
            buf2[i] = 0;
        }
    }

void quit()
{
	if(buf1) {
		delete buf1;
		buf1 = NULL;
	}
	if(buf2) {
		delete buf2;
		buf2 = NULL;
	}

}

void ripple()
{


        for(int i=WIDTH; i<WIDTH*HEIGHT-WIDTH; i++)
        {
            buf2[i] = ((buf1[i-1]+buf1[i+1]+buf1[i-WIDTH]+buf1[i+WIDTH])>>1)-buf2[i];
            buf2[i]-=buf2[i]>>6;
        }

    short *ptmp = buf1;
    buf1=buf2;
    buf2=ptmp;
}

int Putpixel(SDL_Surface *surface, int x, int y,Uint32 pixel)
{

    if( x < 0 || x >= surface->w || y<0 || y >= surface->h)return 0;
     int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    switch(bpp) {
    case 1:
        *p = pixel;
        break;
    case 2:
        *(Uint16 *)p = pixel;
        break;
    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;
    case 4:
        *(Uint32 *)p = pixel;
        break;
    }

    return 0;
}



void fuzz(SDL_Surface * src, SDL_Surface *tar)
{
    if(SDL_MUSTLOCK(src))
        if(SDL_LockSurface(src) < 0)
           return ;
    if(SDL_MUSTLOCK(tar))
        if(SDL_LockSurface(tar) < 0)
           return ;

    int w = src->w,h= src->h;
    Uint8 *psrc;
    Uint8 *ptar;
     int xf,yf;
        for(int i = 0; i< w-1 ; i++)
        {
            for(int j = 0; j < h-1; j++)
            {
              xf =  buf1[i + j*WIDTH] - buf1[i+1+j*WIDTH];
              yf =  buf1[i + j*WIDTH] - buf1[i +(j+1)*WIDTH];
              xf = xf / 60;
              yf = yf / 60;
                psrc = (Uint8 *)src->pixels + j*src->pitch  + i*src->format->BytesPerPixel;
                Putpixel(tar,i+xf,j+yf,*(Uint32 *)psrc);
            }
        }


    if(SDL_MUSTLOCK(src))
        SDL_UnlockSurface(src);
    if(SDL_MUSTLOCK(tar))
        SDL_UnlockSurface(tar);
}

int main(int argc ,char *argv[])
{
    int isquit = 0;
    init();
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window * win = SDL_CreateWindow(
                                        "hello,water ; e-mail:liubailinprivate@163.com",
                                        SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED,
                                        WIDTH,HEIGHT,
                                        SDL_WINDOW_SHOWN);

    SDL_Surface *win_sur = SDL_GetWindowSurface(win);
    SDL_Event event;
    SDL_Event initevent;

    while(!isquit)
    {
        fuzz(img,win_sur);
        SDL_Delay(20);
         ripple();
           SDL_PollEvent(&event);
            switch(event.type)
            {
                case SDL_QUIT :
                     isquit = 1;
                break;
                case SDL_MOUSEBUTTONDOWN:
                        buf1[event.button.x+event.button.y*WIDTH] = -256*100;
                 break;
            }
            event = initevent;
            SDL_UpdateWindowSurface(win);
    }
	quit();/*exit this program*/
    return 0;
}

