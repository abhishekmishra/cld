/**
 * Copyright (c) 2020 Abhishek Mishra
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <stdlib.h>
#include <stdio.h>
#include "docker_util.h"
#include "cld_progress.h"

int create_cld_progress(cld_progress** progress, char* name, int length,
		double total) {
	(*progress) = (cld_progress*) calloc(1, sizeof(cld_progress));
	if ((*progress) == NULL) {
		return -1;
	}
	(*progress)->before = CLD_PROGRESS_DEFAULT_BEFORE;
	(*progress)->after = CLD_PROGRESS_DEFAULT_AFTER;
	(*progress)->bar = CLD_PROGRESS_DEFAULT_BAR;
	(*progress)->current = 0;
	(*progress)->total = total;
	(*progress)->name = name;
	(*progress)->length = length;
	(*progress)->message = NULL;
	return 0;
}

void free_cld_progress(cld_progress* progress) {
	free(progress);
}

void show_progress(cld_progress* progress) {
	printf("\r");
	printf("%s ", progress->name);
	printf("%s", progress->before);

	double complete = (progress->current / progress->total) * progress->length;
	int complete_bars = (int) complete;
	for (int i = 0; i < complete_bars; i++) {
		printf("%s", progress->bar);
	}
	for (int i = complete_bars; i < progress->length; i++) {
		printf("%s", " ");
	}

	printf("%s", progress->after);
	fflush(stdout);
}

int create_cld_multi_progress(cld_multi_progress** multi_progress) {
	(*multi_progress) = (cld_multi_progress*) calloc(1,
			sizeof(cld_multi_progress));
	if ((*multi_progress) == NULL) {
		return -1;
	}
	(*multi_progress)->old_count = 0;
	arraylist_new(&((*multi_progress)->progress_ls), (void (*)(void *))&free_cld_progress);
	return 0;
}

void free_cld_multi_progress(cld_multi_progress* multi_progress) {
	free(multi_progress->progress_ls);
	free(multi_progress);
}

