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
#include "cli_table.h"
#include "cld_lua.h"

cli_cmd_err ctr_ls_cmd_handler(void* handler_args, arraylist* options,
	arraylist* args, cli_command_output_handler success_handler,
	cli_command_output_handler error_handler) {

	json_object* obj;
	cli_cmd_err err = execute_lua_command(&obj, "ctr", "ls", handler_args, options, args, success_handler, error_handler);
	if(obj != NULL) {
		docker_log_debug("Received json object -> %s\n", get_json_string(obj));
	}
	return err;
}

cli_cmd_err ctr_create_cmd_handler(void* handler_args,
	arraylist* options, arraylist* args,
	cli_command_output_handler success_handler,
	cli_command_output_handler error_handler) {
	int quiet = 0;
	docker_context* ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(args);
	if (len != 1) {
		error_handler(CLI_COMMAND_ERR_UNKNOWN, CLI_RESULT_STRING,
			"Image name not provided.");
		return CLI_COMMAND_ERR_UNKNOWN;
	}
	else {
		cli_argument* image_name_arg = (cli_argument*)arraylist_get(args,
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
				success_handler(CLI_COMMAND_SUCCESS, CLI_RESULT_STRING,
					res_str);
			}
		}
	}
	return CLI_COMMAND_SUCCESS;
}

cli_cmd_err ctr_start_cmd_handler(void* handler_args,
	arraylist* options, arraylist* args,
	cli_command_output_handler success_handler,
	cli_command_output_handler error_handler) {
	int quiet = 0;
	docker_context* ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(args);
	if (len != 1) {
		error_handler(CLI_COMMAND_ERR_UNKNOWN, CLI_RESULT_STRING,
			"Container not provided.");
		return CLI_COMMAND_ERR_UNKNOWN;
	}
	else {
		cli_argument* container_arg = (cli_argument*)arraylist_get(args,
			0);
		char* container = container_arg->val->str_value;
		d_err_t e = docker_start_container(ctx, container, NULL);
		if (e == E_SUCCESS) {
			char res_str[1024];
			sprintf(res_str, "Started container %s", container);
			success_handler(CLI_COMMAND_SUCCESS, CLI_RESULT_STRING, res_str);
		}
	}
	return CLI_COMMAND_SUCCESS;
}

cli_cmd_err ctr_stop_cmd_handler(void* handler_args, arraylist* options,
	arraylist* args, cli_command_output_handler success_handler,
	cli_command_output_handler error_handler) {
	int quiet = 0;
	docker_context* ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(args);
	if (len != 1) {
		error_handler(CLI_COMMAND_ERR_UNKNOWN, CLI_RESULT_STRING,
			"Container not provided.");
		return CLI_COMMAND_ERR_UNKNOWN;
	}
	else {
		cli_argument* container_arg = (cli_argument*)arraylist_get(args,
			0);
		char* container = container_arg->val->str_value;
		d_err_t e = docker_stop_container(ctx, container, 0);
		if (e == E_SUCCESS) {
			char res_str[1024];
			sprintf(res_str, "Stopped container %s", container);
			success_handler(CLI_COMMAND_SUCCESS, CLI_RESULT_STRING, res_str);
		}
	}
	return CLI_COMMAND_SUCCESS;
}

cli_cmd_err ctr_restart_cmd_handler(void* handler_args,
	arraylist* options, arraylist* args,
	cli_command_output_handler success_handler,
	cli_command_output_handler error_handler) {
	int quiet = 0;
	docker_context* ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(args);
	if (len != 1) {
		error_handler(CLI_COMMAND_ERR_UNKNOWN, CLI_RESULT_STRING,
			"Container not provided.");
		return CLI_COMMAND_ERR_UNKNOWN;
	}
	else {
		cli_argument* container_arg = (cli_argument*)arraylist_get(args,
			0);
		char* container = container_arg->val->str_value;
		d_err_t e = docker_restart_container(ctx, container, 0);
		if (e == E_SUCCESS) {
			char res_str[1024];
			sprintf(res_str, "Restarted container %s", container);
			success_handler(CLI_COMMAND_SUCCESS, CLI_RESULT_STRING, res_str);
		}
	}
	return CLI_COMMAND_SUCCESS;
}

cli_cmd_err ctr_kill_cmd_handler(void* handler_args, arraylist* options,
	arraylist* args, cli_command_output_handler success_handler,
	cli_command_output_handler error_handler) {
	int quiet = 0;
	docker_context* ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(args);
	if (len != 1) {
		error_handler(CLI_COMMAND_ERR_UNKNOWN, CLI_RESULT_STRING,
			"Container not provided.");
		return CLI_COMMAND_ERR_UNKNOWN;
	}
	else {
		cli_argument* container_arg = (cli_argument*)arraylist_get(args,
			0);
		char* container = container_arg->val->str_value;
		d_err_t e = docker_kill_container(ctx, container, NULL);
		if (e == E_SUCCESS) {
			char res_str[1024];
			sprintf(res_str, "Killed container %s", container);
			success_handler(CLI_COMMAND_SUCCESS, CLI_RESULT_STRING, res_str);
		}
	}
	return CLI_COMMAND_SUCCESS;
}

cli_cmd_err ctr_ren_cmd_handler(void* handler_args, arraylist* options,
	arraylist* args, cli_command_output_handler success_handler,
	cli_command_output_handler error_handler) {
	int quiet = 0;
	docker_context* ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(args);
	if (len != 2) {
		error_handler(CLI_COMMAND_ERR_UNKNOWN, CLI_RESULT_STRING,
			"Container name and new name not provided.");
		return CLI_COMMAND_ERR_UNKNOWN;
	}
	else {
		cli_argument* container_arg = (cli_argument*)arraylist_get(args,
			0);
		char* container = container_arg->val->str_value;
		char* new_name =
			((cli_argument*)arraylist_get(args, 1))->val->str_value;
		d_err_t e = docker_rename_container(ctx, container, new_name);
		if (e == E_SUCCESS) {
			char res_str[1024];
			sprintf(res_str, "Renamed container %s to %s", container, new_name);
			success_handler(CLI_COMMAND_SUCCESS, CLI_RESULT_STRING, res_str);
		}
	}
	return CLI_COMMAND_SUCCESS;
}

cli_cmd_err ctr_pause_cmd_handler(void* handler_args,
	arraylist* options, arraylist* args,
	cli_command_output_handler success_handler,
	cli_command_output_handler error_handler) {
	int quiet = 0;
	docker_context* ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(args);
	if (len != 1) {
		error_handler(CLI_COMMAND_ERR_UNKNOWN, CLI_RESULT_STRING,
			"Container not provided.");
		return CLI_COMMAND_ERR_UNKNOWN;
	}
	else {
		cli_argument* container_arg = (cli_argument*)arraylist_get(args,
			0);
		char* container = container_arg->val->str_value;
		d_err_t e = docker_pause_container(ctx, container);
		if (e == E_SUCCESS) {
			char res_str[1024];
			sprintf(res_str, "Paused container %s", container);
			success_handler(CLI_COMMAND_SUCCESS, CLI_RESULT_STRING, res_str);
		}
	}
	return CLI_COMMAND_SUCCESS;
}

cli_cmd_err ctr_unpause_cmd_handler(void* handler_args,
	arraylist* options, arraylist* args,
	cli_command_output_handler success_handler,
	cli_command_output_handler error_handler) {
	int quiet = 0;
	docker_context* ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(args);
	if (len != 1) {
		error_handler(CLI_COMMAND_ERR_UNKNOWN, CLI_RESULT_STRING,
			"Container not provided.");
		return CLI_COMMAND_ERR_UNKNOWN;
	}
	else {
		cli_argument* container_arg = (cli_argument*)arraylist_get(args,
			0);
		char* container = container_arg->val->str_value;
		d_err_t e = docker_unpause_container(ctx, container);
		if (e == E_SUCCESS) {
			char res_str[1024];
			sprintf(res_str, "UnPaused container %s", container);
			success_handler(CLI_COMMAND_SUCCESS, CLI_RESULT_STRING, res_str);
		}
	}
	return CLI_COMMAND_SUCCESS;
}

cli_cmd_err ctr_wait_cmd_handler(void* handler_args, arraylist* options,
	arraylist* args, cli_command_output_handler success_handler,
	cli_command_output_handler error_handler) {
	int quiet = 0;
	docker_context* ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(args);
	if (len != 1) {
		error_handler(CLI_COMMAND_ERR_UNKNOWN, CLI_RESULT_STRING,
			"Container not provided.");
		return CLI_COMMAND_ERR_UNKNOWN;
	}
	else {
		cli_argument* container_arg = (cli_argument*)arraylist_get(args,
			0);
		char* container = container_arg->val->str_value;
		d_err_t e = docker_wait_container(ctx, container, NULL);
		if (e == E_SUCCESS) {
			char res_str[1024];
			sprintf(res_str, "Waiting for container %s", container);
			success_handler(CLI_COMMAND_SUCCESS, CLI_RESULT_STRING, res_str);
		}
	}
	return CLI_COMMAND_SUCCESS;
}

void cld_log_line_handler(void* args, int stream_id, int line_num, char* line) {
	cli_command_output_handler success_handler = (cli_command_output_handler)args;
	docker_log_info("Stream %d, line# %d :: %s", stream_id, line_num, line);
	success_handler(CLI_COMMAND_SUCCESS, CLI_RESULT_STRING, line);
}

cli_cmd_err ctr_logs_cmd_handler(void* handler_args, arraylist* options,
	arraylist* args, cli_command_output_handler success_handler,
	cli_command_output_handler error_handler) {
	int quiet = 0;
	docker_context* ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(args);
	if (len != 1) {
		error_handler(CLI_COMMAND_ERR_UNKNOWN, CLI_RESULT_STRING,
			"Container not provided.");
		return CLI_COMMAND_ERR_UNKNOWN;
	}
	else {
		cli_argument* container_arg = (cli_argument*)arraylist_get(args,
			0);
		char* container = container_arg->val->str_value;
		char* log;
		size_t log_len;
		d_err_t e = docker_container_logs(ctx, &log, &log_len, container, 0, 1, 1, -1, -1, 1,
			10);
		if (e == E_SUCCESS) {
			char res_str[1024];
			sprintf(res_str, "Logs for container %s", container);
			success_handler(CLI_COMMAND_SUCCESS, CLI_RESULT_STRING, res_str);
			//success_handler(CLI_COMMAND_SUCCESS, CLI_RESULT_STRING, log);
			docker_container_logs_foreach(success_handler, log, log_len, &cld_log_line_handler);
		}
	}
	return CLI_COMMAND_SUCCESS;
}

cli_cmd_err ctr_top_cmd_handler(void* handler_args, arraylist* options,
	arraylist* args, cli_command_output_handler success_handler,
	cli_command_output_handler error_handler) {
	int quiet = 0;
	docker_context* ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(args);
	if (len != 1) {
		error_handler(CLI_COMMAND_ERR_UNKNOWN, CLI_RESULT_STRING,
			"Container not provided.");
		return CLI_COMMAND_ERR_UNKNOWN;
	}
	else {
		cli_argument* container_arg = (cli_argument*)arraylist_get(args,
			0);
		char* container = container_arg->val->str_value;
		docker_container_ps* ps;
		d_err_t e = docker_process_list_container(ctx, &ps, container, NULL);
		if (e == E_SUCCESS) {
			char res_str[1024];
			sprintf(res_str, "Process list for container %s", container);
			success_handler(CLI_COMMAND_SUCCESS, CLI_RESULT_STRING, res_str);

			cli_table* ctr_tbl;
			size_t num_labels = arraylist_length(ps->titles);
			size_t num_processes = arraylist_length(ps->processes);
			if (create_cli_table(&ctr_tbl, num_processes, num_labels) == 0) {
				for (size_t i = 0; i < num_labels; i++) {
					cli_table_set_header(ctr_tbl, i,
						(char*)arraylist_get(ps->titles, i));
				}
				for (size_t i = 0; i < num_processes; i++) {
					arraylist* psvals = (arraylist*)arraylist_get(
						ps->processes, i);
					for (size_t j = 0; j < num_labels; j++) {
						cli_table_set_row_val(ctr_tbl, i, j,
							(char*)arraylist_get(psvals, j));
					}
				}
				success_handler(CLI_COMMAND_SUCCESS, CLI_RESULT_TABLE, ctr_tbl);
			}
		}
	}
	return CLI_COMMAND_SUCCESS;
}

cli_cmd_err ctr_remove_cmd_handler(void* handler_args,
	arraylist* options, arraylist* args,
	cli_command_output_handler success_handler,
	cli_command_output_handler error_handler) {
	int quiet = 0;
	docker_context* ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(args);
	if (len != 1) {
		error_handler(CLI_COMMAND_ERR_UNKNOWN, CLI_RESULT_STRING,
			"Container not provided.");
		return CLI_COMMAND_ERR_UNKNOWN;
	}
	else {
		cli_argument* container_arg = (cli_argument*)arraylist_get(args,
			0);
		char* container = container_arg->val->str_value;
		d_err_t e = docker_remove_container(ctx, container, 0, 0, 0);
		if (e == E_SUCCESS) {
			char res_str[1024];
			sprintf(res_str, "Removed container %s", container);
			success_handler(CLI_COMMAND_SUCCESS, CLI_RESULT_STRING, res_str);
		}
	}
	return CLI_COMMAND_SUCCESS;
}

typedef struct stats_args_t {
	char* id;
	char* name;
	cli_command_output_handler success_handler;
} stats_args;

void docker_container_stats_cb(docker_container_stats* stats, void* cbargs) {
	stats_args* sarg = (stats_args*)cbargs;
	cli_table* ctr_tbl;
	if (create_cli_table(&ctr_tbl, 1, 8) == 0) {
		cli_table_set_header(ctr_tbl, 0, "CONTAINER ID");
		cli_table_set_header(ctr_tbl, 1, "NAME");
		cli_table_set_header(ctr_tbl, 2, "CPU %");
		cli_table_set_header(ctr_tbl, 3, "MEM USAGE / LIMIT");
		cli_table_set_header(ctr_tbl, 4, "MEM %");
		cli_table_set_header(ctr_tbl, 5, "NET I/O");
		cli_table_set_header(ctr_tbl, 6, "BLOCK I/O");
		cli_table_set_header(ctr_tbl, 7, "PIDS");

		cli_table_set_row_val(ctr_tbl, 0, 0, sarg->id);
		cli_table_set_row_val(ctr_tbl, 0, 1, sarg->name);

		char cpu_usg_str[128];
		sprintf(cpu_usg_str, "%ld", docker_container_cpu_stats_cpu_usage_get(docker_container_stats_cpu_stats_get(stats)));
		cli_table_set_row_val(ctr_tbl, 0, 2, cpu_usg_str);

		char mem_usg_str[128];
		sprintf(mem_usg_str, "%ld", docker_container_mem_stats_usage_get(docker_container_stats_mem_stats_get(stats)));
		cli_table_set_row_val(ctr_tbl, 0, 3, mem_usg_str);

		cli_table_set_row_val(ctr_tbl, 0, 4, "");
		cli_table_set_row_val(ctr_tbl, 0, 5, "");
		cli_table_set_row_val(ctr_tbl, 0, 6, "");
		cli_table_set_row_val(ctr_tbl, 0, 7, "");
	}
	sarg->success_handler(CLI_COMMAND_IS_RUNNING, CLI_RESULT_STRING,
		"\033[0;0H\033[2J");
	sarg->success_handler(CLI_COMMAND_IS_RUNNING, CLI_RESULT_TABLE, ctr_tbl);
}

cli_cmd_err ctr_stats_cmd_handler(void* handler_args,
	arraylist* options, arraylist* args,
	cli_command_output_handler success_handler,
	cli_command_output_handler error_handler) {
	int quiet = 0;
	docker_context* ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(args);
	if (len != 1) {
		error_handler(CLI_COMMAND_ERR_UNKNOWN, CLI_RESULT_STRING,
			"Container not provided.");
		return CLI_COMMAND_ERR_UNKNOWN;
	}
	else {
		cli_argument* container_arg = (cli_argument*)arraylist_get(args,
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
			success_handler(CLI_COMMAND_SUCCESS, CLI_RESULT_STRING, res_str);
		}
	}
	return CLI_COMMAND_SUCCESS;
}

cli_command* ctr_commands() {
	cli_command* container_command;
	if (make_command(&container_command, "container", "ctr",
		"Docker Container Commands",
		NULL) == CLI_COMMAND_SUCCESS) {
		cli_command* ctr_command;

		ctr_command = create_command("list", "ls", "Docker Container List",
			&ctr_ls_cmd_handler);
		arraylist_add(ctr_command->options,
			create_option("all", "a", CLI_VAL_FLAG(0), CLI_VAL_FLAG(0), "Show all containers (by default shows only running ones)."));
		arraylist_add(ctr_command->options,
			create_option("filter", "f", CLI_VAL_STRING(NULL), CLI_VAL_STRING(NULL), "Filter output based on conditions provided"));
		arraylist_add(ctr_command->options,
			create_option("format", NULL, CLI_VAL_STRING(NULL), CLI_VAL_STRING(NULL), "Pretty-print containers using a Go template"));
		arraylist_add(ctr_command->options,
			create_option("last", "n", CLI_VAL_INT(-1), CLI_VAL_INT(10), "Show n last created containers (includes all states)"));
		arraylist_add(ctr_command->options,
			create_option("latest", "l", CLI_VAL_FLAG(0), CLI_VAL_FLAG(0), "Show the latest created container (includes all states)"));
		arraylist_add(ctr_command->options,
			create_option("no-trunc", NULL, CLI_VAL_FLAG(0), CLI_VAL_FLAG(0), "Don't truncate output"));
		arraylist_add(ctr_command->options,
			create_option("quiet", "q", CLI_VAL_FLAG(0), CLI_VAL_FLAG(0), "Only display numeric IDs"));
		arraylist_add(ctr_command->options,
			create_option("size", "s", CLI_VAL_FLAG(0), CLI_VAL_FLAG(0), "Display total file sizes"));
		arraylist_add(container_command->sub_commands, ctr_command);

		ctr_command = create_command("create", "create",
			"Docker Container Create", &ctr_create_cmd_handler);
		arraylist_add(ctr_command->args,
			create_argument("Image Name", CLI_VAL_STRING(NULL), CLI_VAL_STRING(NULL), "Name of Docker Image to use."));
		arraylist_add(container_command->sub_commands, ctr_command);

		ctr_command = create_command("start", "on",
			"Docker Container Start", &ctr_start_cmd_handler);
		arraylist_add(ctr_command->args,
			create_argument("Container", CLI_VAL_STRING(NULL), CLI_VAL_STRING(NULL),
				"Name of container to start."));
		arraylist_add(container_command->sub_commands, ctr_command);

		ctr_command = create_command("stop", "off",
			"Docker Container Stop", &ctr_stop_cmd_handler);
		arraylist_add(ctr_command->args,
			create_argument("Container", CLI_VAL_STRING(NULL), CLI_VAL_STRING(NULL),
				"Name of container to stop."));
		arraylist_add(container_command->sub_commands, ctr_command);

		ctr_command = create_command("restart", "restart",
			"Docker Container Restart", &ctr_restart_cmd_handler);
		arraylist_add(ctr_command->args,
			create_argument("Container", CLI_VAL_STRING(NULL), CLI_VAL_STRING(NULL),
				"Name of container to restart."));
		arraylist_add(container_command->sub_commands, ctr_command);

		ctr_command = create_command("kill", "kill",
			"Docker Container Kill", &ctr_kill_cmd_handler);
		arraylist_add(ctr_command->args,
			create_argument("Container", CLI_VAL_STRING(NULL), CLI_VAL_STRING(NULL),
				"Name of container to kill."));
		arraylist_add(container_command->sub_commands, ctr_command);

		ctr_command = create_command("rename", "ren",
			"Docker Container Rename", &ctr_ren_cmd_handler);
		arraylist_add(ctr_command->args,
			create_argument("Container", CLI_VAL_STRING(NULL), CLI_VAL_STRING(NULL),
				"Name of container to rename."));
		arraylist_add(ctr_command->args,
			create_argument("Name", CLI_VAL_STRING(NULL), CLI_VAL_STRING(NULL),
				"New name of container."));
		arraylist_add(container_command->sub_commands, ctr_command);

		ctr_command = create_command("pause", "pause",
			"Docker Container Pause", &ctr_pause_cmd_handler);
		arraylist_add(ctr_command->args,
			create_argument("Container", CLI_VAL_STRING(NULL), CLI_VAL_STRING(NULL),
				"Name of container to pause."));
		arraylist_add(container_command->sub_commands, ctr_command);

		ctr_command = create_command("unpause", "unpause",
			"Docker Container UnPause", &ctr_unpause_cmd_handler);
		arraylist_add(ctr_command->args,
			create_argument("Container", CLI_VAL_STRING(NULL), CLI_VAL_STRING(NULL),
				"Name of container to unpause."));
		arraylist_add(container_command->sub_commands, ctr_command);

		ctr_command = create_command("wait", "wait",
			"Docker Container Wait", &ctr_wait_cmd_handler);
		arraylist_add(ctr_command->args,
			create_argument("Container", CLI_VAL_STRING(NULL), CLI_VAL_STRING(NULL),
				"Name of container to wait."));
		arraylist_add(container_command->sub_commands, ctr_command);

		ctr_command = create_command("logs", "lg",
			"Docker Container Logs", &ctr_logs_cmd_handler);
		arraylist_add(ctr_command->args,
			create_argument("Container", CLI_VAL_STRING(NULL), CLI_VAL_STRING(NULL),
				"Name of container."));
		arraylist_add(container_command->sub_commands, ctr_command);

		ctr_command = create_command("top", "ps",
			"Docker Container Top/PS", &ctr_top_cmd_handler);
		arraylist_add(ctr_command->args,
			create_argument("Container", CLI_VAL_STRING(NULL), CLI_VAL_STRING(NULL),
				"Name of container."));
		arraylist_add(container_command->sub_commands, ctr_command);

		ctr_command = create_command("remove", "del",
			"Docker Remove Container", &ctr_remove_cmd_handler);
		arraylist_add(ctr_command->args,
			create_argument("Container", CLI_VAL_STRING(NULL), CLI_VAL_STRING(NULL),
				"Name of container."));
		arraylist_add(container_command->sub_commands, ctr_command);

		ctr_command = create_command("stats", "stats",
			"Docker Container Stats", &ctr_stats_cmd_handler);
		arraylist_add(ctr_command->args,
			create_argument("Container", CLI_VAL_STRING(NULL), CLI_VAL_STRING(NULL),
				"Name of container."));
		arraylist_add(container_command->sub_commands, ctr_command);

	}
	return container_command;
}
