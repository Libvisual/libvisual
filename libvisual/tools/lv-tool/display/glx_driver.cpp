#define _POSIX_C_SOURCE 200112L

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/select.h>

#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <X11/extensions/xf86vmode.h>
#include <X11/keysym.h>

#include "lv_x11_key.hpp"
#include "glx_driver.hpp"

#define GLX_NATIVE(obj)                 (VISUAL_CHECK_CAST ((obj), GLXNative))

typedef struct _GLXNative GLXNative;

namespace {

  int glx_gl_attribute_map[] = {
      -1,                   // VISUAL_GL_ATTRIBUTE_NONE
      GLX_BUFFER_SIZE,      // VISUAL_GL_ATTRIBUTE_BUFFER_SIZE
      GLX_LEVEL,            // VISUAL_GL_ATTRIBUTE_LEVEL
      GLX_RGBA,             // VISUAL_GL_ATTRIBUTE_RGBA
      GLX_DOUBLEBUFFER,     // VISUAL_GL_ATTRIBUTE_DOUBLEBUFFER
      GLX_STEREO,           // VISUAL_GL_ATTRIBUTE_STEREO
      GLX_AUX_BUFFERS,      // VISUAL_GL_ATTRIBUTE_AUX_BUFFERS
      GLX_RED_SIZE,         // VISUAL_GL_ATTRIBUTE_RED_SIZE
      GLX_GREEN_SIZE,       // VISUAL_GL_ATTRIBUTE_GREEN_SIZE
      GLX_BLUE_SIZE,        // VISUAL_GL_ATTRIBUTE_BLUE_SIZE
      GLX_ALPHA_SIZE,       // VISUAL_GL_ATTRIBUTE_ALPHA_SIZE
      GLX_DEPTH_SIZE,       // VISUAL_GL_ATTRIBUTE_DEPTH_SIZE
      GLX_STENCIL_SIZE,     // VISUAL_GL_ATTRIBUTE_STENCIL_SIZE
      GLX_ACCUM_RED_SIZE,   // VISUAL_GL_ATTRIBUTE_ACCUM_RED_SIZE
      GLX_ACCUM_GREEN_SIZE, // VISUAL_GL_ATTRIBUTE_ACCUM_RED_SIZE
      GLX_ACCUM_BLUE_SIZE,  // VISUAL_GL_ATTRIBUTE_ACCUM_RED_SIZE
      GLX_ACCUM_ALPHA_SIZE, // VISUAL_GL_ATTRIBUTE_ACCUM_RED_SIZE
      -1                    // VISUAL_GL_ATTRIBUTE_LAST
  };

  // int get_nearest_resolution (SADisplay *display, int *width, int *height);

  int X11_Pending(Display *display);

  XVisualInfo *get_xvisualinfo_filter_capabilities (Display *dpy, int screen, VisVideoAttributeOptions const* vidoptions);

}

class GLXDriver
    : public SADisplayDriver
{
public:

    GLXDriver (SADisplay& display)
        : m_display (display)
        , m_running (false)
    {}

    virtual ~GLXDriver ()
    {
        close ();
    }

    virtual int create (VisVideoDepth depth, VisVideoAttributeOptions const* vidoptions,
                        unsigned int width, unsigned int height, bool resizable)
    {
        lv_x11_key_init (&m_key);

        // get a connection
        m_dpy = XOpenDisplay(0);
        m_screen = DefaultScreen(m_dpy);

        int glxMajorVersion, glxMinorVersion;
        int vidModeMajorVersion, vidModeMinorVersion;

        XF86VidModeQueryVersion(m_dpy, &vidModeMajorVersion, &vidModeMinorVersion);
        std::printf ("XF86VidModeExtension-Version %d.%d\n", vidModeMajorVersion, vidModeMinorVersion);

        XF86VidModeModeInfo **modes;
        int modeNum;

        XF86VidModeGetAllModeLines(m_dpy, m_screen, &modeNum, &modes);

        // save desktop-resolution before switching modes
        m_deskMode = *modes[0];

        // look for mode with requested resolution
        /*
          bestMode = 0;

          for (i = 0; i < modeNum; i++)
          {
              if ((modes[i]->hdisplay == width) && (modes[i]->vdisplay == height))
              {
                  bestMode = i;
              }
          }
        */

        // get an appropriate visual
        XVisualInfo* vi = get_xvisualinfo_filter_capabilities (m_dpy, m_screen, vidoptions);
        if (!vi) {
            std::printf ("No visual found.\n");
            visual_error_raise (VISUAL_ERROR_GENERAL);
        }

        glXQueryVersion(m_dpy, &glxMajorVersion, &glxMinorVersion);
        std::printf ("glX-Version %d.%d\n", glxMajorVersion, glxMinorVersion);

        // create a GLX context
        m_ctx = glXCreateContext(m_dpy, vi, 0, GL_TRUE);

        // create a color map
        Colormap cmap = XCreateColormap(m_dpy, RootWindow(m_dpy, vi->screen), vi->visual, AllocNone);
        m_attr.colormap = cmap;
        m_attr.border_pixel = 0;

        // create a window in window mode
        m_attr.event_mask = KeyPressMask | KeyReleaseMask
                          | ButtonPressMask | ButtonReleaseMask
                          | StructureNotifyMask | VisibilityChangeMask;

        m_win = XCreateWindow(m_dpy, RootWindow(m_dpy, vi->screen),
                              0, 0, width, height, 0, vi->depth, InputOutput, vi->visual,
                              CWBorderPixel | CWColormap | CWEventMask, &m_attr);

        XFree (vi);

        // only set window title and handle wm_delete_events if in windowed mode

        Atom wmDelete = XInternAtom(m_dpy, "WM_DELETE_WINDOW", True);

        XSetWMProtocols(m_dpy, m_win, &wmDelete, 1);
        XSetStandardProperties(m_dpy, m_win, "jahoor", "jahoor", None, NULL, 0, NULL);
        XMapRaised(m_dpy, m_win);

        // connect the glx-context to the window

        Window winDummy;
        unsigned int borderDummy;

        glXMakeCurrent(m_dpy, m_win, m_ctx);
        XGetGeometry(m_dpy, m_win, &winDummy, &m_x, &m_y,
                     &m_width, &m_height, &borderDummy, &m_depth);

        std::printf ("Depth %d\n", m_depth);

        if (glXIsDirect(m_dpy, m_ctx))
            std::printf("Congrats, you have Direct Rendering!\n");
        else
            std::printf("Sorry, no Direct Rendering possible!\n");

        m_WM_DELETE_WINDOW = XInternAtom(m_dpy, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(m_dpy, m_win, &m_WM_DELETE_WINDOW, 1);

        m_lastwidth = width;
        m_lastheight = height;

        m_running = true;

        return 0;
    }

    virtual int close ()
    {
        if (!m_running)
            return 0;

        if (m_ctx) {
            if (!glXMakeCurrent (m_dpy, None, NULL)) {
                printf("Could not release drawing context.\n");
            }

            glXDestroyContext (m_dpy, m_ctx);
            m_ctx = NULL;
        }

        // switch back to original desktop resolution if we were in fs
        if (m_fs) {
            XF86VidModeSwitchToMode (m_dpy, m_screen, &m_deskMode);
            XF86VidModeSetViewPort (m_dpy, m_screen, 0, 0);
        }

        XCloseDisplay (m_dpy);

        m_running = false;

        return 0;
    }

    virtual int lock ()
    {
        return 0;
    }

    virtual int unlock ()
    {
        return 0;
    }

    virtual int set_fullscreen (bool fullscreen, bool autoscale)
    {
        // GLXNative *native = GLX_NATIVE (display->native);
        // Surface *screen = m_screen;

        // if (fullscreen == TRUE) {
        //     if (!(screen->flags & FULLSCREEN)) {
        //         if (autoscale == TRUE) {
        //             int width = display->screen->width;
        //             int height = display->screen->height;

        //             m_oldwidth = width;
        //             m_oldheight = height;

        //             get_nearest_resolution (display, &width, &height);

        //             native_create (display, m_requested_depth, NULL, width, height, m_resizable);
        //         }

        //         ShowCursor (SDL_FALSE);
        //         WM_ToggleFullScreen (screen);
        //     }
        // } else {
        //     if ((screen->flags & FULLSCREEN)) {
        //         ShowCursor (SDL_TRUE);
        //         WM_ToggleFullScreen (screen);

        //         if (autoscale == TRUE)
        //             native_create (display, m_requested_depth, NULL, m_oldwidth,
        //                            m_oldheight, m_resizable);
        //     }
        // }

        return 0;
    }

    virtual int get_video (VisVideo* screen)
    {
        visual_video_set_depth (screen, VISUAL_VIDEO_DEPTH_GL);

        visual_video_set_dimension (screen, m_width, m_height);

        m_video = screen;

        return 0;
    }

    virtual int update_rect (LV::Rect const& rect)
    {
        glXSwapBuffers (m_dpy, m_win);

        return 0;
    }

    virtual int drain_events (VisEventQueue& eventqueue)
    {
        XEvent xevent;

        while (X11_Pending (m_dpy) > 0) {
            VisKeySym keysym;

            XNextEvent (m_dpy, &xevent);

            switch (xevent.type) {
                case ConfigureNotify:
                    if (xevent.xconfigure.width  != int (m_lastwidth) ||
                        xevent.xconfigure.height != int (m_lastheight)) {

                        m_width = xevent.xconfigure.width;
                        m_height = xevent.xconfigure.height;

                        visual_event_queue_add_resize (&eventqueue, m_video,
                                                       xevent.xconfigure.width, xevent.xconfigure.height);
                    }

                    break;

                case ButtonPress:
                    visual_event_queue_add_mousebutton (&eventqueue, xevent.xbutton.button, VISUAL_MOUSE_DOWN,
                                                        xevent.xbutton.x, xevent.xbutton.y);
                    break;

                case ButtonRelease:
                    visual_event_queue_add_mousebutton (&eventqueue, xevent.xbutton.button, VISUAL_MOUSE_UP,
                                                        xevent.xbutton.x, xevent.xbutton.y);
                    break;

                case KeyPress:
                    lv_x11_key_lookup (&m_key, m_dpy, &xevent.xkey, xevent.xkey.keycode, &keysym, TRUE);
                    visual_event_queue_add_keyboard (&eventqueue, keysym.sym, keysym.mod, VISUAL_KEY_DOWN);

                    break;

                case KeyRelease:
                    lv_x11_key_lookup (&m_key, m_dpy, &xevent.xkey, xevent.xkey.keycode, &keysym, FALSE);
                    visual_event_queue_add_keyboard (&eventqueue, keysym.sym, keysym.mod, VISUAL_KEY_UP);

                    break;

                case ClientMessage:
                    if (xevent.xclient.format == 32 &&
                        xevent.xclient.data.l[0] == int(m_WM_DELETE_WINDOW)) {

                        visual_event_queue_add_quit (&eventqueue, FALSE);
                    }

                    break;

                case MotionNotify:
                    visual_event_queue_add_mousemotion (&eventqueue, xevent.xmotion.x, xevent.xmotion.y);
                    break;

                case VisibilityNotify:
                    if (xevent.xvisibility.state == VisibilityUnobscured ||
                        xevent.xvisibility.state == VisibilityPartiallyObscured) {
                        visual_event_queue_add_visibility (&eventqueue, TRUE);
                    } else if (xevent.xvisibility.state == VisibilityFullyObscured) {
                        visual_event_queue_add_visibility (&eventqueue, FALSE);
                    }

                    break;
            }
        }

        return 0;
    }

private:

    SADisplay&  m_display;
    Display    *m_dpy;
    Window      m_win;
    int         m_screen;
    GLXContext  m_ctx;
    XSetWindowAttributes m_attr;
    Bool        m_fs;
    Bool        m_doubleBuffered;
    XF86VidModeModeInfo m_deskMode;

    VisVideoDepth m_requested_depth;

    LVX11Key m_key;

    unsigned int m_lastwidth;
    unsigned int m_lastheight;

    unsigned int m_width;
    unsigned int m_height;
    int          m_x;
    int          m_y;

    unsigned int m_depth;

    int m_oldx;
    int m_oldy;

    int m_oldwidth;
    int m_oldheight;

    bool m_resizable;
    bool m_running;

    VisVideo *m_video;

    // Atoms
    Atom m_WM_DELETE_WINDOW;
};


namespace
{
  // Ack!  XPending() actually performs a blocking read if no events available */
  // Taken from SDL
  int X11_Pending(Display *display)
  {
      /* Flush the display connection and look to see if events are queued */
      XFlush(display);
      if ( XEventsQueued(display, QueuedAlready) ) {
          return(1);
      }

      /* More drastic measures are required -- see if X is ready to talk */
      {
          static struct timeval zero_time;        /* static == 0 */
          int x11_fd;
          fd_set fdset;

          x11_fd = ConnectionNumber(display);
          FD_ZERO(&fdset);
          FD_SET(x11_fd, &fdset);
          if ( select(x11_fd+1, &fdset, NULL, NULL, &zero_time) == 1 ) {
              return(XPending(display));
          }
      }

      /* Oh well, nothing is ready .. */
      return(0);
  }

  XVisualInfo *get_xvisualinfo_filter_capabilities (Display *dpy, int screen, VisVideoAttributeOptions const* vidoptions)
  {
      int attrList[64];
      int attrc = 0;
      int i;

      if (vidoptions == NULL)
          return NULL;

      /* FIXME filter for capabilities, like doublebuffer */
      for (i = VISUAL_GL_ATTRIBUTE_NONE; i < VISUAL_GL_ATTRIBUTE_LAST; i++) {
          if (vidoptions->gl_attributes[i].mutated == TRUE) {
              int glx_attribute =
                  glx_gl_attribute_map[vidoptions->gl_attributes[i].attribute];

              if (glx_attribute < 0)
                  continue;

              attrList[attrc++] = glx_attribute;

              /* Check if it's a non boolean attribute */
              if (glx_attribute != GLX_RGBA && glx_attribute != GLX_DOUBLEBUFFER && glx_attribute != GLX_STEREO) {
                  attrList[attrc++] = vidoptions->gl_attributes[i].value;
              }
          }
      }

      attrList[attrc++] = None;

      return glXChooseVisual (dpy, screen, attrList);
  }

} // anonymous namespace

SADisplayDriver* glx_driver_new (SADisplay& display)
{
    return new GLXDriver (display);
}
