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

const char *
dukky_mouse_event_init_get_region(duk_context *ctx, duk_idx_t idx)
{
	const char *ret = NULL; /* No default */
	/* ... obj@idx ... */
	duk_get_prop_string(ctx, idx, "region");
	/* ... obj@idx ... value/undefined */
	if (!duk_is_undefined(ctx, -1)) {
		/* Note, this throws a duk_error if it's not a string */
		ret = duk_require_string(ctx, -1);
	}
	duk_pop(ctx);
	return ret;
}

duk_int_t
dukky_mouse_event_init_get_screenX(duk_context *ctx, duk_idx_t idx)
{
	duk_int_t ret = 0; /* Default value of screenX */
	/* ... obj@idx ... */
	duk_get_prop_string(ctx, idx, "screenX");
	/* ... obj@idx ... value/undefined */
	if (!duk_is_undefined(ctx, -1)) {
		/* Note, this throws a duk_error if it's not a int */
		ret = duk_require_int(ctx, -1);
	}
	duk_pop(ctx);
	return ret;
}

duk_int_t
dukky_mouse_event_init_get_screenY(duk_context *ctx, duk_idx_t idx)
{
	duk_int_t ret = 0; /* Default value of screenY */
	/* ... obj@idx ... */
	duk_get_prop_string(ctx, idx, "screenY");
	/* ... obj@idx ... value/undefined */
	if (!duk_is_undefined(ctx, -1)) {
		/* Note, this throws a duk_error if it's not a int */
		ret = duk_require_int(ctx, -1);
	}
	duk_pop(ctx);
	return ret;
}

duk_int_t
dukky_mouse_event_init_get_clientX(duk_context *ctx, duk_idx_t idx)
{
	duk_int_t ret = 0; /* Default value of clientX */
	/* ... obj@idx ... */
	duk_get_prop_string(ctx, idx, "clientX");
	/* ... obj@idx ... value/undefined */
	if (!duk_is_undefined(ctx, -1)) {
		/* Note, this throws a duk_error if it's not a int */
		ret = duk_require_int(ctx, -1);
	}
	duk_pop(ctx);
	return ret;
}

duk_int_t
dukky_mouse_event_init_get_clientY(duk_context *ctx, duk_idx_t idx)
{
	duk_int_t ret = 0; /* Default value of clientY */
	/* ... obj@idx ... */
	duk_get_prop_string(ctx, idx, "clientY");
	/* ... obj@idx ... value/undefined */
	if (!duk_is_undefined(ctx, -1)) {
		/* Note, this throws a duk_error if it's not a int */
		ret = duk_require_int(ctx, -1);
	}
	duk_pop(ctx);
	return ret;
}

duk_int_t
dukky_mouse_event_init_get_button(duk_context *ctx, duk_idx_t idx)
{
	duk_int_t ret = 0; /* Default value of button */
	/* ... obj@idx ... */
	duk_get_prop_string(ctx, idx, "button");
	/* ... obj@idx ... value/undefined */
	if (!duk_is_undefined(ctx, -1)) {
		/* Note, this throws a duk_error if it's not a int */
		ret = duk_require_int(ctx, -1);
	}
	duk_pop(ctx);
	return ret;
}

duk_int_t
dukky_mouse_event_init_get_buttons(duk_context *ctx, duk_idx_t idx)
{
	duk_int_t ret = 0; /* Default value of buttons */
	/* ... obj@idx ... */
	duk_get_prop_string(ctx, idx, "buttons");
	/* ... obj@idx ... value/undefined */
	if (!duk_is_undefined(ctx, -1)) {
		/* Note, this throws a duk_error if it's not a int */
		ret = duk_require_int(ctx, -1);
	}
	duk_pop(ctx);
	return ret;
}

/* Dictionary MouseEventInit:relatedTarget unhandled type (1) */

