#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <X11/Xlib.h>

typedef struct s_env s_env;
struct s_env
{
  GC            gc;
  Display       *display;
  int           screen;
  Window        win,
                root;
  unsigned long white_pixel,
                black_pixel;
};

/* TODO Fix crach */

/*
 * Function write on window
 */
void
write_string (s_env *e, char *s)
{
  XClearWindow(e->display, e->win);
  XDrawString(e->display, e->win, e->gc, 10, 30, s, strlen(s));
}


/*
 * Main function
 */
int
main (int argc, char **argv)
{
  /* variable */
  bool quit = false;
  /* Mouse */
  int     x = 0,
          y = 0;
  /* Buufer */
  char    *buf      = malloc(256);
  char    *str_buf  = malloc(256);
  /* Xlib */
  s_env   window_1;
  XEvent  ev;
  /* Key */
  KeySym  keysym;

  /* Open connection with X11 server */
  if ((window_1.display = XOpenDisplay(NULL)) == NULL)
    {
      fprintf(stderr, "[ERROR] Display %s not know\n", argv[1]);
      exit(EXIT_FAILURE);
    }

  /* Set X variable */
  window_1.gc      = DefaultGC(window_1.display, window_1.screen);
  window_1.screen  = DefaultScreen(window_1.display);
  window_1.root    = RootWindow(window_1.display, window_1.screen);
  window_1.white_pixel = WhitePixel(window_1.display, window_1.screen);
  window_1.black_pixel = BlackPixel(window_1.display, window_1.screen);

  /* Creat window */
  window_1.win = XCreateSimpleWindow( window_1.display,
                                      window_1.root,
                                      0, 0, 200, 75, 0,
                                      window_1.black_pixel,
                                      window_1.white_pixel);

  /* Create event listener */
  XSelectInput( window_1.display, window_1.win,
                ExposureMask|ButtonPressMask|
                ButtonReleaseMask|KeyReleaseMask);
  /* Set window name */
  XStoreName(window_1.display, window_1.win, "HelloWorld");
  /* Display window */
  XMapWindow(window_1.display, window_1.win);

  /* Main loop */
  while(!quit)
    {
      /* Get event */
      XNextEvent(window_1.display, &ev);

      if (ev.type = Expose)
        {
          write_string(&window_1, "HelloWorld ! <3");
        }

      if (ev.type = ButtonRelease)
        {
          if (ev.xbutton.button)
            {
              x = ev.xbutton.x;
              y = ev.xbutton.y;

              sprintf(buf, "Mouse click on (%d,%d)", x, y);
              printf("%s\n", buf);
              write_string(&window_1, buf);
            }
        }

      if (ev.type = KeyRelease)
        {
          strcpy(buf, "");
          if (XLookupString(&ev.xkey, buf, 256, &keysym, NULL))
            {
              printf("%s\n", buf);
              strcat(str_buf, buf);
              write_string(&window_1, str_buf);
              if (strstr(str_buf, "quit"))
                quit = true;
            }
        }
    }


  write_string(&window_1, "Bye ! <3");
  printf("Bye ! <3\n");

  sleep(1);

  /* Close X11 server connection */
  XCloseDisplay(window_1.display);

  return EXIT_SUCCESS;
}
