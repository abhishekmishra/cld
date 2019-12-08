/*
 * clibdocker: cld_net.c
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

cld_cmd_err net_ls_cmd_handler(void *handler_args, arraylist *options,
		arraylist *args, cld_command_output_handler success_handler,
		cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_context *ctx = get_docker_context(handler_args);
	docker_network_list* networks;

	d_err_t docker_error = docker_networks_list(ctx, &networks, NULL,
			NULL, NULL, NULL, NULL, NULL);
	if (docker_error == E_SUCCESS) {
		char res_str[1024];
		sprintf(res_str, "Listing networks");
		success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, res_str);

		size_t col_num = 0;
		size_t len_networks = docker_network_list_length(networks);
		cld_table* net_tbl;
		if (create_cld_table(&net_tbl, len_networks, 4) == 0) {
			cld_table_set_header(net_tbl, 0, "NETWORK ID");
			cld_table_set_header(net_tbl, 1, "NAME");
			cld_table_set_header(net_tbl, 2, "DRIVER");
			cld_table_set_header(net_tbl, 3, "SCOPE");
			for (size_t i = 0; i < len_networks; i++) {
				docker_network* net = (docker_network*) docker_network_list_get_idx(
						networks, i);
				cld_table_set_row_val(net_tbl, i, 0, docker_network_id_get(net));
				cld_table_set_row_val(net_tbl, i, 1, docker_network_name_get(net));
				cld_table_set_row_val(net_tbl, i, 2, docker_network_driver_get(net));
				cld_table_set_row_val(net_tbl, i, 3, docker_network_scope_get(net));
			}
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_TABLE, net_tbl);
		}
	} else {
		return CLD_COMMAND_ERR_UNKNOWN;
	}
	return CLD_COMMAND_SUCCESS;
}

cld_command *net_commands() {
	cld_command *image_command;
	if (make_command(&image_command, "network", "net",
			"Docker Network Commands",
			NULL) == CLD_COMMAND_SUCCESS) {
		cld_command *netcreate_command, *netls_command;
//		if (make_command(&imgpl_command, "create", "create", "Docker Volume Create",
//				&img_pl_cmd_handler) == CLD_COMMAND_SUCCESS) {
//			cld_argument* image_name_arg;
//			make_argument(&image_name_arg, "Image Name", CLD_TYPE_STRING,
//					"Name of Docker Image to be pulled.");
//			arraylist_add(imgpl_command->args, image_name_arg);
//
//			arraylist_add(image_command->sub_commands, imgpl_command);
//		}
		if (make_command(&netls_command, "list", "ls", "Docker Networks List",
				&net_ls_cmd_handler) == CLD_COMMAND_SUCCESS) {
			arraylist_add(image_command->sub_commands, netls_command);
		}
	}
	return image_command;
}
