/*
 * cld_dict.c
 *
 *  Created on: 19-Mar-2019
 *      Author: abhis
 */

#include <stdlib.h>
#include "cld_dict.h"
#include "docker_util.h"

int create_cld_dict(cld_dict** dict) {
	(*dict) = (cld_dict*)calloc(1, sizeof(cld_dict));
	if(!(*dict)) {
		return -1;
	}
	arraylist_new(&((*dict)->keys), &free);
	arraylist_new(&((*dict)->vals), &free);
	return 0;
}

void free_cld_dict(cld_dict* dict) {
	arraylist_free(dict->keys);
	arraylist_free(dict->vals);
	free(dict);
}

int cld_dict_put(cld_dict* dict, char* key, char* value) {
	arraylist_add(dict->keys, str_clone(key));
	arraylist_add(dict->vals, str_clone(value));
	return 0;
}

//TODO: Not implemented
int cld_dict_remove(cld_dict* dict, char* key) {
	return -1;
}

//TODO: Not implemented
int cld_dict_get(cld_dict* dict, char* key, char** value) {
	return -1;
}

//TODO: Not implemented
int cld_dict_keys(cld_dict* dict, char** keys) {
	return -1;
}

