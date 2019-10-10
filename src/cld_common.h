/*
 * cld_common.h
 *
 *  Created on: 17-Feb-2019
 *      Author: abhis
 */

#ifndef SRC_CLD_COMMON_H_
#define SRC_CLD_COMMON_H_

#include "docker_connection_util.h"
#include "cld_command.h"

docker_context* get_docker_context(void* handler_args);

void handle_docker_error(docker_result* res,
		cld_command_output_handler success_handler,
		cld_command_output_handler error_handler);

#endif /* SRC_CLD_COMMON_H_ */
