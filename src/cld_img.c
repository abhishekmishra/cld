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

#include "docker_all.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "cld_img.h"
#include "zclk_table.h"
#include "zclk_progress.h"
#include <zclk.h>

#include "mustach-json-c.h"

typedef struct
{
	zclk_command_output_handler success_handler;
	zclk_multi_progress *multi_progress;
} docker_image_update_args;

void log_pull_message(docker_image_create_status *status, void *client_cbargs)
{
	docker_image_update_args *upd_args = (docker_image_update_args *)client_cbargs;
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
				zclk_progress *p = (zclk_progress *)arraylist_get(
					upd_args->multi_progress->progress_ls, i);
				if (strcmp(status->id, p->name) == 0)
				{
					found = 1;
					loc = i;
				}
			}
			if (found == 0)
			{
				zclk_progress *p;
				if (create_zclk_progress(&p, status->id, 0, 0) == 0)
				{
					arraylist_add(upd_args->multi_progress->progress_ls, p);
					upd_args->multi_progress->old_count = (int)arraylist_length(
															  upd_args->multi_progress->progress_ls) -
														  1;
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
				zclk_progress *p = (zclk_progress *)arraylist_get(
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
			upd_args->success_handler(ZCLK_RES_IS_RUNNING,
									  ZCLK_RESULT_PROGRESS, upd_args->multi_progress);
		}
		else
		{
			upd_args->success_handler(ZCLK_RES_IS_RUNNING, ZCLK_RESULT_STRING,
									  status->status);
		}
	}
}

zclk_res img_pl_cmd_handler(zclk_command* cmd, void *handler_args)
{
	int quiet = 0;
	docker_context *ctx = get_docker_context(handler_args);

	docker_image_update_args *upd_args = (docker_image_update_args *)calloc(1,
																			sizeof(docker_image_update_args));
	if (upd_args == NULL)
	{
		return ZCLK_RES_ERR_ALLOC_FAILED;
	}
	upd_args->success_handler = cmd->success_handler;
	create_zclk_multi_progress(&(upd_args->multi_progress));

	size_t len = arraylist_length(cmd->args);
	if (len != 1)
	{
		cmd->error_handler(ZCLK_RES_ERR_UNKNOWN, ZCLK_RESULT_STRING,
					  "Image name not provided.");
		return ZCLK_RES_ERR_UNKNOWN;
	}
	else
	{
		zclk_argument *image_name_arg = (zclk_argument *)arraylist_get(cmd->args,
																	 0);
		char *image_name = zclk_argument_get_val_string(image_name_arg);
		d_err_t docker_error = docker_image_create_from_image_cb(ctx,
																 &log_pull_message, upd_args, image_name, NULL, NULL);
		if (docker_error == E_SUCCESS)
		{
			char *res_str = (char *)calloc(strlen(image_name) + 100, sizeof(char));
			if (res_str == NULL)
			{
				return ZCLK_RES_ERR_ALLOC_FAILED;
			}
			sprintf(res_str, "Image pull successful -> %s", image_name);
			cmd->success_handler(ZCLK_RES_SUCCESS, ZCLK_RESULT_STRING, res_str);
			free(res_str);
			return ZCLK_RES_SUCCESS;
		}
		else
		{
			return ZCLK_RES_ERR_UNKNOWN;
		}
	}
	free_zclk_multi_progress(upd_args->multi_progress);
}

char *concat_tags(json_object *tags_ls)
{
	char *tags = NULL;
	if (tags_ls)
	{
		size_t len_tags = json_object_array_length(tags_ls);
		size_t tag_strlen = 0;
		for (size_t i = 0; i < len_tags; i++)
		{
			tag_strlen += strlen((char *)json_object_array_get_idx(tags_ls, i));
			tag_strlen += 1; // for newline
		}
		tag_strlen += 1; // for null terminator
		tags = (char *)calloc(tag_strlen, sizeof(char));
		if (tags != NULL)
		{
			tags[0] = '\0';
			for (size_t i = 0; i < len_tags; i++)
			{
				strcat(tags, (char *)json_object_array_get_idx(tags_ls, i));
				if (i != (len_tags - 1))
				{
					strcat(tags, "\n");
				}
			}
		}
	}
	return tags;
}

char *get_image_tags_concat(docker_image *img)
{
	json_object *tags_ls = docker_image_repo_tags_get(img);
	char *tags = concat_tags(tags_ls);
	return tags;
}

zclk_res img_ls_cmd_handler(zclk_command* cmd, void *handler_args)
{
	int quiet = 0;
	docker_context *ctx = get_docker_context(handler_args);
	docker_image_list *images;

	d_err_t docker_error = docker_images_list(ctx, &images, 0, 1, NULL, 0,
											  NULL, NULL, NULL);

	if (docker_error == E_SUCCESS)
	{
		char res_str[1024];
		sprintf(res_str, "Listing images");
		cmd->success_handler(ZCLK_RES_SUCCESS, ZCLK_RESULT_STRING, res_str);

		int col_num = 0;
		size_t len_images = docker_image_list_length(images);
		zclk_table *img_tbl;
		if (create_zclk_table(&img_tbl, len_images, 5) == 0)
		{
			int col = 0;
			zclk_table_set_header(img_tbl, col++, "REPOSITORY");
			zclk_table_set_header(img_tbl, col++, "TAG");
			zclk_table_set_header(img_tbl, col++, "IMAGE ID");
			zclk_table_set_header(img_tbl, col++, "CREATED");
			zclk_table_set_header(img_tbl, col++, "SIZE");

			char *outstr;
			size_t outstrlen;

			printf("\n%s\n", get_json_string(images));

			mustach_json_c(
				"{{#.}}"
				"  {{Created}}"
				"{{/.}}",
				images, &outstr, &outstrlen);

			if (outstr != NULL)
			{
				printf("%s\n", outstr);
			}

			for (size_t i = 0; i < len_images; i++)
			{
				docker_image *img = docker_image_list_get_idx(images,
															  i);

				col_num = 0;
				char cstr[1024];
				const time_t created_time = (time_t)docker_image_created_get(img);
				struct tm *ctm = gmtime(&created_time);
				int len = strftime(cstr, 1023, "%d/%m/%Y %H:%M:%S", ctm);
				cstr[len] = '\0';

				char sstr[1024];
				sprintf(sstr, "%s", calculate_size(docker_image_size_get(img)));

				col = 0;
				if (docker_image_repo_tags_get(img) != NULL && docker_image_repo_tags_length(img) > 0)
				{
					char *repo_tag = docker_image_repo_tags_get_idx(img, 0);
					char *tag = strrchr(repo_tag, ':');
					if (tag == NULL)
					{
						zclk_table_set_row_val(img_tbl, i, col++, repo_tag);
						zclk_table_set_row_val(img_tbl, i, col++, "<none>");
					}
					else
					{
						char *repo_val = (char *)calloc(tag - repo_tag + 1, sizeof(char));
						if (repo_val == NULL)
						{
							return ZCLK_RES_ERR_ALLOC_FAILED;
						}
						strncpy(repo_val, repo_tag, tag - repo_tag);
						repo_val[tag - repo_tag] = '\0';
						zclk_table_set_row_val(img_tbl, i, col++, repo_val);
						zclk_table_set_row_val(img_tbl, i, col++, tag + 1);
						free(repo_val);
					}
				}
				else
				{
					zclk_table_set_row_val(img_tbl, i, col++, "<none>");
					zclk_table_set_row_val(img_tbl, i, col++, "<none>");
				}
				char *img_id = docker_image_id_get(img);
				char *id_val = strrchr(img_id, ':');
				if (id_val == NULL)
				{
					zclk_table_set_row_val(img_tbl, i, col++, img_id);
				}
				else
				{
					zclk_table_set_row_val(img_tbl, i, col++, id_val + 1);
				}
				zclk_table_set_row_val(img_tbl, i, col++, cstr);
				zclk_table_set_row_val(img_tbl, i, col++, sstr);
			}
			cmd->success_handler(ZCLK_RES_SUCCESS, 
				ZCLK_RESULT_TABLE, img_tbl);
		}
	}
	else
	{
		return ZCLK_RES_ERR_UNKNOWN;
	}
	return ZCLK_RES_SUCCESS;
}

void log_build_message(docker_build_status *status, void *client_cbargs)
{
	docker_image_update_args *upd_args = (docker_image_update_args *)client_cbargs;
	if (status)
	{
		if (status->stream)
		{
			upd_args->success_handler(ZCLK_RES_IS_RUNNING,
									  ZCLK_RESULT_STRING, status->stream);
		}
	}
}

zclk_res img_build_cmd_handler(zclk_command* cmd, void *handler_args)
{
	int quiet = 0;
	docker_context *ctx = get_docker_context(handler_args);

	docker_image_update_args *upd_args = (docker_image_update_args *)calloc(1,
																			sizeof(docker_image_update_args));
	if (upd_args == NULL)
	{
		return ZCLK_RES_ERR_ALLOC_FAILED;
	}
	upd_args->success_handler = cmd->success_handler;
	create_zclk_multi_progress(&(upd_args->multi_progress));

	size_t len = arraylist_length(cmd->args);
	if (len != 1)
	{
		cmd->error_handler(ZCLK_RES_ERR_UNKNOWN, ZCLK_RESULT_STRING,
					  "Image name not provided.");
		return ZCLK_RES_ERR_UNKNOWN;
	}
	else
	{
		zclk_argument *folder_url_dash_arg = (zclk_argument *)arraylist_get(
			cmd->args, 0);
		char *folder_url_dash = zclk_argument_get_val_string(folder_url_dash_arg);
		d_err_t docker_error = docker_image_build_cb(ctx, folder_url_dash,
													 NULL, &log_build_message, upd_args, NULL);
		if (docker_error == E_SUCCESS)
		{
			char *res_str = (char *)calloc(strlen(folder_url_dash) + 100, sizeof(char));
			if (res_str == NULL)
			{
				return ZCLK_RES_ERR_ALLOC_FAILED;
			}
			sprintf(res_str, "Image pull successful -> %s\n", folder_url_dash);
			cmd->success_handler(ZCLK_RES_SUCCESS, ZCLK_RESULT_STRING, res_str);
			free(res_str);
			return ZCLK_RES_SUCCESS;
		}
		else
		{
			return ZCLK_RES_ERR_UNKNOWN;
		}
	}
	free_zclk_multi_progress(upd_args->multi_progress);
}

zclk_command *img_commands()
{
	zclk_command *image_command = new_zclk_command("image", 
			"img", "Docker Image Commands", NULL);
	if(image_command != NULL)
	{
		zclk_command *imgpl_command = new_zclk_command("pull", 
				"pl", "Docker Image Pull", &img_pl_cmd_handler);
		if(imgpl_command != NULL)
		{
			zclk_command_string_argument(imgpl_command, "Image Name", 
					NULL, "Name of Docker Image to be pulled.", 1);

			zclk_command_subcommand_add(image_command, imgpl_command);
		}
		zclk_command *imgls_command = new_zclk_command("list", 
				"ls", "Docker Image List", &img_ls_cmd_handler);
		if(imgls_command != NULL)
		{
			zclk_command_subcommand_add(image_command, imgls_command);
		}
		zclk_command *imgbuild_command = new_zclk_command("build", 
				"make", "Docker Image Build", &img_build_cmd_handler);
		if(imgbuild_command != NULL)
		{
			zclk_command_string_argument(imgbuild_command, "Folder | URL | -", 
					NULL, "Docker resources to build (folder/url/stdin)", 1);

			zclk_command_subcommand_add(image_command, imgbuild_command);
		}
	}
	return image_command;
}
