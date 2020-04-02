/**
 * Copyright (c) 2020 Abhishek Mishra
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <string.h>
#include <getopt.h>

#include "docker_all.h"
#include "cld_command.h"
#include "cld_sys.h"
#include "cld_ctr.h"
#include "cld_img.h"
#include "cld_vol.h"
#include "cld_net.h"
#include "cld_table.h"
#include "cld_dict.h"
#include "cld_progress.h"
#include "cld_lua.h"
#include <arraylist.h>

#define CMD_NOT_FOUND -1

#define CLD_OPTION_MAIN_CONFIG_LONG "config"
#define CLD_OPTION_MAIN_CONFIG_SHORT "c"
#define CLD_OPTION_MAIN_CONFIG_DESC "Client Config"

#define CLD_OPTION_MAIN_DEBUG_LONG "debug"
#define CLD_OPTION_MAIN_DEBUG_SHORT "D"
#define CLD_OPTION_MAIN_DEBUG_DESC "Debug Mode"

#define CLD_OPTION_MAIN_LOG_LEVEL_LONG "loglevel"
#define CLD_OPTION_MAIN_LOG_LEVEL_SHORT "l"
#define CLD_OPTION_MAIN_LOG_LEVEL_DESC "Set the Log Level (\"debug\"|\"info\"|\"warn\"|\"error\"|\"fatal\") (default \"info\")"

#define CLD_LOGLEVEL_DEBUG "debug"
#define CLD_LOGLEVEL_INFO "info"
#define CLD_LOGLEVEL_WARN "warn"
#define CLD_LOGLEVEL_ERROR "error"
#define CLD_LOGLEVEL_FATAL "fatal"

#define CLD_OPTION_MAIN_TLS_LONG "tls"
#define CLD_OPTION_MAIN_TLS_SHORT NULL
#define CLD_OPTION_MAIN_TLS_DESC "Enable tls"

#define CLD_OPTION_MAIN_TLSCACERT_LONG "tlscacert"
#define CLD_OPTION_MAIN_TLSCACERT_SHORT NULL
#define CLD_OPTION_MAIN_TLSCACERT_DESC "Set TLS CA Certificate"

#define CLD_OPTION_MAIN_TLSCERT_LONG "tlscert"
#define CLD_OPTION_MAIN_TLSCERT_SHORT NULL
#define CLD_OPTION_MAIN_TLSCERT_DESC "Set TLS Certificate"

#define CLD_OPTION_MAIN_TLSKEY_LONG "tlskey"
#define CLD_OPTION_MAIN_TLSKEY_SHORT NULL
#define CLD_OPTION_MAIN_TLSKEY_DESC "Set TLS Key"

#define CLD_OPTION_MAIN_TLSVERIFY_LONG "tlsverify"
#define CLD_OPTION_MAIN_TLSVERIFY_SHORT NULL
#define CLD_OPTION_MAIN_TLSVERIFY_DESC "Enable TLS Verify"

#define CLD_OPTION_MAIN_INTERACTIVE_LONG "interactive"
#define CLD_OPTION_MAIN_INTERACTIVE_SHORT "i"
#define CLD_OPTION_MAIN_INTERACTIVE_DESC "Show REPL"

#define CLD_OPTION_MAIN_HOST_LONG "host"
#define CLD_OPTION_MAIN_HOST_SHORT "H"
#define CLD_OPTION_MAIN_HOST_DESC "Set Docker Host"

#define CLD_OPTION_MAIN_VERSION_LONG "version"
#define CLD_OPTION_MAIN_VERSION_SHORT "v"
#define CLD_OPTION_MAIN_VERSION_DESC "Show CLD version."

static char *main_command_name;
static docker_context *ctx;
static bool connected = false;
static arraylist *CLD_COMMANDS;

void print_table_result(void *result)
{
	cld_table *result_tbl = (cld_table *)result;
	int *col_widths;
	col_widths = (int *)calloc(result_tbl->num_cols, sizeof(int));
	char **col_fmtspec;
	col_fmtspec = (char **)calloc(result_tbl->num_cols, sizeof(char *));
	char *header;
	char *value;
	int min_width = 4, max_width = 25;

	//calculate column widths, and create format specifiers
	for (int i = 0; i < result_tbl->num_cols; i++)
	{
		cld_table_get_header(&header, result_tbl, i);
		int col_width = strlen(header);
		for (int j = 0; j < result_tbl->num_rows; j++)
		{
			cld_table_get_row_val(&value, result_tbl, j, i);
			if (value != NULL)
			{
				if (strlen(value) > col_width)
				{
					col_width = strlen(value);
				}
			}
		}
		if (col_width < min_width)
		{
			col_width = min_width;
		}
		if (col_width > max_width)
		{
			col_width = max_width;
		}
		char *fmtspec = (char *)calloc(16, sizeof(char));
		sprintf(fmtspec, "%%-%d.%ds", (col_width + 1), col_width);
		col_widths[i] = col_width;
		col_fmtspec[i] = fmtspec;
		//printf("%d and %s\n", col_width, fmtspec);
	}

	printf("\n");
	for (int i = 0; i < result_tbl->num_cols; i++)
	{
		cld_table_get_header(&header, result_tbl, i);
		printf(col_fmtspec[i], header);
	}
	printf("\n");
	for (int i = 0; i < result_tbl->num_cols; i++)
	{
		for (int j = 0; j < col_widths[i] + 1; j++)
		{
			printf("-");
		}
	}
	printf("\n");

	for (int i = 0; i < result_tbl->num_rows; i++)
	{
		for (int j = 0; j < result_tbl->num_cols; j++)
		{
			cld_table_get_row_val(&value, result_tbl, i, j);
			if (value == NULL)
			{
				printf(col_fmtspec[j], "");
			}
			else
			{
				printf(col_fmtspec[j], value);
			}
		}
		printf("\n");
	}
	printf("\n");

	for (int i = 0; i < result_tbl->num_cols; i++)
	{
		free(col_fmtspec[i]);
	}
}

cld_cmd_err print_handler(cld_cmd_err result_flag, cld_result_type res_type,
						  void *result)
{
	if (res_type == CLD_RESULT_STRING)
	{
		if (result != NULL)
		{
			char *result_str = (char *)result;
			printf("%s", result_str);
		}
	}
	else if (res_type == CLD_RESULT_TABLE)
	{
		print_table_result(result);
	}
	else if (res_type == CLD_RESULT_DICT)
	{
		cld_dict *result_dict = (cld_dict *)result;
		cld_dict_foreach(result_dict, k, v)
		{
			printf("%-26.25s: %s\n", k, v);
		}
		printf("\n");
	}
	else if (res_type == CLD_RESULT_PROGRESS)
	{
		cld_multi_progress *result_progress = (cld_multi_progress *)result;
		if (result_progress->old_count > 0)
		{
			printf("\033[%dA", result_progress->old_count);
			fflush(stdout);
		}
		int new_len = arraylist_length(result_progress->progress_ls);
		//		printf("To remove %d, to write %d\n", result_progress->old_count, new_len);
		for (int i = 0; i < new_len; i++)
		{
			cld_progress *p = (cld_progress *)arraylist_get(
				result_progress->progress_ls, i);
			printf("\033[K%s: %s", p->name, p->message);
			char *progress = p->extra;
			if (progress != NULL)
			{
				printf(" %s", progress);
			}
			printf("\n");
		}
	}
	else
	{
		printf("This result type is not handled %d\n", res_type);
	}
	return CLD_COMMAND_SUCCESS;
}

void docker_result_handler(docker_context *ctx, docker_result *res)
{
	handle_docker_error(res, (cld_command_output_handler)&print_handler, (cld_command_output_handler)&print_handler);
}

cld_cmd_err main_cmd_handler(void *handler_args,
							 arraylist *options, arraylist *args,
							 cld_command_output_handler success_handler,
							 cld_command_output_handler error_handler)
{

	cld_option *host_option = get_option_by_name(options, CLD_OPTION_MAIN_HOST_LONG);

	if (!connected)
	{
		char *url;

		if (host_option->val->str_value == NULL)
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
			url = host_option->val->str_value;
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
			docker_context_result_handler_set(ctx, (docker_result_handler_fn *)&docker_result_handler);
			//if (docker_ping(ctx) != E_SUCCESS)
			//{
			//	docker_log_fatal("Could not ping the server %s", url);
			//	connected = 0;
			//}
			//else
			//{
			//	docker_log_debug("%s is alive.", url);
			//}
		}
	}

	if (!connected)
	{
		exit(-1);
	}

	cld_option *debug_option = get_option_by_name(options, CLD_OPTION_MAIN_LOG_LEVEL_LONG);
	if (debug_option->val->str_value != NULL)
	{
		if (strcmp(debug_option->val->str_value, CLD_LOGLEVEL_DEBUG) == 0)
		{
			docker_log_set_level(LOG_DEBUG);
		}
		if (strcmp(debug_option->val->str_value, CLD_LOGLEVEL_INFO) == 0)
		{
			docker_log_set_level(LOG_INFO);
		}
		if (strcmp(debug_option->val->str_value, CLD_LOGLEVEL_WARN) == 0)
		{
			docker_log_set_level(LOG_WARN);
		}
		if (strcmp(debug_option->val->str_value, CLD_LOGLEVEL_ERROR) == 0)
		{
			docker_log_set_level(LOG_ERROR);
		}
		if (strcmp(debug_option->val->str_value, CLD_LOGLEVEL_FATAL) == 0)
		{
			docker_log_set_level(LOG_FATAL);
		}
	}

	return CLD_COMMAND_SUCCESS;
}

cld_command *create_main_command()
{
	cld_command *main_command;
	if (make_command(&main_command, main_command_name, "cld",
					 "CLD Docker Client",
					 &main_cmd_handler) == CLD_COMMAND_SUCCESS)
	{
		cld_option *config_option, *debug_option, *log_level_option, *tls_option,
			*tlscacert_option, *tlscert_option, *tlskey_option, *tlsverify_option, *interactive_option,
			*host_option, *version_option, *help_option;

		make_option(&config_option, CLD_OPTION_MAIN_CONFIG_LONG,
					CLD_OPTION_MAIN_CONFIG_SHORT, CLD_TYPE_STRING, CLD_OPTION_MAIN_CONFIG_DESC);
		arraylist_add(main_command->options, config_option);

		make_option(&debug_option, CLD_OPTION_MAIN_DEBUG_LONG,
					CLD_OPTION_MAIN_DEBUG_SHORT, CLD_TYPE_FLAG, CLD_OPTION_MAIN_DEBUG_DESC);
		arraylist_add(main_command->options, debug_option);

		make_option(&log_level_option, CLD_OPTION_MAIN_LOG_LEVEL_LONG,
					CLD_OPTION_MAIN_LOG_LEVEL_SHORT, CLD_TYPE_STRING, CLD_OPTION_MAIN_LOG_LEVEL_DESC);
		arraylist_add(main_command->options, log_level_option);

		make_option(&tls_option, CLD_OPTION_MAIN_TLS_LONG,
					CLD_OPTION_MAIN_TLS_SHORT, CLD_TYPE_FLAG, CLD_OPTION_MAIN_TLS_DESC);
		arraylist_add(main_command->options, tls_option);

		make_option(&tlscacert_option, CLD_OPTION_MAIN_TLSCACERT_LONG,
					CLD_OPTION_MAIN_TLSCACERT_SHORT, CLD_TYPE_STRING, CLD_OPTION_MAIN_TLSCACERT_DESC);
		arraylist_add(main_command->options, tlscacert_option);

		make_option(&tlscert_option, CLD_OPTION_MAIN_TLSCERT_LONG,
					CLD_OPTION_MAIN_TLSCERT_SHORT, CLD_TYPE_STRING, CLD_OPTION_MAIN_TLSCERT_DESC);
		arraylist_add(main_command->options, tlscert_option);

		make_option(&tlskey_option, CLD_OPTION_MAIN_TLSKEY_LONG,
					CLD_OPTION_MAIN_TLSKEY_SHORT, CLD_TYPE_STRING, CLD_OPTION_MAIN_TLSKEY_DESC);
		arraylist_add(main_command->options, tlskey_option);

		make_option(&tlsverify_option, CLD_OPTION_MAIN_TLSVERIFY_LONG,
					CLD_OPTION_MAIN_TLSVERIFY_SHORT, CLD_TYPE_FLAG, CLD_OPTION_MAIN_TLSVERIFY_DESC);
		arraylist_add(main_command->options, tlsverify_option);

		make_option(&interactive_option, CLD_OPTION_MAIN_INTERACTIVE_LONG,
					CLD_OPTION_MAIN_INTERACTIVE_SHORT, CLD_TYPE_FLAG, CLD_OPTION_MAIN_INTERACTIVE_DESC);
		arraylist_add(main_command->options, interactive_option);

		make_option(&host_option, CLD_OPTION_MAIN_HOST_LONG,
					CLD_OPTION_MAIN_HOST_SHORT, CLD_TYPE_STRING, CLD_OPTION_MAIN_HOST_DESC);
		arraylist_add(main_command->options, host_option);

		make_option(&version_option, CLD_OPTION_MAIN_VERSION_LONG,
					CLD_OPTION_MAIN_VERSION_SHORT, CLD_TYPE_FLAG, CLD_OPTION_MAIN_VERSION_DESC);
		arraylist_add(main_command->options, version_option);

		make_option(&help_option, CLD_OPTION_HELP_LONG,
					CLD_OPTION_HELP_SHORT, CLD_TYPE_FLAG, CLD_OPTION_HELP_DESC);
		arraylist_add(main_command->options, help_option);

		arraylist_add(main_command->sub_commands, sys_commands());
		arraylist_add(main_command->sub_commands, ctr_commands());
		arraylist_add(main_command->sub_commands, img_commands());
		arraylist_add(main_command->sub_commands, vol_commands());
		arraylist_add(main_command->sub_commands, net_commands());
		return main_command;
	}
}

arraylist *create_commands()
{
	int err = arraylist_new(&CLD_COMMANDS, (void (*)(void *)) & free_command);
	if (err == 0)
	{
		arraylist_add(CLD_COMMANDS, create_main_command());
	}
	else
	{
		printf("Error creating commands list");
	}
	return CLD_COMMANDS;
}

int main(int argc, char *argv[])
{
	docker_log_set_level(LOG_INFO);

	start_lua_interpreter();

	if (argc > 0)
	{
		docker_log_debug("command name is %s\n", argv[0]);
		main_command_name = argv[0];

		create_commands();

		cld_cmd_err err = exec_command(CLD_COMMANDS, &ctx, argc,
									   argv, (cld_command_output_handler)&print_handler,
									   (cld_command_output_handler)&print_handler);
		if (err != CLD_COMMAND_SUCCESS)
		{
			docker_log_error("Error: invalid command.\n");
		}
	}

	stop_lua_interpreter();
	
	return 0;
}
