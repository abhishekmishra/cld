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

#include "docker_all.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "cld_ctr.h"
#include "zclk_table.h"
#include "cld_lua.h"

zclk_res ctr_ls_cmd_handler(zclk_command* cmd, void *handler_args)
{

	json_object *obj;
	zclk_res err = execute_lua_command(&obj, "ctr", "ls", handler_args,
		cmd->options, cmd->args, cmd->success_handler, cmd->error_handler);
	if (obj != NULL)
	{
		docker_log_debug("Received json object -> %s\n", get_json_string(obj));
	}
	return err;
}

zclk_res ctr_create_cmd_handler(zclk_command* cmd, void *handler_args)
{
	int quiet = 0;
	docker_context *ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(cmd->args);
	if (len != 1)
	{
		cmd->error_handler(ZCLK_RES_ERR_UNKNOWN, ZCLK_RESULT_STRING,
					  "Image name not provided.");
		return ZCLK_RES_ERR_UNKNOWN;
	}
	else
	{
		zclk_argument *image_name_arg = 
				(zclk_argument *)arraylist_get(cmd->args, 0);
		char *image_name = zclk_argument_get_val_string(image_name_arg);
		char *id = NULL;
		docker_ctr_create_params *p = make_docker_ctr_create_params();
		docker_ctr_create_params_image_set(p, image_name);
		d_err_t e = docker_create_container(ctx, &id, p);
		if (e == E_SUCCESS)
		{
			if (id != NULL)
			{
				char res_str[1024];
				sprintf(res_str, "Created container with id %s", id);
				cmd->success_handler(ZCLK_RES_SUCCESS, ZCLK_RESULT_STRING,
								res_str);
			}
		}
	}
	return ZCLK_RES_SUCCESS;
}

zclk_res ctr_start_cmd_handler(zclk_command* cmd, void *handler_args)
{
	int quiet = 0;
	docker_context *ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(cmd->args);
	if (len != 1)
	{
		cmd->error_handler(ZCLK_RES_ERR_UNKNOWN, ZCLK_RESULT_STRING,
					  "Container not provided.");
		return ZCLK_RES_ERR_UNKNOWN;
	}
	else
	{
		zclk_argument *container_arg = 
			(zclk_argument *)arraylist_get(cmd->args, 0);
		char *container = zclk_argument_get_val_string(container_arg);
		d_err_t e = docker_start_container(ctx, container, NULL);
		if (e == E_SUCCESS)
		{
			char res_str[1024];
			sprintf(res_str, "Started container %s", container);
			cmd->success_handler(ZCLK_RES_SUCCESS, ZCLK_RESULT_STRING, res_str);
		}
	}
	return ZCLK_RES_SUCCESS;
}

zclk_res ctr_stop_cmd_handler(zclk_command* cmd, void *handler_args)
{
	int quiet = 0;
	docker_context *ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(cmd->args);
	if (len != 1)
	{
		cmd->error_handler(ZCLK_RES_ERR_UNKNOWN, ZCLK_RESULT_STRING,
					  "Container not provided.");
		return ZCLK_RES_ERR_UNKNOWN;
	}
	else
	{
		zclk_argument *container_arg = 
				(zclk_argument *)arraylist_get(cmd->args, 0);
		char *container = zclk_argument_get_val_string(container_arg);
		d_err_t e = docker_stop_container(ctx, container, 0);
		if (e == E_SUCCESS)
		{
			char res_str[1024];
			sprintf(res_str, "Stopped container %s", container);
			cmd->success_handler(ZCLK_RES_SUCCESS, ZCLK_RESULT_STRING, res_str);
		}
	}
	return ZCLK_RES_SUCCESS;
}

zclk_res ctr_restart_cmd_handler(zclk_command* cmd, void *handler_args)
{
	int quiet = 0;
	docker_context *ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(cmd->args);
	if (len != 1)
	{
		cmd->error_handler(ZCLK_RES_ERR_UNKNOWN, ZCLK_RESULT_STRING,
					  "Container not provided.");
		return ZCLK_RES_ERR_UNKNOWN;
	}
	else
	{
		zclk_argument *container_arg = 
				(zclk_argument *)arraylist_get(cmd->args, 0);
		char *container = zclk_argument_get_val_string(container_arg);
		d_err_t e = docker_restart_container(ctx, container, 0);
		if (e == E_SUCCESS)
		{
			char res_str[1024];
			sprintf(res_str, "Restarted container %s", container);
			cmd->success_handler(ZCLK_RES_SUCCESS, ZCLK_RESULT_STRING, res_str);
		}
	}
	return ZCLK_RES_SUCCESS;
}

zclk_res ctr_kill_cmd_handler(zclk_command* cmd, void *handler_args)
{
	int quiet = 0;
	docker_context *ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(cmd->args);
	if (len != 1)
	{
		cmd->error_handler(ZCLK_RES_ERR_UNKNOWN, ZCLK_RESULT_STRING,
					  "Container not provided.");
		return ZCLK_RES_ERR_UNKNOWN;
	}
	else
	{
		zclk_argument *container_arg = 
				(zclk_argument *)arraylist_get(cmd->args, 0);
		char *container = zclk_argument_get_val_string(container_arg);
		d_err_t e = docker_kill_container(ctx, container, NULL);
		if (e == E_SUCCESS)
		{
			char res_str[1024];
			sprintf(res_str, "Killed container %s", container);
			cmd->success_handler(ZCLK_RES_SUCCESS, ZCLK_RESULT_STRING, res_str);
		}
	}
	return ZCLK_RES_SUCCESS;
}

zclk_res ctr_ren_cmd_handler(zclk_command* cmd, void *handler_args)
{
	int quiet = 0;
	docker_context *ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(cmd->args);
	if (len != 2)
	{
		cmd->error_handler(ZCLK_RES_ERR_UNKNOWN, ZCLK_RESULT_STRING,
					  "Container name and new name not provided.");
		return ZCLK_RES_ERR_UNKNOWN;
	}
	else
	{
		zclk_argument *container_arg = 
				(zclk_argument *)arraylist_get(cmd->args, 0);
		char *container = zclk_argument_get_val_string(container_arg);
		char *new_name = zclk_argument_get_val_string
			((zclk_argument *)arraylist_get(cmd->args, 1));
		d_err_t e = docker_rename_container(ctx, container, new_name);
		if (e == E_SUCCESS)
		{
			char res_str[1024];
			sprintf(res_str, "Renamed container %s to %s", container, new_name);
			cmd->success_handler(ZCLK_RES_SUCCESS, ZCLK_RESULT_STRING, res_str);
		}
	}
	return ZCLK_RES_SUCCESS;
}

zclk_res ctr_pause_cmd_handler(zclk_command* cmd, void *handler_args)
{
	int quiet = 0;
	docker_context *ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(cmd->args);
	if (len != 1)
	{
		cmd->error_handler(ZCLK_RES_ERR_UNKNOWN, ZCLK_RESULT_STRING,
					  "Container not provided.");
		return ZCLK_RES_ERR_UNKNOWN;
	}
	else
	{
		zclk_argument *container_arg =
				(zclk_argument *)arraylist_get(cmd->args, 0);
		char *container = zclk_argument_get_val_string(container_arg);
		d_err_t e = docker_pause_container(ctx, container);
		if (e == E_SUCCESS)
		{
			char res_str[1024];
			sprintf(res_str, "Paused container %s", container);
			cmd->success_handler(ZCLK_RES_SUCCESS, ZCLK_RESULT_STRING, res_str);
		}
	}
	return ZCLK_RES_SUCCESS;
}

zclk_res ctr_unpause_cmd_handler(zclk_command* cmd, void *handler_args)
{
	int quiet = 0;
	docker_context *ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(cmd->args);
	if (len != 1)
	{
		cmd->error_handler(ZCLK_RES_ERR_UNKNOWN, ZCLK_RESULT_STRING,
					  "Container not provided.");
		return ZCLK_RES_ERR_UNKNOWN;
	}
	else
	{
		zclk_argument *container_arg =
				(zclk_argument *)arraylist_get(cmd->args, 0);
		char *container = zclk_argument_get_val_string(container_arg);
		d_err_t e = docker_unpause_container(ctx, container);
		if (e == E_SUCCESS)
		{
			char res_str[1024];
			sprintf(res_str, "UnPaused container %s", container);
			cmd->success_handler(ZCLK_RES_SUCCESS, ZCLK_RESULT_STRING, res_str);
		}
	}
	return ZCLK_RES_SUCCESS;
}

zclk_res ctr_wait_cmd_handler(zclk_command* cmd, void *handler_args)
{
	int quiet = 0;
	docker_context *ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(cmd->args);
	if (len != 1)
	{
		cmd->error_handler(ZCLK_RES_ERR_UNKNOWN, ZCLK_RESULT_STRING,
					  "Container not provided.");
		return ZCLK_RES_ERR_UNKNOWN;
	}
	else
	{
		zclk_argument *container_arg =
				(zclk_argument *)arraylist_get(cmd->args, 0);
		char *container = zclk_argument_get_val_string(container_arg);
		d_err_t e = docker_wait_container(ctx, container, NULL);
		if (e == E_SUCCESS)
		{
			char res_str[1024];
			sprintf(res_str, "Waiting for container %s", container);
			cmd->success_handler(ZCLK_RES_SUCCESS, ZCLK_RESULT_STRING, res_str);
		}
	}
	return ZCLK_RES_SUCCESS;
}

void cld_log_line_handler(void *args, int stream_id, int line_num, char *line)
{
	zclk_command_output_handler success_handler = (zclk_command_output_handler)args;
	docker_log_info("Stream %d, line# %d :: %s", stream_id, line_num, line);
	success_handler(ZCLK_RES_SUCCESS, ZCLK_RESULT_STRING, line);
}

zclk_res ctr_logs_cmd_handler(zclk_command* cmd, void *handler_args)
{
	int quiet = 0;
	docker_context *ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(cmd->args);
	if (len != 1)
	{
		cmd->error_handler(ZCLK_RES_ERR_UNKNOWN, ZCLK_RESULT_STRING,
					  "Container not provided.");
		return ZCLK_RES_ERR_UNKNOWN;
	}
	else
	{
		zclk_argument *container_arg =
				(zclk_argument *)arraylist_get(cmd->args, 0);
		char *container = zclk_argument_get_val_string(container_arg);
		char *log;
		size_t log_len;
		d_err_t e = docker_container_logs(ctx, &log, &log_len, container, 0, 1, 1, -1, -1, 1,
										  10);
		if (e == E_SUCCESS)
		{
			char res_str[1024];
			sprintf(res_str, "Logs for container %s", container);
			cmd->success_handler(ZCLK_RES_SUCCESS, ZCLK_RESULT_STRING, res_str);
			// success_handler(ZCLK_RES_SUCCESS, ZCLK_RESULT_STRING, log);
			docker_container_logs_foreach(cmd->success_handler, 
					log, log_len, &cld_log_line_handler);
		}
	}
	return ZCLK_RES_SUCCESS;
}

zclk_res ctr_top_cmd_handler(zclk_command* cmd, void *handler_args)
{
	json_object *obj;
	zclk_res err = execute_lua_command(&obj, "ctr", "top", handler_args,
			cmd->options, cmd->args, cmd->success_handler, cmd->error_handler);
	if (obj != NULL)
	{
		docker_log_debug("Received json object -> %s\n", get_json_string(obj));
	}
	return err;
}

// zclk_res ctr_top_cmd_handler(void* handler_args, arraylist* options,
// 	arraylist* args, zclk_command_output_handler success_handler,
// 	zclk_command_output_handler error_handler) {
// 	int quiet = 0;
// 	docker_context* ctx = get_docker_context(handler_args);
// 	size_t len = arraylist_length(args);
// 	if (len != 1) {
// 		error_handler(ZCLK_RES_ERR_UNKNOWN, ZCLK_RESULT_STRING,
// 			"Container not provided.");
// 		return ZCLK_RES_ERR_UNKNOWN;
// 	}
// 	else {
// 		zclk_argument* container_arg = (zclk_argument*)arraylist_get(args,
// 			0);
// 		char* container = container_arg->val->str_value;
// 		docker_container_ps* ps;
// 		d_err_t e = docker_process_list_container(ctx, &ps, container, NULL);
// 		if (e == E_SUCCESS) {
// 			char res_str[1024];
// 			sprintf(res_str, "Process list for container %s", container);
// 			success_handler(ZCLK_RES_SUCCESS, ZCLK_RESULT_STRING, res_str);

// 			zclk_table* ctr_tbl;
// 			size_t num_labels = arraylist_length(ps->titles);
// 			size_t num_processes = arraylist_length(ps->processes);
// 			if (create_zclk_table(&ctr_tbl, num_processes, num_labels) == 0) {
// 				for (size_t i = 0; i < num_labels; i++) {
// 					zclk_table_set_header(ctr_tbl, i,
// 						(char*)arraylist_get(ps->titles, i));
// 				}
// 				for (size_t i = 0; i < num_processes; i++) {
// 					arraylist* psvals = (arraylist*)arraylist_get(
// 						ps->processes, i);
// 					for (size_t j = 0; j < num_labels; j++) {
// 						zclk_table_set_row_val(ctr_tbl, i, j,
// 							(char*)arraylist_get(psvals, j));
// 					}
// 				}
// 				success_handler(ZCLK_RES_SUCCESS, ZCLK_RESULT_TABLE, ctr_tbl);
// 			}
// 		}
// 	}
// 	return ZCLK_RES_SUCCESS;
// }

zclk_res ctr_remove_cmd_handler(zclk_command* cmd, void *handler_args)
{
	int quiet = 0;
	docker_context *ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(cmd->args);
	if (len != 1)
	{
		cmd->error_handler(ZCLK_RES_ERR_UNKNOWN, ZCLK_RESULT_STRING,
					  "Container not provided.");
		return ZCLK_RES_ERR_UNKNOWN;
	}
	else
	{
		zclk_argument *container_arg =
				(zclk_argument *)arraylist_get(cmd->args, 0);
		char *container = zclk_argument_get_val_string(container_arg);
		d_err_t e = docker_remove_container(ctx, container, 0, 0, 0);
		if (e == E_SUCCESS)
		{
			char res_str[1024];
			sprintf(res_str, "Removed container %s", container);
			cmd->success_handler(ZCLK_RES_SUCCESS, ZCLK_RESULT_STRING, res_str);
		}
	}
	return ZCLK_RES_SUCCESS;
}

typedef struct stats_args_t
{
	char *id;
	char *name;
	zclk_command_output_handler success_handler;
} stats_args;

void docker_container_stats_cb(docker_container_stats *stats, void *cbargs)
{
	stats_args *sarg = (stats_args *)cbargs;
	zclk_table *ctr_tbl;
	if (create_zclk_table(&ctr_tbl, 1, 8) == 0)
	{
		zclk_table_set_header(ctr_tbl, 0, "CONTAINER ID");
		zclk_table_set_header(ctr_tbl, 1, "NAME");
		zclk_table_set_header(ctr_tbl, 2, "CPU %");
		zclk_table_set_header(ctr_tbl, 3, "MEM USAGE / LIMIT");
		zclk_table_set_header(ctr_tbl, 4, "MEM %");
		zclk_table_set_header(ctr_tbl, 5, "NET I/O");
		zclk_table_set_header(ctr_tbl, 6, "BLOCK I/O");
		zclk_table_set_header(ctr_tbl, 7, "PIDS");

		zclk_table_set_row_val(ctr_tbl, 0, 0, sarg->id);
		zclk_table_set_row_val(ctr_tbl, 0, 1, sarg->name);

		char cpu_usg_str[128];
		sprintf(cpu_usg_str, "%ld", docker_container_cpu_stats_cpu_usage_get(docker_container_stats_cpu_stats_get(stats)));
		zclk_table_set_row_val(ctr_tbl, 0, 2, cpu_usg_str);

		char mem_usg_str[128];
		sprintf(mem_usg_str, "%ld", docker_container_mem_stats_usage_get(docker_container_stats_mem_stats_get(stats)));
		zclk_table_set_row_val(ctr_tbl, 0, 3, mem_usg_str);

		zclk_table_set_row_val(ctr_tbl, 0, 4, "");
		zclk_table_set_row_val(ctr_tbl, 0, 5, "");
		zclk_table_set_row_val(ctr_tbl, 0, 6, "");
		zclk_table_set_row_val(ctr_tbl, 0, 7, "");
	}
	sarg->success_handler(ZCLK_RES_IS_RUNNING, ZCLK_RESULT_STRING,
						  "\033[0;0H\033[2J");
	sarg->success_handler(ZCLK_RES_IS_RUNNING, ZCLK_RESULT_TABLE, ctr_tbl);
}

zclk_res ctr_stats_cmd_handler(zclk_command* cmd, void *handler_args)
{
	int quiet = 0;
	docker_context *ctx = get_docker_context(handler_args);
	size_t len = arraylist_length(cmd->args);
	if (len != 1)
	{
		cmd->error_handler(ZCLK_RES_ERR_UNKNOWN, ZCLK_RESULT_STRING,
					  "Container not provided.");
		return ZCLK_RES_ERR_UNKNOWN;
	}
	else
	{
		zclk_argument *container_arg =
				(zclk_argument *)arraylist_get(cmd->args, 0);
		char *container = zclk_argument_get_val_string(container_arg);
		stats_args *sarg = (stats_args *)calloc(1, sizeof(stats_args));
		sarg->id = container;
		sarg->name = container;
		sarg->success_handler = cmd->success_handler;
		d_err_t e = docker_container_get_stats_cb(ctx, &docker_container_stats_cb,
												  sarg, container);
		if (e == E_SUCCESS)
		{
			char res_str[1024];
			sprintf(res_str, "done %s", container);
			cmd->success_handler(ZCLK_RES_SUCCESS, ZCLK_RESULT_STRING, res_str);
		}
	}
	return ZCLK_RES_SUCCESS;
}

zclk_command *ctr_commands()
{
	zclk_command *container_command = new_zclk_command("container", "ctr",
			"Docker Container Commands", NULL);
	if(container_command != NULL)
	{
		zclk_command *ctr_command = new_zclk_command("list", "ls", "Docker Container List",
									 &ctr_ls_cmd_handler);
		if(ctr_command != NULL)
		{
			zclk_command_flag_option(ctr_command, "all", "a", 0, "Show all containers (by default shows only running ones).");
			zclk_command_string_option(ctr_command, "filter", "f", NULL, "Filter output based on conditions provided");
			zclk_command_string_option(ctr_command, "format", NULL, NULL, "Pretty-print containers using a Go template");
			zclk_command_int_option(ctr_command, "last", "n", 10, "Show n last created containers (includes all states)");
			zclk_command_flag_option(ctr_command, "latest", "l", 0, "Show the latest created container (includes all states)");
			zclk_command_flag_option(ctr_command, "no-trunc", NULL, 0, "Don't truncate output");
			zclk_command_flag_option(ctr_command, "quiet", "q", 0, "Only display numeric IDs");
			zclk_command_flag_option(ctr_command, "size", "s", 0, "Display total file sizes");
			zclk_command_subcommand_add(container_command, ctr_command);
		}

		ctr_command = new_zclk_command("create", "create",
									 "Docker Container Create", &ctr_create_cmd_handler);
		if(ctr_command != NULL)
		{
			zclk_command_string_argument(ctr_command, "Image Name", 
				NULL, "Name of Docker Image to use.", 1);
			zclk_command_subcommand_add(container_command, ctr_command);
		}

		ctr_command = new_zclk_command("start", "on",
									 "Docker Container Start", &ctr_start_cmd_handler);
		if(ctr_command != NULL)
		{
			zclk_command_string_argument(ctr_command, "Container", NULL,
										"Name of container to start.", 1);;
			zclk_command_subcommand_add(container_command, ctr_command);
		}

		ctr_command = new_zclk_command("stop", "off",
									 "Docker Container Stop", &ctr_stop_cmd_handler);
		if(ctr_command != NULL)
		{
			zclk_command_string_argument(ctr_command, "Container", NULL,
										"Name of container to stop.", 1);;
			zclk_command_subcommand_add(container_command, ctr_command);
		}

		ctr_command = new_zclk_command("restart", "restart",
									 "Docker Container Restart", &ctr_restart_cmd_handler);
		if(ctr_command != NULL)
		{
			zclk_command_string_argument(ctr_command, "Container", NULL,
										"Name of container to restart.", 1);;
			zclk_command_subcommand_add(container_command, ctr_command);
		}

		ctr_command = new_zclk_command("kill", "kill",
									 "Docker Container Kill", &ctr_kill_cmd_handler);
		if(ctr_command != NULL)
		{
			zclk_command_string_argument(ctr_command, "Container", NULL,
										"Name of container to kill.", 1);;
			zclk_command_subcommand_add(container_command, ctr_command);
		}

		ctr_command = new_zclk_command("rename", "ren",
									 "Docker Container Rename", &ctr_ren_cmd_handler);
		if(ctr_command != NULL)
		{
			zclk_command_string_argument(ctr_command, "Container", NULL,
										"Name of container to rename.", 1);;
			zclk_command_string_argument(ctr_command, "Name", NULL,
										"New name of container.", 1);;
			zclk_command_subcommand_add(container_command, ctr_command);
		}

		ctr_command = new_zclk_command("pause", "pause",
									 "Docker Container Pause", &ctr_pause_cmd_handler);
		if(ctr_command != NULL)
		{
			zclk_command_string_argument(ctr_command, "Container", NULL,
										"Name of container to pause.", 1);;
			zclk_command_subcommand_add(container_command, ctr_command);
		}

		ctr_command = new_zclk_command("unpause", "unpause",
									 "Docker Container UnPause", &ctr_unpause_cmd_handler);
		if(ctr_command != NULL)
		{
			zclk_command_string_argument(ctr_command, "Container", NULL,
										"Name of container to unpause.", 1);;
			zclk_command_subcommand_add(container_command, ctr_command);
		}

		ctr_command = new_zclk_command("wait", "wait",
									 "Docker Container Wait", &ctr_wait_cmd_handler);
		if(ctr_command != NULL)
		{
			zclk_command_string_argument(ctr_command, "Container", NULL,
										"Name of container to wait.", 1);;
			zclk_command_subcommand_add(container_command, ctr_command);
		}

		ctr_command = new_zclk_command("logs", "lg",
									 "Docker Container Logs", &ctr_logs_cmd_handler);
		if(ctr_command != NULL)
		{
			zclk_command_string_argument(ctr_command, "Container", NULL,
										"Name of container.", 1);;
			zclk_command_subcommand_add(container_command, ctr_command);
		}

		ctr_command = new_zclk_command("top", "ps",
									 "Docker Container Top/PS", &ctr_top_cmd_handler);
		if(ctr_command != NULL)
		{
			zclk_command_string_argument(ctr_command, "Container", NULL,
										"Name of container.", 1);;
			zclk_command_subcommand_add(container_command, ctr_command);
		}

		ctr_command = new_zclk_command("remove", "del",
									 "Docker Remove Container", &ctr_remove_cmd_handler);
		if(ctr_command != NULL)
		{
			zclk_command_string_argument(ctr_command, "Container", NULL,
										"Name of container.", 1);;
			zclk_command_subcommand_add(container_command, ctr_command);
		}

		ctr_command = new_zclk_command("stats", "stats",
									 "Docker Container Stats", &ctr_stats_cmd_handler);
		if(ctr_command != NULL)
		{
			zclk_command_string_argument(ctr_command, "Container", NULL,
										"Name of container.", 1);;
			zclk_command_subcommand_add(container_command, ctr_command);
		}
	}
	return container_command;
}
