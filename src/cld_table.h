// Copyright (c) 2020 Abhishek Mishra
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef SRC_CLD_TABLE_H_
#define SRC_CLD_TABLE_H_

#include <stdlib.h>

typedef struct cld_table_t {
	size_t num_rows;
	size_t num_cols;
	char** header;
	char*** values;
} cld_table;

int create_cld_table(cld_table** table, size_t num_rows, size_t num_cols);

void free_cld_table(cld_table* table);

int cld_table_set_header(cld_table* table, size_t col_id, char* name);

int cld_table_set_row_val(cld_table* table, size_t row_id, size_t col_id,
		char* value);

int cld_table_get_header(char** name, cld_table* table, size_t col_id);

int cld_table_get_row_val(char** value, cld_table* table, size_t row_id,
	size_t col_id);

#endif /* SRC_CLD_TABLE_H_ */
