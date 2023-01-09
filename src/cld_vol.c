/*
 *
 * Copyright (c) 2018-2022 Abhishek Mishra
 *
 * This file is part of cld.
 *
 * cld is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation,
 * either version 3 of the License, or (at your option)
 * any later version.
 *
 * cld is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with cld.
 * If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "zclk_table.h"
#include "docker_all.h"
#include "cld_vol.h"

zclk_cmd_err vol_ls_cmd_handler(zclk_command* cmd, void *handler_args)
{
	int quiet = 0;
	docker_context *ctx = get_docker_context(handler_args);
	docker_volume_list *volumes;
	docker_volume_warnings *warnings;

	d_err_t docker_error = docker_volumes_list(ctx, &volumes, &warnings, 0, NULL, NULL, NULL);
	if (docker_error == E_SUCCESS)
	{
		char res_str[1024];
		sprintf(res_str, "Listing volumes");
		cmd->success_handler(ZCLK_COMMAND_SUCCESS, ZCLK_RESULT_STRING, res_str);

		size_t col_num = 0;
		size_t len_volumes = docker_volume_list_length(volumes);
		zclk_table *vol_tbl;
		if (create_zclk_table(&vol_tbl, len_volumes, 3) == 0)
		{
			zclk_table_set_header(vol_tbl, 0, "DRIVER");
			zclk_table_set_header(vol_tbl, 1, "VOLUME NAME");
			zclk_table_set_header(vol_tbl, 2, "MOUNT");
			for (size_t i = 0; i < len_volumes; i++)
			{
				docker_volume *vol = (docker_volume *)docker_volume_list_get_idx(volumes,
																				 i);
				zclk_table_set_row_val(vol_tbl, i, 0, docker_volume_driver_get(vol));
				zclk_table_set_row_val(vol_tbl, i, 1, docker_volume_name_get(vol));
				zclk_table_set_row_val(vol_tbl, i, 2, docker_volume_mountpoint_vol_get(vol));
			}
			cmd->success_handler(ZCLK_COMMAND_SUCCESS, ZCLK_RESULT_TABLE, vol_tbl);
		}
	}
	else
	{
		return ZCLK_COMMAND_ERR_UNKNOWN;
	}
	return ZCLK_COMMAND_SUCCESS;
}

zclk_command *vol_commands()
{
	zclk_command *image_command = new_zclk_command("volume", "vol", "Docker Volume Commands", NULL);
	if(image_command != NULL)
	{
		zclk_command *volcreate_command, *volls_command;
		//		if (make_command(&imgpl_command, "create", "create", "Docker Volume Create",
		//				&img_pl_cmd_handler) == ZCLK_COMMAND_SUCCESS) {
		//			cld_argument* image_name_arg;
		//			make_argument(&image_name_arg, "Image Name", CLD_TYPE_STRING,
		//					"Name of Docker Image to be pulled.");
		//			arraylist_add(imgpl_command->args, image_name_arg);
		//
		//			arraylist_add(image_command->sub_commands, imgpl_command);
		//		}

		volls_command = new_zclk_command("list", "ls", "Docker Volumes List",
										 &vol_ls_cmd_handler);
		if(volls_command != NULL)
		{
			zclk_command_subcommand_add(image_command, volls_command);
		}
	}
	return image_command;
}
