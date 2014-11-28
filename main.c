/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcohere <vcohere@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/11/18 15:40:58 by vcohere           #+#    #+#             */
/*   Updated: 2014/11/23 19:33:03 by vcohere          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ls.h"

t_file				*save_infos(char *name_file, t_file *first, char *path)
{
	struct stat		st;
	t_file			*tmp;
	t_file			*mover;

	tmp = (t_file *)malloc(sizeof(t_file));
	mover = first;
	if (stat(path, &st) == 0)
	{
		tmp->rights = rights(&st);
		tmp->name = ft_strdup(name_file);
		tmp->size = st.st_size;
		tmp->links = st.st_nlink;
		tmp->user = ft_strdup(getpwuid(st.st_uid)->pw_name);
		tmp->group = ft_strdup(getgrgid(st.st_gid)->gr_name);
		tmp->last_modif = (long long)st.st_mtime;
		tmp->is_dir = (tmp->rights[0] == 'd' && ( ft_strcmp(tmp->name, "..") != 0 && ft_strcmp(tmp->name, ".") != 0));
		tmp->next = NULL;
	}
	if (first == NULL)
		return (tmp);
	while (mover->next != NULL)
		mover = mover->next;
	mover->next = tmp;
	return (first);
}

int					ft_options(char *input)
{
	int				options;

	options = 0b00000;
	while (*input++)
	{
		if (*input == 't')
			options = options | 0b00001;
		else if (*input == 'r')
			options = options | 0b00010;
		else if (*input == 'a')
			options = options | 0b00100;
		else if (*input == 'R')
			options = options | 0b01000;
		else if (*input == 'l')
			options = options | 0b10000;
	}
	return (options);
}

void				recursive(char *path, t_file *list, int options, int nb_dir)
{
	char			**all_dir;
	int				i;

	i = 0;
	all_dir = (char **)malloc(sizeof(char *) * nb_dir + 1);
	all_dir[nb_dir + 1] = '\0';
	while (list != NULL)
	{
		if (list->is_dir == 1)
		{
			all_dir[i] = ft_strdup(list->name);
			i++;
		}
		list = list->next;
	}
	i = -1;
	while (++i < nb_dir)
	{
		ft_putchar('\n');
		ft_putstr(ft_strjoin(path, all_dir[i]));
		ft_putstr(":\n");
		browse(ft_strjoin(path, slash(all_dir[i])), options);
	}
}

int					browse(char *path, int options)
{
	DIR				*ret;
	struct dirent	*file;
	t_file			*list;

	ret = opendir(path);
	file = readdir(ret);
	list = (t_file *)malloc(sizeof(t_file));
	list = NULL;
	while (file)
	{
		list = save_infos(file->d_name, list, ft_strjoin(path, file->d_name));
		file = readdir(ret);
	}
	if (list != NULL)
		print(list, options, path);
	if (options & 0b01000)
		recursive(path, list, options, count_dir(list));
	return (1);
}

int					main(int argc, char **argv)
{
	int				i;
	int				options;
	char			*path;

	i = 0;
	options = 0b00000;
	path = NULL;
	while (++i < argc)
	{
		if (argv[i][0] == '-')
			options = options | ft_options(argv[i]++);
		else
		{
			path = argv[i];
			browse(slash(path), options);
		}
	}
	if (!path)
		browse("./", options);
	return (0);
}
