#ifndef _LV_EVENT_H
#define _LV_EVENT_H

#include <libvisual/lv_songinfo.h>
#include <libvisual/lv_video.h>
#include <libvisual/lv_list.h>
#include <libvisual/lv_keysym.h>

/**
 * @defgroup VisEvent VisEvent
 * @{
 */

VISUAL_BEGIN_DECLS

#define VISUAL_EVENT_KEYBOARD(obj)			(VISUAL_CHECK_CAST ((obj), VisEventKeyboard))
#define VISUAL_EVENT_MOUSEMOTION(obj)			(VISUAL_CHECK_CAST ((obj), VisEventMouseMotion))
#define VISUAL_EVENT_MOUSEBUTTON(obj)			(VISUAL_CHECK_CAST ((obj), VisEventMouseButton))
#define VISUAL_EVENT_RESIZE(obj)			(VISUAL_CHECK_CAST ((obj), VisEventResize))
#define VISUAL_EVENT_NEWSONG(obj)			(VISUAL_CHECK_CAST ((obj), VisEventNewSong))
#define VISUAL_EVENT_QUIT(obj)				(VISUAL_CHECK_CAST ((obj), VisEventQuit))
#define VISUAL_EVENT_VISIBILITY(obj)			(VISUAL_CHECK_CAST ((obj), VisEventVisibility))
#define VISUAL_EVENT_GENERIC(obj)			(VISUAL_CHECK_CAST ((obj), VisEventGeneric))
#define VISUAL_EVENT_PARAM(obj)				(VISUAL_CHECK_CAST ((obj), VisEventParam))
#define VISUAL_EVENT(obj)				(VISUAL_CHECK_CAST ((obj), VisEvent))
#define VISUAL_EVENTQUEUE(obj)				(VISUAL_CHECK_CAST ((obj), VisEventQueue))

/**
 * Number of events allowed in the queue
 */
#define VISUAL_EVENT_MAXEVENTS	256

/**
 * Used to define what kind of event is emitted by the VisEvent system.
 */
typedef enum {
	VISUAL_EVENT_NOEVENT,		/**< No event. */
	VISUAL_EVENT_KEYDOWN,		/**< Keyboard key pressed event. */
	VISUAL_EVENT_KEYUP,		/**< Keyboard key released event. */
	VISUAL_EVENT_MOUSEMOTION,	/**< Mouse movement event. */
	VISUAL_EVENT_MOUSEBUTTONDOWN,	/**< Mouse button pressed event. */
	VISUAL_EVENT_MOUSEBUTTONUP,	/**< Mouse button released event. */
	VISUAL_EVENT_NEWSONG,		/**< Song change event. */
	VISUAL_EVENT_RESIZE,		/**< Window dimension change event. */
	VISUAL_EVENT_PARAM,		/**< Param set event, gets emitted when a parameter has been changed. */
	VISUAL_EVENT_QUIT,		/**< Quit event, Should get emitted when a program is about to quit. */
	VISUAL_EVENT_GENERIC,		/**< A Generic event. Libvisual has nothing to do with it, this can be used for custom events. */
	VISUAL_EVENT_VISIBILITY,	/**< A visibility event. Will be emited by lvdisplay (?) or app itself when window becomes (in)visible */

	VISUAL_EVENT_LAST = 0xffffff	/**< last event number. Libvisual & friends will never use ids greater than this */
} VisEventType;

/**
 * Used to define key states within the VisEvent system.
 */
typedef enum {
	VISUAL_KEY_DOWN,		/**< Key is pressed. */
	VISUAL_KEY_UP			/**< Key is released. */
} VisKeyState;

/**
 * Used to define mouse button states within the VisEvent system.
 */
typedef enum {
	VISUAL_MOUSE_DOWN,		/**< Mouse button is pressed. */
	VISUAL_MOUSE_UP			/**< Mouse button is released. */
} VisMouseState;


typedef struct _VisEventKeyboard VisEventKeyboard;
typedef struct _VisEventMouseMotion VisEventMouseMotion;
typedef struct _VisEventMouseButton VisEventMouseButton;
typedef struct _VisEventResize VisEventResize;
typedef struct _VisEventNewSong VisEventNewSong;
typedef struct _VisEventQuit VisEventQuit;
typedef struct _VisEventVisibility VisEventVisibility;
typedef struct _VisEventGeneric VisEventGeneric;
typedef struct _VisEventParam VisEventParam;
typedef struct _VisEvent VisEvent;
typedef struct _VisEventQueue VisEventQueue;

/**
 * Keyboard event data structure.
 *
 * Contains information about keyboard events.
 *
 * @see visual_event_queue_add_keyboard
 */
struct _VisEventKeyboard {
	VisKeySym	keysym;		/**< A keysym entry that contains which key had an
					  * event and information about modifier keys. */
};

/**
 * Mouse movement event data structure.
 *
 * Contains information about mouse movement events.
 *
 * @see visual_event_queue_add_mousemotion
 */
struct _VisEventMouseMotion {
	VisMouseState	state;		/**< Mouse button state. */
	int		x;		/**< The absolute mouse X value, where 0 is left of screen. */
	int		y;		/**< The absolute mouse Y value. where 0 is top of screen. */
	int		xrel;		/**< Relative X movement to the previous location. */
	int		yrel;		/**< Relative Y movement to the previous location. */
};

/**
 * Mouse button event data structure.
 *
 * Contains information about mouse button events.
 *
 * @see visual_event_queue_add_mousebutton
 */
struct _VisEventMouseButton {
	VisMouseState	state;		/**< Mouse button state. */
	int		button;		/**< The button the state relates to. */
	int		x;		/**< The absolute mouse X value. */
	int		y;		/**< The absolute mouse Y value. */
};

/**
 * Dimension change event data structure.
 *
 * Contains information about screen dimension and depth changes.
 *
 * @see visual_event_queue_add_resize
 */
struct _VisEventResize {
	VisVideo	*video;		/**< Pointer to the VisVideo structure containing all the screen information. */
	int		 width;		/**< Width of the surface. */
	int		 height;	/**< Height of the surface. */
};

/**
 * Song change event data structure.
 *
 * Contains information about song changes.
 *
 * @see visual_event_queue_add_newsong
 */
struct _VisEventNewSong {
	VisSongInfo	*songinfo;	/**< Pointer to the VisSongInfo structure containing all the information about
					 * the new song. */
};

/**
 * Quit event data structure.
 *
 * Contains quit request.
 *
 * @see visual_event_queue_add_quit
 */
struct _VisEventQuit {
	int		 dummy;		/**< some day may contain a request urgency: quit NOW or schedule quit. */
};

/**
 * Visibility event data structure.
 *
 * Contains window's new visibility value.
 *
 * @see visual_event_queue_add_visibiity
 */
struct _VisEventVisibility {
	int		 dummy;		/**< Some day will contain window ID. very OS-specific. maybe use indices ? */
	int		 is_visible;	/**< Set TRUE or FALSE to indicate visibility. */
};

/**
 * Generic event data structure.
 *
 * Contains pointer to some struct.
 *
 * @see visual_event_queue_add_generic
 */
struct _VisEventGeneric {
	int		 event_id;	/**< some event id. */
	int		 data_int;	/**< Data in the form of an integer. */
	void		*data_ptr;	/**< More advanced generic events can use a pointer. */
};

/**
 * Param change event data structure.
 *
 * Contains information about parameter changes.
 *
 * @see visual_event_queue_add_param
 */
struct _VisEventParam {
	/* FiXME: Having VisEventParam here creates a circulair depency in lv_event.h and lv_param.h */
	void		*param;		/**< The parameter entry which has been changed. */
};

/**
 * The main event data structure.
 *
 * All events are encapsulated using the VisEvent structure.
 *
 * @see visual_event_new
 */
struct _VisEvent {
	VisObject	object;		/**< The VisObject data. */
	VisEventType	type;

	union {
		VisEventKeyboard	keyboard;	/**< Keyboard event. */
		VisEventMouseMotion	mousemotion;	/**< Mouse movement event. */
		VisEventMouseButton	mousebutton;	/**< Mouse button event. */
		VisEventResize		resize;		/**< Dimension change event. */
		VisEventNewSong		newsong;	/**< Song change event. */
		VisEventQuit		quit;		/**< Quit event. */
		VisEventVisibility	visibility;	/**< Plugin visible event. */
		VisEventGeneric		generic;	/**< Generic event. */
		VisEventParam		param;		/**< Param change event. */
	} event;
};

/**
 * The event queue data structure.
 *
 * Used to manage events queues and also provides quick access to
 * high piority data from events.
 *
 * @see visual_event_queue_new
 */
struct _VisEventQueue {
	VisObject	 object;	/**< The VisObject data. */
	VisList		 events;	/**< List of VisEvents in the queue. */
	VisEvent	 lastresize;	/**< Last resize event to provide quick access
					  * to this high piority event. */
	int		 resizenew;	/**< Flag that is set when there is a new resize event. */
	int		 eventcount;	/**< Contains the number of events in queue. */

	int		 mousex;	/**< Current absolute mouse X value. */
	int		 mousey;	/**< Current absolute mouse Y value. */
	VisMouseState	 mousestate;	/**< Current mouse button state. */
};


/**
 * Creates a new VisEvent structure.
 *
 * @see visual_actor_get_eventqueue
 *
 * @return A newly allocated VisEvent
 */
VisEvent *visual_event_new (void);

int visual_event_init (VisEvent *event);
int visual_event_copy (VisEvent *dest, VisEvent *src);

/**
 * Creates a new VisEventQueue data structure.
 *
 * @return Newly allocated VisEventQueue.
 */
VisEventQueue *visual_event_queue_new (void);

int visual_event_queue_init (VisEventQueue *eventqueue);

/**
 * Polls for new events. Looks at the event queue for new events and deletes
 * them from the queue while loading them into the event argument.
 *
 * @param eventqueue Pointer to a VisEventQueue from which new events should be taken.
 * @param event Pointer to a VisEvent in which the new events should be loaded.
 *
 * @return TRUE when events are handled and FALSE when the queue is out of events.
 */
int visual_event_queue_poll (VisEventQueue *eventqueue, VisEvent *event);

int visual_event_queue_poll_by_reference (VisEventQueue *eventqueue, VisEvent **event);

/**
 * Adds an event to the event queue. Add new VisEvents into the VisEventQueue.
 *
 * @param eventqueue Pointer to the VisEventQueue to which new events are added.
 * @param event Pointer to a VisEvent that needs to be added to the queue.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_EVENT_QUEUE_NULL or -VISUAL_ERROR_EVENT_NULL
 *	on failure.
 */
int visual_event_queue_add (VisEventQueue *eventqueue, VisEvent *event);

/**
 * Adds a new keyboard event to the event queue. By giving keyboard state information
 * a new VisEvent will be created and added to the event queue.
 *
 * @param eventqueue Pointer to the VisEventQueue to which new events are added.
 * @param keysym A keysym from the VisKey enumerate to set the key to which the event relates.
 * @param keymod Key modifier information from the VisKeyMod enumerate.
 * @param state Contains information about whatever the key is down or up.
 *
 * return VISUAL_OK on succes -VISUAL_ERROR_EVENT_QUEUE_NULL, -VISUAL_ERROR_EVENT_NULL or error values
 *	returned by visual_event_queue_add on failure.
 */
int visual_event_queue_add_keyboard (VisEventQueue *eventqueue, VisKey keysym, int keymod, VisKeyState state);

/**
 * Adds a new mouse movement event to the event queue. By giving absolute X and Y coordinates
 * for the mouse a new VisEvent will be created and added to the event queue.
 *
 * @param eventqueue Pointer to the VisEventQueue to which new events are added.
 * @param x Absolute X value for the mouse location.
 * @param y Absolute Y value for the mouse location.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_EVENT_QUEUE_NULL or error values returned by
 *	visual_event_queue_add () on failure.
 */
int visual_event_queue_add_mousemotion (VisEventQueue *eventqueue, int x, int y);

/**
 * Adds a new mouse button event to the event queue. By giving a mouse button index number and
 * a mouse button key state a new VisEvent will be created and added to the event queue.
 *
 * @param eventqueue Pointer to the VisEventQueue to which new events are added.
 * @param button Index that indicates to which mouse button the state relates.
 * @param state Contains information about whatever the button is down or up
 * @param x Absolute X value for the mouse location.
 * @param y Absolute Y value for the mouse location.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_EVENT_QUEUE_NULL or error values returned by
 *	visual_event_queue_add () on failure.
 */
int visual_event_queue_add_mousebutton (VisEventQueue *eventqueue, int button, VisMouseState state, int x, int y);

/**
 * Adds a new dimension change event to the event queue. By giving a pointer to
 * the VisVideo containing all the surface information and new width and height
 * a new VisEvent will be created and added to the event queue.
 *
 * @param eventqueue Pointer to the VisEventQueue to which new events are added.
 * @param video Pointer to the VisVideo containing all the display information,
 * 	also used for negotiation so values can change within the VisVideo
 * 	structure.
 * @param width The width for the new surface.
 * @param height The height for the new surface.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_EVENT_QUEUE_NULL on failure.
 */
int visual_event_queue_add_resize (VisEventQueue *eventqueue, VisVideo *video, int width, int height);

/**
 * Adds a new song change event to the event queue. By giving a pointer to the
 * new VisSongInfo structure a new VisEvent will be created and added to the event queue.
 *
 * @param eventqueue Pointer to the VisEventQueue to which new events are added.
 * @param songinfo Pointer to the VisSongInfo containing all the new song information.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_EVENT_QUEUE_NULL, -VISUAL_ERROR_SONGINFO_NULL
 * 	or error values returned by visual_event_queue_add () on failure.
 */
int visual_event_queue_add_newsong (VisEventQueue *eventqueue, VisSongInfo *songinfo);

/**
 * Adds a new parameter change event to the event queue. By giving the pointer to the
 * VisParamEntry structure a new VisEvent will be created and added to the event queue.
 *
 * @param eventqueue Pointer to the VisEventQueue to which new events are added.
 * @param param Pointer to the VisParamEntry containing the parameter that has been changed.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_EVENT_QUEUE_NULL, -VISUAL_ERROR_PARAM_NULL
 * 	or error values returned by visual_event_queue_add () on failure.
 */
int visual_event_queue_add_param (VisEventQueue *eventqueue, void *param);

/**
 * Adds a new quit event to the event queue.
 *
 * @param eventqueue Pointer to the VisEventQueue to which new events are added.
 * @param pass_zero_please Might be used in the future, but for now just pass.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_EVENT_QUEUE_NULL
 * 	or error values returned by visual_event_queue_add () on failure.
 */
int visual_event_queue_add_quit (VisEventQueue *eventqueue, int pass_zero_please);

/**
 * Adds a new visibility event to the event queue.
 *
 * @param eventqueue Pointer to the VisEventQueue to which new events are added.
 * @param is_visible TRUE when visible, FALSE when not visible.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_EVENT_QUEUE_NULL
 * 	or error values returned by visual_event_queue_add () on failure.
 */
int visual_event_queue_add_visibility (VisEventQueue *eventqueue, int is_visible);

/**
 * Adds a new generic event to the event queue.
 *
 * @param eventqueue Pointer to the VisEventQueue to which new events are added.
 * @param eid ID of the custom event..
 * @param param_int Integer value for the custom event.
 * @param param_ptr Pointer to data for the custom event..
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_EVENT_QUEUE_NULL
 * 	or error values returned by visual_event_queue_add () on failure.
 */
int visual_event_queue_add_generic (VisEventQueue *eventqueue, int eid, int param_int, void *param_ptr);

VISUAL_END_DECLS

/**
 * @}
 */

#endif /* _LV_EVENT_H */
