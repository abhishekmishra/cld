/**
 * Copyright (c) 2020 Abhishek Mishra
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <string.h>
#include "cliutils.h"
#include "docker_all.h"

docker_context* get_docker_context(void* handler_args) {
	docker_context** ctx = (docker_context**) handler_args;
	return *ctx;
}

void handle_docker_error(docker_result* res,
		cli_command_output_handler success_handler,
		cli_command_output_handler error_handler) {
	char res_str[4096];
	memset(res_str, NULL, 4096);
	//sprintf(res_str,"\nURL: %s\n", res->url);
	success_handler(CLI_COMMAND_IS_RUNNING, CLI_RESULT_STRING, res_str);
	if (!is_ok(res)) {
		printf("DOCKER RESULT: Response error_code = %d, http_response = %ld\n",
				res->error_code, res->http_error_code);
		sprintf(res_str, "ERROR: %s\n", res->message);
		error_handler(CLI_COMMAND_ERR_UNKNOWN, CLI_RESULT_STRING, res_str);
	} else {
		if(res->message) {
			success_handler(CLI_COMMAND_IS_RUNNING, CLI_RESULT_STRING, res->message);
		}
	}
}
