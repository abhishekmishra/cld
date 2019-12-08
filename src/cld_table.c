/*
 * cld_table.c
 *
 *  Created on: 19-Mar-2019
 *      Author: abhis
 */

#include <stdlib.h>
#include "docker_util.h"
#include "cld_table.h"

int create_cld_table(cld_table** table, size_t num_rows, size_t num_cols) {
	(*table) = (cld_table*) calloc(1, sizeof(cld_table));
	if (!(*table)) {
		return 1;
	}
	(*table)->num_cols = num_cols;
	(*table)->num_rows = num_rows;
	(*table)->header = (char**) calloc(num_cols, sizeof(char*));
	for (int i = 0; i < num_cols; i++) {
		(*table)->header[i] = NULL;
	}
	(*table)->values = (char***) calloc(num_rows, sizeof(char**));
	for (int i = 0; i < num_rows; i++) {
		(*table)->values[i] = (char**) calloc(num_cols, sizeof(char*));
		for (int j = 0; j < num_cols; j++) {
			(*table)->values[i][j] = NULL;
		}
	}
	return 0;
}

void free_cld_table(cld_table* table) {
	for (size_t i = 0; i < table->num_cols; i++) {
		free(table->header[i]);
	}
	free(table->header);
	for (size_t i = 0; i < table->num_rows; i++) {
		for (size_t j = 0; j < table->num_cols; j++) {
			free(table->values[i][j]);
		}
		free(table->values[i]);
	}
	free(table->values);
	free(table);
}

int cld_table_set_header(cld_table* table, size_t col_id, char* name) {
	if (col_id >= 0 && col_id < table->num_cols) {
		table->header[col_id] = str_clone(name);
		return 0;
	} else {
		return -1;
	}
}

int cld_table_set_row_val(cld_table* table, size_t row_id, size_t col_id, char* value) {
	if (col_id >= 0 && col_id < table->num_cols && row_id >= 0
			&& row_id < table->num_rows) {
		table->values[row_id][col_id] = str_clone(value);
		return 0;
	} else {
		return -1;
	}
}

int cld_table_get_header(char** name, cld_table* table, size_t col_id) {
	if (col_id >= 0 && col_id < table->num_cols) {
		(*name) = table->header[col_id];
		return 0;
	}
	return -1;
}

int cld_table_get_row_val(char** value, cld_table* table, size_t row_id,
	size_t col_id) {
	if (col_id >= 0 && col_id < table->num_cols && row_id >= 0
			&& row_id < table->num_rows) {
		(*value) = table->values[row_id][col_id];
		return 0;
	}
	return -1;
}

