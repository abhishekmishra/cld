// Copyright (c) 2020 Abhishek Mishra
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef SRC_CLD_LUA_H_
#define SRC_CLD_LUA_H_

#include "cld_common.h"

/**
 * Execute a lua function representing a docker command.
 * The command is passed arguments identical to the C command handlers.
 */
cld_cmd_err execute_lua_command(const char* command_name, void* handler_args, 
    arraylist* options,	arraylist* args, cld_command_output_handler success_handler,
    cld_command_output_handler error_handler);

#endif //SRC_CLD_LUA_H_