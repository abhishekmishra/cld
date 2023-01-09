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

#ifndef SRC_CLD_LUA_H_
#define SRC_CLD_LUA_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include <json-c/json_object.h>
#include <docker_all.h>
#include <lua.h>
#include <lauxlib.h>
#include <zclk.h>

zclk_cmd_err start_lua_interpreter();

zclk_cmd_err lua_set_docker_context(docker_context *ctx, int loglevel);

zclk_cmd_err stop_lua_interpreter();

/**
 * Execute a lua function representing a docker command.
 * The command is passed arguments identical to the C command handlers.
 */
zclk_cmd_err execute_lua_command(json_object **res, const char *module_name, const char *command_name, void *handler_args,
                                arraylist *options, arraylist *args, zclk_command_output_handler success_handler,
                                zclk_command_output_handler error_handler);

#ifdef __cplusplus
}
#endif

#endif // SRC_CLD_LUA_H_