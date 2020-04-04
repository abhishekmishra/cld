/**
 * Copyright (c) 2020 Abhishek Mishra
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "docker_all.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "cld_img.h"
#include "cld_table.h"
#include "cld_progress.h"
#include "cld_command.h"

typedef struct
{
	cld_command_output_handler success_handler;
	cld_multi_progress* multi_progress;
} docker_image_update_args;

void log_pull_message(docker_image_create_status* status, void* client_cbargs)
{
	docker_image_update_args* upd_args = (docker_image_update_args*)client_cbargs;
	if (status)
	{
		if (status->id)
		{
			size_t len = arraylist_length(upd_args->multi_progress->progress_ls);
			size_t new_len = len;
			size_t found = 0;
			size_t loc = -1;
			for (size_t i = 0; i < len; i++)
			{
				cld_progress* p = (cld_progress*)arraylist_get(
					upd_args->multi_progress->progress_ls, i);
				if (strcmp(status->id, p->name) == 0)
				{
					found = 1;
					loc = i;
				}
			}
			if (found == 0)
			{
				cld_progress* p;
				if (create_cld_progress(&p, status->id, 0, 0) == 0)
				{
					arraylist_add(upd_args->multi_progress->progress_ls, p);
					upd_args->multi_progress->old_count = (int)arraylist_length(
						upd_args->multi_progress->progress_ls) - 1;
					new_len += 1;
					p->message = status->status;
					if (status->progress != NULL)
					{
						p->extra = status->progress;
						p->current = status->progress_detail->current;
						p->total = status->progress_detail->total;
					}
					else
					{
						p->extra = NULL;
					}
				}
			}
			else
			{
				cld_progress* p = (cld_progress*)arraylist_get(
					upd_args->multi_progress->progress_ls, loc);
				upd_args->multi_progress->old_count = (int)arraylist_length(
					upd_args->multi_progress->progress_ls);
				p->message = status->status;
				if (status->progress != NULL)
				{
					p->extra = status->progress;
					p->current = status->progress_detail->current;
					p->total = status->progress_detail->total;
				}
				else
				{
					p->extra = NULL;
				}
			}
			upd_args->success_handler(CLD_COMMAND_IS_RUNNING,
				CLD_RESULT_PROGRESS, upd_args->multi_progress);
		}
		else
		{
			upd_args->success_handler(CLD_COMMAND_IS_RUNNING, CLD_RESULT_STRING,
				status->status);
		}
	}
}

cld_cmd_err img_pl_cmd_handler(void* handler_args, arraylist* options,
	arraylist* args, cld_command_output_handler success_handler,
	cld_command_output_handler error_handler)
{
	int quiet = 0;
	docker_context* ctx = get_docker_context(handler_args);

	docker_image_update_args* upd_args = (docker_image_update_args*)calloc(1,
		sizeof(docker_image_update_args));
	if (upd_args == NULL) {
		return CLD_COMMAND_ERR_ALLOC_FAILED;
	}
	upd_args->success_handler = success_handler;
	create_cld_multi_progress(&(upd_args->multi_progress));

	size_t len = arraylist_length(args);
	if (len != 1)
	{
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
			"Image name not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	}
	else
	{
		cld_argument* image_name_arg = (cld_argument*)arraylist_get(args,
			0);
		char* image_name = image_name_arg->val->str_value;
		d_err_t docker_error = docker_image_create_from_image_cb(ctx,
			&log_pull_message, upd_args, image_name, NULL, NULL);
		if (docker_error == E_SUCCESS)
		{
			char* res_str = (char*)calloc(strlen(image_name) + 100, sizeof(char));
			if (res_str == NULL) {
				return CLD_COMMAND_ERR_ALLOC_FAILED;
			}
			sprintf(res_str, "Image pull successful -> %s", image_name);
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, res_str);
			free(res_str);
			return CLD_COMMAND_SUCCESS;
		}
		else
		{
			return CLD_COMMAND_ERR_UNKNOWN;
		}
	}
	free_cld_multi_progress(upd_args->multi_progress);
}

char* concat_tags(json_object* tags_ls)
{
	char* tags = NULL;
	if (tags_ls)
	{
		size_t len_tags = json_object_array_length(tags_ls);
		size_t tag_strlen = 0;
		for (size_t i = 0; i < len_tags; i++)
		{
			tag_strlen += strlen((char*)json_object_array_get_idx(tags_ls, i));
			tag_strlen += 1; //for newline
		}
		tag_strlen += 1; //for null terminator
		tags = (char*)calloc(tag_strlen, sizeof(char));
		if (tags != NULL)
		{
			tags[0] = '\0';
			for (size_t i = 0; i < len_tags; i++)
			{
				strcat(tags, (char*)json_object_array_get_idx(tags_ls, i));
				if (i != (len_tags - 1))
				{
					strcat(tags, "\n");
				}
			}
		}
	}
	return tags;
}

char* get_image_tags_concat(docker_image* img)
{
	json_object* tags_ls = docker_image_repo_tags_get(img);
	char* tags = concat_tags(tags_ls);
	return tags;
}

cld_cmd_err img_ls_cmd_handler(void* handler_args, arraylist* options,
	arraylist* args, cld_command_output_handler success_handler,
	cld_command_output_handler error_handler)
{
	int quiet = 0;
	docker_context* ctx = get_docker_context(handler_args);
	docker_image_list* images;

	d_err_t docker_error = docker_images_list(ctx, &images, 0, 1, NULL, 0,
		NULL, NULL, NULL);

	if (docker_error == E_SUCCESS)
	{
		char res_str[1024];
		sprintf(res_str, "Listing images");
		success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, res_str);

		int col_num = 0;
		size_t len_images = docker_image_list_length(images);
		cld_table* img_tbl;
		if (create_cld_table(&img_tbl, len_images, 5) == 0)
		{
			int col = 0;
			cld_table_set_header(img_tbl, col++, "REPOSITORY");
			cld_table_set_header(img_tbl, col++, "TAG");
			cld_table_set_header(img_tbl, col++, "IMAGE ID");
			cld_table_set_header(img_tbl, col++, "CREATED");
			cld_table_set_header(img_tbl, col++, "SIZE");

			for (size_t i = 0; i < len_images; i++)
			{
				docker_image* img = docker_image_list_get_idx(images,
					i);

				col_num = 0;
				char cstr[1024];
				const time_t created_time = (time_t)docker_image_created_get(img);
				struct tm* ctm = gmtime(&created_time);
				int len = strftime(cstr, 1023, "%d/%m/%Y %H:%M:%S", ctm);
				cstr[len] = '\0';

				char sstr[1024];
				sprintf(sstr, "%s", calculate_size(docker_image_size_get(img)));

				col = 0;
				if (docker_image_repo_tags_get(img) != NULL
					&& docker_image_repo_tags_length(img) > 0) {
					char* repo_tag = docker_image_repo_tags_get_idx(img, 0);
					char* tag = strrchr(repo_tag, ':');
					if (tag == NULL) {
						cld_table_set_row_val(img_tbl, i, col++, repo_tag);
						cld_table_set_row_val(img_tbl, i, col++, "<none>");
					}
					else {
						char* repo_val = (char*)calloc(tag - repo_tag + 1, sizeof(char));
						if (repo_val == NULL) {
							return CLD_COMMAND_ERR_ALLOC_FAILED;
						}
						strncpy(repo_val, repo_tag, tag - repo_tag);
						repo_val[tag - repo_tag] = '\0';
						cld_table_set_row_val(img_tbl, i, col++, repo_val);
						cld_table_set_row_val(img_tbl, i, col++, tag + 1);
						free(repo_val);
					}
				}
				else {
					cld_table_set_row_val(img_tbl, i, col++, "<none>");
					cld_table_set_row_val(img_tbl, i, col++, "<none>");
				}
				char* img_id = docker_image_id_get(img);
				char* id_val = strrchr(img_id, ':');
				if (id_val == NULL) {
					cld_table_set_row_val(img_tbl, i, col++, img_id);
				}
				else {
					cld_table_set_row_val(img_tbl, i, col++, id_val + 1);
				}
				cld_table_set_row_val(img_tbl, i, col++, cstr);
				cld_table_set_row_val(img_tbl, i, col++, sstr);
			}
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_TABLE, img_tbl);
		}
	}
	else
	{
		return CLD_COMMAND_ERR_UNKNOWN;
	}
	return CLD_COMMAND_SUCCESS;
}

void log_build_message(docker_build_status* status, void* client_cbargs) {
	docker_image_update_args* upd_args = (docker_image_update_args*)client_cbargs;
	if (status)
	{
		if (status->stream)
		{
			upd_args->success_handler(CLD_COMMAND_IS_RUNNING,
				CLD_RESULT_STRING, status->stream);
		}
	}
}

cld_cmd_err img_build_cmd_handler(void* handler_args,
	arraylist* options, arraylist* args,
	cld_command_output_handler success_handler,
	cld_command_output_handler error_handler)
{
	int quiet = 0;
	docker_context* ctx = get_docker_context(handler_args);

	docker_image_update_args* upd_args = (docker_image_update_args*)calloc(1,
		sizeof(docker_image_update_args));
	if (upd_args == NULL) {
		return CLD_COMMAND_ERR_ALLOC_FAILED;
	}
	upd_args->success_handler = success_handler;
	create_cld_multi_progress(&(upd_args->multi_progress));

	size_t len = arraylist_length(args);
	if (len != 1)
	{
		error_handler(CLD_COMMAND_ERR_UNKNOWN, CLD_RESULT_STRING,
			"Image name not provided.");
		return CLD_COMMAND_ERR_UNKNOWN;
	}
	else
	{
		cld_argument* folder_url_dash_arg = (cld_argument*)arraylist_get(
			args, 0);
		char* folder_url_dash = folder_url_dash_arg->val->str_value;
		d_err_t docker_error = docker_image_build_cb(ctx, folder_url_dash,
			NULL, &log_build_message, upd_args, NULL);
		if (docker_error == E_SUCCESS)
		{
			char* res_str = (char*)calloc(strlen(folder_url_dash) + 100, sizeof(char));
			if (res_str == NULL) {
				return CLD_COMMAND_ERR_ALLOC_FAILED;
			}
			sprintf(res_str, "Image pull successful -> %s\n", folder_url_dash);
			success_handler(CLD_COMMAND_SUCCESS, CLD_RESULT_STRING, res_str);
			free(res_str);
			return CLD_COMMAND_SUCCESS;
		}
		else
		{
			return CLD_COMMAND_ERR_UNKNOWN;
		}
	}
	free_cld_multi_progress(upd_args->multi_progress);
}

cld_command* img_commands()
{
	cld_command* image_command;
	if (make_command(&image_command, "image", "img", "Docker Image Commands",
		NULL) == CLD_COMMAND_SUCCESS)
	{
		cld_command* imgpl_command, * imgls_command, * imgbuild_command;
		if (make_command(&imgpl_command, "pull", "pl", "Docker Image Pull",
			&img_pl_cmd_handler) == CLD_COMMAND_SUCCESS)
		{
			cld_argument* image_name_arg;
			arraylist_add(imgpl_command->args, create_argument("Image Name", CLD_VAL_STRING(NULL), CLD_VAL_STRING(NULL),
				"Name of Docker Image to be pulled."));
			arraylist_add(image_command->sub_commands, imgpl_command);
		}
		if (make_command(&imgls_command, "list", "ls", "Docker Image List",
			&img_ls_cmd_handler) == CLD_COMMAND_SUCCESS)
		{
			arraylist_add(image_command->sub_commands, imgls_command);
		}
		if (make_command(&imgbuild_command, "build", "make",
			"Docker Image Build", &img_build_cmd_handler)
			== CLD_COMMAND_SUCCESS)
		{
			arraylist_add(imgbuild_command->args,
				create_argument("Folder | URL | -", CLD_VAL_STRING(NULL), CLD_VAL_STRING(NULL),
					"Docker resources to build (folder/url/stdin)"));

			arraylist_add(image_command->sub_commands, imgbuild_command);
		}
	}
	return image_command;
}
