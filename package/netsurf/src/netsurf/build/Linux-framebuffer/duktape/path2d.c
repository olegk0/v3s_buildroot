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

static void dukky_path2d___init(duk_context *ctx, path2d_private_t *priv)
{
}

static void dukky_path2d___fini(duk_context *ctx, path2d_private_t *priv)
{
}

static duk_ret_t dukky_path2d___constructor(duk_context *ctx)
{
	/* create private data and attach to instance */
	path2d_private_t *priv = calloc(1, sizeof(*priv));
	if (priv == NULL) return 0;
	duk_push_pointer(ctx, priv);
	duk_put_prop_string(ctx, 0, dukky_magic_string_private);

	dukky_path2d___init(ctx, priv);
	duk_set_top(ctx, 1);
	return 1;
}

static duk_ret_t dukky_path2d___destructor(duk_context *ctx)
{
	path2d_private_t *priv;
	duk_get_prop_string(ctx, 0, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop(ctx);
	if (priv == NULL) return 0;

	dukky_path2d___fini(ctx, priv);
	free(priv);
	return 0;
}

static duk_ret_t dukky_path2d_addPath(duk_context *ctx)
{
	/* ensure the parameters are present */
	duk_idx_t dukky_argc = duk_get_top(ctx);
	if (dukky_argc < 1) {
		/* not enough arguments */
		return duk_error(ctx, DUK_RET_TYPE_ERROR, dukky_error_fmt_argument, 1, dukky_argc);
	} else if (dukky_argc == 1) {
		/* 1 optional arguments need adding */
		duk_push_null(ctx);
	} else if (dukky_argc > 2) {
		/* remove extraneous parameters */
		duk_set_top(ctx, 2);
	}

	/* check types of passed arguments are correct */
	if (dukky_argc > 0) {
		/* unhandled type check */
	}
	if (dukky_argc > 1) {
		/* unhandled type check */
	}
	/* Get private data for method */
	path2d_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

	return 0;
}

static duk_ret_t dukky_path2d_addPathByStrokingPath(duk_context *ctx)
{
	/* ensure the parameters are present */
	duk_idx_t dukky_argc = duk_get_top(ctx);
	if (dukky_argc < 2) {
		/* not enough arguments */
		return duk_error(ctx, DUK_RET_TYPE_ERROR, dukky_error_fmt_argument, 2, dukky_argc);
	} else if (dukky_argc == 2) {
		/* 1 optional arguments need adding */
		duk_push_null(ctx);
	} else if (dukky_argc > 3) {
		/* remove extraneous parameters */
		duk_set_top(ctx, 3);
	}

	/* check types of passed arguments are correct */
	if (dukky_argc > 0) {
		/* unhandled type check */
	}
	if (dukky_argc > 1) {
		/* unhandled type check */
	}
	if (dukky_argc > 2) {
		/* unhandled type check */
	}
	/* Get private data for method */
	path2d_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

	return 0;
}

static duk_ret_t dukky_path2d_addText(duk_context *ctx)
{
	/* Get private data for method */
	path2d_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

	return 0;
}

static duk_ret_t dukky_path2d_addPathByStrokingText(duk_context *ctx)
{
	/* Get private data for method */
	path2d_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

	return 0;
}

static duk_ret_t dukky_path2d_closePath(duk_context *ctx)
{
	/* ensure the parameters are present */
	duk_idx_t dukky_argc = duk_get_top(ctx);
	if (dukky_argc > 0) {
		/* remove extraneous parameters */
		duk_set_top(ctx, 0);
	}

	/* check types of passed arguments are correct */
	/* Get private data for method */
	path2d_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

	return 0;
}

static duk_ret_t dukky_path2d_moveTo(duk_context *ctx)
{
	/* ensure the parameters are present */
	duk_idx_t dukky_argc = duk_get_top(ctx);
	if (dukky_argc < 2) {
		/* not enough arguments */
		return duk_error(ctx, DUK_RET_TYPE_ERROR, dukky_error_fmt_argument, 2, dukky_argc);
	} else if (dukky_argc > 2) {
		/* remove extraneous parameters */
		duk_set_top(ctx, 2);
	}

	/* check types of passed arguments are correct */
	if (dukky_argc > 0) {
		if (!duk_is_number(ctx, 0)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 0, "x");
		}
	}
	if (dukky_argc > 1) {
		if (!duk_is_number(ctx, 1)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 1, "y");
		}
	}
	/* Get private data for method */
	path2d_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

	return 0;
}

static duk_ret_t dukky_path2d_lineTo(duk_context *ctx)
{
	/* ensure the parameters are present */
	duk_idx_t dukky_argc = duk_get_top(ctx);
	if (dukky_argc < 2) {
		/* not enough arguments */
		return duk_error(ctx, DUK_RET_TYPE_ERROR, dukky_error_fmt_argument, 2, dukky_argc);
	} else if (dukky_argc > 2) {
		/* remove extraneous parameters */
		duk_set_top(ctx, 2);
	}

	/* check types of passed arguments are correct */
	if (dukky_argc > 0) {
		if (!duk_is_number(ctx, 0)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 0, "x");
		}
	}
	if (dukky_argc > 1) {
		if (!duk_is_number(ctx, 1)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 1, "y");
		}
	}
	/* Get private data for method */
	path2d_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

	return 0;
}

static duk_ret_t dukky_path2d_quadraticCurveTo(duk_context *ctx)
{
	/* ensure the parameters are present */
	duk_idx_t dukky_argc = duk_get_top(ctx);
	if (dukky_argc < 4) {
		/* not enough arguments */
		return duk_error(ctx, DUK_RET_TYPE_ERROR, dukky_error_fmt_argument, 4, dukky_argc);
	} else if (dukky_argc > 4) {
		/* remove extraneous parameters */
		duk_set_top(ctx, 4);
	}

	/* check types of passed arguments are correct */
	if (dukky_argc > 0) {
		if (!duk_is_number(ctx, 0)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 0, "cpx");
		}
	}
	if (dukky_argc > 1) {
		if (!duk_is_number(ctx, 1)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 1, "cpy");
		}
	}
	if (dukky_argc > 2) {
		if (!duk_is_number(ctx, 2)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 2, "x");
		}
	}
	if (dukky_argc > 3) {
		if (!duk_is_number(ctx, 3)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 3, "y");
		}
	}
	/* Get private data for method */
	path2d_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

	return 0;
}

static duk_ret_t dukky_path2d_bezierCurveTo(duk_context *ctx)
{
	/* ensure the parameters are present */
	duk_idx_t dukky_argc = duk_get_top(ctx);
	if (dukky_argc < 6) {
		/* not enough arguments */
		return duk_error(ctx, DUK_RET_TYPE_ERROR, dukky_error_fmt_argument, 6, dukky_argc);
	} else if (dukky_argc > 6) {
		/* remove extraneous parameters */
		duk_set_top(ctx, 6);
	}

	/* check types of passed arguments are correct */
	if (dukky_argc > 0) {
		if (!duk_is_number(ctx, 0)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 0, "cp1x");
		}
	}
	if (dukky_argc > 1) {
		if (!duk_is_number(ctx, 1)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 1, "cp1y");
		}
	}
	if (dukky_argc > 2) {
		if (!duk_is_number(ctx, 2)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 2, "cp2x");
		}
	}
	if (dukky_argc > 3) {
		if (!duk_is_number(ctx, 3)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 3, "cp2y");
		}
	}
	if (dukky_argc > 4) {
		if (!duk_is_number(ctx, 4)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 4, "x");
		}
	}
	if (dukky_argc > 5) {
		if (!duk_is_number(ctx, 5)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 5, "y");
		}
	}
	/* Get private data for method */
	path2d_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

	return 0;
}

static duk_ret_t dukky_path2d_arcTo(duk_context *ctx)
{
	/* Get private data for method */
	path2d_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

	return 0;
}

static duk_ret_t dukky_path2d_rect(duk_context *ctx)
{
	/* ensure the parameters are present */
	duk_idx_t dukky_argc = duk_get_top(ctx);
	if (dukky_argc < 4) {
		/* not enough arguments */
		return duk_error(ctx, DUK_RET_TYPE_ERROR, dukky_error_fmt_argument, 4, dukky_argc);
	} else if (dukky_argc > 4) {
		/* remove extraneous parameters */
		duk_set_top(ctx, 4);
	}

	/* check types of passed arguments are correct */
	if (dukky_argc > 0) {
		if (!duk_is_number(ctx, 0)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 0, "x");
		}
	}
	if (dukky_argc > 1) {
		if (!duk_is_number(ctx, 1)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 1, "y");
		}
	}
	if (dukky_argc > 2) {
		if (!duk_is_number(ctx, 2)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 2, "w");
		}
	}
	if (dukky_argc > 3) {
		if (!duk_is_number(ctx, 3)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 3, "h");
		}
	}
	/* Get private data for method */
	path2d_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

	return 0;
}

static duk_ret_t dukky_path2d_arc(duk_context *ctx)
{
	/* ensure the parameters are present */
	duk_idx_t dukky_argc = duk_get_top(ctx);
	if (dukky_argc < 5) {
		/* not enough arguments */
		return duk_error(ctx, DUK_RET_TYPE_ERROR, dukky_error_fmt_argument, 5, dukky_argc);
	} else if (dukky_argc == 5) {
		/* 1 optional arguments need adding */
		duk_push_boolean(ctx, 0);
	} else if (dukky_argc > 6) {
		/* remove extraneous parameters */
		duk_set_top(ctx, 6);
	}

	/* check types of passed arguments are correct */
	if (dukky_argc > 0) {
		if (!duk_is_number(ctx, 0)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 0, "x");
		}
	}
	if (dukky_argc > 1) {
		if (!duk_is_number(ctx, 1)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 1, "y");
		}
	}
	if (dukky_argc > 2) {
		if (!duk_is_number(ctx, 2)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 2, "radius");
		}
	}
	if (dukky_argc > 3) {
		if (!duk_is_number(ctx, 3)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 3, "startAngle");
		}
	}
	if (dukky_argc > 4) {
		if (!duk_is_number(ctx, 4)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 4, "endAngle");
		}
	}
	if (dukky_argc > 5) {
		if (!duk_is_boolean(ctx, 5)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_bool_type, 5, "anticlockwise");
		}
	}
	/* Get private data for method */
	path2d_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

	return 0;
}

static duk_ret_t dukky_path2d_ellipse(duk_context *ctx)
{
	/* ensure the parameters are present */
	duk_idx_t dukky_argc = duk_get_top(ctx);
	if (dukky_argc < 7) {
		/* not enough arguments */
		return duk_error(ctx, DUK_RET_TYPE_ERROR, dukky_error_fmt_argument, 7, dukky_argc);
	} else if (dukky_argc == 7) {
		/* 1 optional arguments need adding */
		duk_push_boolean(ctx, 0);
	} else if (dukky_argc > 8) {
		/* remove extraneous parameters */
		duk_set_top(ctx, 8);
	}

	/* check types of passed arguments are correct */
	if (dukky_argc > 0) {
		if (!duk_is_number(ctx, 0)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 0, "x");
		}
	}
	if (dukky_argc > 1) {
		if (!duk_is_number(ctx, 1)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 1, "y");
		}
	}
	if (dukky_argc > 2) {
		if (!duk_is_number(ctx, 2)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 2, "radiusX");
		}
	}
	if (dukky_argc > 3) {
		if (!duk_is_number(ctx, 3)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 3, "radiusY");
		}
	}
	if (dukky_argc > 4) {
		if (!duk_is_number(ctx, 4)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 4, "rotation");
		}
	}
	if (dukky_argc > 5) {
		if (!duk_is_number(ctx, 5)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 5, "startAngle");
		}
	}
	if (dukky_argc > 6) {
		if (!duk_is_number(ctx, 6)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_number_type, 6, "endAngle");
		}
	}
	if (dukky_argc > 7) {
		if (!duk_is_boolean(ctx, 7)) {
			return duk_error(ctx, DUK_ERR_ERROR, dukky_error_fmt_bool_type, 7, "anticlockwise");
		}
	}
	/* Get private data for method */
	path2d_private_t *priv = NULL;
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, dukky_magic_string_private);
	priv = duk_get_pointer(ctx, -1);
	duk_pop_2(ctx);
	if (priv == NULL) {
		return 0; /* can do? No can do. */
	}

	return 0;
}

duk_ret_t dukky_path2d___proto(duk_context *ctx, void *udata)
{
	/* Add a method */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "addPath");
	duk_push_c_function(ctx, dukky_path2d_addPath, DUK_VARARGS);
	duk_def_prop(ctx, -3,
		     DUK_DEFPROP_HAVE_VALUE |
		     DUK_DEFPROP_HAVE_WRITABLE |
		     DUK_DEFPROP_HAVE_ENUMERABLE |
		     DUK_DEFPROP_ENUMERABLE |
		     DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add a method */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "addPathByStrokingPath");
	duk_push_c_function(ctx, dukky_path2d_addPathByStrokingPath, DUK_VARARGS);
	duk_def_prop(ctx, -3,
		     DUK_DEFPROP_HAVE_VALUE |
		     DUK_DEFPROP_HAVE_WRITABLE |
		     DUK_DEFPROP_HAVE_ENUMERABLE |
		     DUK_DEFPROP_ENUMERABLE |
		     DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add a method */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "addText");
	duk_push_c_function(ctx, dukky_path2d_addText, DUK_VARARGS);
	duk_def_prop(ctx, -3,
		     DUK_DEFPROP_HAVE_VALUE |
		     DUK_DEFPROP_HAVE_WRITABLE |
		     DUK_DEFPROP_HAVE_ENUMERABLE |
		     DUK_DEFPROP_ENUMERABLE |
		     DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add a method */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "addPathByStrokingText");
	duk_push_c_function(ctx, dukky_path2d_addPathByStrokingText, DUK_VARARGS);
	duk_def_prop(ctx, -3,
		     DUK_DEFPROP_HAVE_VALUE |
		     DUK_DEFPROP_HAVE_WRITABLE |
		     DUK_DEFPROP_HAVE_ENUMERABLE |
		     DUK_DEFPROP_ENUMERABLE |
		     DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add a method */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "closePath");
	duk_push_c_function(ctx, dukky_path2d_closePath, DUK_VARARGS);
	duk_def_prop(ctx, -3,
		     DUK_DEFPROP_HAVE_VALUE |
		     DUK_DEFPROP_HAVE_WRITABLE |
		     DUK_DEFPROP_HAVE_ENUMERABLE |
		     DUK_DEFPROP_ENUMERABLE |
		     DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add a method */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "moveTo");
	duk_push_c_function(ctx, dukky_path2d_moveTo, DUK_VARARGS);
	duk_def_prop(ctx, -3,
		     DUK_DEFPROP_HAVE_VALUE |
		     DUK_DEFPROP_HAVE_WRITABLE |
		     DUK_DEFPROP_HAVE_ENUMERABLE |
		     DUK_DEFPROP_ENUMERABLE |
		     DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add a method */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "lineTo");
	duk_push_c_function(ctx, dukky_path2d_lineTo, DUK_VARARGS);
	duk_def_prop(ctx, -3,
		     DUK_DEFPROP_HAVE_VALUE |
		     DUK_DEFPROP_HAVE_WRITABLE |
		     DUK_DEFPROP_HAVE_ENUMERABLE |
		     DUK_DEFPROP_ENUMERABLE |
		     DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add a method */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "quadraticCurveTo");
	duk_push_c_function(ctx, dukky_path2d_quadraticCurveTo, DUK_VARARGS);
	duk_def_prop(ctx, -3,
		     DUK_DEFPROP_HAVE_VALUE |
		     DUK_DEFPROP_HAVE_WRITABLE |
		     DUK_DEFPROP_HAVE_ENUMERABLE |
		     DUK_DEFPROP_ENUMERABLE |
		     DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add a method */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "bezierCurveTo");
	duk_push_c_function(ctx, dukky_path2d_bezierCurveTo, DUK_VARARGS);
	duk_def_prop(ctx, -3,
		     DUK_DEFPROP_HAVE_VALUE |
		     DUK_DEFPROP_HAVE_WRITABLE |
		     DUK_DEFPROP_HAVE_ENUMERABLE |
		     DUK_DEFPROP_ENUMERABLE |
		     DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add a method */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "arcTo");
	duk_push_c_function(ctx, dukky_path2d_arcTo, DUK_VARARGS);
	duk_def_prop(ctx, -3,
		     DUK_DEFPROP_HAVE_VALUE |
		     DUK_DEFPROP_HAVE_WRITABLE |
		     DUK_DEFPROP_HAVE_ENUMERABLE |
		     DUK_DEFPROP_ENUMERABLE |
		     DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add a method */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "rect");
	duk_push_c_function(ctx, dukky_path2d_rect, DUK_VARARGS);
	duk_def_prop(ctx, -3,
		     DUK_DEFPROP_HAVE_VALUE |
		     DUK_DEFPROP_HAVE_WRITABLE |
		     DUK_DEFPROP_HAVE_ENUMERABLE |
		     DUK_DEFPROP_ENUMERABLE |
		     DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add a method */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "arc");
	duk_push_c_function(ctx, dukky_path2d_arc, DUK_VARARGS);
	duk_def_prop(ctx, -3,
		     DUK_DEFPROP_HAVE_VALUE |
		     DUK_DEFPROP_HAVE_WRITABLE |
		     DUK_DEFPROP_HAVE_ENUMERABLE |
		     DUK_DEFPROP_ENUMERABLE |
		     DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Add a method */
	duk_dup(ctx, 0);
	duk_push_string(ctx, "ellipse");
	duk_push_c_function(ctx, dukky_path2d_ellipse, DUK_VARARGS);
	duk_def_prop(ctx, -3,
		     DUK_DEFPROP_HAVE_VALUE |
		     DUK_DEFPROP_HAVE_WRITABLE |
		     DUK_DEFPROP_HAVE_ENUMERABLE |
		     DUK_DEFPROP_ENUMERABLE |
		     DUK_DEFPROP_HAVE_CONFIGURABLE);
	duk_pop(ctx);

	/* Set the destructor */
	duk_dup(ctx, 0);
	duk_push_c_function(ctx, dukky_path2d___destructor, 1);
	duk_set_finalizer(ctx, -2);
	duk_pop(ctx);

	/* Set the constructor */
	duk_dup(ctx, 0);
	duk_push_c_function(ctx, dukky_path2d___constructor, 1);
	duk_put_prop_string(ctx, -2, "\xFF\xFFNETSURF_DUKTAPE_INIT");
	duk_pop(ctx);

	return 1; /* The prototype object */
}


