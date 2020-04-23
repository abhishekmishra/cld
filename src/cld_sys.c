/**
 * Copyright (c) 2020 Abhishek Mishra
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "docker_all.h"
#include <string.h>
#include "cld_sys.h"
#include "cli_dict.h"

cli_cmd_err sys_version_cmd_handler(void* handler_args,
		arraylist* options, arraylist* args,
		cli_command_output_handler success_handler,
		cli_command_output_handler error_handler) {
	docker_version* version = NULL;
	docker_context* ctx = get_docker_context(handler_args);
	d_err_t err = docker_system_version(ctx, &version);
	if (err == E_SUCCESS) {
		cli_dict* ver_dict;
		if (create_cli_dict(&ver_dict) == 0) {
			cli_dict_put(ver_dict, "Docker Version", docker_version_version_get(version));
			cli_dict_put(ver_dict, "OS", docker_version_os_get(version));
			cli_dict_put(ver_dict, "Kernel", docker_version_kernel_version_get(version));
			cli_dict_put(ver_dict, "Arch", docker_version_arch_get(version));
			cli_dict_put(ver_dict, "API Version", docker_version_api_version_get(version));
			cli_dict_put(ver_dict, "Min API Version", docker_version_min_api_version_get(version));
			cli_dict_put(ver_dict, "Go Version", docker_version_go_version_get(version));
			cli_dict_put(ver_dict, "Git Commit", docker_version_git_commit_get(version));
			cli_dict_put(ver_dict, "Build Time", docker_version_build_time_get(version));
			cli_dict_put(ver_dict, "Experimental",
				docker_version_experimental_get(version) == 0 ? "False" : "True");

			success_handler(CLI_COMMAND_SUCCESS, CLI_RESULT_DICT, ver_dict);

			free_cli_dict(ver_dict);
		}

		if (version != NULL) {
			free_docker_version(version);
		}
	}
	return CLI_COMMAND_SUCCESS;
}

cli_cmd_err sys_connection_cmd_handler(void* handler_args,
		arraylist* options, arraylist* args,
		cli_command_output_handler success_handler,
		cli_command_output_handler error_handler) {
	docker_context* ctx = get_docker_context(handler_args);
	if (ctx->url) {
		printf("Connected to URL: %s\n", ctx->url);
	}
	return CLI_COMMAND_SUCCESS;
}

void docker_events_cb(docker_event* event, void* cbargs) {
	cli_command_output_handler success_handler = (cli_command_output_handler)cbargs;
	char* content = (char*) calloc(2048, sizeof(char));
	strcat(content, "");
	time_t x = (time_t)(docker_event_time_get(event));
	struct tm* timeinfo = localtime(&x);
	char evt_time_str[256];
	//sprintf(evt_time_str, "%s", asctime(timeinfo));
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
	success_handler(CLI_COMMAND_SUCCESS, CLI_RESULT_STRING, content);
}

cli_cmd_err sys_events_cmd_handler(void *handler_args,
		arraylist *options, arraylist *args,
		cli_command_output_handler success_handler,
		cli_command_output_handler error_handler) {
	docker_result *res;
	docker_context *ctx = get_docker_context(handler_args);

	arraylist* events;
	time_t now = time(NULL);
	d_err_t err = docker_system_events_cb(ctx, &docker_events_cb, success_handler, &events,  now - (3600 * 24), 0);
	if (err == E_SUCCESS) {
		success_handler(CLI_COMMAND_SUCCESS, CLI_RESULT_STRING, "done.");
	}
	return CLI_COMMAND_SUCCESS;
}

cli_command* sys_commands() {
	cli_command* system_command;
	if (make_command(&system_command, "system", "sys", "Docker System Commands",
	NULL) == CLI_COMMAND_SUCCESS) {
		cli_command *sysver_command, *syscon_command, *sysevt_command;
		if (make_command(&sysver_command, "version", "ver",
				"Docker System Version", &sys_version_cmd_handler)
				== CLI_COMMAND_SUCCESS) {
			arraylist_add(system_command->sub_commands, sysver_command);
		}
		if (make_command(&syscon_command, "connection", "con",
				"Docker System Connection", &sys_connection_cmd_handler)
				== CLI_COMMAND_SUCCESS) {
			arraylist_add(system_command->sub_commands, syscon_command);
		}
		if (make_command(&sysevt_command, "events", "evt",
				"Docker System Connection", &sys_events_cmd_handler)
				== CLI_COMMAND_SUCCESS) {
			arraylist_add(system_command->sub_commands, sysevt_command);
		}
	}
	return system_command;
}

