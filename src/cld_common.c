/*
 * cld_common.c
 *
 *  Created on: 17-Feb-2019
 *      Author: abhis
 */

#include "cld_common.h"
#include "docker_all.h"

docker_context* get_docker_context(void* handler_args) {
	docker_context* ctx = (docker_context*) handler_args;
	return ctx;
}

void handle_docker_error(docker_result* res,
		cld_command_output_handler success_handler,
		cld_command_output_handler error_handler) {
	char res_str[1024];
	sprintf(res_str,"\nURL: %s\n", res->url);
	success_handler(CLD_COMMAND_IS_RUNNING, CLD_RESULT_STRING, res_str);
	if (!is_ok(res)) {
		printf("DOCKER RESULT: Response error_code = %d, http_response = %ld\n",
				res->error_code, res->http_error_code);
		sprintf(res_str, "ERROR: %s\n", res->message);
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING, res_str);
	} else {
		if(res->message) {
			success_handler(CLD_COMMAND_IS_RUNNING, CLD_RESULT_STRING, res->message);
		}
	}
	free_docker_result(&res);
}
