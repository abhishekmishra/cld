/*
* clibdocker: cld_vol.c
* Created on: 03-Apr-2019
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

#include "cld_table.h"
#include "docker_all.h"
#include "cld_vol.h"

cld_cmd_err vol_ls_cmd_handler(void *handler_args, struct array_list *options,
		struct array_list *args, cld_command_output_handler success_handler,
		cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_result *res;
	docker_context *ctx = get_docker_context(handler_args);
	array_list* volumes;
	array_list* warnings;

	d_err_t docker_error = docker_volumes_list(ctx, &res, &volumes, &warnings, 0, NULL, NULL, NULL);
	int success = is_ok(res);
	handle_docker_error(res, success_handler, error_handler);
	if (success) {
		char res_str[1024];
		sprintf(res_str, "Listing volumes");
		success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, res_str);

		int col_num = 0;
		int len_volumes = array_list_length(volumes);
		cld_table* vol_tbl;
		if (create_cld_table(&vol_tbl, len_volumes, 3) == 0) {
			cld_table_set_header(vol_tbl, 0, "DRIVER");
			cld_table_set_header(vol_tbl, 1, "VOLUME NAME");
			cld_table_set_header(vol_tbl, 2, "MOUNT");
			for (int i = 0; i < len_volumes; i++) {
				docker_volume* vol = (docker_volume*) array_list_get_idx(volumes,
						i);
				cld_table_set_row_val(vol_tbl, i, 0,vol->driver);
				cld_table_set_row_val(vol_tbl, i, 1,vol->name);
				cld_table_set_row_val(vol_tbl, i, 2,vol->mountpoint);
			}
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_TABLE, vol_tbl);
		}
	} else {
		return CLD_COMMAND_ERR_UNKNOWN;
	}
	return CLD_COMMAND_SUCCESS;
}


cld_command *vol_commands() {
	cld_command *image_command;
	if (make_command(&image_command, "volume", "vol", "Docker Volume Commands",
	NULL) == CLD_COMMAND_SUCCESS) {
		cld_command *volcreate_command, *volls_command;
//		if (make_command(&imgpl_command, "create", "create", "Docker Volume Create",
//				&img_pl_cmd_handler) == CLD_COMMAND_SUCCESS) {
//			cld_argument* image_name_arg;
//			make_argument(&image_name_arg, "Image Name", CLD_TYPE_STRING,
//					"Name of Docker Image to be pulled.");
//			array_list_add(imgpl_command->args, image_name_arg);
//
//			array_list_add(image_command->sub_commands, imgpl_command);
//		}
		if (make_command(&volls_command, "list", "ls", "Docker Volumes List",
				&vol_ls_cmd_handler) == CLD_COMMAND_SUCCESS) {
			array_list_add(image_command->sub_commands, volls_command);
		}
	}
	return image_command;
}
