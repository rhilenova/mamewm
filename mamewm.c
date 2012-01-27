/**************************************************************************//**
 * @brief      Main program for mamewm
 *
 * @details    Handles starting mamewm, then hands off
 *
 * @authors    Daniel Sirgey <rhile_nova@gmail.com>
 *
 * @date       Created 2012/12/1
 *
 * @file
 *****************************************************************************/
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <xcb/xcb.h>

#define bool short
#define false 0
#define true 1

bool Verbose = false;
int sigcode;
xcb_connection_t *connection;
char *program_name;
xcb_generic_error_t *xcb_error;

/**
 * @brief Default mamewm signal handler
 *
 * @param sig signal received 
 */
/*****************************************************************************/
void sigcatch(int sig)
{
    sigcode = sig;
}

/**
 * @brief Cleanup and shutdown mamewm
 *
 * @param code exit code
 */
/*****************************************************************************/
void cleanup(int code)
{
    xcb_set_input_focus(connection, XCB_NONE, XCB_INPUT_FOCUS_POINTER_ROOT,
                        XCB_CURRENT_TIME);
    xcb_flush(connection);
    xcb_disconnect(connection);

    // Print shutdown message
    printf("Shutting down %s...\n", program_name);

    exit(code);
}

/**
 * @brief Program entry point
 *
 * @param argc number of command line arguments
 * @param argv command line arguments
 */
/*****************************************************************************/
int main(int argc, char* argv[])
{
    /**************************************************************************
     * Variables
     *************************************************************************/
    xcb_screen_t     *screen;
    xcb_window_t      root;
    const short ATOM_NUMBER = 3;
    char *atom_names[] = {"_NET_WM_DESKTOP",
                          "WM_DELETE_WINDOW",
                          "WM_PROTOCOLS"};
    xcb_intern_atom_cookie_t cookies[ATOM_NUMBER];
    xcb_atom_t atoms[ATOM_NUMBER];
    int counter;
    bool error = false;
    uint32_t mask = 0;
    uint32_t values[2];
    xcb_void_cookie_t cookie;

    /**************************************************************************
     * Main setup
     *************************************************************************/
    // handle command line arguments
    for(counter = 1; counter < argc; ++counter)
    {
        if(argv[counter][0] == '-')
        {
            switch (argv[counter][1])
            {
                case 'v': // -verbose
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

    program_name = argv[0];
    // Command line argument failed, print usage and exit
    if(error == true)
    {
        fprintf(stderr, "usage:  %s [-v]\n", program_name);
        exit(-1);
    }

    // Print startup message
    printf("Starting %s...\n", argv[0]);

    // Setup signal handlers
    if(signal(SIGINT, sigcatch) == SIG_ERR)
    {
        perror("setting SIGINT handler");
        exit(-1);
    }
    if(signal(SIGTERM, sigcatch) == SIG_ERR)
    {
        perror("setting SIGTERM handler");
        exit(-1);
    }

    // Ignore child exiting
    if(signal(SIGCHLD, SIG_IGN) == SIG_ERR)
    {
        perror("setting SIGCHLD handler");
        exit(-1);
    }

    /**************************************************************************
     * XCB setup
     *************************************************************************/
    // connect to DISPLAY:0
    connection = xcb_connect(NULL, NULL);
    if(xcb_connection_has_error(connection))
    {
        perror("xcb_connect");
        exit(-1);
    }

    // get the first screen
    screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;

    // get the root window
    root = screen->root;

    // get atoms
    //  send atom requests
    for(counter = 0; counter < ATOM_NUMBER; ++counter)
    {
        cookies[counter] = xcb_intern_atom(connection, 0,
                                           strlen(atom_names[counter]),
                                           atom_names[counter]);
    }

    //  get server responses
    for(counter = 0; counter < ATOM_NUMBER; ++counter)
    {
        // @todo TODO handle errors?
        xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(connection,
                                                               cookies[counter],
                                                               &xcb_error);

        // if no error, get atom from response
        if(xcb_error == NULL)
        {
            atoms[counter] = reply->atom;
            free(reply);
        }
    }

    // Subscribe to events
    mask = XCB_CW_EVENT_MASK;
    values[0] = XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
                XCB_EVENT_MASK_STRUCTURE_NOTIFY |
                XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY;
    cookie = xcb_change_window_attributes_checked(connection, root, mask,
                                                  values);
    xcb_error = xcb_request_check(connection, cookie);

    xcb_flush(connection);

    if (xcb_error != NULL)
    {
        fprintf(stderr, "MAMEwm: Can't subscribe to events - error code %d."
                " Another window manager already running?\n",
                xcb_error->error_code);
    }

    /**************************************************************************
     * Cleanup and shutdown
     *************************************************************************/
    cleanup(sigcode);

    return 0;
}

