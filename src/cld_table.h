/*
 * clibdocker: cld_table.h
 * Created on: 18-Mar-2019
 *
 * clibdocker
 * Copyright (C) 2018 Abhishek Mishra <abhishekmishra3@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SRC_CLD_TABLE_H_
#define SRC_CLD_TABLE_H_

typedef struct cld_table_t {
	int num_rows;
	int num_cols;
	char** header;
	char*** values;
} cld_table;

int create_cld_table(cld_table** table, int num_rows, int num_cols);

void free_cld_table(cld_table* table);

int cld_table_set_header(cld_table* table, int col_id, char* name);

int cld_table_set_row_val(cld_table* table, int row_id, int col_id,
		char* value);

int cld_table_get_header(char** name, cld_table* table, int col_id);

int cld_table_get_row_val(char** value, cld_table* table, int row_id,
		int col_id);

#endif /* SRC_CLD_TABLE_H_ */
