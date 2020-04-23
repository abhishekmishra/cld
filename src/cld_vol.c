/**
 * Copyright (c) 2020 Abhishek Mishra
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "cli_table.h"
#include "docker_all.h"
#include "cld_vol.h"

cli_cmd_err vol_ls_cmd_handler(void *handler_args, arraylist *options,
		arraylist *args, cli_command_output_handler success_handler,
		cli_command_output_handler error_handler) {
	int quiet = 0;
	docker_context *ctx = get_docker_context(handler_args);
	docker_volume_list* volumes;
	docker_volume_warnings* warnings;

	d_err_t docker_error = docker_volumes_list(ctx, &volumes, &warnings, 0, NULL, NULL, NULL);
	if (docker_error == E_SUCCESS) {
		char res_str[1024];
		sprintf(res_str, "Listing volumes");
		success_handler(CLI_COMMAND_SUCCESS, CLI_RESULT_STRING, res_str);

		size_t col_num = 0;
		size_t len_volumes = docker_volume_list_length(volumes);
		cli_table* vol_tbl;
		if (create_cli_table(&vol_tbl, len_volumes, 3) == 0) {
			cli_table_set_header(vol_tbl, 0, "DRIVER");
			cli_table_set_header(vol_tbl, 1, "VOLUME NAME");
			cli_table_set_header(vol_tbl, 2, "MOUNT");
			for (size_t i = 0; i < len_volumes; i++) {
				docker_volume* vol = (docker_volume*)docker_volume_list_get_idx(volumes,
						i);
				cli_table_set_row_val(vol_tbl, i, 0, docker_volume_driver_get(vol));
				cli_table_set_row_val(vol_tbl, i, 1, docker_volume_name_get(vol));
				cli_table_set_row_val(vol_tbl, i, 2, docker_volume_mountpoint_vol_get(vol));
			}
			success_handler(CLI_COMMAND_SUCCESS, CLI_RESULT_TABLE, vol_tbl);
		}
	} else {
		return CLI_COMMAND_ERR_UNKNOWN;
	}
	return CLI_COMMAND_SUCCESS;
}


cli_command *vol_commands() {
	cli_command *image_command;
	if (make_command(&image_command, "volume", "vol", "Docker Volume Commands",
	NULL) == CLI_COMMAND_SUCCESS) {
		cli_command *volcreate_command, *volls_command;
//		if (make_command(&imgpl_command, "create", "create", "Docker Volume Create",
//				&img_pl_cmd_handler) == CLI_COMMAND_SUCCESS) {
//			cld_argument* image_name_arg;
//			make_argument(&image_name_arg, "Image Name", CLD_TYPE_STRING,
//					"Name of Docker Image to be pulled.");
//			arraylist_add(imgpl_command->args, image_name_arg);
//
//			arraylist_add(image_command->sub_commands, imgpl_command);
//		}
		if (make_command(&volls_command, "list", "ls", "Docker Volumes List",
				&vol_ls_cmd_handler) == CLI_COMMAND_SUCCESS) {
			arraylist_add(image_command->sub_commands, volls_command);
		}
	}
	return image_command;
}
