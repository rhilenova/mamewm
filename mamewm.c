#include <stdio.h>

#define bool short
#define false 0
#define true 1

bool Verbose = false;

int main(int argc, char* argv[])
{
  /* handle command line arguments */
  bool error = false;
  short i;
  for (i = 1; i < argc; ++i)
  {
    if (argv[i][0] == '-')
    {
      switch (argv[i][1])
      {
        /* -verbose */
        case 'v':
          Verbose = true;
          continue;
      }
      error = true;
      break;
    } else
    {
      error = true;
    }
  }

  /* Command line argument failed, print usage and exit */
  if (error == true)
  {
    fprintf(stderr, "usage:  %s [-v]\n", argv[0]);
    exit(1);
  }

#if 0
  xcb_create_window(connection,                    /* xcb_connection */
                    depth,                         /* depth */
                    window,                        /* windowid */
                    root->root,                    /* parent window */
                    0,                             /* x */
                    0,                             /* y */
                    150,                           /* width */
                    150,                           /* height */
                    10,                            /* border_width */
                    XCB_WINDOW_CLASS_INPUT_OUTPUT, /* _class */
                    root->root_visual,             /* visualid */
                    mask,                          /* value mask */
                    valwin);                       /* value list */
#endif
}
