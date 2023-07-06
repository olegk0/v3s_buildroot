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

duk_bool_t
dukky_mutation_observer_init_get_childList(duk_context *ctx, duk_idx_t idx)
{
	duk_bool_t ret = false; /* Default value of childList */
	/* ... obj@idx ... */
	duk_get_prop_string(ctx, idx, "childList");
	/* ... obj@idx ... value/undefined */
	if (!duk_is_undefined(ctx, -1)) {
		/* Note, this throws a duk_error if it's not a boolean */
		ret = duk_require_boolean(ctx, -1);
	}
	duk_pop(ctx);
	return ret;
}

duk_bool_t
dukky_mutation_observer_init_get_attributes(duk_context *ctx, duk_idx_t idx)
{
	duk_bool_t ret = false; /* No default */
	/* ... obj@idx ... */
	duk_get_prop_string(ctx, idx, "attributes");
	/* ... obj@idx ... value/undefined */
	if (!duk_is_undefined(ctx, -1)) {
		/* Note, this throws a duk_error if it's not a boolean */
		ret = duk_require_boolean(ctx, -1);
	}
	duk_pop(ctx);
	return ret;
}

duk_bool_t
dukky_mutation_observer_init_get_characterData(duk_context *ctx, duk_idx_t idx)
{
	duk_bool_t ret = false; /* No default */
	/* ... obj@idx ... */
	duk_get_prop_string(ctx, idx, "characterData");
	/* ... obj@idx ... value/undefined */
	if (!duk_is_undefined(ctx, -1)) {
		/* Note, this throws a duk_error if it's not a boolean */
		ret = duk_require_boolean(ctx, -1);
	}
	duk_pop(ctx);
	return ret;
}

duk_bool_t
dukky_mutation_observer_init_get_subtree(duk_context *ctx, duk_idx_t idx)
{
	duk_bool_t ret = false; /* Default value of subtree */
	/* ... obj@idx ... */
	duk_get_prop_string(ctx, idx, "subtree");
	/* ... obj@idx ... value/undefined */
	if (!duk_is_undefined(ctx, -1)) {
		/* Note, this throws a duk_error if it's not a boolean */
		ret = duk_require_boolean(ctx, -1);
	}
	duk_pop(ctx);
	return ret;
}

duk_bool_t
dukky_mutation_observer_init_get_attributeOldValue(duk_context *ctx, duk_idx_t idx)
{
	duk_bool_t ret = false; /* No default */
	/* ... obj@idx ... */
	duk_get_prop_string(ctx, idx, "attributeOldValue");
	/* ... obj@idx ... value/undefined */
	if (!duk_is_undefined(ctx, -1)) {
		/* Note, this throws a duk_error if it's not a boolean */
		ret = duk_require_boolean(ctx, -1);
	}
	duk_pop(ctx);
	return ret;
}

duk_bool_t
dukky_mutation_observer_init_get_characterDataOldValue(duk_context *ctx, duk_idx_t idx)
{
	duk_bool_t ret = false; /* No default */
	/* ... obj@idx ... */
	duk_get_prop_string(ctx, idx, "characterDataOldValue");
	/* ... obj@idx ... value/undefined */
	if (!duk_is_undefined(ctx, -1)) {
		/* Note, this throws a duk_error if it's not a boolean */
		ret = duk_require_boolean(ctx, -1);
	}
	duk_pop(ctx);
	return ret;
}

/* Dictionary MutationObserverInit:attributeFilter unhandled type (11) */


