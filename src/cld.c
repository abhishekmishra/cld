/*
 * clibdocker: docker_cli.c
 * Created on: 20-Jan-2019
 *
 * clibdocker
 * Copyright (C) 2018 Abhishek Mishra <abhishekmishra3@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

//based on example at http://www.cs.utah.edu/~bigler/code/libedit.html
//see https://stackoverflow.com/questions/36953960/is-editline-tab-completion-supposed-to-work
// #if defined(__linux__) // can also use Linux here
// #include <editline/readline.h>
// #elif defined(__OpenBSD__)
// #include <readline/readline.h>
// #elif defined(_WIN32)
// #include <readline/readline.h>
// #include <readline/history.h>
// #endif

#include <linenoise.h>
#include <histedit.h>

#include <string.h>
#include <getopt.h>
#include <json-c/arraylist.h>

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

#define DOCKER_DEFINE_DEFAULT_UNIX_SOCKET "blah"

#define CMD_NOT_FOUND -1

struct array_list *create_commands()
{
	struct array_list *commands = array_list_new(
		(void (*)(void *)) & free_command);
	array_list_add(commands, sys_commands());
	array_list_add(commands, ctr_commands());
	array_list_add(commands, img_commands());
	array_list_add(commands, vol_commands());
	array_list_add(commands, net_commands());
	return commands;
}

char* prompt(EditLine *e)
{
	return "CLD> ";
}

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
		char *result_str = (char *)result;
		printf("%s", result_str);
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
		int new_len = array_list_length(result_progress->progress_ls);
		//		printf("To remove %d, to write %d\n", result_progress->old_count, new_len);
		for (int i = 0; i < new_len; i++)
		{
			cld_progress *p = (cld_progress *)array_list_get_idx(
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

int parse_line_run_command(Tokenizer *tokenizer, const char *line,
						   int *cmd_argc, char ***cmd_argv, docker_context *ctx)
{
	if (strcmp(line, "\n") == 0)
	{
		return 0;
	}
	int tok_err = tok_str(tokenizer, line, &*cmd_argc,
						  (const char ***)&*cmd_argv);
	if (tok_err == 0)
	{
		cld_cmd_err err = exec_command(create_commands(), ctx, *cmd_argc,
									   *cmd_argv, (cld_command_output_handler)&print_handler,
									   (cld_command_output_handler)&print_handler);
		if (err != CLD_COMMAND_SUCCESS)
		{
			printf("Error: invalid command.\n");
		}
	}
	else
	{
		printf("Error: invalid command.\n");
	}

	//reset tokenizer;
	tok_reset(tokenizer);

	return tok_err;
}

int parse_line_help_command(Tokenizer *tokenizer, const char *line,
							int *cmd_argc, char ***cmd_argv, docker_context *ctx)
{
	int tok_err = tok_str(tokenizer, line, &*cmd_argc,
						  (const char ***)&*cmd_argv);
	if (tok_err == 0)
	{
		cld_cmd_err err = help_cmd_handler(create_commands(), ctx, *cmd_argc,
										   *cmd_argv, (cld_command_output_handler)&print_handler,
										   (cld_command_output_handler)&print_handler);
		if (err != CLD_COMMAND_SUCCESS)
		{
			printf("Error: invalid command.\n");
		}
	}
	else
	{
		printf("Error: invalid command.\n");
	}

	//reset tokenizer;
	tok_reset(tokenizer);

	return tok_err;
}

int main(int argc, char *argv[])
{
	static docker_context *ctx;

	EditLine *el;
	History *myhistory;
	Tokenizer *tokenizer;
	int interactive = 1;
	int command = 0;
	char *command_str;
	char *url;
	int connected = 0;
	int show_help = 0;

	/* Temp variables */
	int count;
	const char *line;
	int keepreading = 1;
	HistEvent ev;
	int cmd_argc;
	char **cmd_argv;

	/* Initialize the tokenizer */
	tokenizer = tok_init(NULL);

	/** No logging **/
	docker_log_set_quiet(1);

	/** Initialize docker context **/
	//make_docker_context_socket(&ctx, DOCKER_DEFINE_DEFAULT_UNIX_SOCKET);
	make_docker_context_url(&ctx, DOCKER_DEFINE_DEFAULT_UNIX_SOCKET);

	int c;

	while (1)
	{
		/* These options don’t set a flag.
		 We distinguish them by their indices. */
		static struct option long_options[] =
			{
				{"config", required_argument, 0, 0},
				{"debug", no_argument, 0, 'D'},
				{"help", no_argument, 0, 'h'},
				{"log-level", required_argument, 0, 'l'},
				{"tls",
				 no_argument, 0, 0},
				{"tlscacert", required_argument, 0, 0},
				{"tlscert", required_argument, 0, 0},
				{"tlskey",
				 required_argument, 0, 0},
				{"tlsverify", no_argument, 0, 0},
				{"command", required_argument, 0, 'c'},
				{"interactive",
				 no_argument, 0, 'i'},
				{"host",
				 required_argument, 0, 'H'},
				{"version",
				 no_argument, 0, 'v'},
				{0, 0, 0, 0}};
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long(argc, argv, "Dhl:c:iH:v", long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c)
		{
		case 0:
			/* If this option set a flag, do nothing else now. */
			if (long_options[option_index].flag != 0)
				break;
			printf("option %s", long_options[option_index].name);
			if (optarg)
				printf(" with arg %s", optarg);
			printf("\n");
			break;

		case 'i':
			//puts("option -i\n");
			interactive = 2;
			break;

			//		case 'c':
			////			printf("option -c with value `%s'\n", optarg);
			//			command = 1;
			//			command_str = optarg;
			//			break;

		case 'h':
			show_help = 1;
			break;

		case 'H':
			url = optarg;
			if (is_http_url(url))
			{
				if (make_docker_context_url(&ctx, url) == E_SUCCESS)
				{
					connected = 1;
				}
			}
			else if (is_unix_socket(url))
			{
				if (make_docker_context_socket(&ctx, url) == E_SUCCESS)
				{
					connected = 1;
				}
			}
			break;

		case '?':
			/* getopt_long already printed an error message. */
			break;

		default:
			abort();
		}
	}

	/* Print any remaining command line arguments (not options). */
	docker_result *res;
	if (optind < argc)
	{
		//		printf("There are extra arguments: ");
		command = 1;
		int total_len = 1;
		for (int i = optind; i < argc; i++)
		{
			total_len += strlen(argv[i]);
			total_len += 1; // for space
		}
		command_str = (char *)calloc(total_len, sizeof(char));
		command_str[0] = '\0';
		for (int i = optind; i < argc; i++)
		{
			//printf("%s ", argv[optind++]);
			strcat(command_str, argv[i]);
			if (i != (argc - 1))
			{
				strcat(command_str, " ");
			}
		}
		printf("Command is <%s>\n", command_str);
	}

	if (!connected)
	{
		url = DOCKER_DEFINE_DEFAULT_UNIX_SOCKET;
		if (make_docker_context_socket(&ctx, url) == E_SUCCESS)
		{
			connected = 1;
		}
	}

	if (connected)
	{
		if (docker_ping(ctx, &res) != E_SUCCESS)
		{
			docker_log_fatal("Could not ping the server %s", url);
			connected = 0;
		}
		else
		{
			docker_log_info("%s is alive.", url);
		}
	}

	if (command)
	{
		if (show_help == 1)
		{
			int tok_err = parse_line_help_command(tokenizer, command_str,
												  &cmd_argc, &cmd_argv, ctx);
		}
		else
		{
			int tok_err = parse_line_run_command(tokenizer, command_str,
												 &cmd_argc, &cmd_argv, ctx);
		}
	}

	if (interactive > command)
	{
		printf("=========================================\n");
		printf("== CLD (clibdocker) Docker CLI.        ==\n");
		printf("== Version: 0.1a                       ==\n");
		printf("==                                     ==\n");
		printf("== For help type help <command>        ==\n");
		printf("=========================================\n");

		/* Initialize the EditLine state to use our prompt function and
		 emacs style editing. */
		el = el_init(argv[0], stdin, stdout, stderr);
		el_set(el, EL_PROMPT, &prompt);
		el_set(el, EL_EDITOR, "emacs");

		/* Initialize the history */
		myhistory = history_init();
		if (myhistory == 0)
		{
			fprintf(stderr, "history could not be initialized\n");
			return 1;
		}

		/* Set the size of the history */
		history(myhistory, &ev, H_SETSIZE, 800);

		/* This sets up the call back functions for history functionality */
		el_set(el, EL_HIST, history, myhistory);

		while (keepreading)
		{
			/* count is the number of characters read.
			 line is a const char* of our command line with the tailing \n */
			line = el_gets(el, &count);
			if (line != NULL)
			{
				if (strcmp("quit\n", line) == 0 || strcmp("q\n", line) == 0)
				{
					printf("Ending session.\n");
					el_beep(el);
					goto CLEANUP_AND_EXIT;
				}
				int tok_err = parse_line_run_command(tokenizer, line, &cmd_argc,
													 &cmd_argv, ctx);
			}

			/* In order to use our history we have to explicitly add commands
			 to the history */
			if (count > 0)
			{
				history(myhistory, &ev, H_ENTER, line);
				//			printf("You typed \"%s\"\n", line);
			}
		}
	CLEANUP_AND_EXIT:
		/* Clean up our memory */
		history_end(myhistory);
		el_end(el);
	}

	tok_end(tokenizer);
	return 0;
}
