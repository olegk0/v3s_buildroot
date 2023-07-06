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

static void dukky_html_option_element___init(duk_context *ctx, html_option_element_private_t *priv, struct dom_html_element *html_option_element)
{
	dukky_html_element___init(ctx, &priv->parent, html_option_element);
#line 12 "HTMLOptionElement.bnd"
#line 48 "html_option_element.c"
}

static void dukky_html_option_element___fini(duk_context *ctx, html_option_element_private_t *priv)
{
	dukky_html_element___fini(ctx, &priv->parent);
}

static duk_ret_t dukky_html_option_element___constructor(duk_context *ctx)
{
	/* create private data and attach to instance */
	html_option_element_private_t *priv = calloc(1, sizeof(*priv));
	if (priv == NULL) return 0;
	duk_push_pointer(ctx, priv);
	duk_put_prop_string(ctx, 0, dukky_magic_string_private);

	dukky_html_option_element___init(ctx, priv, duk_get_pointer(ctx, 1));
	duk_set_top(ctx, 1);
	return 1;
}

static duk_ret_t dukky_html_option_element___destructor(duk_context *ctx)
{
	html_option_element_private_t *priv;
	duk_get_prop_string(ctx, 0, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop(ctx);
	if (priv == NULL) return 0;

	dukky_html_option_element___fini(ctx, priv);
	free(priv);
	return 0;
}

static duk_ret_t dukky_html_option_element_disabled_getter(duk_context *ctx)
{
	/* Get private data for method */
	html_option_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 17 "HTMLOptionElement.bnd"
#line 95 "html_option_element.c"
	dom_exception exc;
	bool b;

	exc = dom_html_option_element_get_disabled((struct dom_html_option_element *)((node_private_t*)priv)->node, &b);
	if (exc != DOM_NO_ERR) {
		return 0;
	}

	duk_push_boolean(ctx, b);

	return 1;
}

static duk_ret_t dukky_html_option_element_disabled_setter(duk_context *ctx)
{
	/* Get private data for method */
	html_option_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 18 "HTMLOptionElement.bnd"
#line 122 "html_option_element.c"
	dom_exception exc;
	bool b;

	b = duk_get_boolean(ctx, 0);

	exc = dom_html_option_element_set_disabled((struct dom_html_option_element *)((node_private_t*)priv)->node, b);
	if (exc != DOM_NO_ERR) {
		return 0;
	}

	return 0;
}

static duk_ret_t dukky_html_option_element_form_getter(duk_context *ctx)
{
	/* Get private data for method */
	html_option_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

	return 0;
}

static duk_ret_t dukky_html_option_element_label_getter(duk_context *ctx)
{
	/* Get private data for method */
	html_option_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 20 "HTMLOptionElement.bnd"
#line 164 "html_option_element.c"
	dom_exception exc;
	dom_string *str;

	exc = dom_html_option_element_get_label((struct dom_html_option_element *)((node_private_t*)priv)->node, &str);
	if (exc != DOM_NO_ERR) {
		return 0;
	}

	if (str != NULL) {
		duk_push_lstring(ctx,
			dom_string_data(str),
			dom_string_length(str));
		dom_string_unref(str);
	} else {
		duk_push_lstring(ctx, NULL, 0);
	}

	return 1;
}

static duk_ret_t dukky_html_option_element_label_setter(duk_context *ctx)
{
	/* Get private data for method */
	html_option_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 21 "HTMLOptionElement.bnd"
#line 198 "html_option_element.c"
	dom_exception exc;
	dom_string *str;
	duk_size_t slen;
	const char *s;
	s = duk_safe_to_lstring(ctx, 0, &slen);

	exc = dom_string_create((const uint8_t *)s, slen, &str);
	if (exc != DOM_NO_ERR) {
		return 0;
	}

	exc = dom_html_option_element_set_label((struct dom_html_option_element *)((node_private_t*)priv)->node, str);
	dom_string_unref(str);
	if (exc != DOM_NO_ERR) {
		return 0;
	}

	return 0;
}

static duk_ret_t dukky_html_option_element_defaultSelected_getter(duk_context *ctx)
{
	/* Get private data for method */
	html_option_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 14 "HTMLOptionElement.bnd"
#line 232 "html_option_element.c"
	dom_exception exc;
	bool b;

	exc = dom_html_option_element_get_default_selected((struct dom_html_option_element *)((node_private_t*)priv)->node, &b);
	if (exc != DOM_NO_ERR) {
		return 0;
	}

	duk_push_boolean(ctx, b);

	return 1;
}

static duk_ret_t dukky_html_option_element_defaultSelected_setter(duk_context *ctx)
{
	/* Get private data for method */
	html_option_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 15 "HTMLOptionElement.bnd"
#line 259 "html_option_element.c"
	dom_exception exc;
	bool b;

	b = duk_get_boolean(ctx, 0);

	exc = dom_html_option_element_set_default_selected((struct dom_html_option_element *)((node_private_t*)priv)->node, b);
	if (exc != DOM_NO_ERR) {
		return 0;
	}

	return 0;
}

static duk_ret_t dukky_html_option_element_selected_getter(duk_context *ctx)
{
	/* Get private data for method */
	html_option_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 23 "HTMLOptionElement.bnd"
#line 286 "html_option_element.c"
	dom_exception exc;
	bool b;

	exc = dom_html_option_element_get_selected((struct dom_html_option_element *)((node_private_t*)priv)->node, &b);
	if (exc != DOM_NO_ERR) {
		return 0;
	}

	duk_push_boolean(ctx, b);

	return 1;
}

static duk_ret_t dukky_html_option_element_selected_setter(duk_context *ctx)
{
	/* Get private data for method */
	html_option_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 24 "HTMLOptionElement.bnd"
#line 313 "html_option_element.c"
	dom_exception exc;
	bool b;

	b = duk_get_boolean(ctx, 0);

	exc = dom_html_option_element_set_selected((struct dom_html_option_element *)((node_private_t*)priv)->node, b);
	if (exc != DOM_NO_ERR) {
		return 0;
	}

	return 0;
}

static duk_ret_t dukky_html_option_element_value_getter(duk_context *ctx)
{
	/* Get private data for method */
	html_option_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 28 "HTMLOptionElement.bnd"
#line 340 "html_option_element.c"
	dom_exception exc;
	dom_string *str;

	exc = dom_html_option_element_get_value((struct dom_html_option_element *)((node_private_t*)priv)->node, &str);
	if (exc != DOM_NO_ERR) {
		return 0;
	}

	if (str != NULL) {
		duk_push_lstring(ctx,
			dom_string_data(str),
			dom_string_length(str));
		dom_string_unref(str);
	} else {
		duk_push_lstring(ctx, NULL, 0);
	}

	return 1;
}

static duk_ret_t dukky_html_option_element_value_setter(duk_context *ctx)
{
	/* Get private data for method */
	html_option_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 29 "HTMLOptionElement.bnd"
#line 374 "html_option_element.c"
	dom_exception exc;
	dom_string *str;
	duk_size_t slen;
	const char *s;
	s = duk_safe_to_lstring(ctx, 0, &slen);

	exc = dom_string_create((const uint8_t *)s, slen, &str);
	if (exc != DOM_NO_ERR) {
		return 0;
	}

	exc = dom_html_option_element_set_value((struct dom_html_option_element *)((node_private_t*)priv)->node, str);
	dom_string_unref(str);
	if (exc != DOM_NO_ERR) {
		return 0;
	}

	return 0;
}

static duk_ret_t dukky_html_option_element_text_getter(duk_context *ctx)
{
	/* Get private data for method */
	html_option_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 26 "HTMLOptionElement.bnd"
#line 408 "html_option_element.c"
	dom_exception exc;
	dom_string *str;

	exc = dom_html_option_element_get_text((struct dom_html_option_element *)((node_private_t*)priv)->node, &str);
	if (exc != DOM_NO_ERR) {
		return 0;
	}

	if (str != NULL) {
		duk_push_lstring(ctx,
			dom_string_data(str),
			dom_string_length(str));
		dom_string_unref(str);
	} else {
		duk_push_lstring(ctx, NULL, 0);
	}

	return 1;
}

static duk_ret_t dukky_html_option_element_text_setter(duk_context *ctx)
{
	/* Get private data for method */
	html_option_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

	return 0;
}

static duk_ret_t dukky_html_option_element_index_getter(duk_context *ctx)
{
	/* Get private data for method */
	html_option_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

	return 0;
}

duk_ret_t dukky_html_option_element___proto(duk_context *ctx, void *udata)
{
	/* Set this prototype's prototype (left-parent) */
	/* get prototype */
	duk_get_global_string(ctx, dukky_magic_string_prototypes);
	duk_get_prop_string(ctx, -1, "\xFF\xFFNETSURF_DUKTAPE_PROTOTYPE_HTMLELEMENT");
	duk_replace(ctx, -2);
	duk_set_prototype(ctx, 0);

	/* Add read/write property */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "disabled");
	duk_push_c_function(ctx, dukky_html_option_element_disabled_getter, 0);
	duk_push_c_function(ctx, dukky_html_option_element_disabled_setter, 1);
	duk_def_prop(ctx, -4, DUK_DEFPROP_HAVE_GETTER |
		DUK_DEFPROP_HAVE_SETTER |
		DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE |
		DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add readonly property */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "form");
	duk_push_c_function(ctx, dukky_html_option_element_form_getter, 0);
	duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_GETTER |
		DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE |
		DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add read/write property */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "label");
	duk_push_c_function(ctx, dukky_html_option_element_label_getter, 0);
	duk_push_c_function(ctx, dukky_html_option_element_label_setter, 1);
	duk_def_prop(ctx, -4, DUK_DEFPROP_HAVE_GETTER |
		DUK_DEFPROP_HAVE_SETTER |
		DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE |
		DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add read/write property */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "defaultSelected");
	duk_push_c_function(ctx, dukky_html_option_element_defaultSelected_getter, 0);
	duk_push_c_function(ctx, dukky_html_option_element_defaultSelected_setter, 1);
	duk_def_prop(ctx, -4, DUK_DEFPROP_HAVE_GETTER |
		DUK_DEFPROP_HAVE_SETTER |
		DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE |
		DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add read/write property */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "selected");
	duk_push_c_function(ctx, dukky_html_option_element_selected_getter, 0);
	duk_push_c_function(ctx, dukky_html_option_element_selected_setter, 1);
	duk_def_prop(ctx, -4, DUK_DEFPROP_HAVE_GETTER |
		DUK_DEFPROP_HAVE_SETTER |
		DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE |
		DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add read/write property */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "value");
	duk_push_c_function(ctx, dukky_html_option_element_value_getter, 0);
	duk_push_c_function(ctx, dukky_html_option_element_value_setter, 1);
	duk_def_prop(ctx, -4, DUK_DEFPROP_HAVE_GETTER |
		DUK_DEFPROP_HAVE_SETTER |
		DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE |
		DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add read/write property */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "text");
	duk_push_c_function(ctx, dukky_html_option_element_text_getter, 0);
	duk_push_c_function(ctx, dukky_html_option_element_text_setter, 1);
	duk_def_prop(ctx, -4, DUK_DEFPROP_HAVE_GETTER |
		DUK_DEFPROP_HAVE_SETTER |
		DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE |
		DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add readonly property */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "index");
	duk_push_c_function(ctx, dukky_html_option_element_index_getter, 0);
	duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_GETTER |
		DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE |
		DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Set the destructor */
	duk_dup(ctx, 0);
	duk_push_c_function(ctx, dukky_html_option_element___destructor, 1);
	duk_set_finalizer(ctx, -2);
	duk_pop(ctx);

	/* Set the constructor */
	duk_dup(ctx, 0);
	duk_push_c_function(ctx, dukky_html_option_element___constructor, 2);
	duk_put_prop_string(ctx, -2, "\xFF\xFFNETSURF_DUKTAPE_INIT");
	duk_pop(ctx);

	return 1; /* The prototype object */
}


