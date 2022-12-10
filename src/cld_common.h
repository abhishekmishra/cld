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

#ifndef SRC_CLD_COMMON_H_
#define SRC_CLD_COMMON_H_

#include "docker_connection_util.h"
#include "cliutils.h"

docker_context *get_docker_context(void *handler_args);

void handle_docker_error(docker_result *res,
						 cli_command_output_handler success_handler,
						 cli_command_output_handler error_handler);

#endif /* SRC_CLD_COMMON_H_ */
