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

#include <string.h>
#include "cld_common.h"
#include "docker_all.h"

docker_context *get_docker_context(void *handler_args)
{
	docker_context **ctx = (docker_context **)handler_args;
	return *ctx;
}

void handle_docker_error(docker_result *res,
						 zclk_command_output_handler success_handler,
						 zclk_command_output_handler error_handler)
{
	char res_str[4096];
	memset(res_str, NULL, 4096);
	// sprintf(res_str,"\nURL: %s\n", res->url);
	success_handler(ZCLK_RES_IS_RUNNING, ZCLK_RESULT_STRING, res_str);
	if (!is_ok(res))
	{
		printf("DOCKER RESULT: Response error_code = %d, http_response = %ld\n",
			   res->error_code, res->http_error_code);
		sprintf(res_str, "ERROR: %s\n", res->message);
		error_handler(ZCLK_RES_ERR_UNKNOWN, ZCLK_RESULT_STRING, res_str);
	}
	else
	{
		if (res->message)
		{
			success_handler(ZCLK_RES_IS_RUNNING, ZCLK_RESULT_STRING, res->message);
		}
	}
}
