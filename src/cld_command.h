/*
 * clibdocker: cld_command.h
 * Created on: 06-Feb-2019
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

#ifndef SRC_CLD_COMMAND_H_
#define SRC_CLD_COMMAND_H_

#include "docker_connection_util.h"
#include <arraylist.h>

typedef enum
{
	CLD_COMMAND_IS_RUNNING = -1,
	CLD_COMMAND_SUCCESS = 0,
	CLD_COMMAND_ERR_UNKNOWN = 1,
	CLD_COMMAND_ERR_ALLOC_FAILED = 2,
	CLD_COMMAND_ERR_COMMAND_NOT_FOUND = 3,
	CLD_COMMAND_ERR_OPTION_NOT_FOUND = 4,
	CLD_COMMAND_ERR_ARG_NOT_FOUND = 5,
	CLD_COMMAND_ERR_EXTRA_ARGS_FOUND = 6
} cld_cmd_err;

typedef enum
{
	CLD_TYPE_BOOLEAN = 0,
	CLD_TYPE_INT = 1,
	CLD_TYPE_DOUBLE = 2,
	CLD_TYPE_STRING = 3,
	CLD_TYPE_FLAG = 4
} cld_type;

typedef enum
{
	CLD_RESULT_STRING = 0,
	CLD_RESULT_TABLE = 1,
	CLD_RESULT_DICT = 2,
	CLD_RESULT_PROGRESS = 3
} cld_result_type;

//TODO add defaults
typedef struct cld_val_t
{
	cld_type type;
	int bool_value;
	int int_value;
	double dbl_value;
	char* str_value;
	char* description;
} cld_val;

typedef struct cld_option_t
{
	char* name;
	char* short_name;
	cld_val* val;
	cld_val* default_val;
	char* description;
} cld_option;

typedef struct cld_argument_t
{
	char* name;
	cld_val* val;
	cld_val* default_val;
	char* description;
	int optional;
} cld_argument;

typedef cld_cmd_err (*cld_command_output_handler)(cld_cmd_err result_flag,
		cld_result_type result_type, void* result);

typedef cld_cmd_err (*cld_command_handler)(void* handler_args,
		arraylist* options, arraylist* args,
		cld_command_output_handler success_handler,
		cld_command_output_handler error_handler);

typedef struct cld_command_t
{
	char* name;
	char* short_name;
	char* description;
	arraylist* sub_commands;
	arraylist* options;
	arraylist* args;
	cld_command_handler handler;
} cld_command;

/**
 * Create a new value object of given type.
 *
 * \param val object to create
 * \param type
 * \return error code
 */
cld_cmd_err make_cld_val(cld_val** val, cld_type type);

/**
 * Free the created value
 */
void free_cld_val(cld_val* val);

/**
 * Reset values to system defaults.
 */
void clear_cld_val(cld_val* val);

/**
 * Copy values from 'from' to 'to'.
 * Can be used to reset to defaults.
 *
 * \param to val to set
 * \param from val to read from
 */
void copy_cld_val(cld_val* to, cld_val* from);

/**
 * Parse the input and read the value of the type of the val object.
 * (Should not be called when the values is a flag.)
 * The value should be set as soon as the argument/option is seen
 *
 * \param val object whose value will be set
 * \param input string input
 * \return error code
 */
cld_cmd_err parse_cld_val(cld_val* val, char* input);

/**
 * Create a new option given a name and type.
 *
 * \param option object to create
 * \param name
 * \param short_name
 * \param type
 * \param description
 * \return error code
 */
cld_cmd_err make_option(cld_option** option, char* name, char* short_name,
		cld_type type, char* description);

/**
 * Free resources used by option
 */
void free_option(cld_option* option);

/**
 * Create a new argument given a name and type.
 *
 * \param argument object to create
 * \param name
 * \param type
 * \param description
 * \return error code
 */
cld_cmd_err make_argument(cld_argument** arg, char* name, cld_type type,
		char* description);

/**
 * Free resources used by argument
 */
void free_argument(cld_argument* arg);

/**
 * Create a new command with the given name and handler
 * Options and sub-commands need to be added after creation.
 * The sub-commands, arguments and options lists will be initialized,
 * so one just needs to add items using the arraylist add function.
 *
 * \param command obj to be created
 * \param name
 * \param short_name
 * \param description
 * \param handler function ptr to handler
 * \return error code
 */
cld_cmd_err make_command(cld_command** command, char* name, char* short_name,
		char* description, cld_command_handler handler);

/**
 * Free a command object
 */
void free_command(cld_command* command);

/**
 * Run the help command for all commands or single command
 *
 * \param commands the list of commands registered (this is a list of cld_command*)
 * \param handler_args an args value to be passed to the command handler
 * \param argc the number of tokens in the line
 * \param argv args as an array of strings
 * \param success_handler handle success results
 * \param error_handler handler error results
 */
cld_cmd_err help_cmd_handler(arraylist* commands, void* handler_args,
		int argc, char** argv, cld_command_output_handler success_handler,
		cld_command_output_handler error_handler);

/**
 * Get the help string for the arg_commands from the registered commands list.
 * \param help_str the help string to return
 * \param commands is the configured list of commands
 * \param arg_commands is a list of string
 * \return error code
 */
cld_cmd_err get_help_for(char** help_str, arraylist* commands,
		arraylist* arg_commands);

/**
 * Execute a single line containing one top-level command.
 * All output is written to stdout, all errors to stderr
 *
 * \param commands the list of commands registered (this is a list of cld_command*)
 * \param handler_args an args value to be passed to the command handler
 * \param argc the number of tokens in the line
 * \param argv args as an array of strings
 * \param success_handler handle success results
 * \param error_handler handler error results
 */
cld_cmd_err exec_command(arraylist* commands, void* handler_args,
		int argc, char** argv, cld_command_output_handler success_handler,
		cld_command_output_handler error_handler);

#endif /* SRC_CLD_COMMAND_H_ */