#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "obj.h"

static inline struct object _object_get(struct object_node * restrict n, uint64_t key) {
	if (n == NULL) {
		return null_obj;
	}

	if (key == n->key) {
		return n->val;
	} else if (key < n->key) {
		return _object_get(n->l, key);
	} else {
		return _object_get(n->r, key);
	}
}

struct object object_to_module(struct object o);

static void _object_to_module(struct object mod, struct object_node * restrict n) {
	if (n != NULL) {
		if (isupper(*n->name)) {
			if (n->val.type == obj_object) {
				object_set(mod, n->name, object_to_module(n->val));
			} else {
				object_set(mod, n->name, n->val);
			}
		}
		_object_to_module(mod, n->l);
		_object_to_module(mod, n->r);
	}
}

static inline void _object_set(struct object_node **n, uint64_t key, char *name, struct object val) {
	if (*n == NULL) {
		*n = malloc(sizeof(struct object_node));
		(*n)->name = strdup(name);
		(*n)->key = key;
		(*n)->val = val;
		(*n)->l = NULL;
		(*n)->r = NULL;
		return;
	}

	uint64_t cur = (*n)->key;
	if (key == cur) {
		(*n)->val = val;
	} else if (key < cur) {
		_object_set(&(*n)->l, key, name, val);
	} else {
		_object_set(&(*n)->r, key, name, val);
	}
}

static inline void _object_dispose(struct object_node * restrict n) {
	if (n != NULL) {
		if (n->l != NULL) _object_dispose(n->l);
		if (n->r != NULL) _object_dispose(n->r);
		free(n->name);
		free(n);
	}
}

struct object object_get(struct object obj, char *name) {
	return _object_get(*obj.data.obj, fnv64a(name));
}

struct object object_set(struct object obj, char *name, struct object val) {
	_object_set(obj.data.obj, fnv64a(name), name, val);
	return val;
}

static void dispose_object_obj(struct object obj) {
	_object_dispose(*obj.data.obj);
	free(obj.data.obj);
}

// TODO: actually return object content as string.
static char *object_obj_str(struct object obj) {
	char *str = malloc(sizeof(char) * 64);
	str[63] = '\0';
	sprintf(str, "object[%p]", *obj.data.obj);

	return str;
}

struct object new_object() {
	return (struct object) {
		.data.obj = calloc(1, sizeof(struct object_node *)),
		.type = obj_object,
		.marked = MARKPTR(),
		.dispose = dispose_object_obj,
		.string = object_obj_str
	};
}

struct object object_to_module(struct object o) {
	struct object mod = new_object();

	_object_to_module(mod, *o.data.obj);
	return mod;
}
