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

#include "docker_all.h"
#include "cld_common.h"
#include <zclk.h>
// #include "cld_sys.h"
// #include "cld_ctr.h"
// #include "cld_img.h"
#include "cld_vol.h"
// #include "cld_net.h"
#include "cld_lua.h"
#include <coll_arraylist.h>

#define CMD_NOT_FOUND -1

#define ZCLK_OPTION_MAIN_CONFIG_LONG "config"
#define ZCLK_OPTION_MAIN_CONFIG_SHORT "c"
#define ZCLK_OPTION_MAIN_CONFIG_DESC "Client Config"

#define ZCLK_OPTION_MAIN_DEBUG_LONG "debug"
#define ZCLK_OPTION_MAIN_DEBUG_SHORT "D"
#define ZCLK_OPTION_MAIN_DEBUG_DESC "Debug Mode"

#define ZCLK_OPTION_MAIN_LOG_LEVEL_LONG "loglevel"
#define ZCLK_OPTION_MAIN_LOG_LEVEL_SHORT "l"
#define ZCLK_OPTION_MAIN_LOG_LEVEL_DESC "Set the Log Level (\"debug\"|\"info\"|\"warn\"|\"error\"|\"fatal\") (default \"info\")"

#define CLD_LOGLEVEL_DEBUG "debug"
#define CLD_LOGLEVEL_INFO "info"
#define CLD_LOGLEVEL_WARN "warn"
#define CLD_LOGLEVEL_ERROR "error"
#define CLD_LOGLEVEL_FATAL "fatal"

#define ZCLK_OPTION_MAIN_TLS_LONG "tls"
#define ZCLK_OPTION_MAIN_TLS_SHORT NULL
#define ZCLK_OPTION_MAIN_TLS_DESC "Enable tls"

#define ZCLK_OPTION_MAIN_TLSCACERT_LONG "tlscacert"
#define ZCLK_OPTION_MAIN_TLSCACERT_SHORT NULL
#define ZCLK_OPTION_MAIN_TLSCACERT_DESC "Set TLS CA Certificate"

#define ZCLK_OPTION_MAIN_TLSCERT_LONG "tlscert"
#define ZCLK_OPTION_MAIN_TLSCERT_SHORT NULL
#define ZCLK_OPTION_MAIN_TLSCERT_DESC "Set TLS Certificate"

#define ZCLK_OPTION_MAIN_TLSKEY_LONG "tlskey"
#define ZCLK_OPTION_MAIN_TLSKEY_SHORT NULL
#define ZCLK_OPTION_MAIN_TLSKEY_DESC "Set TLS Key"

#define ZCLK_OPTION_MAIN_TLSVERIFY_LONG "tlsverify"
#define ZCLK_OPTION_MAIN_TLSVERIFY_SHORT NULL
#define ZCLK_OPTION_MAIN_TLSVERIFY_DESC "Enable TLS Verify"

#define ZCLK_OPTION_MAIN_INTERACTIVE_LONG "interactive"
#define ZCLK_OPTION_MAIN_INTERACTIVE_SHORT "i"
#define ZCLK_OPTION_MAIN_INTERACTIVE_DESC "Show REPL"

#define ZCLK_OPTION_MAIN_HOST_LONG "host"
#define ZCLK_OPTION_MAIN_HOST_SHORT "H"
#define ZCLK_OPTION_MAIN_HOST_DESC "Set Docker Host"

#define ZCLK_OPTION_MAIN_VERSION_LONG "version"
#define ZCLK_OPTION_MAIN_VERSION_SHORT "v"
#define ZCLK_OPTION_MAIN_VERSION_DESC "Show CLD version."

static char *main_command_name;
static docker_context *ctx;
static bool connected = false;
static arraylist *CLI_COMMANDS;
static int loglevel = LOG_ERROR;

void docker_result_handler(docker_context *ctx, docker_result *res)
{
	handle_docker_error(res, (zclk_command_output_handler)&print_handler, (zclk_command_output_handler)&print_handler);
}

zclk_cmd_err main_cmd_handler(zclk_command* cmd, void *handler_args)
{
	zclk_option *debug_option = get_option_by_name(cmd->options, ZCLK_OPTION_MAIN_LOG_LEVEL_LONG);
	if (zclk_option_get_val_string(debug_option) != NULL)
	{
		if (strcmp(zclk_option_get_val_string(debug_option), CLD_LOGLEVEL_DEBUG) == 0)
		{
			docker_log_set_level(LOG_DEBUG);
			loglevel = LOG_DEBUG;
		}
		if (strcmp(zclk_option_get_val_string(debug_option), CLD_LOGLEVEL_INFO) == 0)
		{
			docker_log_set_level(LOG_INFO);
			loglevel = LOG_INFO;
		}
		if (strcmp(zclk_option_get_val_string(debug_option), CLD_LOGLEVEL_WARN) == 0)
		{
			docker_log_set_level(LOG_WARN);
			loglevel = LOG_WARN;
		}
		if (strcmp(zclk_option_get_val_string(debug_option), CLD_LOGLEVEL_ERROR) == 0)
		{
			docker_log_set_level(LOG_ERROR);
			loglevel = LOG_ERROR;
		}
		if (strcmp(zclk_option_get_val_string(debug_option), CLD_LOGLEVEL_FATAL) == 0)
		{
			docker_log_set_level(LOG_FATAL);
			loglevel = LOG_FATAL;
		}
	}

	zclk_option *host_option = get_option_by_name(cmd->options, ZCLK_OPTION_MAIN_HOST_LONG);

	if (!connected)
	{
		char *url;

		if (zclk_option_get_val_string(host_option) == NULL)
		{
			{
				if (make_docker_context_default_local(&ctx) == E_SUCCESS)
				{
					url = str_clone(ctx->url);
					connected = 1;
				}
			}
		}
		else
		{
			url = zclk_option_get_val_string(host_option);
			if (is_http_url(url))
			{
				if (make_docker_context_url(&ctx, url) == E_SUCCESS)
				{
					connected = true;
				}
			}
			else if (is_unix_socket(url))
			{
				if (make_docker_context_url(&ctx, url) == E_SUCCESS)
				{
					connected = true;
				}
			}
		}

		if (connected)
		{
			lua_set_docker_context(ctx, loglevel);
			docker_context_result_handler_set(ctx, (docker_result_handler_fn *)&docker_result_handler);
			// if (docker_ping(ctx) != E_SUCCESS)
			//{
			//	docker_log_fatal("Could not ping the server %s", url);
			//	connected = 0;
			// }
			// else
			//{
			//	docker_log_debug("%s is alive.", url);
			// }
		}
	}

	if (!connected)
	{
		exit(-1);
	}

	return ZCLK_COMMAND_SUCCESS;
}

zclk_command *create_main_command()
{
	zclk_command *main_command = new_zclk_command(main_command_name, "cld",
												 "CLD Docker Client",
												 &main_cmd_handler);
	
	if (main_command != NULL)
	{
		zclk_command_string_option(main_command, ZCLK_OPTION_MAIN_CONFIG_LONG,
								   ZCLK_OPTION_MAIN_CONFIG_SHORT, NULL, NULL, ZCLK_OPTION_MAIN_CONFIG_DESC);
		zclk_command_string_option(main_command, ZCLK_OPTION_MAIN_DEBUG_LONG,
								   ZCLK_OPTION_MAIN_DEBUG_SHORT, 0, 0, ZCLK_OPTION_MAIN_DEBUG_DESC);
		zclk_command_string_option(main_command, ZCLK_OPTION_MAIN_LOG_LEVEL_LONG,
								   ZCLK_OPTION_MAIN_LOG_LEVEL_SHORT, NULL, NULL, ZCLK_OPTION_MAIN_LOG_LEVEL_DESC);
		zclk_command_string_option(main_command, ZCLK_OPTION_MAIN_TLS_LONG,
								   ZCLK_OPTION_MAIN_TLS_SHORT, 0, 0, ZCLK_OPTION_MAIN_TLS_DESC);
		zclk_command_string_option(main_command, ZCLK_OPTION_MAIN_TLSCACERT_LONG,
								   ZCLK_OPTION_MAIN_TLSCACERT_SHORT, NULL, NULL, ZCLK_OPTION_MAIN_TLSCACERT_DESC);
		zclk_command_string_option(main_command, ZCLK_OPTION_MAIN_TLSCERT_LONG,
								   ZCLK_OPTION_MAIN_TLSCERT_SHORT, NULL, NULL, ZCLK_OPTION_MAIN_TLSCERT_DESC);
		zclk_command_string_option(main_command, ZCLK_OPTION_MAIN_TLSKEY_LONG,
								   ZCLK_OPTION_MAIN_TLSKEY_SHORT, NULL, NULL, ZCLK_OPTION_MAIN_TLSKEY_DESC);
		zclk_command_string_option(main_command, ZCLK_OPTION_MAIN_TLSVERIFY_LONG,
								   ZCLK_OPTION_MAIN_TLSVERIFY_SHORT, 0, 0, ZCLK_OPTION_MAIN_TLSVERIFY_DESC);
		zclk_command_string_option(main_command, ZCLK_OPTION_MAIN_INTERACTIVE_LONG,
								   ZCLK_OPTION_MAIN_INTERACTIVE_SHORT, 0, 0, ZCLK_OPTION_MAIN_INTERACTIVE_DESC);
		zclk_command_string_option(main_command, ZCLK_OPTION_MAIN_HOST_LONG,
								   ZCLK_OPTION_MAIN_HOST_SHORT, NULL, NULL, ZCLK_OPTION_MAIN_HOST_DESC);
		zclk_command_string_option(main_command, ZCLK_OPTION_MAIN_VERSION_LONG,
								   ZCLK_OPTION_MAIN_VERSION_SHORT, 0, 0, ZCLK_OPTION_MAIN_VERSION_DESC);

		// arraylist_add(main_command->sub_commands, sys_commands());
		// arraylist_add(main_command->sub_commands, ctr_commands());
		// arraylist_add(main_command->sub_commands, img_commands());
		arraylist_add(main_command->sub_commands, vol_commands());
		// arraylist_add(main_command->sub_commands, net_commands());
		return main_command;
	}
	return NULL;
}

arraylist *create_commands()
{
	int err = arraylist_new(&CLI_COMMANDS, (void (*)(void *)) & free_command);
	if (err == 0)
	{
		arraylist_add(CLI_COMMANDS, create_main_command());
	}
	else
	{
		printf("Error creating commands list");
	}
	return CLI_COMMANDS;
}

int main(int argc, char *argv[])
{
	docker_log_set_level(LOG_INFO);

	d_err_t res = docker_api_init();

	if (res == 0)
	{
		if (argc > 0)
		{
			docker_log_debug("command name is %s\n", argv[0]);
			main_command_name = argv[0];

			start_lua_interpreter();


			zclk_cmd_err err = zclk_command_exec(create_main_command(), &ctx, argc, argv);

			if (err != ZCLK_COMMAND_SUCCESS)
			{
				docker_log_error("Error: invalid command.\n");
			}

			stop_lua_interpreter();
		}
	}
	else
	{
		printf("Docker API initialization error!\n");
	}

	docker_api_cleanup();

	return 0;
}
