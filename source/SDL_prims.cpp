/* SDL_prims.c -- 2D graphical primitives for SDL
 * 
 * Copyright (c) 2008 Ian Piumarta
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the 'Software'),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, provided that the above copyright notice(s) and this
 * permission notice appear in all copies of the Software and that both the
 * above copyright notice(s) and this permission notice appear in supporting
 * documentation.
 * 
 * THE SOFTWARE IS PROVIDED 'AS IS'.  USE ENTIRELY AT YOUR OWN RISK.
 * 
 * Last edited: 2008-06-12 12:32:57 by piumarta on WINDOWS-XP.piumarta.com
 */

#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <SDL.h>
#include "SDL_prims.h"

#define abs(N)		(((N) < 0) ? -(N) : (N))
#define sgn(N)		(((N) < 0) ? -1 : ((N) > 0 ? 1 : 0))
#define swap(T, A, B)	do { T tmp= A;  A= B;  B= tmp; } while (0)
#define min(A, B)	(((A) < (B) ? (A) : (B)))
#define max(A, B)	(((A) > (B) ? (A) : (B)))
#define clamp(A, X, B)	min(max(A, X), B)

#define CLIPX0(S)	((S)->clip_rect.x)
#define CLIPY0(S)	((S)->clip_rect.y)
#define CLIPW(S)	((S)->clip_rect.w)
#define CLIPH(S)	((S)->clip_rect.h)
#define CLIPX1(S)	(CLIPX0(S) + CLIPW(S))
#define CLIPY1(S)	(CLIPY0(S) + CLIPH(S))

#define CLIPX(S, X)	clamp(CLIPX0(S), X, CLIPX1(S) - 1)
#define CLIPY(S, Y)	clamp(CLIPY0(S), Y, CLIPY1(S) - 1)

#define INCLIPX(S, X)	((CLIPX0(S) <= (X)) && ((X) < CLIPX1(S)))
#define INCLIPY(S, Y)	((CLIPY0(S) <= (Y)) && ((Y) < CLIPY1(S)))
#define INCLIP(S, X, Y)	(INCLIPX(S, X) && INCLIPY(S, Y))

/* ---------------------------------------------------------------- */
/* DrawPixel							    */
/* ---------------------------------------------------------------- */



/* ---------------------------------------------------------------- */

/* Cohen-Sutherland clipping algorithm.
 * James D. Foley, Andries van Dam, et al, "Computer Graphics:
 * Principles and Practice", Addison-Wesley, 1995.  ISBN 0201848406.
 * (Section 3.12.3.) */


/* ---------------------------------------------------------------- */
/* DrawPolygon							    */
/* ---------------------------------------------------------------- */


void SDL_DrawPolygon(SDL_Renderer*& render, SDL_Point *v, int n, const SDL_Color& c)
{
  SDL_SetRenderDrawColor(render, c.r * 255, c.g * 255, c.b * 255, c.a * 255);

  if (n == 1) SDL_RenderDrawPoint(render, v->x, v->y);
  int i;
  for (i= 1;  i < n;  ++i)
    SDL_RenderDrawLine(render, v[i-1].x, v[i-1].y, v[i].x, v[i].y);
  SDL_RenderDrawLine(render, v[n-1].x, v[n-1].y, v[0].x, v[0].y);
}

/* ---------------------------------------------------------------- */
/* FillPolygon							    */
/* ---------------------------------------------------------------- */

/* Trivial scan-line fill algorithm.  For each scan line intersecting
 * the polygon, make an LR ordered list of crossing points.  Successive
 * pairs of points define runs of pixels lying within the polygon. */

void SDL_FillPolygon(SDL_Renderer*& render, SDL_Surface* s, SDL_Point *v, int n, const SDL_Color& c)
{
  SDL_SetRenderDrawColor(render, c.r * 255, c.g * 255, c.b * 255, c.a * 255);

  if (n == 1) SDL_RenderDrawPoint(render, v->x, v->y);
  int nxs;
  int* xs = (int*)alloca(sizeof(int) * n);
  int y, i, j, k;
  int y0= v[0].y, y1= y0;
  for (i= 1;  i < n;  ++i)
    {
      y= v[i].y;
      if (y < y0) y0= y;
      if (y > y1) y1= y;
    }
  if (y0 < CLIPY0(s)) y0= CLIPY0(s);
  if (y1 >= CLIPY1(s)) y1= CLIPY1(s) - 1;
  for (y= y0;  y <= y1;  ++y)
    {
      nxs= 0;
      j= n - 1;
      for (i= 0;  i < n;  j= i++)
	if ((v[i].y < y && y <= v[j].y) || (v[j].y < y && y <= v[i].y))
	  {
	    xs[nxs++]= (int)rint(v[i].x + ((double)y - v[i].y) / ((double)v[j].y - v[i].y) * ((double)v[j].x - v[i].x));
	    for (k= nxs - 1;  k && xs[k-1] > xs[k];  --k)
	      swap(int, xs[k-1], xs[k]);
	  }
      for (i= 0;  i < nxs;  i += 2)
	SDL_RenderDrawLine(render, xs[i], y, xs[i+1], y);
    }
}
