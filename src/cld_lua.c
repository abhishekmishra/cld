// Copyright (c) 2020 Abhishek Mishra
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "cld_lua.h"
#include <docker_log.h>
#include "lua_docker.h"
#include <json-c/json_object.h>

static lua_State *L;

// https://stackoverflow.com/questions/56230859/how-to-properly-print-error-messages-from-lual-dostring
bool doString(const char *s) {
    const int ret = luaL_dostring(L, s);

    if (ret != LUA_OK) {
      printf("Error: %s\n", lua_tostring(L, -1));
      lua_pop(L, 1); // pop error message
      return false;
    }

    return true;
}

cli_cmd_err start_lua_interpreter()
{
    docker_log_debug("Starting LUA interpreter...\n");
    L = luaL_newstate();
    luaL_openlibs(L);

    //Load the cld_cmd library
    doString("CLD = require('cld')");

    //Load the luaclibdocker library
    doString("docker = require('luaclibdocker')");

    //execute a dummy command to ensure all is well.
    //execute_lua_command("ctr", "dummy", NULL, NULL, NULL, NULL, NULL);

    return CLI_COMMAND_SUCCESS;
}

cli_cmd_err lua_set_docker_context(docker_context* ctx, int loglevel) {
    docker_log_debug("Setting docker context");
    DockerClient_from_context(L, ctx);
    lua_setglobal(L, "d");

    char* cmdStr = (char*)calloc(1024, sizeof(char));
    if(cmdStr != NULL) {
        sprintf(cmdStr, "d:set_loglevel(%d)", loglevel);
        luaL_dostring(L, cmdStr);
        free(cmdStr);
    }

    // create CLD instance
    doString("cld = CLD:new(d)");

    return CLI_COMMAND_SUCCESS;
}

cli_cmd_err stop_lua_interpreter()
{
    docker_log_debug("Stopping LUA interpreter...\n");
    lua_close(L);

    return CLI_COMMAND_SUCCESS;
}

/**
 * Execute a lua function representing a docker command.
 * The command is passed arguments identical to the C command handlers.
 */
cli_cmd_err execute_lua_command(json_object** res, const char* module_name, const char *command_name, void *handler_args,
                                arraylist *options, arraylist *args, cli_command_output_handler success_handler,
                                cli_command_output_handler error_handler)
{
    //function name is cld:run(module, command, options, args)
    lua_getglobal(L, "cld");
    lua_getfield(L, -1, "run");
    // lua_getfield(L, -1, command_name);

    //first arg is class instance, to mimic the `:` call
    lua_getglobal(L, "cld"); 

    lua_pushstring(L, module_name);
    lua_pushstring(L, command_name);
    //lua_getfield(L, -1, module_name);
    //lua_getfield(L, -1, command_name);

    //second arg is command options
    if (options == NULL) {
        lua_pushnil(L);
    }
    else {
        size_t len = arraylist_length(options);
        lua_createtable(L, 0, len);
        for (size_t i = 0; i < len; i++) {
            cli_option* o = (cli_option*)arraylist_get(options, i);
            options->convert_to_lua(L, i, o);
            lua_setfield(L, -2, o->name);
        }
    }

    //third arg is command args
    if (args == NULL) {
        lua_pushnil(L);
    }
    else {
        size_t len = arraylist_length(args);
        lua_createtable(L, 0, len);
        for (size_t i = 0; i < len; i++) {
            cli_option* o = (cli_option*)arraylist_get(args, i);
            args->convert_to_lua(L, i, o);
            lua_setfield(L, -2, o->name);
        }
    }

    /* do the call (4 arguments, 1 result) */
    if (lua_pcall(L, 5, 1, 0) != 0)
    {
        luaL_error(L, "error running function '%s': %s", command_name,
              lua_tostring(L, -1));
        return CLI_COMMAND_ERR_UNKNOWN;
    }

    char* result_str = lua_tostring(L, -1);
    *res = json_tokener_parse(result_str);

    return CLI_COMMAND_SUCCESS;
}
