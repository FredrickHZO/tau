#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "obj.h"

static void dummy_dispose(struct object o) {}

// ============================= CLOSURE OBJECT =============================
static void dispose_closure_obj(struct object o) {
	free(o.data.cl->fn->instructions);
	free(o.data.cl->fn);
	free(o.data.cl);
}

static char *closure_str(struct object o) {
	char *str = calloc(35, sizeof(char));
	sprintf(str, "closure[%p]", o.data.cl->fn);

	return str;
}

struct object new_closure_obj(struct function *fn, struct object *free, size_t num_free) {
	struct closure *cl = malloc(sizeof(struct closure));
	cl->fn = fn;
	cl->free = free;
	cl->num_free = num_free;

	return (struct object) {
		.data.cl = cl,
		.type = obj_closure,
		.dispose = dispose_closure_obj,
		.string = closure_str
	};
}

// ============================= FUNCTION OBJECT =============================
static void dispose_function_obj(struct object o) {
	for (int i = 0; i < o.data.fn->bklen; i++) {
		free(o.data.fn->bookmarks[i].line);
	}
	free(o.data.fn->bookmarks);
	free(o.data.fn->instructions);
	free(o.data.fn);
}

static char *function_str(struct object o) {
	char *str = calloc(35, sizeof(char));
	sprintf(str, "closure[%p]", o.data.fn);

	return str;
}

// ============================= ERROR OBJECT =============================
static void dispose_error_obj(struct object o) {
	free(o.data.str);
}

static char *error_str(struct object o) {
	return strdup(o.data.str);
}

struct object new_error_obj(char *str, size_t len) {
	return (struct object) {
		.data.str = str,
		.len = len,
		.type = obj_error,
		.dispose = dispose_error_obj,
		.string = error_str
	};
}

// ============================= FLOAT OBJECT =============================
static char *float_str(struct object o) {
	char *str = calloc(35, sizeof(char));
	sprintf(str, "%f", o.data.f);

	return str;
}

struct object new_float_obj(double val) {
	return (struct object) {
		.data.f = val,
		.type = obj_float,
		.dispose = dummy_dispose,
		.string = float_str
	};
}

struct object new_function_obj(uint8_t *insts, size_t len, uint32_t num_params, uint32_t num_locals, struct bookmark *bmarks, uint32_t bklen) {
	struct function *fn = malloc(sizeof(struct function));
	fn->instructions = insts;
	fn->len = len;
	fn->num_locals = num_locals;
	fn->num_params = num_params;
	fn->bookmarks = bmarks;
	fn->bklen = bklen;

	return (struct object) {
		.data.fn = fn,
		.type = obj_function,
		.dispose = dispose_function_obj,
		.string = function_str
	};
}

// ============================= INTEGER OBJECT =============================
static char *integer_str(struct object o) {
	char *str = calloc(30, sizeof(char));

#ifdef __unix__
	sprintf(str, "%ld", o.data.i);
#else
	sprintf(str, "%lld", o.data.i);
#endif

	return str;
}

struct object new_integer_obj(int64_t val) {
	return (struct object) {
		.data.i = val,
		.type = obj_integer,
		.dispose = dummy_dispose,
		.string = integer_str
	};
}

// ============================= STRING OBJECT =============================
static void dispose_string_obj(struct object o) {
	free(o.data.str);
}

static char *string_str(struct object o) {
	return strdup(o.data.str);
}

struct object new_string_obj(char *str, size_t len) {
	return (struct object) {
		.data.str = str,
		.len = len,
		.type = obj_string,
		.dispose = dispose_string_obj,
		.string = string_str
	};
}

// ============================= LIST OBJECT =============================
static void dispose_list_obj(struct object o) {
	free(o.data.list);
}

// TODO: optimise this.
static char *list_str(struct object o) {
	char *strings[o.len];
	size_t total_len = 3;

	for (int i = 0; i < o.len; i++) {
		char *s = object_str(o.data.list[i]);
		strings[i] = s;
		total_len += i < o.len-1 ? strlen(s) + 2 : strlen(s);
	}

	char *str = calloc(total_len, sizeof(char));
	str[0] = '[';

	for (int i = 0; i < o.len; i++) {
		strcat(str, strings[i]);
		if (i < o.len-1) strcat(str, ", ");
		free(strings[i]);
	}
	strcat(str, "]");

	return str;
}

struct object new_list_obj(struct object *list, size_t len) {
	return (struct object) {
		.data.list = list,
		.len = len,
		.type = obj_list,
		.dispose = dispose_list_obj,
		.string = list_str
	};
}

// ============================= STATIC OBJECTS =============================
static char *boolean_str(struct object o) {
	char *str = calloc(6, sizeof(char));
	strcpy(str, o.data.i == 1 ? "true" : "false");

	return str;
}

struct object parse_bool(uint32_t b) {
	return b ? true_obj : false_obj;
}

static char *null_str(struct object o) {
	char *str = calloc(5, sizeof(char));
	strcpy(str, "null");

	return str;
}

struct object true_obj = (struct object) {
	.data.i = 1,
	.type = obj_boolean,
	.len = 0,
	.dispose = dummy_dispose,
	.string = boolean_str
};

struct object false_obj = (struct object) {
	.data.i = 0,
	.type = obj_boolean,
	.len = 0,
	.dispose = dummy_dispose,
	.string = boolean_str
};

struct object null_obj = (struct object) {
	.data.i = 0,
	.type = obj_null,
	.len = 0,
	.dispose = dummy_dispose,
	.string = null_str
};

char *otype_str(enum obj_type t) {
	static char *strings[] = {
		"null",
		"bool",
		"int",
		"float",
		"string",
		"error",
		"list",
		"map",
		"builtin",
		"function",
		"closure",
		"object",
		"pipe",
		"bytes",
		"getsetter",
	};

	return strings[t];
}

char *object_str(struct object o) {
	return o.string(o);
}

void print_obj(struct object o) {
	char *str = o.string(o);
	puts(str);
	free(str);
}
