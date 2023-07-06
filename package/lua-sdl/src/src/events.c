/*
 * events.c -- general event management and enumerations
 *
 * Copyright (c) 2013, 2014 David Demelier <markand@malikania.fr>
 * Copyright (c) 2016-2017 Webster Sheets <webster@web-eworks.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <config.h>

#include <common/table.h>

#include "keyboard.h"
#include "events.h"
#include "mouse.h"

/**
 * @struct filter
 * @brief A structure for event filter
 *
 * SDL_AddEventWatch() and SDL_SetEventFilter() will use this object to
 * know what to destroy.
 *
 * SDL_DelEventWatch() checks for the function / userdata to remove the
 * correct one, so we need to create a new filter for each new filter
 * function since the function is the same (eventFilter).
 */
typedef struct
{
  lua_State* L; /*! the Lua state */
  int ref;      /*! the function to call */
  /*
          enum {
                  EventTypeFilter,
                  EventTypeWatcher,
          }		type;		! type of filter */
} Filter;

Filter filter_data; // TODO

/*
 * Filter function for SDL_FilterEvents and SDL_SetEventFilter.
 */
static int
eventFilter(const SDL_Event* ev)
{
  lua_rawgeti(filter_data.L, LUA_REGISTRYINDEX, filter_data.ref);
  eventPush(filter_data.L, ev);
  lua_call(filter_data.L, 1, 1);

  /* Return value is needed for EventFilter */
  return lua_toboolean(filter_data.L, -1);
}

static int
eventIterator(lua_State* L)
{
  SDL_Event ev;

  if (SDL_PollEvent(&ev) == 0)
    return 0;

  eventPush(L, &ev);

  return 1;
}

static int
l_event_setEventFilter(lua_State* L)
{
  luaL_checktype(L, 1, LUA_TFUNCTION);

  filter_data.L = L;

  /* Push the function and ref it */
  lua_pushvalue(L, 1);
  filter_data.ref = luaL_ref(L, LUA_REGISTRYINDEX);

  SDL_SetEventFilter((SDL_EventFilter)eventFilter);

  /* Push the object to the user */
  commonPushUserdata(L, EventFilterName, &filter_data);

  return 1;
}

/* ---------------------------------------------------------
 * Lua functions
 * --------------------------------------------------------- */

/*
 * SDL.addEventWatch(f)
 *
 * The function must have the following signature:
 *	function f(event)
 *
 * Arguments:
 *	f the function
 */
/*static int
l_event_addEventWatch(lua_State *L)
{
        return eventAddFilter(L, EventTypeWatcher);
}*/

/*
 * SDL.eventState(type, state)
 *
 * Arguments:
 *	type the type of event
 *	state the state -1, 0 or 1
 *
 * Returns:
 *	The current state or the new state
 */
static int
l_event_eventState(lua_State* L)
{
  int type = luaL_checkinteger(L, 1);
  int state = luaL_checkinteger(L, 2);

  return commonPush(L, "i", SDL_EventState(type, state));
}

/*
 * SDL.filterEvents(func)
 *
 * The func must have the following signature:
 * 	function filter(event) -> return false to disable
 *
 * Arguments:
 *	func the function
 */
static int
l_event_filterEvents(lua_State* L)
{
  luaL_checktype(L, 1, LUA_TFUNCTION);

  filter_data.L = L;
  /* Push the temporarly function */
  lua_pushvalue(L, 1);
  filter_data.ref = luaL_ref(L, LUA_REGISTRYINDEX);

  SDL_SetEventFilter(eventFilter);
  luaL_unref(L, LUA_REGISTRYINDEX, filter_data.ref);

  return 0;
}

/*
 * SDL.flushEvent(type)
 *
 * Arguments:
 *	type the event type
 */
/*static int
l_event_flushEvent(lua_State* L)
{
  int type = luaL_checkinteger(L, 1);

  SDL_FlushEvent(type);

  return 0;
}*/

/*
 * SDL.flushEvents(min, max)
 *
 * Arguments:
 *	min the minimum type event
 *	max the maximum type event
 */
/*static int
l_event_flushEvents(lua_State* L)
{
  int min = luaL_checkinteger(L, 1);
  int max = luaL_checkinteger(L, 2);

  SDL_FlushEvents(min, max);

  return 0;
}*/

/*
 * SDL.hasEvent(type)
 *
 * Arguments:
 *	type the event type
 *
 * Returns:
 *	True if has event
 */
/*static int
l_event_hasEvent(lua_State* L)
{
  int type = luaL_checkinteger(L, 1);

  return commonPush(L, "b", SDL_HasEvent(type));
}*/

/*
 * SDL.hasEvents(min, max)
 *
 * Arguments:
 *	min the minimum event type
 *	max the maximum event type
 *
 * Returns:
 *	True if has events
 */
/*static int
l_event_hasEvents(lua_State* L)
{
  int min = luaL_checkinteger(L, 1);
  int max = luaL_checkinteger(L, 2);

  return commonPush(L, "b", SDL_HasEvents(min, max));
}*/

/*
 * SDL.peepEvents(count, action, first, last)
 *
 * Arguments:
 *	count number of events to process
 *	action the action to execute
 *	min the minimal event
 *	max the maximal event
 *
 * Returns:
 *	A sequence table of events or nil on failure
 *	The error message
 */
static int
l_event_peepEvents(lua_State* L)
{
  int count = luaL_checkinteger(L, 1);
  int action = luaL_checkinteger(L, 2);
  unsigned mask = 0;
  int ret, toreturn;
  SDL_Event* events;

  /*
   * First and last are defaulted to SDL_FIRSTEVENT and SDL_LASTEVENT.
   */
  if (lua_gettop(L) >= 3)
    mask = luaL_checkinteger(L, 3);

  if ((events = calloc(sizeof(SDL_Event), count)) == NULL)
    return commonPushErrno(L, 1);

  ret = SDL_PeepEvents(events, count, action, mask);

  if (ret >= 0) {
    int i;

    lua_createtable(L, ret, ret);
    for (i = 0; i < ret; ++i) {
      eventPush(L, &events[i]);
      lua_rawseti(L, -2, i + 1);
    }

    toreturn = 1;
  } else
    toreturn = commonPushSDLError(L, 1);

  free(events);

  return toreturn;
}

/*
 * SDL.pollEvent()
 *
 * Returns:
 *	An iterator function which returns an event each time it is called
 */
static int
l_event_pollEvent(lua_State* L)
{
  lua_pushcclosure(L, eventIterator, 0);

  return 1;
}

/*
 * SDL.pumpEvents()
 */
static int
l_event_pumpEvents(lua_State* L)
{
  SDL_PumpEvents();

  (void)L;

  return 0;
}

/*
 * SDL.quitRequested()
 *
 * Returns:
 *	True if quit was requested
 */
static int
l_event_quitRequested(lua_State* L)
{
  return commonPush(L, "b", SDL_QuitRequested());
}

/*
 * SDL.registerEvents(num)
 *
 * Arguments:
 *	num the number of events to register
 *
 * Returns:
 *	The beginning number
 */

/*static int
l_event_registerEvents(lua_State* L)
{
  int num = luaL_checkinteger(L, 1);
  Uint32 ret;

  if ((ret = SDL_RegisterEvents(num)) == (Uint32)-1)
    return commonPushSDLError(L, 1);

  return commonPush(L, "i", ret);
}*/

/*
 * SDL.setEventFilter(func)
 *
 * Func must have the following signature:
 *	function filter(event) -> return true if the event should be added
 *
 * Arguments:
 *	func the filter function
 *
 * Returns:
 *	True on success or false on failure
 *	The error message
 */
/*static int
l_event_setEventFilter(lua_State *L)
{
        return eventAddFilter(L);
}*/

/*
 * SDL.waitEvent(timeout)
 *
 * Arguments:
 *	timeout (optional) the timeout
 *
 * Returns:
 *	The event or nil on failure
 *	The error message
 */
static int
l_event_waitEvent(lua_State* L)
{
  SDL_Event ev;
  int ret;

  ret = SDL_WaitEvent(&ev);

  if (!ret)
    return commonPushSDLError(L, 1);

  eventPush(L, &ev);

  return 1;
}

const luaL_Reg EventFunctions[] = {
  /*{ "addEventWatch",		l_event_addEventWatch		},*/
  { "eventState", l_event_eventState },
  { "filterEvents", l_event_filterEvents },
  /*{ "flushEvent", l_event_flushEvent },
  { "flushEvents", l_event_flushEvents },
  { "hasEvent", l_event_hasEvent },
  { "hasEvents", l_event_hasEvents },*/
  { "peepEvents", l_event_peepEvents },
  { "pollEvent", l_event_pollEvent },
  { "pumpEvents", l_event_pumpEvents },
#if 0
	{ "pushEvent",			l_event_pushEvent		},
#endif
  { "quitRequested", l_event_quitRequested },
  /*{ "registerEvents", l_event_registerEvents },*/
  { "setEventFilter", l_event_setEventFilter },
  { "waitEvent", l_event_waitEvent },
  { NULL, NULL }
};

/*
 * SDL.eventAction
 */
const CommonEnum EventAction[] = { { "Add", SDL_ADDEVENT },
                                   { "Peek", SDL_PEEKEVENT },
                                   { "Get", SDL_GETEVENT },
                                   { NULL, -1 } };

/*
 * SDL.event
 */
const CommonEnum EventType[] = {
  /*{ "First", SDL_FIRSTEVENT },*/
  { "Quit", SDL_QUIT },
  /*{ "AppTerminating", SDL_APP_TERMINATING },
  { "AppLowMemory", SDL_APP_LOWMEMORY },
  { "AppWillEnterBackground", SDL_APP_WILLENTERBACKGROUND },
  { "AppDidEnterBackground", SDL_APP_DIDENTERBACKGROUND },
  { "AppWillEnterForeground", SDL_APP_WILLENTERFOREGROUND },
  { "AppDidEnterForeground", SDL_APP_DIDENTERFOREGROUND },
  { "WindowEvent", SDL_WINDOWEVENT },*/
  { "KeyDown", SDL_KEYDOWN },
  { "KeyUp", SDL_KEYUP },
  /*{ "TextEditing", SDL_TEXTEDITING },
  { "TextInput", SDL_TEXTINPUT },*/
  { "MouseMotion", SDL_MOUSEMOTION },
  { "MouseButtonDown", SDL_MOUSEBUTTONDOWN },
  { "MouseButtonUp", SDL_MOUSEBUTTONUP },
  /*{ "MouseWheel", SDL_MOUSEWHEEL },*/
  { "JoyAxisMotion", SDL_JOYAXISMOTION },
  { "JoyBallMotion", SDL_JOYBALLMOTION },
  { "JoyHatMotion", SDL_JOYHATMOTION },
  { "JoyButtonDown", SDL_JOYBUTTONDOWN },
  { "JoyButtonUp", SDL_JOYBUTTONUP },
/*{ "JoyDeviceAdded", SDL_JOYDEVICEADDED },
{ "JoyDeviceRemoved", SDL_JOYDEVICEREMOVED },
{ "ControllerAxisMotion", SDL_CONTROLLERAXISMOTION },
{ "ControllerButtonDown", SDL_CONTROLLERBUTTONDOWN },
{ "ControllerButtonUp", SDL_CONTROLLERBUTTONUP },
{ "ControllerDeviceAdded", SDL_CONTROLLERDEVICEADDED },
{ "ControllerDeviceRemoved", SDL_CONTROLLERDEVICEREMOVED },
{ "ControllerDeviceRemapped", SDL_CONTROLLERDEVICEREMAPPED },
{ "FingerDown", SDL_FINGERDOWN },
{ "FingerUp", SDL_FINGERUP },
{ "FingerMotion", SDL_FINGERMOTION },
{ "DollarGesture", SDL_DOLLARGESTURE },
{ "DollarRecord", SDL_DOLLARRECORD },
{ "MultiGesture", SDL_MULTIGESTURE },
{ "ClipboardUpdate", SDL_CLIPBOARDUPDATE },
{ "DropFile", SDL_DROPFILE },*/
#if SDL_VERSION_ATLEAST(2, 0, 2)
  { "RenderTargetsReset", SDL_RENDER_TARGETS_RESET },
#endif
#if SDL_VERSION_ATLEAST(2, 0, 4)
  { "RenderDeviceReset", SDL_RENDER_DEVICE_RESET },
  { "AudioDeviceAdded", SDL_AUDIODEVICEADDED },
  { "AudioDeviceRemoved", SDL_AUDIODEVICEREMOVED },
#endif
#if SDL_VERSION_ATLEAST(2, 0, 5)
  { "DropText", SDL_DROPTEXT },
  { "DropBegin", SDL_DROPBEGIN },
  { "DropComplete", SDL_DROPCOMPLETE },
#endif
  { "UserEvent", SDL_USEREVENT },
  /*{ "Last", SDL_LASTEVENT },*/
  { NULL, -1 }
};

/*
 * SDL.eventWindow
 */
const CommonEnum EventWindow[] = {
/*{ "Shown", SDL_WINDOWEVENT_SHOWN },
  { "Hidden", SDL_WINDOWEVENT_HIDDEN },
  { "Exposed", SDL_WINDOWEVENT_EXPOSED },
  { "Moved", SDL_WINDOWEVENT_MOVED },
  { "Resized", SDL_WINDOWEVENT_RESIZED },
  { "SizeChanged", SDL_WINDOWEVENT_SIZE_CHANGED },
  { "Minimized", SDL_WINDOWEVENT_MINIMIZED },
  { "Maximized", SDL_WINDOWEVENT_MAXIMIZED },
  { "Restored", SDL_WINDOWEVENT_RESTORED },
  { "Enter", SDL_WINDOWEVENT_ENTER },
  { "Leave", SDL_WINDOWEVENT_LEAVE },
  { "FocusGained", SDL_WINDOWEVENT_FOCUS_GAINED },
  { "FocusLost", SDL_WINDOWEVENT_FOCUS_LOST },
  { "Close", SDL_WINDOWEVENT_CLOSE },*/
#if SDL_VERSION_ATLEAST(2, 0, 5)
  { "TakeFocus", SDL_WINDOWEVENT_TAKE_FOCUS },
  { "HitTest", SDL_WINDOWEVENT_HIT_TEST },
#endif
  { NULL, -1 }
};

/* --------------------------------------------------------
 * Filter object metamethods
 * -------------------------------------------------------- */

static int
l_filter_eq(lua_State* L)
{
  Filter* f1 = commonGetAs(L, 1, EventFilterName, Filter*);
  Filter* f2 = commonGetAs(L, 1, EventFilterName, Filter*);

  return commonPush(L, "b", f1 == f2);
}

static int
l_filter_gc(lua_State* L)
{
  SDL_SetEventFilter(NULL);
  /* Unref the function */
  luaL_unref(L, LUA_REGISTRYINDEX, filter_data.ref);
  return 0;
}

static int
l_filter_tostring(lua_State* L)
{
  Filter* f = commonGetAs(L, 1, EventFilterName, Filter*);

  lua_pushfstring(L, "filter %p", f);

  return 1;
}

/* --------------------------------------------------------
 * Event object definition
 * -------------------------------------------------------- */

const luaL_Reg filterMethods[] = { { "remove", l_filter_gc }, { NULL, NULL } };

const luaL_Reg filterMetamethods[] = { { "__eq", l_filter_eq },
                                       { "__gc", l_filter_gc },
                                       { "__tostring", l_filter_tostring },
                                       { NULL, NULL } };

const CommonObject EventFilter = { "Event", filterMethods, filterMetamethods };

/* --------------------------------------------------------
 * Shared functions
 * -------------------------------------------------------- */

typedef void (*PushFunc)(lua_State* L, const SDL_Event*);

static void
pushKey(lua_State* L, const SDL_Event* ev)
{
  tableSetInt(L, -1, "state", ev->key.state);
  tableSetInt(L, -1, "type", ev->key.type);

  /* Table keysym for the Key information */
  lua_createtable(L, 3, 3);
  tableSetInt(L, -1, "scancode", ev->key.keysym.scancode);
  tableSetInt(L, -1, "sym", ev->key.keysym.sym);
  // tableSetEnum(L, -1, ev->key.keysym.mod, KeyboardModifiers, "mod");
  tableSetInt(L, -1, "mod", ev->key.keysym.mod);
  lua_setfield(L, -2, "keysym");
}

static void
pushMouseMotion(lua_State* L, const SDL_Event* ev)
{
  tableSetInt(L, -1, "x", ev->motion.x);
  tableSetInt(L, -1, "y", ev->motion.y);
  tableSetInt(L, -1, "xrel", ev->motion.xrel);
  tableSetInt(L, -1, "yrel", ev->motion.yrel);
  tableSetInt(L, -1, "which", ev->motion.which);
  tableSetEnum(L, -1, ev->motion.state, MouseMask, "state");

  //  if (ev->motion.which == SDL_TOUCH_MOUSEID)
  //   tableSetBool(L, -1, "touch", 1);
}

static void
pushMouseButton(lua_State* L, const SDL_Event* ev)
{
  tableSetInt(L, -1, "button", ev->button.button);
  tableSetInt(L, -1, "x", ev->button.x);
  tableSetInt(L, -1, "y", ev->button.y);
  tableSetInt(L, -1, "which", ev->button.which);
  tableSetBool(L, -1, "state", ev->button.state);

  //  if (ev->motion.which == SDL_TOUCH_MOUSEID)
  //    tableSetBool(L, -1, "touch", 1);
}

static void
pushJoyAxisMotion(lua_State* L, const SDL_Event* ev)
{
  tableSetInt(L, -1, "which", ev->jaxis.which);
  tableSetInt(L, -1, "axis", ev->jaxis.axis);
  tableSetInt(L, -1, "value", ev->jaxis.value);
}

static void
pushJoyBallMotion(lua_State* L, const SDL_Event* ev)
{
  tableSetInt(L, -1, "which", ev->jball.which);
  tableSetInt(L, -1, "ball", ev->jball.ball);
  tableSetInt(L, -1, "xrel", ev->jball.xrel);
  tableSetInt(L, -1, "yrel", ev->jball.yrel);
}

static void
pushJoyHatMotion(lua_State* L, const SDL_Event* ev)
{
  tableSetInt(L, -1, "which", ev->jhat.which);
  tableSetInt(L, -1, "hat", ev->jhat.hat);
  tableSetInt(L, -1, "value", ev->jhat.value);
}

static void
pushJoyButton(lua_State* L, const SDL_Event* ev)
{
  tableSetInt(L, -1, "which", ev->jbutton.which);
  tableSetInt(L, -1, "button", ev->jbutton.button);
  tableSetBool(L, -1, "state", ev->jbutton.state);
}

void
eventPush(lua_State* L, const SDL_Event* ev)
{
  PushFunc func = NULL;

  /*
   * Creates a table like:
   * {
   *	type = <number>
   *	<data> = <event_data>
   * }
   *
   * data will be defined by the push_* function.
   */
  lua_createtable(L, 1, 1);
  lua_pushinteger(L, ev->type);
  lua_setfield(L, -2, "type");

  /*
   * I prefer using a switch rather than an array of functions pointer
   * because it's less convenient to initialize as C ANSI does not
   * support index initialization.
   */
  switch (ev->type) {
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      func = pushKey;
      break;
    case SDL_MOUSEMOTION:
      func = pushMouseMotion;
      break;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
      func = pushMouseButton;
      break;
    case SDL_JOYAXISMOTION:
      func = pushJoyAxisMotion;
      break;
    case SDL_JOYBALLMOTION:
      func = pushJoyBallMotion;
      break;
    case SDL_JOYHATMOTION:
      func = pushJoyHatMotion;
      break;
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP:
      func = pushJoyButton;
      break;
    case SDL_USEREVENT: /* XXX: TO IMPLEMENT */
    default:
      break;
  }

  if (func != NULL)
    func(L, ev);
}
