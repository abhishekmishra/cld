/*
 * cld_progress.h
 *
 *  Created on: 19-Mar-2019
 *      Author: abhis
 */

#ifndef SRC_CLD_PROGRESS_H_
#define SRC_CLD_PROGRESS_H_

#include <json-c/arraylist.h>

#define CLD_PROGRESS_DEFAULT_BAR "#"
#define CLD_PROGRESS_DEFAULT_BEFORE "["
#define CLD_PROGRESS_DEFAULT_AFTER "]"

typedef struct cld_progress_t {
	char* name;
	char* message;
	int length;
	char* bar;
	char* before;
	char* after;
	char* extra;
	double current;
	double total;
} cld_progress;

int create_cld_progress(cld_progress** progress, char* name, int length,
		double total);

void free_cld_progress(cld_progress* progress);

void show_progress(cld_progress* progress);

typedef struct cld_multi_progress_t {
	int old_count;
	array_list* progress_ls;
} cld_multi_progress;

int create_cld_multi_progress(cld_multi_progress** multi_progress);

void free_cld_multi_progress(cld_multi_progress* multi_progress);

#endif /* SRC_CLD_PROGRESS_H_ */
