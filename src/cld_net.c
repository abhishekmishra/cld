/**
 * Copyright (c) 2020 Abhishek Mishra
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "cli_table.h"
#include "docker_all.h"
#include "cld_vol.h"

cli_cmd_err net_ls_cmd_handler(void *handler_args, arraylist *options,
		arraylist *args, cli_command_output_handler success_handler,
		cli_command_output_handler error_handler) {
	int quiet = 0;
	docker_context *ctx = get_docker_context(handler_args);
	docker_network_list* networks;

	d_err_t docker_error = docker_networks_list(ctx, &networks, NULL,
			NULL, NULL, NULL, NULL, NULL);
	if (docker_error == E_SUCCESS) {
		char res_str[1024];
		sprintf(res_str, "Listing networks");
		success_handler(CLI_COMMAND_SUCCESS, CLI_RESULT_STRING, res_str);

		size_t col_num = 0;
		size_t len_networks = docker_network_list_length(networks);
		cli_table* net_tbl;
		if (create_cli_table(&net_tbl, len_networks, 4) == 0) {
			cli_table_set_header(net_tbl, 0, "NETWORK ID");
			cli_table_set_header(net_tbl, 1, "NAME");
			cli_table_set_header(net_tbl, 2, "DRIVER");
			cli_table_set_header(net_tbl, 3, "SCOPE");
			for (size_t i = 0; i < len_networks; i++) {
				docker_network* net = (docker_network*) docker_network_list_get_idx(
						networks, i);
				cli_table_set_row_val(net_tbl, i, 0, docker_network_id_get(net));
				cli_table_set_row_val(net_tbl, i, 1, docker_network_name_get(net));
				cli_table_set_row_val(net_tbl, i, 2, docker_network_driver_get(net));
				cli_table_set_row_val(net_tbl, i, 3, docker_network_scope_get(net));
			}
			success_handler(CLI_COMMAND_SUCCESS, CLI_RESULT_TABLE, net_tbl);
		}
	} else {
		return CLI_COMMAND_ERR_UNKNOWN;
	}
	return CLI_COMMAND_SUCCESS;
}

cli_command *net_commands() {
	cli_command *image_command;
	if (make_command(&image_command, "network", "net",
			"Docker Network Commands",
			NULL) == CLI_COMMAND_SUCCESS) {
		cli_command *netcreate_command, *netls_command;
//		if (make_command(&imgpl_command, "create", "create", "Docker Volume Create",
//				&img_pl_cmd_handler) == CLI_COMMAND_SUCCESS) {
//			cld_argument* image_name_arg;
//			make_argument(&image_name_arg, "Image Name", CLD_TYPE_STRING,
//					"Name of Docker Image to be pulled.");
//			arraylist_add(imgpl_command->args, image_name_arg);
//
//			arraylist_add(image_command->sub_commands, imgpl_command);
//		}
		if (make_command(&netls_command, "list", "ls", "Docker Networks List",
				&net_ls_cmd_handler) == CLI_COMMAND_SUCCESS) {
			arraylist_add(image_command->sub_commands, netls_command);
		}
	}
	return image_command;
}
