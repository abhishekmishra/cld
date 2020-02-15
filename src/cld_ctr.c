/**
 * Copyright (c) 2020 Abhishek Mishra
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "docker_all.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "cld_ctr.h"
#include "cld_table.h"

cld_cmd_err ctr_ls_cmd_handler(void* handler_args, arraylist* options,
	arraylist* args, cld_command_output_handler success_handler,
	cld_command_output_handler error_handler) {
	bool quiet = false;
	int all = 0;

	//handle options
	size_t opt_len = arraylist_length(options);
	if (opt_len > 0) {
		for (size_t i = 0; i < opt_len; i++) {
			cld_option* o = (cld_option*)arraylist_get(options, i);
			if (o->name == CLD_OPTION_LONG_LS_ALL) {
				if (o->val->bool_value) {
					all = 1;
				}
			}
			//if (o->name == CLD_OPTION_LONG_LS_ALL) {
			//	if (o->val->bool_value) {
			//		all = 1;
			//	}
			//}
		}
	}

	docker_context* ctx = get_docker_context(handler_args);
	docker_ctr_list* containers;
	d_err_t err = docker_container_list(ctx, &containers, all, 0, 1, NULL);

	if (err == E_SUCCESS) {
		if (quiet) {
			for (size_t i = 0; i < docker_ctr_list_length(containers); i++) {
				docker_ctr_ls_item* ctr = docker_ctr_list_get_idx(
					containers, i);
				printf("%.*s\n", 12, docker_ctr_ls_item_id_get(ctr));
			}
		}
		else {
			size_t num_containers = docker_ctr_list_length(containers);
			cld_table* ctr_tbl;
			if (create_cld_table(&ctr_tbl, num_containers, 7) == 0) {
				cld_table_set_header(ctr_tbl, 0, "CONTAINER ID");
				cld_table_set_header(ctr_tbl, 1, "IMAGE");
				cld_table_set_header(ctr_tbl, 2, "COMMAND");
				cld_table_set_header(ctr_tbl, 3, "CREATED");
				cld_table_set_header(ctr_tbl, 4, "STATUS");
				cld_table_set_header(ctr_tbl, 5, "PORTS");
				cld_table_set_header(ctr_tbl, 6, "NAMES");

				for (size_t i = 0; i < docker_ctr_list_length(containers);
					i++) {
					docker_ctr_ls_item* ctr =
						docker_ctr_list_get_idx(containers, i);

					//get ports
					char ports_str[1024];
					ports_str[0] = '\0';
					for (int j = 0; j < docker_ctr_ls_item_ports_length(ctr); j++) {
						char port_str[100];
						docker_ctr_port* ports = docker_ctr_ls_item_ports_get_idx(ctr, 0);
						sprintf(port_str, "%ld:%ld", docker_ctr_port_private_port_get(ports),
							docker_ctr_port_public_port_get(ports));
						if (j == 0) {
							strcpy(ports_str, port_str);
						}
						else {
							strcat(ports_str, ", ");
							strcat(ports_str, port_str);
						}
					}

					//get created time
					time_t t = (time_t)docker_ctr_ls_item_created_get(ctr);
					struct tm* timeinfo = localtime(&t);
					char evt_time_str[256];
					strftime(evt_time_str, 255, "%d-%m-%Y:%H:%M:%S", timeinfo);

					//get names
					char names[1024];
					names[0] = '\0';
					size_t names_len = docker_ctr_ls_item_names_length(ctr);
					for (size_t j = 0; j < names_len; j++) {
						if (j == 0) {
							strcpy(names, docker_ctr_ls_item_names_get_idx(ctr, j));
						}
						else {
							strcat(names, ",");
							strcat(names, docker_ctr_ls_item_names_get_idx(ctr, j));
						}
					}
					cld_table_set_row_val(ctr_tbl, i, 0, docker_ctr_ls_item_id_get(ctr));
					cld_table_set_row_val(ctr_tbl, i, 1, docker_ctr_ls_item_image_get(ctr));
					cld_table_set_row_val(ctr_tbl, i, 2, docker_ctr_ls_item_command_get(ctr));
					cld_table_set_row_val(ctr_tbl, i, 3, evt_time_str);
					cld_table_set_row_val(ctr_tbl, i, 4, docker_ctr_ls_item_status_get(ctr));
					cld_table_set_row_val(ctr_tbl, i, 5, ports_str);
					cld_table_set_row_val(ctr_tbl, i, 6, names);
				}
				success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_TABLE, ctr_tbl);
				free_cld_table(ctr_tbl);
			}
		}

		free_docker_ctr_list(containers);
	}
	return CLD_COMMAND_SUCCESS;
}

cld_cmd_err ctr_create_cmd_handler(void* handler_args,
	arraylist* options, arraylist* args,
	cld_command_output_handler success_handler,
	cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_context* ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(args);
	if (len != 1) {
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
			"Image name not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	}
	else {
		cld_argument* image_name_arg = (cld_argument*)arraylist_get(args,
			0);
		char* image_name = image_name_arg->val->str_value;
		char* id = NULL;
		docker_ctr_create_params* p = make_docker_ctr_create_params();
		docker_ctr_create_params_image_set(p, image_name);
		d_err_t e = docker_create_container(ctx, &id, p);
		if (e == E_SUCCESS) {
			if (id != NULL) {
				char res_str[1024];
				sprintf(res_str, "Created container with id %s", id);
				success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING,
					res_str);
			}
		}
	}
	return CLD_COMMAND_SUCCESS;
}

cld_cmd_err ctr_start_cmd_handler(void* handler_args,
	arraylist* options, arraylist* args,
	cld_command_output_handler success_handler,
	cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_context* ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(args);
	if (len != 1) {
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
			"Container not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	}
	else {
		cld_argument* container_arg = (cld_argument*)arraylist_get(args,
			0);
		char* container = container_arg->val->str_value;
		d_err_t e = docker_start_container(ctx, container, NULL);
		if (e == E_SUCCESS) {
			char res_str[1024];
			sprintf(res_str, "Started container %s", container);
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, res_str);
		}
	}
	return CLD_COMMAND_SUCCESS;
}

cld_cmd_err ctr_stop_cmd_handler(void* handler_args, arraylist* options,
	arraylist* args, cld_command_output_handler success_handler,
	cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_context* ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(args);
	if (len != 1) {
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
			"Container not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	}
	else {
		cld_argument* container_arg = (cld_argument*)arraylist_get(args,
			0);
		char* container = container_arg->val->str_value;
		d_err_t e = docker_stop_container(ctx, container, 0);
		if (e == E_SUCCESS) {
			char res_str[1024];
			sprintf(res_str, "Stopped container %s", container);
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, res_str);
		}
	}
	return CLD_COMMAND_SUCCESS;
}

cld_cmd_err ctr_restart_cmd_handler(void* handler_args,
	arraylist* options, arraylist* args,
	cld_command_output_handler success_handler,
	cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_context* ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(args);
	if (len != 1) {
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
			"Container not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	}
	else {
		cld_argument* container_arg = (cld_argument*)arraylist_get(args,
			0);
		char* container = container_arg->val->str_value;
		d_err_t e = docker_restart_container(ctx, container, 0);
		if (e == E_SUCCESS) {
			char res_str[1024];
			sprintf(res_str, "Restarted container %s", container);
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, res_str);
		}
	}
	return CLD_COMMAND_SUCCESS;
}

cld_cmd_err ctr_kill_cmd_handler(void* handler_args, arraylist* options,
	arraylist* args, cld_command_output_handler success_handler,
	cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_context* ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(args);
	if (len != 1) {
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
			"Container not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	}
	else {
		cld_argument* container_arg = (cld_argument*)arraylist_get(args,
			0);
		char* container = container_arg->val->str_value;
		d_err_t e = docker_kill_container(ctx, container, NULL);
		if (e == E_SUCCESS) {
			char res_str[1024];
			sprintf(res_str, "Killed container %s", container);
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, res_str);
		}
	}
	return CLD_COMMAND_SUCCESS;
}

cld_cmd_err ctr_ren_cmd_handler(void* handler_args, arraylist* options,
	arraylist* args, cld_command_output_handler success_handler,
	cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_context* ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(args);
	if (len != 2) {
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
			"Container name and new name not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	}
	else {
		cld_argument* container_arg = (cld_argument*)arraylist_get(args,
			0);
		char* container = container_arg->val->str_value;
		char* new_name =
			((cld_argument*)arraylist_get(args, 1))->val->str_value;
		d_err_t e = docker_rename_container(ctx, container, new_name);
		if (e == E_SUCCESS) {
			char res_str[1024];
			sprintf(res_str, "Renamed container %s to %s", container, new_name);
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, res_str);
		}
	}
	return CLD_COMMAND_SUCCESS;
}

cld_cmd_err ctr_pause_cmd_handler(void* handler_args,
	arraylist* options, arraylist* args,
	cld_command_output_handler success_handler,
	cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_context* ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(args);
	if (len != 1) {
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
			"Container not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	}
	else {
		cld_argument* container_arg = (cld_argument*)arraylist_get(args,
			0);
		char* container = container_arg->val->str_value;
		d_err_t e = docker_pause_container(ctx, container);
		if (e == E_SUCCESS) {
			char res_str[1024];
			sprintf(res_str, "Paused container %s", container);
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, res_str);
		}
	}
	return CLD_COMMAND_SUCCESS;
}

cld_cmd_err ctr_unpause_cmd_handler(void* handler_args,
	arraylist* options, arraylist* args,
	cld_command_output_handler success_handler,
	cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_context* ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(args);
	if (len != 1) {
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
			"Container not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	}
	else {
		cld_argument* container_arg = (cld_argument*)arraylist_get(args,
			0);
		char* container = container_arg->val->str_value;
		d_err_t e = docker_unpause_container(ctx, container);
		if (e == E_SUCCESS) {
			char res_str[1024];
			sprintf(res_str, "UnPaused container %s", container);
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, res_str);
		}
	}
	return CLD_COMMAND_SUCCESS;
}

cld_cmd_err ctr_wait_cmd_handler(void* handler_args, arraylist* options,
	arraylist* args, cld_command_output_handler success_handler,
	cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_context* ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(args);
	if (len != 1) {
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
			"Container not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	}
	else {
		cld_argument* container_arg = (cld_argument*)arraylist_get(args,
			0);
		char* container = container_arg->val->str_value;
		d_err_t e = docker_wait_container(ctx, container, NULL);
		if (e == E_SUCCESS) {
			char res_str[1024];
			sprintf(res_str, "Waiting for container %s", container);
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, res_str);
		}
	}
	return CLD_COMMAND_SUCCESS;
}

void cld_log_line_handler(void* args, int stream_id, int line_num, char* line) {
	cld_command_output_handler success_handler = (cld_command_output_handler)args;
	docker_log_info("Stream %d, line# %d :: %s", stream_id, line_num, line);
	success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, line);
}

cld_cmd_err ctr_logs_cmd_handler(void* handler_args, arraylist* options,
	arraylist* args, cld_command_output_handler success_handler,
	cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_context* ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(args);
	if (len != 1) {
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
			"Container not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	}
	else {
		cld_argument* container_arg = (cld_argument*)arraylist_get(args,
			0);
		char* container = container_arg->val->str_value;
		char* log;
		size_t log_len;
		d_err_t e = docker_container_logs(ctx, &log, &log_len, container, 0, 1, 1, -1, -1, 1,
			10);
		if (e == E_SUCCESS) {
			char res_str[1024];
			sprintf(res_str, "Logs for container %s", container);
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, res_str);
			//success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, log);
			docker_container_logs_foreach(success_handler, log, log_len, &cld_log_line_handler);
		}
	}
	return CLD_COMMAND_SUCCESS;
}

cld_cmd_err ctr_top_cmd_handler(void* handler_args, arraylist* options,
	arraylist* args, cld_command_output_handler success_handler,
	cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_context* ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(args);
	if (len != 1) {
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
			"Container not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	}
	else {
		cld_argument* container_arg = (cld_argument*)arraylist_get(args,
			0);
		char* container = container_arg->val->str_value;
		docker_container_ps* ps;
		d_err_t e = docker_process_list_container(ctx, &ps, container, NULL);
		if (e == E_SUCCESS) {
			char res_str[1024];
			sprintf(res_str, "Process list for container %s", container);
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, res_str);

			cld_table* ctr_tbl;
			size_t num_labels = arraylist_length(ps->titles);
			size_t num_processes = arraylist_length(ps->processes);
			if (create_cld_table(&ctr_tbl, num_processes, num_labels) == 0) {
				for (size_t i = 0; i < num_labels; i++) {
					cld_table_set_header(ctr_tbl, i,
						(char*)arraylist_get(ps->titles, i));
				}
				for (size_t i = 0; i < num_processes; i++) {
					arraylist* psvals = (arraylist*)arraylist_get(
						ps->processes, i);
					for (size_t j = 0; j < num_labels; j++) {
						cld_table_set_row_val(ctr_tbl, i, j,
							(char*)arraylist_get(psvals, j));
					}
				}
				success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_TABLE, ctr_tbl);
			}
		}
	}
	return CLD_COMMAND_SUCCESS;
}

cld_cmd_err ctr_remove_cmd_handler(void* handler_args,
	arraylist* options, arraylist* args,
	cld_command_output_handler success_handler,
	cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_context* ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(args);
	if (len != 1) {
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
			"Container not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	}
	else {
		cld_argument* container_arg = (cld_argument*)arraylist_get(args,
			0);
		char* container = container_arg->val->str_value;
		d_err_t e = docker_remove_container(ctx, container, 0, 0, 0);
		if (e == E_SUCCESS) {
			char res_str[1024];
			sprintf(res_str, "Removed container %s", container);
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, res_str);
		}
	}
	return CLD_COMMAND_SUCCESS;
}

typedef struct stats_args_t {
	char* id;
	char* name;
	cld_command_output_handler success_handler;
} stats_args;

void docker_container_stats_cb(docker_container_stats* stats, void* cbargs) {
	stats_args* sarg = (stats_args*)cbargs;
	cld_table* ctr_tbl;
	if (create_cld_table(&ctr_tbl, 1, 8) == 0) {
		cld_table_set_header(ctr_tbl, 0, "CONTAINER ID");
		cld_table_set_header(ctr_tbl, 1, "NAME");
		cld_table_set_header(ctr_tbl, 2, "CPU %");
		cld_table_set_header(ctr_tbl, 3, "MEM USAGE / LIMIT");
		cld_table_set_header(ctr_tbl, 4, "MEM %");
		cld_table_set_header(ctr_tbl, 5, "NET I/O");
		cld_table_set_header(ctr_tbl, 6, "BLOCK I/O");
		cld_table_set_header(ctr_tbl, 7, "PIDS");

		cld_table_set_row_val(ctr_tbl, 0, 0, sarg->id);
		cld_table_set_row_val(ctr_tbl, 0, 1, sarg->name);

		char cpu_usg_str[128];
		sprintf(cpu_usg_str, "%ld", docker_container_cpu_stats_cpu_usage_get(docker_container_stats_cpu_stats_get(stats)));
		cld_table_set_row_val(ctr_tbl, 0, 2, cpu_usg_str);

		char mem_usg_str[128];
		sprintf(mem_usg_str, "%ld", docker_container_mem_stats_usage_get(docker_container_stats_mem_stats_get(stats)));
		cld_table_set_row_val(ctr_tbl, 0, 3, mem_usg_str);

		cld_table_set_row_val(ctr_tbl, 0, 4, "");
		cld_table_set_row_val(ctr_tbl, 0, 5, "");
		cld_table_set_row_val(ctr_tbl, 0, 6, "");
		cld_table_set_row_val(ctr_tbl, 0, 7, "");
	}
	sarg->success_handler(CLD_COMMAND_IS_RUNNING, CLD_RESULT_STRING,
		"\033[0;0H\033[2J");
	sarg->success_handler(CLD_COMMAND_IS_RUNNING, CLD_RESULT_TABLE, ctr_tbl);
}

cld_cmd_err ctr_stats_cmd_handler(void* handler_args,
	arraylist* options, arraylist* args,
	cld_command_output_handler success_handler,
	cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_context* ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(args);
	if (len != 1) {
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
			"Container not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	}
	else {
		cld_argument* container_arg = (cld_argument*)arraylist_get(args,
			0);
		char* container = container_arg->val->str_value;
		stats_args* sarg = (stats_args*)calloc(1, sizeof(stats_args));
		sarg->id = container;
		sarg->name = container;
		sarg->success_handler = success_handler;
		d_err_t e = docker_container_get_stats_cb(ctx, &docker_container_stats_cb,
			sarg, container);
		if (e == E_SUCCESS) {
			char res_str[1024];
			sprintf(res_str, "done %s", container);
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, res_str);
		}
	}
	return CLD_COMMAND_SUCCESS;
}

cld_command* ctr_commands() {
	cld_command* container_command;
	if (make_command(&container_command, "container", "ctr",
		"Docker Container Commands",
		NULL) == CLD_COMMAND_SUCCESS) {
		cld_command* ctrls_command, * ctrcreate_command, * ctrstart_command,
			* ctrstop_command, * ctrrestart_command, * ctrkill_command,
			* ctrren_command, * ctrpause_command, * ctrunpause_command,
			* ctrwait_command, * ctrlogs_command, * ctrtop_command,
			* ctrremove_command, * ctrstats_command;
		if (make_command(&ctrls_command, "list", "ls", "Docker Container List",
			&ctr_ls_cmd_handler) == CLD_COMMAND_SUCCESS) {
			cld_option* all_option, * filter_option, * format_option,
				* last_option, * latest_option, * notrunc_option,
				* quiet_option, * size_option;
			make_option(&all_option, CLD_OPTION_LONG_LS_ALL,
				CLD_OPTION_SHORT_LS_ALL, CLD_TYPE_FLAG, "Show all containers (by default shows only running ones).");
			arraylist_add(ctrls_command->options, all_option);
			arraylist_add(container_command->sub_commands, ctrls_command);
		}
		if (make_command(&ctrcreate_command, "create", "create",
			"Docker Container Create", &ctr_create_cmd_handler)
			== CLD_COMMAND_SUCCESS) {
			cld_argument* image_name_arg;
			make_argument(&image_name_arg, "Image Name", CLD_TYPE_STRING,
				"Name of Docker Image to use.");
			arraylist_add(ctrcreate_command->args, image_name_arg);

			arraylist_add(container_command->sub_commands, ctrcreate_command);
		}
		if (make_command(&ctrstart_command, "start", "on",
			"Docker Container Start", &ctr_start_cmd_handler)
			== CLD_COMMAND_SUCCESS) {
			cld_argument* container_arg;
			make_argument(&container_arg, "Container", CLD_TYPE_STRING,
				"Name of container to start.");
			arraylist_add(ctrstart_command->args, container_arg);

			arraylist_add(container_command->sub_commands, ctrstart_command);
		}
		if (make_command(&ctrstop_command, "stop", "off",
			"Docker Container Stop", &ctr_stop_cmd_handler)
			== CLD_COMMAND_SUCCESS) {
			cld_argument* container_arg;
			make_argument(&container_arg, "Container", CLD_TYPE_STRING,
				"Name of container to stop.");
			arraylist_add(ctrstop_command->args, container_arg);

			arraylist_add(container_command->sub_commands, ctrstop_command);
		}
		if (make_command(&ctrrestart_command, "restart", "restart",
			"Docker Container Restart", &ctr_restart_cmd_handler)
			== CLD_COMMAND_SUCCESS) {
			cld_argument* container_arg;
			make_argument(&container_arg, "Container", CLD_TYPE_STRING,
				"Name of container to restart.");
			arraylist_add(ctrrestart_command->args, container_arg);

			arraylist_add(container_command->sub_commands, ctrrestart_command);
		}
		if (make_command(&ctrkill_command, "kill", "kill",
			"Docker Container Kill", &ctr_kill_cmd_handler)
			== CLD_COMMAND_SUCCESS) {
			cld_argument* container_arg;
			make_argument(&container_arg, "Container", CLD_TYPE_STRING,
				"Name of container to kill.");
			arraylist_add(ctrkill_command->args, container_arg);

			arraylist_add(container_command->sub_commands, ctrkill_command);
		}
		if (make_command(&ctrren_command, "rename", "ren",
			"Docker Container Rename", &ctr_ren_cmd_handler)
			== CLD_COMMAND_SUCCESS) {
			cld_argument* container_arg;
			make_argument(&container_arg, "Container", CLD_TYPE_STRING,
				"Name of container to rename.");
			arraylist_add(ctrren_command->args, container_arg);

			cld_argument* name_arg;
			make_argument(&name_arg, "Name", CLD_TYPE_STRING,
				"New name of container.");
			arraylist_add(ctrren_command->args, name_arg);

			arraylist_add(container_command->sub_commands, ctrren_command);
		}
		if (make_command(&ctrpause_command, "pause", "pause",
			"Docker Container Pause", &ctr_pause_cmd_handler)
			== CLD_COMMAND_SUCCESS) {
			cld_argument* container_arg;
			make_argument(&container_arg, "Container", CLD_TYPE_STRING,
				"Name of container to pause.");
			arraylist_add(ctrpause_command->args, container_arg);

			arraylist_add(container_command->sub_commands, ctrpause_command);
		}
		if (make_command(&ctrunpause_command, "unpause", "unpause",
			"Docker Container UnPause", &ctr_unpause_cmd_handler)
			== CLD_COMMAND_SUCCESS) {
			cld_argument* container_arg;
			make_argument(&container_arg, "Container", CLD_TYPE_STRING,
				"Name of container to unpause.");
			arraylist_add(ctrunpause_command->args, container_arg);

			arraylist_add(container_command->sub_commands, ctrunpause_command);
		}
		if (make_command(&ctrwait_command, "wait", "wait",
			"Docker Container Wait", &ctr_wait_cmd_handler)
			== CLD_COMMAND_SUCCESS) {
			cld_argument* container_arg;
			make_argument(&container_arg, "Container", CLD_TYPE_STRING,
				"Name of container to wait.");
			arraylist_add(ctrwait_command->args, container_arg);

			arraylist_add(container_command->sub_commands, ctrwait_command);
		}
		if (make_command(&ctrlogs_command, "logs", "lg",
			"Docker Container Logs", &ctr_logs_cmd_handler)
			== CLD_COMMAND_SUCCESS) {
			cld_argument* container_arg;
			make_argument(&container_arg, "Container", CLD_TYPE_STRING,
				"Name of container.");
			arraylist_add(ctrlogs_command->args, container_arg);

			arraylist_add(container_command->sub_commands, ctrlogs_command);
		}
		if (make_command(&ctrtop_command, "top", "ps",
			"Docker Container Top/PS", &ctr_top_cmd_handler)
			== CLD_COMMAND_SUCCESS) {
			cld_argument* container_arg;
			make_argument(&container_arg, "Container", CLD_TYPE_STRING,
				"Name of container.");
			arraylist_add(ctrtop_command->args, container_arg);

			arraylist_add(container_command->sub_commands, ctrtop_command);
		}
		if (make_command(&ctrremove_command, "remove", "del",
			"Docker Remove Container", &ctr_remove_cmd_handler)
			== CLD_COMMAND_SUCCESS) {
			cld_argument* container_arg;
			make_argument(&container_arg, "Container", CLD_TYPE_STRING,
				"Name of container.");
			arraylist_add(ctrremove_command->args, container_arg);

			arraylist_add(container_command->sub_commands, ctrremove_command);
		}
		if (make_command(&ctrstats_command, "stats", "stats",
			"Docker Container Stats", &ctr_stats_cmd_handler)
			== CLD_COMMAND_SUCCESS) {
			cld_argument* container_arg;
			make_argument(&container_arg, "Container", CLD_TYPE_STRING,
				"Name of container.");
			arraylist_add(ctrstats_command->args, container_arg);

			arraylist_add(container_command->sub_commands, ctrstats_command);
		}
	}
	return container_command;
}
