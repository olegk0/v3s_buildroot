/* Generated by nsgenbind
 *
 * nsgenbind is published under the MIT Licence.
 * nsgenbind is similar to a compiler is a purely transformative tool which
 * explicitly makes no copyright claim on this generated output
 */

/* DukTape JavaScript bindings for NetSurf browser
 *
 * Copyright 2015 Vincent Sanders <vince@netsurf-browser.org>
 * This file is part of NetSurf, http://www.netsurf-browser.org/
 * Released under the terms of the MIT License,
 *        http://www.opensource.org/licenses/mit-license
 */

#include <dom/dom.h>

#include "utils/log.h"
#include "utils/nsurl.h"

#include "javascript/duktape/duktape.h"

struct browser_window;
struct html_content;
struct dom_node;
struct dom_element;
struct dom_document;
struct dom_html_element;
struct dom_node_character_data;
struct dom_node_text;
struct dom_node_list;
struct dom_node_comment;
struct dom_html_collection;
struct dom_html_br_element;


#include "build/Linux-framebuffer/duktape/binding.h"
#include "build/Linux-framebuffer/duktape/private.h"
#include "build/Linux-framebuffer/duktape/prototype.h"

#include "javascript/duktape/dukky.h"

#include <nsutils/time.h>
#include "netsurf/browser_window.h"

#define CONSOLE_TIMERS MAGIC(ConsoleTimers)

static void
write_log_entry(duk_context *ctx, unsigned int group, browser_window_console_flags flags)
{
	/* objs... */
	dukky_push_generics(ctx, "consoleFormatter");
	duk_insert(ctx, 0);
	if (dukky_pcall(ctx, duk_get_top(ctx) - 1, false)) {
		/* Failed to convert somehow, oh dear, you get to keep
		 * all the pieces.
		 */
		duk_pop(ctx);
		duk_push_string(ctx, "Oh dear, formatter went banananas");
	}
	/* str?objs?... */
	for (int i = 0; i < duk_get_top(ctx); ++i) {
		(void)duk_safe_to_string(ctx, i);
	}
	/* strs... */
	for (unsigned int u = 0; u < group; ++u) {
		duk_push_lstring(ctx, " ", 1);
		duk_insert(ctx, 0);
	}
	/* spcs... strs... */
	duk_concat(ctx, duk_get_top(ctx));
	/* str */

	duk_push_global_object(ctx);
	duk_get_prop_string(ctx, -1, PRIVATE_MAGIC);
	window_private_t *priv_win = duk_get_pointer(ctx, -1);
	duk_pop(ctx);

	duk_size_t msglen;
	const char *msg = duk_safe_to_lstring(ctx, 0, &msglen);

	if (priv_win == NULL || priv_win->win == NULL ||
	    browser_window_console_log(priv_win->win, BW_CS_SCRIPT_CONSOLE,
				       msg, msglen,
				       flags) != NSERROR_OK) {
		NSLOG(netsurf, DEBUG, "Unable to log: %s", duk_safe_to_string(ctx, 0));
	}
}


static void dukky_console___init(duk_context *ctx, console_private_t *priv)
{
#line 66 "Console.bnd"

	priv->group = 0;
	duk_push_object(ctx);
	duk_put_prop_string(ctx, 0, CONSOLE_TIMERS);
#line 99 "console.c"
}

static void dukky_console___fini(duk_context *ctx, console_private_t *priv)
{
}

static duk_ret_t dukky_console___constructor(duk_context *ctx)
{
	/* create private data and attach to instance */
	console_private_t *priv = calloc(1, sizeof(*priv));
	if (priv == NULL) return 0;
	duk_push_pointer(ctx, priv);
	duk_put_prop_string(ctx, 0, dukky_magic_string_private);

	dukky_console___init(ctx, priv);
	duk_set_top(ctx, 1);
	return 1;
}

static duk_ret_t dukky_console___destructor(duk_context *ctx)
{
	console_private_t *priv;
	duk_get_prop_string(ctx, 0, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop(ctx);
	if (priv == NULL) return 0;

	dukky_console___fini(ctx, priv);
	free(priv);
	return 0;
}

static duk_ret_t dukky_console_debug(duk_context *ctx)
{
	/* Get private data for method */
	console_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 98 "Console.bnd"

	write_log_entry(ctx, priv->group, BW_CS_FLAG_LEVEL_DEBUG);
	return 0;
#line 148 "console.c"
}

static duk_ret_t dukky_console_dir(duk_context *ctx)
{
	/* ensure the parameters are present */
	duk_idx_t dukky_argc = duk_get_top(ctx);
	if (dukky_argc < 1) {
		/* not enough arguments */
		return duk_error(ctx, DUK_RET_TYPE_ERROR, dukky_error_fmt_argument, 1, dukky_argc);
	} else if (dukky_argc > 1) {
		/* remove extraneous parameters */
		duk_set_top(ctx, 1);
	}

	/* check types of passed arguments are correct */
	if (dukky_argc > 0) {
		/* unhandled type check */
	}
	/* Get private data for method */
	console_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 122 "Console.bnd"

	write_log_entry(ctx, priv->group, BW_CS_FLAG_LEVEL_INFO);
	return 0;
#line 181 "console.c"
}

static duk_ret_t dukky_console_error(duk_context *ctx)
{
	/* Get private data for method */
	console_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 104 "Console.bnd"

	write_log_entry(ctx, priv->group, BW_CS_FLAG_LEVEL_ERROR);
	return 0;
#line 200 "console.c"
}

static duk_ret_t dukky_console_group(duk_context *ctx)
{
	/* ensure the parameters are present */
	duk_idx_t dukky_argc = duk_get_top(ctx);
	if (dukky_argc > 0) {
		/* remove extraneous parameters */
		duk_set_top(ctx, 0);
	}

	/* check types of passed arguments are correct */
	/* Get private data for method */
	console_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 73 "Console.bnd"

	priv->group ++;
	return 0;
#line 227 "console.c"
}

static duk_ret_t dukky_console_groupCollapsed(duk_context *ctx)
{
	/* ensure the parameters are present */
	duk_idx_t dukky_argc = duk_get_top(ctx);
	if (dukky_argc > 0) {
		/* remove extraneous parameters */
		duk_set_top(ctx, 0);
	}

	/* check types of passed arguments are correct */
	/* Get private data for method */
	console_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 79 "Console.bnd"

	priv->group ++;
	return 0;
#line 254 "console.c"
}

static duk_ret_t dukky_console_groupEnd(duk_context *ctx)
{
	/* ensure the parameters are present */
	duk_idx_t dukky_argc = duk_get_top(ctx);
	if (dukky_argc > 0) {
		/* remove extraneous parameters */
		duk_set_top(ctx, 0);
	}

	/* check types of passed arguments are correct */
	/* Get private data for method */
	console_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 85 "Console.bnd"

	if (priv->group)
		priv->group --;
	return 0;
#line 282 "console.c"
}

static duk_ret_t dukky_console_info(duk_context *ctx)
{
	/* Get private data for method */
	console_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 92 "Console.bnd"

	write_log_entry(ctx, priv->group, BW_CS_FLAG_LEVEL_INFO);
	return 0;
#line 301 "console.c"
}

static duk_ret_t dukky_console_log(duk_context *ctx)
{
	/* Get private data for method */
	console_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 110 "Console.bnd"

	write_log_entry(ctx, priv->group, BW_CS_FLAG_LEVEL_LOG);
	return 0;
#line 320 "console.c"
}

static duk_ret_t dukky_console_time(duk_context *ctx)
{
	/* ensure the parameters are present */
	duk_idx_t dukky_argc = duk_get_top(ctx);
	if (dukky_argc < 1) {
		/* not enough arguments */
		return duk_error(ctx, DUK_RET_TYPE_ERROR, dukky_error_fmt_argument, 1, dukky_argc);
	} else if (dukky_argc > 1) {
		/* remove extraneous parameters */
		duk_set_top(ctx, 1);
	}

	/* check types of passed arguments are correct */
	if (dukky_argc > 0) {
		if (!duk_is_string(ctx, 0)) {
			duk_to_string(ctx, 0);
		}
	}
	/* Get private data for method */
	console_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 128 "Console.bnd"

	uint64_t time_ms = 0;

	if (nsu_getmonotonic_ms(&time_ms) != NSUERROR_OK)
		return 0;

	if (!duk_is_string(ctx, 0)) {
		return duk_error(ctx, DUK_ERR_ERROR, "Console.time() takes a string");
	}

	duk_set_top(ctx, 1);

	duk_push_uint(ctx, (duk_uint_t)time_ms);

	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, CONSOLE_TIMERS);
	duk_insert(ctx, 0);
	duk_pop(ctx);

	duk_put_prop(ctx, 0);

	return 0;
#line 374 "console.c"
}

static duk_ret_t dukky_console_timeEnd(duk_context *ctx)
{
	/* ensure the parameters are present */
	duk_idx_t dukky_argc = duk_get_top(ctx);
	if (dukky_argc < 1) {
		/* not enough arguments */
		return duk_error(ctx, DUK_RET_TYPE_ERROR, dukky_error_fmt_argument, 1, dukky_argc);
	} else if (dukky_argc > 1) {
		/* remove extraneous parameters */
		duk_set_top(ctx, 1);
	}

	/* check types of passed arguments are correct */
	if (dukky_argc > 0) {
		if (!duk_is_string(ctx, 0)) {
			duk_to_string(ctx, 0);
		}
	}
	/* Get private data for method */
	console_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 153 "Console.bnd"

	uint64_t time_ms = 0;
	uint64_t old_time_ms = 0;

	if (nsu_getmonotonic_ms(&time_ms) != NSUERROR_OK)
		return 0;

	if (!duk_is_string(ctx, 0)) {
		return duk_error(ctx, DUK_ERR_ERROR, "Console.time() takes a string");
	}

	duk_set_top(ctx, 1);

	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, CONSOLE_TIMERS);
	duk_insert(ctx, 0);
	duk_pop(ctx);

	duk_dup(ctx, -1);
	duk_get_prop(ctx, 0);
	if (duk_is_undefined(ctx, -1)) {
		duk_pop(ctx);
		duk_push_uint(ctx, (duk_uint_t)time_ms);
	}
	/* timers timername oldval */
	old_time_ms = duk_to_uint32(ctx, -1);
	duk_pop(ctx);
	duk_dup(ctx, -1);
	duk_insert(ctx, 0);
	duk_del_prop(ctx, 0);
	duk_push_string(ctx, "Timer elapsed: ");
	duk_insert(ctx, 0);
	duk_push_sprintf(ctx, "%lu ms", (duk_uint_t)(time_ms - old_time_ms));
	write_log_entry(ctx, priv->group, BW_CS_FLAG_LEVEL_INFO);
	return 0;
#line 441 "console.c"
}

static duk_ret_t dukky_console_trace(duk_context *ctx)
{
	/* ensure the parameters are present */
	duk_idx_t dukky_argc = duk_get_top(ctx);
	if (dukky_argc > 0) {
		/* remove extraneous parameters */
		duk_set_top(ctx, 0);
	}

	/* check types of passed arguments are correct */
	/* Get private data for method */
	console_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 191 "Console.bnd"

	duk_idx_t i = duk_push_error_object(ctx, DUK_ERR_ERROR, "Dummy Error");
	duk_get_prop_string(ctx, i, "stack");
	duk_safe_to_string(ctx, -1);
	duk_insert(ctx, 0);
	duk_set_top(ctx, 1);
	write_log_entry(ctx, priv->group, BW_CS_FLAG_LEVEL_INFO);
	return 0;
#line 473 "console.c"
}

static duk_ret_t dukky_console_warn(duk_context *ctx)
{
	/* Get private data for method */
	console_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 116 "Console.bnd"

	write_log_entry(ctx, priv->group, BW_CS_FLAG_LEVEL_WARN);
	return 0;
#line 492 "console.c"
}

duk_ret_t dukky_console___proto(duk_context *ctx, void *udata)
{
	/* Add a method */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "debug");
	duk_push_c_function(ctx, dukky_console_debug, DUK_VARARGS);
	duk_def_prop(ctx, -3,
		     DUK_DEFPROP_HAVE_VALUE |
		     DUK_DEFPROP_HAVE_WRITABLE |
		     DUK_DEFPROP_HAVE_ENUMERABLE |
		     DUK_DEFPROP_ENUMERABLE |
		     DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add a method */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "dir");
	duk_push_c_function(ctx, dukky_console_dir, DUK_VARARGS);
	duk_def_prop(ctx, -3,
		     DUK_DEFPROP_HAVE_VALUE |
		     DUK_DEFPROP_HAVE_WRITABLE |
		     DUK_DEFPROP_HAVE_ENUMERABLE |
		     DUK_DEFPROP_ENUMERABLE |
		     DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add a method */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "error");
	duk_push_c_function(ctx, dukky_console_error, DUK_VARARGS);
	duk_def_prop(ctx, -3,
		     DUK_DEFPROP_HAVE_VALUE |
		     DUK_DEFPROP_HAVE_WRITABLE |
		     DUK_DEFPROP_HAVE_ENUMERABLE |
		     DUK_DEFPROP_ENUMERABLE |
		     DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add a method */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "group");
	duk_push_c_function(ctx, dukky_console_group, DUK_VARARGS);
	duk_def_prop(ctx, -3,
		     DUK_DEFPROP_HAVE_VALUE |
		     DUK_DEFPROP_HAVE_WRITABLE |
		     DUK_DEFPROP_HAVE_ENUMERABLE |
		     DUK_DEFPROP_ENUMERABLE |
		     DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add a method */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "groupCollapsed");
	duk_push_c_function(ctx, dukky_console_groupCollapsed, DUK_VARARGS);
	duk_def_prop(ctx, -3,
		     DUK_DEFPROP_HAVE_VALUE |
		     DUK_DEFPROP_HAVE_WRITABLE |
		     DUK_DEFPROP_HAVE_ENUMERABLE |
		     DUK_DEFPROP_ENUMERABLE |
		     DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add a method */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "groupEnd");
	duk_push_c_function(ctx, dukky_console_groupEnd, DUK_VARARGS);
	duk_def_prop(ctx, -3,
		     DUK_DEFPROP_HAVE_VALUE |
		     DUK_DEFPROP_HAVE_WRITABLE |
		     DUK_DEFPROP_HAVE_ENUMERABLE |
		     DUK_DEFPROP_ENUMERABLE |
		     DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add a method */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "info");
	duk_push_c_function(ctx, dukky_console_info, DUK_VARARGS);
	duk_def_prop(ctx, -3,
		     DUK_DEFPROP_HAVE_VALUE |
		     DUK_DEFPROP_HAVE_WRITABLE |
		     DUK_DEFPROP_HAVE_ENUMERABLE |
		     DUK_DEFPROP_ENUMERABLE |
		     DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add a method */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "log");
	duk_push_c_function(ctx, dukky_console_log, DUK_VARARGS);
	duk_def_prop(ctx, -3,
		     DUK_DEFPROP_HAVE_VALUE |
		     DUK_DEFPROP_HAVE_WRITABLE |
		     DUK_DEFPROP_HAVE_ENUMERABLE |
		     DUK_DEFPROP_ENUMERABLE |
		     DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add a method */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "time");
	duk_push_c_function(ctx, dukky_console_time, DUK_VARARGS);
	duk_def_prop(ctx, -3,
		     DUK_DEFPROP_HAVE_VALUE |
		     DUK_DEFPROP_HAVE_WRITABLE |
		     DUK_DEFPROP_HAVE_ENUMERABLE |
		     DUK_DEFPROP_ENUMERABLE |
		     DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add a method */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "timeEnd");
	duk_push_c_function(ctx, dukky_console_timeEnd, DUK_VARARGS);
	duk_def_prop(ctx, -3,
		     DUK_DEFPROP_HAVE_VALUE |
		     DUK_DEFPROP_HAVE_WRITABLE |
		     DUK_DEFPROP_HAVE_ENUMERABLE |
		     DUK_DEFPROP_ENUMERABLE |
		     DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add a method */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "trace");
	duk_push_c_function(ctx, dukky_console_trace, DUK_VARARGS);
	duk_def_prop(ctx, -3,
		     DUK_DEFPROP_HAVE_VALUE |
		     DUK_DEFPROP_HAVE_WRITABLE |
		     DUK_DEFPROP_HAVE_ENUMERABLE |
		     DUK_DEFPROP_ENUMERABLE |
		     DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add a method */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "warn");
	duk_push_c_function(ctx, dukky_console_warn, DUK_VARARGS);
	duk_def_prop(ctx, -3,
		     DUK_DEFPROP_HAVE_VALUE |
		     DUK_DEFPROP_HAVE_WRITABLE |
		     DUK_DEFPROP_HAVE_ENUMERABLE |
		     DUK_DEFPROP_ENUMERABLE |
		     DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Set the destructor */
	duk_dup(ctx, 0);
	duk_push_c_function(ctx, dukky_console___destructor, 1);
	duk_set_finalizer(ctx, -2);
	duk_pop(ctx);

	/* Set the constructor */
	duk_dup(ctx, 0);
	duk_push_c_function(ctx, dukky_console___constructor, 1);
	duk_put_prop_string(ctx, -2, "\xFF\xFFNETSURF_DUKTAPE_INIT");
	duk_pop(ctx);

	return 1; /* The prototype object */
}


