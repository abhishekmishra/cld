#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "cld_ctr.h"
#include "docker_all.h"
#include "cld_table.h"

cld_cmd_err ctr_ls_cmd_handler(void *handler_args, struct array_list *options,
		struct array_list *args, cld_command_output_handler success_handler,
		cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_result *res;
	docker_context *ctx = get_docker_context(handler_args);
	docker_containers_list *containers;
	docker_container_list(ctx, &res, &containers, 1, 0, 1, NULL);
	handle_docker_error(res, success_handler, error_handler);

	if (quiet) {
		for (int i = 0; i < docker_containers_list_length(containers); i++) {
			docker_container_list_item *ctr = docker_containers_list_get_idx(
					containers, i);
			printf("%.*s\n", 12, ctr->id);
		}
	} else {
		int num_containers = docker_containers_list_length(containers);
		cld_table* ctr_tbl;
		if (create_cld_table(&ctr_tbl, num_containers, 7) == 0) {
			cld_table_set_header(ctr_tbl, 0, "CONTAINER ID");
			cld_table_set_header(ctr_tbl, 1, "IMAGE");
			cld_table_set_header(ctr_tbl, 2, "COMMAND");
			cld_table_set_header(ctr_tbl, 3, "CREATED");
			cld_table_set_header(ctr_tbl, 4, "STATUS");
			cld_table_set_header(ctr_tbl, 5, "PORTS");
			cld_table_set_header(ctr_tbl, 6, "NAMES");

			for (int i = 0; i < docker_containers_list_length(containers);
					i++) {
				docker_container_list_item *ctr =
						docker_containers_list_get_idx(containers, i);

				//get ports
				char ports_str[1024];
				ports_str[0] = '\0';
				for (int j = 0; j < array_list_length(ctr->ports); j++) {
					char port_str[100];
					docker_container_ports *ports = array_list_get_idx(
							ctr->ports, 0);
					sprintf(port_str, "%ld:%ld", ports->private_port,
							ports->public_port);
					if (j == 0) {
						strcpy(ports_str, port_str);
					} else {
						strcat(ports_str, ", ");
						strcat(ports_str, port_str);
					}
				}

				//get created time
				time_t t = (time_t) ctr->created;
				struct tm *timeinfo = localtime(&t);
				char evt_time_str[256];
				strftime(evt_time_str, 255, "%d-%m-%Y:%H:%M:%S", timeinfo);

				//get names
				char names[1024];
				names[0] = '\0';
				for (int j = 0; j < array_list_length(ctr->names); j++) {
					if (j == 0) {
						strcpy(names, array_list_get_idx(ctr->names, j));
					} else {
						strcat(names, ",");
						strcat(names, array_list_get_idx(ctr->names, j));
					}
				}
				cld_table_set_row_val(ctr_tbl, i, 0, ctr->id);
				cld_table_set_row_val(ctr_tbl, i, 1, ctr->image);
				cld_table_set_row_val(ctr_tbl, i, 2, ctr->command);
				cld_table_set_row_val(ctr_tbl, i, 3, evt_time_str);
				cld_table_set_row_val(ctr_tbl, i, 4, ctr->status);
				cld_table_set_row_val(ctr_tbl, i, 5, ports_str);
				cld_table_set_row_val(ctr_tbl, i, 6, names);
			}
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_TABLE, ctr_tbl);
			free_cld_table(ctr_tbl);
		}
	}

	array_list_free(containers);
	return CLD_COMMAND_SUCCESS;
}

cld_cmd_err ctr_create_cmd_handler(void *handler_args,
		struct array_list *options, struct array_list *args,
		cld_command_output_handler success_handler,
		cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_result *res;
	docker_context *ctx = get_docker_context(handler_args);
	int len = array_list_length(args);
	if (len != 1) {
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
				"Image name not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	} else {
		cld_argument* image_name_arg = (cld_argument*) array_list_get_idx(args,
				0);
		char* image_name = image_name_arg->val->str_value;
		char* id = NULL;
		docker_create_container_params* p;
		make_docker_create_container_params(&p);
		p->image = image_name;
		docker_create_container(ctx, &res, &id, p);
		int created = is_ok(res);
		handle_docker_error(res, success_handler, error_handler);
		if (created) {
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

cld_cmd_err ctr_start_cmd_handler(void *handler_args,
		struct array_list *options, struct array_list *args,
		cld_command_output_handler success_handler,
		cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_result *res;
	docker_context *ctx = get_docker_context(handler_args);
	int len = array_list_length(args);
	if (len != 1) {
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
				"Container not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	} else {
		cld_argument* container_arg = (cld_argument*) array_list_get_idx(args,
				0);
		char* container = container_arg->val->str_value;
		docker_start_container(ctx, &res, container, NULL);
		int created = is_ok(res);
		handle_docker_error(res, success_handler, error_handler);
		if (created) {
			char res_str[1024];
			sprintf(res_str, "Started container %s", container);
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, res_str);
		}
	}
	return CLD_COMMAND_SUCCESS;
}

cld_cmd_err ctr_stop_cmd_handler(void *handler_args, struct array_list *options,
		struct array_list *args, cld_command_output_handler success_handler,
		cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_result *res;
	docker_context *ctx = get_docker_context(handler_args);
	int len = array_list_length(args);
	if (len != 1) {
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
				"Container not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	} else {
		cld_argument* container_arg = (cld_argument*) array_list_get_idx(args,
				0);
		char* container = container_arg->val->str_value;
		docker_stop_container(ctx, &res, container, 0);
		int created = is_ok(res);
		handle_docker_error(res, success_handler, error_handler);
		if (created) {
			char res_str[1024];
			sprintf(res_str, "Stopped container %s", container);
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, res_str);
		}
	}
	return CLD_COMMAND_SUCCESS;
}

cld_cmd_err ctr_restart_cmd_handler(void *handler_args,
		struct array_list *options, struct array_list *args,
		cld_command_output_handler success_handler,
		cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_result *res;
	docker_context *ctx = get_docker_context(handler_args);
	int len = array_list_length(args);
	if (len != 1) {
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
				"Container not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	} else {
		cld_argument* container_arg = (cld_argument*) array_list_get_idx(args,
				0);
		char* container = container_arg->val->str_value;
		docker_restart_container(ctx, &res, container, 0);
		int created = is_ok(res);
		handle_docker_error(res, success_handler, error_handler);
		if (created) {
			char res_str[1024];
			sprintf(res_str, "Restarted container %s", container);
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, res_str);
		}
	}
	return CLD_COMMAND_SUCCESS;
}

cld_cmd_err ctr_kill_cmd_handler(void *handler_args, struct array_list *options,
		struct array_list *args, cld_command_output_handler success_handler,
		cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_result *res;
	docker_context *ctx = get_docker_context(handler_args);
	int len = array_list_length(args);
	if (len != 1) {
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
				"Container not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	} else {
		cld_argument* container_arg = (cld_argument*) array_list_get_idx(args,
				0);
		char* container = container_arg->val->str_value;
		docker_kill_container(ctx, &res, container, NULL);
		int created = is_ok(res);
		handle_docker_error(res, success_handler, error_handler);
		if (created) {
			char res_str[1024];
			sprintf(res_str, "Killed container %s", container);
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, res_str);
		}
	}
	return CLD_COMMAND_SUCCESS;
}

cld_cmd_err ctr_ren_cmd_handler(void *handler_args, struct array_list *options,
		struct array_list *args, cld_command_output_handler success_handler,
		cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_result *res;
	docker_context *ctx = get_docker_context(handler_args);
	int len = array_list_length(args);
	if (len != 2) {
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
				"Container name and new name not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	} else {
		cld_argument* container_arg = (cld_argument*) array_list_get_idx(args,
				0);
		char* container = container_arg->val->str_value;
		char* new_name =
				((cld_argument*) array_list_get_idx(args, 1))->val->str_value;
		docker_rename_container(ctx, &res, container, new_name);
		int created = is_ok(res);
		handle_docker_error(res, success_handler, error_handler);
		if (created) {
			char res_str[1024];
			sprintf(res_str, "Renamed container %s to %s", container, new_name);
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, res_str);
		}
	}
	return CLD_COMMAND_SUCCESS;
}

cld_cmd_err ctr_pause_cmd_handler(void *handler_args,
		struct array_list *options, struct array_list *args,
		cld_command_output_handler success_handler,
		cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_result *res;
	docker_context *ctx = get_docker_context(handler_args);
	int len = array_list_length(args);
	if (len != 1) {
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
				"Container not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	} else {
		cld_argument* container_arg = (cld_argument*) array_list_get_idx(args,
				0);
		char* container = container_arg->val->str_value;
		docker_pause_container(ctx, &res, container);
		int created = is_ok(res);
		handle_docker_error(res, success_handler, error_handler);
		if (created) {
			char res_str[1024];
			sprintf(res_str, "Paused container %s", container);
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, res_str);
		}
	}
	return CLD_COMMAND_SUCCESS;
}

cld_cmd_err ctr_unpause_cmd_handler(void *handler_args,
		struct array_list *options, struct array_list *args,
		cld_command_output_handler success_handler,
		cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_result *res;
	docker_context *ctx = get_docker_context(handler_args);
	int len = array_list_length(args);
	if (len != 1) {
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
				"Container not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	} else {
		cld_argument* container_arg = (cld_argument*) array_list_get_idx(args,
				0);
		char* container = container_arg->val->str_value;
		docker_unpause_container(ctx, &res, container);
		int created = is_ok(res);
		handle_docker_error(res, success_handler, error_handler);
		if (created) {
			char res_str[1024];
			sprintf(res_str, "UnPaused container %s", container);
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, res_str);
		}
	}
	return CLD_COMMAND_SUCCESS;
}

cld_cmd_err ctr_wait_cmd_handler(void *handler_args, struct array_list *options,
		struct array_list *args, cld_command_output_handler success_handler,
		cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_result *res;
	docker_context *ctx = get_docker_context(handler_args);
	int len = array_list_length(args);
	if (len != 1) {
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
				"Container not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	} else {
		cld_argument* container_arg = (cld_argument*) array_list_get_idx(args,
				0);
		char* container = container_arg->val->str_value;
		docker_wait_container(ctx, &res, container, NULL);
		int created = is_ok(res);
		handle_docker_error(res, success_handler, error_handler);
		if (created) {
			char res_str[1024];
			sprintf(res_str, "Waiting for container %s", container);
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, res_str);
		}
	}
	return CLD_COMMAND_SUCCESS;
}

cld_cmd_err ctr_logs_cmd_handler(void *handler_args, struct array_list *options,
		struct array_list *args, cld_command_output_handler success_handler,
		cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_result *res;
	docker_context *ctx = get_docker_context(handler_args);
	int len = array_list_length(args);
	if (len != 1) {
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
				"Container not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	} else {
		cld_argument* container_arg = (cld_argument*) array_list_get_idx(args,
				0);
		char* container = container_arg->val->str_value;
		char* log;
		docker_container_logs(ctx, &res, &log, container, 0, 1, 1, -1, -1, 1,
				10);
		int success = is_ok(res);
		handle_docker_error(res, success_handler, error_handler);
		if (success) {
			char res_str[1024];
			sprintf(res_str, "Logs for container %s", container);
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, res_str);
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, log);
		}
	}
	return CLD_COMMAND_SUCCESS;
}

cld_cmd_err ctr_top_cmd_handler(void *handler_args, struct array_list *options,
		struct array_list *args, cld_command_output_handler success_handler,
		cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_result *res;
	docker_context *ctx = get_docker_context(handler_args);
	int len = array_list_length(args);
	if (len != 1) {
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
				"Container not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	} else {
		cld_argument* container_arg = (cld_argument*) array_list_get_idx(args,
				0);
		char* container = container_arg->val->str_value;
		docker_container_ps* ps;
		docker_process_list_container(ctx, &res, &ps, container, NULL);
		int success = is_ok(res);
		handle_docker_error(res, success_handler, error_handler);
		if (success) {
			char res_str[1024];
			sprintf(res_str, "Process list for container %s", container);
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, res_str);

			cld_table* ctr_tbl;
			int num_labels = array_list_length(ps->titles);
			int num_processes = array_list_length(ps->processes);
			if (create_cld_table(&ctr_tbl, num_processes, num_labels) == 0) {
				for (int i = 0; i < num_labels; i++) {
					cld_table_set_header(ctr_tbl, i,
							(char *) array_list_get_idx(ps->titles, i));
				}
				for (int i = 0; i < num_processes; i++) {
					array_list* psvals = (array_list*) array_list_get_idx(
							ps->processes, i);
					for (int j = 0; j < num_labels; j++) {
						cld_table_set_row_val(ctr_tbl, i, j,
								(char *) array_list_get_idx(psvals, j));
					}
				}
				success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_TABLE, ctr_tbl);
			}
		}
	}
	return CLD_COMMAND_SUCCESS;
}

cld_cmd_err ctr_remove_cmd_handler(void *handler_args,
		struct array_list *options, struct array_list *args,
		cld_command_output_handler success_handler,
		cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_result *res;
	docker_context *ctx = get_docker_context(handler_args);
	int len = array_list_length(args);
	if (len != 1) {
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
				"Container not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	} else {
		cld_argument* container_arg = (cld_argument*) array_list_get_idx(args,
				0);
		char* container = container_arg->val->str_value;
		docker_remove_container(ctx, &res, container, 0, 0, 0);
		int created = is_ok(res);
		handle_docker_error(res, success_handler, error_handler);
		if (created) {
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
	stats_args* sarg = (stats_args*) cbargs;
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
		sprintf(cpu_usg_str, "%ld", stats->cpu_stats->total_usage);
		cld_table_set_row_val(ctr_tbl, 0, 2, cpu_usg_str);

		char mem_usg_str[128];
		sprintf(mem_usg_str, "%ld", stats->mem_stats->usage);
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

cld_cmd_err ctr_stats_cmd_handler(void *handler_args,
		struct array_list *options, struct array_list *args,
		cld_command_output_handler success_handler,
		cld_command_output_handler error_handler) {
	int quiet = 0;
	docker_result *res;
	docker_context *ctx = get_docker_context(handler_args);
	int len = array_list_length(args);
	if (len != 1) {
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
				"Container not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	} else {
		cld_argument* container_arg = (cld_argument*) array_list_get_idx(args,
				0);
		char* container = container_arg->val->str_value;
		stats_args* sarg = (stats_args*) calloc(1, sizeof(stats_args));
		sarg->id = container;
		sarg->name = container;
		sarg->success_handler = success_handler;
		docker_container_get_stats_cb(ctx, &res, &docker_container_stats_cb,
				sarg, container);
		int success = is_ok(res);
		handle_docker_error(res, success_handler, error_handler);
		if (success) {
			char res_str[1024];
			sprintf(res_str, "done %s", container);
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, res_str);
		}
	}
	return CLD_COMMAND_SUCCESS;
}

cld_command *ctr_commands() {
	cld_command *container_command;
	if (make_command(&container_command, "container", "ctr",
			"Docker Container Commands",
			NULL) == CLD_COMMAND_SUCCESS) {
		cld_command *ctrls_command, *ctrcreate_command, *ctrstart_command,
				*ctrstop_command, *ctrrestart_command, *ctrkill_command,
				*ctrren_command, *ctrpause_command, *ctrunpause_command,
				*ctrwait_command, *ctrlogs_command, *ctrtop_command,
				*ctrremove_command, *ctrstats_command;
		if (make_command(&ctrls_command, "list", "ls", "Docker Container List",
				&ctr_ls_cmd_handler) == CLD_COMMAND_SUCCESS) {
			cld_option *all_option, *filter_option, *format_option,
					*last_option, *latest_option, *notrunc_option,
					*quiet_option, *size_option;
			make_option(&all_option, "all", "a", CLD_TYPE_FLAG, "Show all containers (by default shows only running ones).");
			array_list_add(container_command->sub_commands, ctrls_command);
			array_list_add(container_command->options, all_option);
		}
		if (make_command(&ctrcreate_command, "create", "create",
				"Docker Container Create", &ctr_create_cmd_handler)
				== CLD_COMMAND_SUCCESS) {
			cld_argument* image_name_arg;
			make_argument(&image_name_arg, "Image Name", CLD_TYPE_STRING,
					"Name of Docker Image to use.");
			array_list_add(ctrcreate_command->args, image_name_arg);

			array_list_add(container_command->sub_commands, ctrcreate_command);
		}
		if (make_command(&ctrstart_command, "start", "on",
				"Docker Container Start", &ctr_start_cmd_handler)
				== CLD_COMMAND_SUCCESS) {
			cld_argument* container_arg;
			make_argument(&container_arg, "Container", CLD_TYPE_STRING,
					"Name of container to start.");
			array_list_add(ctrstart_command->args, container_arg);

			array_list_add(container_command->sub_commands, ctrstart_command);
		}
		if (make_command(&ctrstop_command, "stop", "off",
				"Docker Container Stop", &ctr_stop_cmd_handler)
				== CLD_COMMAND_SUCCESS) {
			cld_argument* container_arg;
			make_argument(&container_arg, "Container", CLD_TYPE_STRING,
					"Name of container to stop.");
			array_list_add(ctrstop_command->args, container_arg);

			array_list_add(container_command->sub_commands, ctrstop_command);
		}
		if (make_command(&ctrrestart_command, "restart", "restart",
				"Docker Container Restart", &ctr_restart_cmd_handler)
				== CLD_COMMAND_SUCCESS) {
			cld_argument* container_arg;
			make_argument(&container_arg, "Container", CLD_TYPE_STRING,
					"Name of container to restart.");
			array_list_add(ctrrestart_command->args, container_arg);

			array_list_add(container_command->sub_commands, ctrrestart_command);
		}
		if (make_command(&ctrkill_command, "kill", "kill",
				"Docker Container Kill", &ctr_kill_cmd_handler)
				== CLD_COMMAND_SUCCESS) {
			cld_argument* container_arg;
			make_argument(&container_arg, "Container", CLD_TYPE_STRING,
					"Name of container to kill.");
			array_list_add(ctrkill_command->args, container_arg);

			array_list_add(container_command->sub_commands, ctrkill_command);
		}
		if (make_command(&ctrren_command, "rename", "ren",
				"Docker Container Rename", &ctr_ren_cmd_handler)
				== CLD_COMMAND_SUCCESS) {
			cld_argument* container_arg;
			make_argument(&container_arg, "Container", CLD_TYPE_STRING,
					"Name of container to rename.");
			array_list_add(ctrren_command->args, container_arg);

			cld_argument* name_arg;
			make_argument(&name_arg, "Name", CLD_TYPE_STRING,
					"New name of container.");
			array_list_add(ctrren_command->args, name_arg);

			array_list_add(container_command->sub_commands, ctrren_command);
		}
		if (make_command(&ctrpause_command, "pause", "pause",
				"Docker Container Pause", &ctr_pause_cmd_handler)
				== CLD_COMMAND_SUCCESS) {
			cld_argument* container_arg;
			make_argument(&container_arg, "Container", CLD_TYPE_STRING,
					"Name of container to pause.");
			array_list_add(ctrpause_command->args, container_arg);

			array_list_add(container_command->sub_commands, ctrpause_command);
		}
		if (make_command(&ctrunpause_command, "unpause", "unpause",
				"Docker Container UnPause", &ctr_unpause_cmd_handler)
				== CLD_COMMAND_SUCCESS) {
			cld_argument* container_arg;
			make_argument(&container_arg, "Container", CLD_TYPE_STRING,
					"Name of container to unpause.");
			array_list_add(ctrunpause_command->args, container_arg);

			array_list_add(container_command->sub_commands, ctrunpause_command);
		}
		if (make_command(&ctrwait_command, "wait", "wait",
				"Docker Container Wait", &ctr_wait_cmd_handler)
				== CLD_COMMAND_SUCCESS) {
			cld_argument* container_arg;
			make_argument(&container_arg, "Container", CLD_TYPE_STRING,
					"Name of container to wait.");
			array_list_add(ctrwait_command->args, container_arg);

			array_list_add(container_command->sub_commands, ctrwait_command);
		}
		if (make_command(&ctrlogs_command, "logs", "lg",
				"Docker Container Logs", &ctr_logs_cmd_handler)
				== CLD_COMMAND_SUCCESS) {
			cld_argument* container_arg;
			make_argument(&container_arg, "Container", CLD_TYPE_STRING,
					"Name of container.");
			array_list_add(ctrlogs_command->args, container_arg);

			array_list_add(container_command->sub_commands, ctrlogs_command);
		}
		if (make_command(&ctrtop_command, "top", "ps",
				"Docker Container Top/PS", &ctr_top_cmd_handler)
				== CLD_COMMAND_SUCCESS) {
			cld_argument* container_arg;
			make_argument(&container_arg, "Container", CLD_TYPE_STRING,
					"Name of container.");
			array_list_add(ctrtop_command->args, container_arg);

			array_list_add(container_command->sub_commands, ctrtop_command);
		}
		if (make_command(&ctrremove_command, "remove", "del",
				"Docker Remove Container", &ctr_remove_cmd_handler)
				== CLD_COMMAND_SUCCESS) {
			cld_argument* container_arg;
			make_argument(&container_arg, "Container", CLD_TYPE_STRING,
					"Name of container.");
			array_list_add(ctrremove_command->args, container_arg);

			array_list_add(container_command->sub_commands, ctrremove_command);
		}
		if (make_command(&ctrstats_command, "stats", "stats",
				"Docker Container Stats", &ctr_stats_cmd_handler)
				== CLD_COMMAND_SUCCESS) {
			cld_argument* container_arg;
			make_argument(&container_arg, "Container", CLD_TYPE_STRING,
					"Name of container.");
			array_list_add(ctrstats_command->args, container_arg);

			array_list_add(container_command->sub_commands, ctrstats_command);
		}
	}
	return container_command;
}
