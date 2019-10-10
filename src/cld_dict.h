/*
 * cld_dict.h
 *
 *  Created on: 19-Mar-2019
 *      Author: abhis
 */

#ifndef SRC_CLD_DICT_H_
#define SRC_CLD_DICT_H_

#include <json-c/arraylist.h>

typedef struct cld_dict_t {
	struct array_list* keys;
	struct array_list* vals;
} cld_dict;

int create_cld_dict(cld_dict** dict);

void free_cld_dict(cld_dict* dict);

int cld_dict_put(cld_dict* dict, char* key, char* value);

int cld_dict_remove(cld_dict* dict, char* key);

int cld_dict_get(cld_dict* dict, char* key, char** value);

int cld_dict_keys(cld_dict* dict, char** keys);

#define cld_dict_foreach(dict, key, value) \
	char* key; \
	char* value; \
	int i; \
	int len = array_list_length(dict->keys); \
	for(i = 0, \
			key = (char*)array_list_get_idx(dict->keys, i), \
			value = (char*)array_list_get_idx(dict->vals, i); \
			i < len; \
			i++, \
			key = (char*)array_list_get_idx(dict->keys, i), \
			value = (char*)array_list_get_idx(dict->vals, i))

#endif /* SRC_CLD_DICT_H_ */
