/*
参考地址: https://blog.csdn.net/yp120yp/article/details/7543298
*/
#include<iostream>
#include<SDL2/SDL.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

using namespace std;
SDL_Surface *win_sur = 0;
 int isquit = 0;
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
            buf2[i]-=buf2[i]>>5;
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
    /*
     int xf,yf;
        SDL_BlitSurface(src,0,tar,0);
        for(int i = 0; i< w-1 ; i++)
        {
            for(int j = 0; j < h-1; j++)
            {
              xf =  buf1[i + j*WIDTH] - buf1[i+1+j*WIDTH];
              yf =  buf1[i + j*WIDTH] - buf1[i +(j+1)*WIDTH];
                psrc = (Uint8 *)src->pixels + j*src->pitch  + i*src->format->BytesPerPixel;
                Putpixel(tar,i+xf,j+yf,*(Uint32 *)psrc);
            }
        }
*/

      int xoff, yoff;
      int k = WIDTH;
      for (int i=1; i<HEIGHT-1; i++)
      {
          for (int j=0; j<WIDTH; j++)
          {
              //计算偏移量
              xoff = buf1[k-1]-buf1[k+1];
              yoff = buf1[k-WIDTH]-buf1[k+WIDTH];
              if ((i+yoff )< 0 ) {k++; continue;}
              if ((i+yoff )> HEIGHT) {k++; continue;}
              if ((j+xoff )< 0 ) {k++; continue;}
              if ((j+xoff )> WIDTH ) {k++; continue;}

             psrc = (Uint8 *)src->pixels + i*src->pitch  + j*src->format->BytesPerPixel;
             Putpixel(tar,j+xoff,i+yoff,*(Uint32 *)psrc);
              k++;
          }
      }

    if(SDL_MUSTLOCK(src))
        SDL_UnlockSurface(src);
    if(SDL_MUSTLOCK(tar))
        SDL_UnlockSurface(tar);
}

void stone(int x,int y,int r,int h) {
    int r_2 = r*r;
    for( int i = -r; i < r; i++ ) {
        int t = sqrt(r_2 - i*i );
        for(int j = -t ; j < t; j++) {
            buf1[x + j + y*WIDTH] = h;
        }
    }
}
SDL_Thread *ripplethread;

int runthread(void *d) {
//线程不安全
    while(!isquit) {
        fuzz(img,win_sur);
    //    SDL_Delay(20);
        ripple();
    }
}

int main(int argc ,char *argv[])
{

    init();
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window * win = SDL_CreateWindow(
                                        "hello,water ; e-mail:liubailinprivate@163.com",
                                        SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED,
                                        WIDTH,HEIGHT,
                                        SDL_WINDOW_SHOWN);

    win_sur = SDL_GetWindowSurface(win);
    SDL_Event event;
    SDL_Event initevent;

    ripplethread = SDL_CreateThread(runthread,"ripple",0);

    while(!isquit)
    {

           SDL_PollEvent(&event);
            switch(event.type)
            {
                case SDL_QUIT :
                     isquit = 1;
                break;
                case SDL_MOUSEBUTTONDOWN:
                stone(event.button.x,event.button.y,50,-180);
                break;
            }
            event = initevent;
            SDL_UpdateWindowSurface(win);
    }
	quit();/*exit this program*/
    return 0;
}

