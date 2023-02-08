#ifndef _LV_EVENT_H
#define _LV_EVENT_H

#include <libvisual/lv_songinfo.h>
#include <libvisual/lv_keysym.h>

/**
 * @defgroup VisEvent VisEvent
 * @{
 */

/**
 * Types of events supported by LV
 */
typedef enum {
    VISUAL_EVENT_NOEVENT,         /**< No event */
    VISUAL_EVENT_KEYDOWN,         /**< Key press event */
    VISUAL_EVENT_KEYUP,           /**< Key release event */
    VISUAL_EVENT_MOUSEMOTION,     /**< Mouse movement event */
    VISUAL_EVENT_MOUSEBUTTONDOWN, /**< Mouse button press event */
    VISUAL_EVENT_MOUSEBUTTONUP,   /**< Mouse button release event */
    VISUAL_EVENT_NEWSONG,         /**< Song change event */
    VISUAL_EVENT_RESIZE,          /**< Video resize event */
    VISUAL_EVENT_PARAM,           /**< Parameter change event */
    VISUAL_EVENT_QUIT,            /**< Quit event */
    VISUAL_EVENT_CUSTOM,          /**< Custom event */
    VISUAL_EVENT_VISIBILITY,      /**< Visibility event */
} VisEventType;

/**
 * Indicates the state of a key.
 */
typedef enum {
    VISUAL_KEY_DOWN,        /**< Key is pressed. */
    VISUAL_KEY_UP           /**< Key is released. */
} VisKeyState;

/**
 * Indicates the state of a mouse button.
 */
typedef enum {
    VISUAL_MOUSE_DOWN,      /**< Mouse button is pressed. */
    VISUAL_MOUSE_UP         /**< Mouse button is released. */
} VisMouseState;

typedef struct _VisEventKeyboard VisEventKeyboard;
typedef struct _VisEventMouseMotion VisEventMouseMotion;
typedef struct _VisEventMouseButton VisEventMouseButton;
typedef struct _VisEventResize VisEventResize;
typedef struct _VisEventNewSong VisEventNewSong;
typedef struct _VisEventQuit VisEventQuit;
typedef struct _VisEventVisibility VisEventVisibility;
typedef struct _VisEventParam VisEventParam;
typedef struct _VisEventCustom VisEventCustom;
typedef struct _VisEvent VisEvent;

/**
 * Keyboard event.
 *
 * @see visual_event_new_keyboard()
 */
struct _VisEventKeyboard {
    VisKeySym keysym;     /**< Key code and key modifier state */
};

/**
 * Mouse movement event.
 *
 * @see visual_event_new_mousemotion()
 */
struct _VisEventMouseMotion {
    VisMouseState state;  /**< Mouse button state. */
    int           x;      /**< X-coordinate of mouse pointer position. */
    int           y;      /**< Y-coordinate of mouse pointer position. */
    int           xrel;   /**< Relative motion along the X-axis */
    int           yrel;   /**< Relative motion along the Y-axis */
};

/**
 * Mouse button event.
 *
 * @see visual_event_new_mousebutton()
 */
struct _VisEventMouseButton {
    VisMouseState state;    /**< Mouse button state. */
    int           button;   /**< Mouse button. */
    int           x;        /**< Relative motion along the X-axis. */
    int           y;        /**< Relative motion along the Y-axis. */
};

/**
 * Resize event.
 *
 * @see visual_event_new_resize()
 */
struct _VisEventResize {
    int width;   /**< Width of the surface. */
    int height;  /**< Height of the surface. */
};

/**
 * Song change event.
 *
 * @see visual_event_new_newsong()
 */
struct _VisEventNewSong {
    VisSongInfo *songinfo;  /**< Song information */
};

/**
 * Application quit event.
 *
 * @see visual_event_new_quit()
 */
struct _VisEventQuit {};

/**
 * Visibility event.
 *
 * @see visual_event_new_visibiity()
 */
struct _VisEventVisibility {
    int is_visible;    /** Visibility */
};

/**
 * Parameter change event.
 *
 * Contains information about parameter changes.
 *
 * @see visual_event_new_param()
 */
struct _VisEventParam {
    // FIXME: Have to use void* as there is a circular dependency
    // between lv_event.h and lv_param.h

    void *param;   /**< Parameter that was changed. */
};

/**
 * Custom event.
 *
 * @note Used for creating custom events.
 *
 * @see visual_event_new_custom()
 */
struct _VisEventCustom {
    int   event_id;  /**< Unique number for identification */
    int   data_int;  /**< Optional integer data */
    void *data_ptr;  /**< Optional data pointer */
};

/**
 * Generic event.
 *
 * @see visual_event_new()
 */
struct _VisEvent
{
    VisEventType type;

    union {
        VisEventKeyboard    keyboard;    /**< Keyboard event. */
        VisEventMouseMotion mousemotion; /**< Mouse movement event. */
        VisEventMouseButton mousebutton; /**< Mouse button event. */
        VisEventResize      resize;      /**< Dimension change event. */
        VisEventNewSong     newsong;     /**< Song change event. */
        VisEventQuit        quit;        /**< Quit event. */
        VisEventVisibility  visibility;  /**< Plugin visible event. */
        VisEventCustom      custom;      /**< Custom event. */
        VisEventParam       param;       /**< Param change event. */
    } event;
};

#ifdef __cplusplus

#include <memory>

namespace LV {

  typedef VisEvent Event;

  class LV_API EventQueue
  {
  public:

      EventQueue ();

      EventQueue (EventQueue const&) = delete;

      ~EventQueue ();

      EventQueue& operator= (EventQueue const&) = delete;

      /**
       * Polls for new events. Looks at the event queue for new events
       * and deletes them from the queue while loading them into the
       * event argument.
       *
       * @param event First queued event, if queue is not empty
       *
       * @return true if an event was returned, false otherwise
       */
      bool poll (Event& event);

      /**
       * Adds an event to the event queue. Add new VisEvents into the
       * VisEventQueue.
       *
       * @param event Event to add
       */
      void add (Event const& event);

  private:

      class Impl;
      const std::unique_ptr<Impl> m_impl;
  };

} // LV namespace

#endif /* __cplusplus */

#ifdef __cplusplus
typedef LV::EventQueue VisEventQueue;
#else
typedef struct _VisEventQueue VisEventQueue;
struct _VisEventQueue;
#endif

LV_BEGIN_DECLS

/**
 * Creates a new keyboard event
 *
 * @param keysym Key used
 * @param keymod Key modifier used
 * @param state  State of key i.e. pressed or released
 */
LV_NODISCARD LV_API VisEvent *visual_event_new_keyboard (VisKey keysym, VisKeyMod keymod, VisKeyState state);

/**
 * Creates a new mouse movement event.
 *
 * @param dx X displacement
 * @param dy Y displacement
 *
 * @return New event object
 */
LV_NODISCARD LV_API VisEvent *visual_event_new_mousemotion (int dx, int dy);

/**
 * Creates a new mouse button event
 *
 * @param button Mouse button that was used
 * @param state  State of mouse button i.e. pressed or released
 * @param x      X-coordinate of the mouse pointer
 * @param y      Y-coordinate of the mouse pointer
 *
 * @return New event object
 */
LV_NODISCARD LV_API VisEvent *visual_event_new_mousebutton (int button, VisMouseState state, int x, int y);

/**
 * Creates a resize event.
 *
 * @param width New width
 * @param height New height
 *
 * @return New event object
 */
LV_NODISCARD LV_API VisEvent *visual_event_new_resize (int width, int height);

/**
 * Creates a new song change event.
 *
 * @param songinfo Song information
 *
 * @return New event object
 */
LV_NODISCARD LV_API VisEvent *visual_event_new_newsong (VisSongInfo *songinfo);

/**
 * Creates a new parameter change event.
 *
 * @param param Param that has changed
 *
 * @return New event object
 */
LV_NODISCARD LV_API VisEvent *visual_event_new_param (void *param);

/**
 * Creates a quit event
 *
 * @return New event object
 */
LV_NODISCARD LV_API VisEvent *visual_event_new_quit (void);

/**
 * Creates a new visibility event.
 *
 * @param is_visible TRUE when visible, FALSE when not visible.
 *
 * @return New event object
 */
LV_NODISCARD LV_API VisEvent *visual_event_new_visibility (int is_visible);

/**
 * Copies a VisEvent.
 *
 * @param dest Event to copy to
 * @param src  Event to copy
 */
LV_API void visual_event_copy (VisEvent *dest, VisEvent *src);

/**
 * Frees a VisEvent.
 *
 * @param event Event to free
 */
LV_API void visual_event_free (VisEvent* event);

/**
 * Creates a new custom event.
 *
 * @param eid       Unique ID for type identification
 * @param param_int Optional integer value
 * @param param_ptr Optional data pointer
 *
 * @return New event object
 */
LV_NODISCARD LV_API VisEvent *visual_event_new_custom (int eid, int param_int, void *param_ptr);

LV_NODISCARD LV_API VisEventQueue *visual_event_queue_new  (void);

LV_API void visual_event_queue_free (VisEventQueue *eventqueue);

LV_API void visual_event_queue_add  (VisEventQueue *eventqueue, VisEvent *event);
LV_API int  visual_event_queue_poll (VisEventQueue *eventqueue, VisEvent *event);


LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_EVENT_H */
