#include <SDL/SDL.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 768

typedef struct { int x, y; } Point;
typedef unsigned char Color[3];

void
put_pixel (SDL_Surface *surface,
           int x,
           int y,
           const Color color)
{
    char *pixels = (char *) surface->pixels;
    size_t offset = (y * surface->pitch) + (x * surface->format->BytesPerPixel);

    pixels[offset] = color[0];
    pixels[offset+1] = color[1];
    pixels[offset+2] = color[2];
}

void
draw_horizontal_line (SDL_Surface *surface,
                      const Point *p0,
                      const Point *p1,
                      const Color color)
{
    int x;

    if (p0->x > p1->x)
    {
        const Point *temp = p0;
        p0 = p1;
        p1 = temp;
    }
    
    for (x = p0->x; x <= p1->x; x++)
        put_pixel (surface, x, p0->y, color);
}

void
draw_vertical_line (SDL_Surface *surface,
                    const Point *p0,
                    const Point *p1,
                    const Color color)
{
    int y;

    if (p0->y > p1->y)
    {
        const Point *temp = p0;
        p0 = p1;
        p1 = temp;
    }

    for (y = p0->y; y <= p1->y; y++)
        put_pixel (surface, p0->x, y, color);
}

void
draw_x_major_line (SDL_Surface *surface,
                   const Point *p0,
                   const Point *p1,
                   const Color color)
{
    int x, y, y_step, dx, dy;
    int error, error_step, threshold;

    dx = p1->x - p0->x;
    dy = p1->y - p0->y;

    if (dy == 0)
    {
        draw_horizontal_line (surface, p0, p1, color);
        return;
    }

    if (dx < 0)
    {
        const Point *temp = p0;
        p0 = p1;
        p1 = temp;

        dx = -dx;
        dy = -dy;
    }

    y = p0->y;
    y_step = (dy < 0) ? -1 : 1;
    
    error = 0;
    error_step = 2 * abs (dy);
    threshold = 2 * dx;

    for (x = p0->x; x <= p1->x; x++)
    {
        put_pixel (surface, x, y, color);

        if ((error += error_step) > threshold)
        {
            error -= threshold;
            y += y_step;
        }
    }
}

void
draw_y_major_line (SDL_Surface *surface, 
                   const Point *p0,
                   const Point *p1,
                   const Color color)
{
    int x, y, x_step, dx, dy;
    int error, error_step, threshold;

    dx = p1->x - p0->x;
    dy = p1->y - p0->y;

    if (dx == 0)
    {
        draw_vertical_line (surface, p0, p1, color);
        return;
    }

    if (dy < 0)
    {
        const Point *temp = p0;
        p0 = p1;
        p1 = temp;

        dx = -dx;
        dy = -dy;
    }

    x = p0->x;
    x_step = (dx < 0) ? -1 : 1;
    
    error = 0;
    error_step = 2 * abs (dx);
    threshold = 2 * dy;

    for (y = p0->y; y <= p1->y; y++)
    {
        put_pixel (surface, x, y, color);

        if ((error += error_step) > threshold)
        {
            error -= threshold;
            x += x_step;
        }
    }
}

int
outcode (const SDL_Rect *bound,
         const Point *p)
{
    int code = 0;

    if (p->y < bound->y)
        code |= 8;
    else if (p->y > bound->h)
        code |= 4;

    if (p->x < bound->x)
        code |= 1;
    else if (p->x > bound->w)
        code |= 2;

    return code;
}

void
clip_horizontal (Point *p,
                 const Point *p0,
                 const Point *p1,
                 int y)
{
    if (p0->y == p1->y)
        return;

    p->x = p0->x + ((p1->x - p0->x) * (y - p0->y)) / (p1->y - p0->y);
    p->y = y;
}

void
clip_vertical (Point *p,
               const Point *p0,
               const Point *p1,
               int x)
{
    if (p0->x == p1->x)
        return;

    p->x = x;
    p->y = p0->y + ((p1->y - p0->y) * (x - p0->x)) / (p1->x - p0->x);
}

int
contains (const SDL_Rect *bound,
          const Point *point)
{
    return (point->x >= bound->x &&
            point->x <= bound->w &&
            point->y >= bound->y &&
            point->y <= bound->h);
}
          

int
clip_line (const SDL_Rect *bound,
           Point *p0,
           Point *p1)
{
    int outcode0, outcode1;

    outcode0 = outcode (bound, p0);
    outcode1 = outcode (bound, p1);

    if (!(outcode0 | outcode1))
        return 1;

    if (outcode0 & outcode1)
        return 0;

    if (p0->y < bound->y)
        clip_horizontal (p0, p0, p1, bound->y);
    else if (p1->y < bound->y)
        clip_horizontal (p1, p0, p1, bound->y);

    if (p0->y > bound->h)
        clip_horizontal (p0, p0, p1, bound->h);
    else if (p1->y > bound->h)
        clip_horizontal (p1, p0, p1, bound->h);
                
    if (p0->x > bound->w)
        clip_horizontal (p0, p0, p1, bound->w);
    else if (p1->x > bound->w)
        clip_horizontal (p1, p0, p1, bound->w);

    if (p0->x < bound->x)
        clip_horizontal (p0, p0, p1, bound->x);
    else if (p1->x < bound->x)
        clip_horizontal (p1, p0, p1, bound->x);

    return (contains (bound, p0) &&
            contains (bound, p1));
}

void
draw_line (SDL_Surface *surface,
           const Point *p0,
           const Point *p1,
           const Color color)
{
    int dx = p1->x - p0->x;
    int dy = p1->y - p0->y;

    if (abs (dx) > abs (dy)) 
        draw_x_major_line (surface, p0, p1, color);
    else
        draw_y_major_line (surface, p0, p1, color);
}

void
draw_clipped_line (SDL_Surface *surface,
                   const SDL_Rect *bound,
                   const Point *p0_,
                   const Point *p1_,
                   const Color color)

{
    Point p0, p1;

    p0.x = p0_->x; p0.y = p0_->y;
    p1.x = p1_->x; p1.y = p1_->y;

    if (clip_line (bound, &p0, &p1))
        draw_line (surface, &p0, &p1, color);
}

void
convert_rect (SDL_Rect *to,
              const SDL_Rect *from)
{
    to->x = from->x;
    to->y = from->y;
    to->w = from->x + from->w - 1;
    to->h = from->y + from->h - 1;
}

int
main ()
{
    SDL_Surface *screen;
    SDL_Event event;
    SDL_Rect bound;

    Point p0, p1;
    Color color;

    int start_time, time_taken;
    unsigned long line_count;

    if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTTHREAD) == -1)
    {
        fprintf (stderr, "Unable to initialize SDL: %s\n", SDL_GetError ());
        exit (1);
    }

    atexit (SDL_Quit);

    screen = SDL_SetVideoMode (SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
    if (!screen)
    {
        fprintf (stderr, "Unable to set video mode: %s\n", SDL_GetError ());
        exit (1);
    }

    SDL_WM_SetCaption ("Line drawing demo in C", NULL);

    bound.x = 100; bound.y = 100;
    bound.w = 400; bound.h = 300;

    srand (time (NULL));

    line_count = 0;
    start_time = SDL_GetTicks ();

    while (1)
    {
        p0.x = rand () % SCREEN_WIDTH;
        p0.y = rand () % SCREEN_HEIGHT;
        p1.x = rand () % SCREEN_WIDTH;
        p1.y = rand () % SCREEN_HEIGHT;

        color[0] = rand () & 0xFF;
        color[1] = rand () & 0xFF;
        color[2] = rand () & 0xFF;
        draw_clipped_line (screen, &bound, &p0, &p1, color);

        line_count++;
        if ((line_count & 0xFFFF) == 0)
            SDL_Flip (screen);

        if (SDL_PollEvent (&event) == 0)
            continue;

        if (event.type == SDL_KEYDOWN)
            break;
    }

    time_taken = SDL_GetTicks () - start_time;
    printf ("Lines per sec: %f\n", (float) (line_count * 1000) / time_taken);
 
    return 0;
}
