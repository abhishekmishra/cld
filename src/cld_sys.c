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
#include "cld_sys.h"
#include "zclk_dict.h"

zclk_res sys_version_cmd_handler(zclk_command* cmd, void *handler_args)
{
	docker_version *version = NULL;
	docker_context *ctx = get_docker_context(handler_args);
	d_err_t err = docker_system_version(ctx, &version);
	if (err == E_SUCCESS)
	{
		zclk_dict *ver_dict;
		if (create_zclk_dict(&ver_dict) == 0)
		{
			zclk_dict_put(ver_dict, "Docker Version", docker_version_version_get(version));
			zclk_dict_put(ver_dict, "OS", docker_version_os_get(version));
			zclk_dict_put(ver_dict, "Kernel", docker_version_kernel_version_get(version));
			zclk_dict_put(ver_dict, "Arch", docker_version_arch_get(version));
			zclk_dict_put(ver_dict, "API Version", docker_version_api_version_get(version));
			zclk_dict_put(ver_dict, "Min API Version", docker_version_min_api_version_get(version));
			zclk_dict_put(ver_dict, "Go Version", docker_version_go_version_get(version));
			zclk_dict_put(ver_dict, "Git Commit", docker_version_git_commit_get(version));
			zclk_dict_put(ver_dict, "Build Time", docker_version_build_time_get(version));
			zclk_dict_put(ver_dict, "Experimental",
						 docker_version_experimental_get(version) == 0 ? "False" : "True");

			cmd->success_handler(ZCLK_RES_SUCCESS, ZCLK_RESULT_DICT, ver_dict);

			free_zclk_dict(ver_dict);
		}

		if (version != NULL)
		{
			free_docker_version(version);
		}
	}
	return ZCLK_RES_SUCCESS;
}

zclk_res sys_connection_cmd_handler(zclk_command* cmd, void *handler_args)
{
	docker_context *ctx = get_docker_context(handler_args);
	if (ctx->url)
	{
		printf("Connected to URL: %s\n", ctx->url);
	}
	return ZCLK_RES_SUCCESS;
}

void docker_events_cb(docker_event *event, void *cbargs)
{
	zclk_command_output_handler success_handler = (zclk_command_output_handler)cbargs;
	char *content = (char *)calloc(2048, sizeof(char));
	strcat(content, "");
	time_t x = (time_t)(docker_event_time_get(event));
	struct tm *timeinfo = localtime(&x);
	char evt_time_str[256];
	// sprintf(evt_time_str, "%s", asctime(timeinfo));
	strftime(evt_time_str, 255, "%d-%m-%Y:%H:%M:%S", timeinfo);
	strcat(content, evt_time_str);
	strcat(content, ": ");
	strcat(content, docker_event_type_get(event));
	strcat(content, " | ");
	strcat(content, docker_event_action_get(event));
	//		strcat(content, "</td><td>");
	//
	strcat(content, " | ");
	strcat(content, docker_event_actor_id_get(event));
	//		strcat(content, "</td><td>");
	//
	//		strcat(content, json_object_get_string(event->actor_attributes));
	strcat(content, "");
	success_handler(ZCLK_RES_SUCCESS, ZCLK_RESULT_STRING, content);
}

zclk_res sys_events_cmd_handler(zclk_command* cmd, void *handler_args)
{
	docker_result *res;
	docker_context *ctx = get_docker_context(handler_args);

	arraylist *events;
	time_t now = time(NULL);
	d_err_t err = docker_system_events_cb(ctx, &docker_events_cb, 
		cmd->success_handler, &events, now - (3600 * 24), 0);
	
	if (err == E_SUCCESS)
	{
		cmd->success_handler(ZCLK_RES_SUCCESS, ZCLK_RESULT_STRING, "done.");
	}
	return ZCLK_RES_SUCCESS;
}

zclk_command *sys_commands()
{
	zclk_command *system_command = new_zclk_command("system", "sys", 
				"Docker System Commands", NULL);
	if(system_command != NULL)
	{
		zclk_command *sysver_command = new_zclk_command("version", "ver",
				"Docker System Version", &sys_version_cmd_handler);
		if(sysver_command != NULL)
		{
			zclk_command_subcommand_add(system_command, sysver_command);
		}
		zclk_command *syscon_command = new_zclk_command("connection", "con",
				"Docker System Connection", &sys_connection_cmd_handler);
		if(syscon_command != NULL)
		{
			zclk_command_subcommand_add(system_command, syscon_command);
		}
		zclk_command *sysevt_command = new_zclk_command("events", "evt",
				"Docker System Connection", &sys_events_cmd_handler);
		if(sysevt_command != NULL)
		{
			zclk_command_subcommand_add(system_command, sysevt_command);
		}
	}
	return system_command;
}
