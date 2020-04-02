// Copyright (c) 2020 Abhishek Mishra
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "cld_lua.h"

static lua_State *L;

cld_cmd_err start_lua_interpreter() {
    printf("Starting LUA interpreter...\n");
    L = luaL_newstate();
    luaL_openlibs(L);
    luaL_dostring(L, "print('Started Lua interpreter.')");
    luaL_dostring(L, "require('cld_cmd')");
    luaL_dostring(L, "docker = require('luaclibdocker')\nd=docker.connect()\noutput = cmd_ctr_ls(d)\nprint(output[1].Names[1])");
    luaL_dofile(L, "./lua/test/main.lua");
}

cld_cmd_err stop_lua_interpreter() {
    printf("Stopping LUA interpreter...\n");
    lua_close(L);
}

/**
 * Execute a lua function representing a docker command.
 * The command is passed arguments identical to the C command handlers.
 */
cld_cmd_err execute_lua_command(const char *command_name, void *handler_args,
                                arraylist *options, arraylist *args, cld_command_output_handler success_handler,
                                cld_command_output_handler error_handler)
{
    return -1;
}
