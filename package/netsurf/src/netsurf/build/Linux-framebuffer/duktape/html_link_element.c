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

static void dukky_html_link_element___init(duk_context *ctx, html_link_element_private_t *priv, struct dom_html_element *html_link_element)
{
	dukky_html_element___init(ctx, &priv->parent, html_link_element);
#line 12 "HTMLLinkElement.bnd"
#line 48 "html_link_element.c"
}

static void dukky_html_link_element___fini(duk_context *ctx, html_link_element_private_t *priv)
{
	dukky_html_element___fini(ctx, &priv->parent);
}

static duk_ret_t dukky_html_link_element___constructor(duk_context *ctx)
{
	/* create private data and attach to instance */
	html_link_element_private_t *priv = calloc(1, sizeof(*priv));
	if (priv == NULL) return 0;
	duk_push_pointer(ctx, priv);
	duk_put_prop_string(ctx, 0, dukky_magic_string_private);

	dukky_html_link_element___init(ctx, priv, duk_get_pointer(ctx, 1));
	duk_set_top(ctx, 1);
	return 1;
}

static duk_ret_t dukky_html_link_element___destructor(duk_context *ctx)
{
	html_link_element_private_t *priv;
	duk_get_prop_string(ctx, 0, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop(ctx);
	if (priv == NULL) return 0;

	dukky_html_link_element___fini(ctx, priv);
	free(priv);
	return 0;
}

static duk_ret_t dukky_html_link_element_href_getter(duk_context *ctx)
{
	/* Get private data for method */
	html_link_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 20 "HTMLLinkElement.bnd"
#line 95 "html_link_element.c"
	dom_exception exc;
	dom_string *str;

	exc = dom_html_link_element_get_href((struct dom_html_link_element *)((node_private_t*)priv)->node, &str);
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

static duk_ret_t dukky_html_link_element_href_setter(duk_context *ctx)
{
	/* Get private data for method */
	html_link_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 21 "HTMLLinkElement.bnd"
#line 129 "html_link_element.c"
	dom_exception exc;
	dom_string *str;
	duk_size_t slen;
	const char *s;
	s = duk_safe_to_lstring(ctx, 0, &slen);

	exc = dom_string_create((const uint8_t *)s, slen, &str);
	if (exc != DOM_NO_ERR) {
		return 0;
	}

	exc = dom_html_link_element_set_href((struct dom_html_link_element *)((node_private_t*)priv)->node, str);
	dom_string_unref(str);
	if (exc != DOM_NO_ERR) {
		return 0;
	}

	return 0;
}

static duk_ret_t dukky_html_link_element_crossOrigin_getter(duk_context *ctx)
{
	/* Get private data for method */
	html_link_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

	return 0;
}

static duk_ret_t dukky_html_link_element_crossOrigin_setter(duk_context *ctx)
{
	/* Get private data for method */
	html_link_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

	return 0;
}

static duk_ret_t dukky_html_link_element_rel_getter(duk_context *ctx)
{
	/* Get private data for method */
	html_link_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 26 "HTMLLinkElement.bnd"
#line 193 "html_link_element.c"
	dom_exception exc;
	dom_string *str;

	exc = dom_html_link_element_get_rel((struct dom_html_link_element *)((node_private_t*)priv)->node, &str);
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

static duk_ret_t dukky_html_link_element_rel_setter(duk_context *ctx)
{
	/* Get private data for method */
	html_link_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 27 "HTMLLinkElement.bnd"
#line 227 "html_link_element.c"
	dom_exception exc;
	dom_string *str;
	duk_size_t slen;
	const char *s;
	s = duk_safe_to_lstring(ctx, 0, &slen);

	exc = dom_string_create((const uint8_t *)s, slen, &str);
	if (exc != DOM_NO_ERR) {
		return 0;
	}

	exc = dom_html_link_element_set_rel((struct dom_html_link_element *)((node_private_t*)priv)->node, str);
	dom_string_unref(str);
	if (exc != DOM_NO_ERR) {
		return 0;
	}

	return 0;
}

static duk_ret_t dukky_html_link_element_relList_getter(duk_context *ctx)
{
	/* Get private data for method */
	html_link_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

	return 0;
}

static duk_ret_t dukky_html_link_element_media_getter(duk_context *ctx)
{
	/* Get private data for method */
	html_link_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 23 "HTMLLinkElement.bnd"
#line 276 "html_link_element.c"
	dom_exception exc;
	dom_string *str;

	exc = dom_html_link_element_get_media((struct dom_html_link_element *)((node_private_t*)priv)->node, &str);
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

static duk_ret_t dukky_html_link_element_media_setter(duk_context *ctx)
{
	/* Get private data for method */
	html_link_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 24 "HTMLLinkElement.bnd"
#line 310 "html_link_element.c"
	dom_exception exc;
	dom_string *str;
	duk_size_t slen;
	const char *s;
	s = duk_safe_to_lstring(ctx, 0, &slen);

	exc = dom_string_create((const uint8_t *)s, slen, &str);
	if (exc != DOM_NO_ERR) {
		return 0;
	}

	exc = dom_html_link_element_set_media((struct dom_html_link_element *)((node_private_t*)priv)->node, str);
	dom_string_unref(str);
	if (exc != DOM_NO_ERR) {
		return 0;
	}

	return 0;
}

static duk_ret_t dukky_html_link_element_hreflang_getter(duk_context *ctx)
{
	/* Get private data for method */
	html_link_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 17 "HTMLLinkElement.bnd"
#line 344 "html_link_element.c"
	dom_exception exc;
	dom_string *str;

	exc = dom_html_link_element_get_hreflang((struct dom_html_link_element *)((node_private_t*)priv)->node, &str);
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

static duk_ret_t dukky_html_link_element_hreflang_setter(duk_context *ctx)
{
	/* Get private data for method */
	html_link_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 18 "HTMLLinkElement.bnd"
#line 378 "html_link_element.c"
	dom_exception exc;
	dom_string *str;
	duk_size_t slen;
	const char *s;
	s = duk_safe_to_lstring(ctx, 0, &slen);

	exc = dom_string_create((const uint8_t *)s, slen, &str);
	if (exc != DOM_NO_ERR) {
		return 0;
	}

	exc = dom_html_link_element_set_hreflang((struct dom_html_link_element *)((node_private_t*)priv)->node, str);
	dom_string_unref(str);
	if (exc != DOM_NO_ERR) {
		return 0;
	}

	return 0;
}

static duk_ret_t dukky_html_link_element_type_getter(duk_context *ctx)
{
	/* Get private data for method */
	html_link_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 35 "HTMLLinkElement.bnd"
#line 412 "html_link_element.c"
	dom_exception exc;
	dom_string *str;

	exc = dom_html_link_element_get_type((struct dom_html_link_element *)((node_private_t*)priv)->node, &str);
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

static duk_ret_t dukky_html_link_element_type_setter(duk_context *ctx)
{
	/* Get private data for method */
	html_link_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 36 "HTMLLinkElement.bnd"
#line 446 "html_link_element.c"
	dom_exception exc;
	dom_string *str;
	duk_size_t slen;
	const char *s;
	s = duk_safe_to_lstring(ctx, 0, &slen);

	exc = dom_string_create((const uint8_t *)s, slen, &str);
	if (exc != DOM_NO_ERR) {
		return 0;
	}

	exc = dom_html_link_element_set_type((struct dom_html_link_element *)((node_private_t*)priv)->node, str);
	dom_string_unref(str);
	if (exc != DOM_NO_ERR) {
		return 0;
	}

	return 0;
}

static duk_ret_t dukky_html_link_element_sizes_getter(duk_context *ctx)
{
	/* Get private data for method */
	html_link_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

	return 0;
}

static duk_ret_t dukky_html_link_element_sizes_setter(duk_context *ctx)
{
	/* Get private data for method */
	html_link_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

	duk_ret_t get_ret;

	get_ret = dukky_html_link_element_sizes_getter(ctx);
	if (get_ret != 1) {
		return 0;
	}

	/* parameter ... attribute */

	duk_dup(ctx, 0);
	/* ... attribute parameter */

	/* call the putforward */
	duk_put_prop_string(ctx, -2, "value");

	return 0;
}

static duk_ret_t dukky_html_link_element_charset_getter(duk_context *ctx)
{
	/* Get private data for method */
	html_link_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 14 "HTMLLinkElement.bnd"
#line 525 "html_link_element.c"
	dom_exception exc;
	dom_string *str;

	exc = dom_html_link_element_get_charset((struct dom_html_link_element *)((node_private_t*)priv)->node, &str);
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

static duk_ret_t dukky_html_link_element_charset_setter(duk_context *ctx)
{
	/* Get private data for method */
	html_link_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 15 "HTMLLinkElement.bnd"
#line 559 "html_link_element.c"
	dom_exception exc;
	dom_string *str;
	duk_size_t slen;
	const char *s;
	s = duk_safe_to_lstring(ctx, 0, &slen);

	exc = dom_string_create((const uint8_t *)s, slen, &str);
	if (exc != DOM_NO_ERR) {
		return 0;
	}

	exc = dom_html_link_element_set_charset((struct dom_html_link_element *)((node_private_t*)priv)->node, str);
	dom_string_unref(str);
	if (exc != DOM_NO_ERR) {
		return 0;
	}

	return 0;
}

static duk_ret_t dukky_html_link_element_rev_getter(duk_context *ctx)
{
	/* Get private data for method */
	html_link_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 29 "HTMLLinkElement.bnd"
#line 593 "html_link_element.c"
	dom_exception exc;
	dom_string *str;

	exc = dom_html_link_element_get_rev((struct dom_html_link_element *)((node_private_t*)priv)->node, &str);
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

static duk_ret_t dukky_html_link_element_rev_setter(duk_context *ctx)
{
	/* Get private data for method */
	html_link_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 30 "HTMLLinkElement.bnd"
#line 627 "html_link_element.c"
	dom_exception exc;
	dom_string *str;
	duk_size_t slen;
	const char *s;
	s = duk_safe_to_lstring(ctx, 0, &slen);

	exc = dom_string_create((const uint8_t *)s, slen, &str);
	if (exc != DOM_NO_ERR) {
		return 0;
	}

	exc = dom_html_link_element_set_rev((struct dom_html_link_element *)((node_private_t*)priv)->node, str);
	dom_string_unref(str);
	if (exc != DOM_NO_ERR) {
		return 0;
	}

	return 0;
}

static duk_ret_t dukky_html_link_element_target_getter(duk_context *ctx)
{
	/* Get private data for method */
	html_link_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 32 "HTMLLinkElement.bnd"
#line 661 "html_link_element.c"
	dom_exception exc;
	dom_string *str;

	exc = dom_html_link_element_get_target((struct dom_html_link_element *)((node_private_t*)priv)->node, &str);
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

static duk_ret_t dukky_html_link_element_target_setter(duk_context *ctx)
{
	/* Get private data for method */
	html_link_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

#line 33 "HTMLLinkElement.bnd"
#line 695 "html_link_element.c"
	dom_exception exc;
	dom_string *str;
	duk_size_t slen;
	const char *s;
	s = duk_safe_to_lstring(ctx, 0, &slen);

	exc = dom_string_create((const uint8_t *)s, slen, &str);
	if (exc != DOM_NO_ERR) {
		return 0;
	}

	exc = dom_html_link_element_set_target((struct dom_html_link_element *)((node_private_t*)priv)->node, str);
	dom_string_unref(str);
	if (exc != DOM_NO_ERR) {
		return 0;
	}

	return 0;
}

static duk_ret_t dukky_html_link_element_sheet_getter(duk_context *ctx)
{
	/* Get private data for method */
	html_link_element_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

	return 0;
}

duk_ret_t dukky_html_link_element___proto(duk_context *ctx, void *udata)
{
	/* Set this prototype's prototype (left-parent) */
	/* get prototype */
	duk_get_global_string(ctx, dukky_magic_string_prototypes);
	duk_get_prop_string(ctx, -1, "\xFF\xFFNETSURF_DUKTAPE_PROTOTYPE_HTMLELEMENT");
	duk_replace(ctx, -2);
	duk_set_prototype(ctx, 0);

	/* Add read/write property */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "href");
	duk_push_c_function(ctx, dukky_html_link_element_href_getter, 0);
	duk_push_c_function(ctx, dukky_html_link_element_href_setter, 1);
	duk_def_prop(ctx, -4, DUK_DEFPROP_HAVE_GETTER |
		DUK_DEFPROP_HAVE_SETTER |
		DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE |
		DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add read/write property */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "crossOrigin");
	duk_push_c_function(ctx, dukky_html_link_element_crossOrigin_getter, 0);
	duk_push_c_function(ctx, dukky_html_link_element_crossOrigin_setter, 1);
	duk_def_prop(ctx, -4, DUK_DEFPROP_HAVE_GETTER |
		DUK_DEFPROP_HAVE_SETTER |
		DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE |
		DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add read/write property */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "rel");
	duk_push_c_function(ctx, dukky_html_link_element_rel_getter, 0);
	duk_push_c_function(ctx, dukky_html_link_element_rel_setter, 1);
	duk_def_prop(ctx, -4, DUK_DEFPROP_HAVE_GETTER |
		DUK_DEFPROP_HAVE_SETTER |
		DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE |
		DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add readonly property */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "relList");
	duk_push_c_function(ctx, dukky_html_link_element_relList_getter, 0);
	duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_GETTER |
		DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE |
		DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add read/write property */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "media");
	duk_push_c_function(ctx, dukky_html_link_element_media_getter, 0);
	duk_push_c_function(ctx, dukky_html_link_element_media_setter, 1);
	duk_def_prop(ctx, -4, DUK_DEFPROP_HAVE_GETTER |
		DUK_DEFPROP_HAVE_SETTER |
		DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE |
		DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add read/write property */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "hreflang");
	duk_push_c_function(ctx, dukky_html_link_element_hreflang_getter, 0);
	duk_push_c_function(ctx, dukky_html_link_element_hreflang_setter, 1);
	duk_def_prop(ctx, -4, DUK_DEFPROP_HAVE_GETTER |
		DUK_DEFPROP_HAVE_SETTER |
		DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE |
		DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add read/write property */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "type");
	duk_push_c_function(ctx, dukky_html_link_element_type_getter, 0);
	duk_push_c_function(ctx, dukky_html_link_element_type_setter, 1);
	duk_def_prop(ctx, -4, DUK_DEFPROP_HAVE_GETTER |
		DUK_DEFPROP_HAVE_SETTER |
		DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE |
		DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add read/write property */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "sizes");
	duk_push_c_function(ctx, dukky_html_link_element_sizes_getter, 0);
	duk_push_c_function(ctx, dukky_html_link_element_sizes_setter, 1);
	duk_def_prop(ctx, -4, DUK_DEFPROP_HAVE_GETTER |
		DUK_DEFPROP_HAVE_SETTER |
		DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE |
		DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add read/write property */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "charset");
	duk_push_c_function(ctx, dukky_html_link_element_charset_getter, 0);
	duk_push_c_function(ctx, dukky_html_link_element_charset_setter, 1);
	duk_def_prop(ctx, -4, DUK_DEFPROP_HAVE_GETTER |
		DUK_DEFPROP_HAVE_SETTER |
		DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE |
		DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add read/write property */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "rev");
	duk_push_c_function(ctx, dukky_html_link_element_rev_getter, 0);
	duk_push_c_function(ctx, dukky_html_link_element_rev_setter, 1);
	duk_def_prop(ctx, -4, DUK_DEFPROP_HAVE_GETTER |
		DUK_DEFPROP_HAVE_SETTER |
		DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE |
		DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add read/write property */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "target");
	duk_push_c_function(ctx, dukky_html_link_element_target_getter, 0);
	duk_push_c_function(ctx, dukky_html_link_element_target_setter, 1);
	duk_def_prop(ctx, -4, DUK_DEFPROP_HAVE_GETTER |
		DUK_DEFPROP_HAVE_SETTER |
		DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE |
		DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add readonly property */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "sheet");
	duk_push_c_function(ctx, dukky_html_link_element_sheet_getter, 0);
	duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_GETTER |
		DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE |
		DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Set the destructor */
	duk_dup(ctx, 0);
	duk_push_c_function(ctx, dukky_html_link_element___destructor, 1);
	duk_set_finalizer(ctx, -2);
	duk_pop(ctx);

	/* Set the constructor */
	duk_dup(ctx, 0);
	duk_push_c_function(ctx, dukky_html_link_element___constructor, 2);
	duk_put_prop_string(ctx, -2, "\xFF\xFFNETSURF_DUKTAPE_INIT");
	duk_pop(ctx);

	return 1; /* The prototype object */
}


