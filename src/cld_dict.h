// Copyright (c) 2020 Abhishek Mishra
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef SRC_CLD_DICT_H_
#define SRC_CLD_DICT_H_

#include <arraylist.h>

typedef struct cld_dict_t {
	arraylist* keys;
	arraylist* vals;
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
	size_t i; \
	size_t len = arraylist_length(dict->keys); \
	for(i = 0, \
			key = (char*)arraylist_get(dict->keys, i), \
			value = (char*)arraylist_get(dict->vals, i); \
			i < len; \
			i++, \
			key = (char*)arraylist_get(dict->keys, i), \
			value = (char*)arraylist_get(dict->vals, i))

#endif /* SRC_CLD_DICT_H_ */
