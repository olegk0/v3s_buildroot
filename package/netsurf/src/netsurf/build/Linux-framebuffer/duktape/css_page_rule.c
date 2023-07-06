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

static void dukky_css_page_rule___init(duk_context *ctx, css_page_rule_private_t *priv)
{
	dukky_css_grouping_rule___init(ctx, &priv->parent);
}

static void dukky_css_page_rule___fini(duk_context *ctx, css_page_rule_private_t *priv)
{
	dukky_css_grouping_rule___fini(ctx, &priv->parent);
}

static duk_ret_t dukky_css_page_rule___constructor(duk_context *ctx)
{
	/* create private data and attach to instance */
	css_page_rule_private_t *priv = calloc(1, sizeof(*priv));
	if (priv == NULL) return 0;
	duk_push_pointer(ctx, priv);
	duk_put_prop_string(ctx, 0, dukky_magic_string_private);

	dukky_css_page_rule___init(ctx, priv);
	duk_set_top(ctx, 1);
	return 1;
}

static duk_ret_t dukky_css_page_rule___destructor(duk_context *ctx)
{
	css_page_rule_private_t *priv;
	duk_get_prop_string(ctx, 0, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop(ctx);
	if (priv == NULL) return 0;

	dukky_css_page_rule___fini(ctx, priv);
	free(priv);
	return 0;
}

static duk_ret_t dukky_css_page_rule_selectorText_getter(duk_context *ctx)
{
	/* Get private data for method */
	css_page_rule_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

	return 0;
}

static duk_ret_t dukky_css_page_rule_selectorText_setter(duk_context *ctx)
{
	/* Get private data for method */
	css_page_rule_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

	return 0;
}

static duk_ret_t dukky_css_page_rule_style_getter(duk_context *ctx)
{
	/* Get private data for method */
	css_page_rule_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

	return 0;
}

static duk_ret_t dukky_css_page_rule_style_setter(duk_context *ctx)
{
	/* Get private data for method */
	css_page_rule_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

	duk_ret_t get_ret;

	get_ret = dukky_css_page_rule_style_getter(ctx);
	if (get_ret != 1) {
		return 0;
	}

	/* parameter ... attribute */

	duk_dup(ctx, 0);
	/* ... attribute parameter */

	/* call the putforward */
	duk_put_prop_string(ctx, -2, "cssText");

	return 0;
}

duk_ret_t dukky_css_page_rule___proto(duk_context *ctx, void *udata)
{
	/* Set this prototype's prototype (left-parent) */
	/* get prototype */
	duk_get_global_string(ctx, dukky_magic_string_prototypes);
	duk_get_prop_string(ctx, -1, "\xFF\xFFNETSURF_DUKTAPE_PROTOTYPE_CSSGROUPINGRULE");
	duk_replace(ctx, -2);
	duk_set_prototype(ctx, 0);

	/* Add read/write property */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "selectorText");
	duk_push_c_function(ctx, dukky_css_page_rule_selectorText_getter, 0);
	duk_push_c_function(ctx, dukky_css_page_rule_selectorText_setter, 1);
	duk_def_prop(ctx, -4, DUK_DEFPROP_HAVE_GETTER |
		DUK_DEFPROP_HAVE_SETTER |
		DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE |
		DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add read/write property */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "style");
	duk_push_c_function(ctx, dukky_css_page_rule_style_getter, 0);
	duk_push_c_function(ctx, dukky_css_page_rule_style_setter, 1);
	duk_def_prop(ctx, -4, DUK_DEFPROP_HAVE_GETTER |
		DUK_DEFPROP_HAVE_SETTER |
		DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE |
		DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Set the destructor */
	duk_dup(ctx, 0);
	duk_push_c_function(ctx, dukky_css_page_rule___destructor, 1);
	duk_set_finalizer(ctx, -2);
	duk_pop(ctx);

	/* Set the constructor */
	duk_dup(ctx, 0);
	duk_push_c_function(ctx, dukky_css_page_rule___constructor, 1);
	duk_put_prop_string(ctx, -2, "\xFF\xFFNETSURF_DUKTAPE_INIT");
	duk_pop(ctx);

	return 1; /* The prototype object */
}


