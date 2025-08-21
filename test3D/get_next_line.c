/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelbied <oelbied@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 11:20:11 by sidrissi          #+#    #+#             */
/*   Updated: 2025/08/20 10:14:17 by oelbied          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./include/cub3d.h"

char	*ft_left_over(char *line)
{
	char			*left_c;
	unsigned int	i;

	i = 0;
	while (line[i] != '\n' && line[i] != '\0')
		i++;
	if (line[i] == '\n')
		i++;
	left_c = ft_substr(line, i, ft_strlen(line) - i);
	if (left_c == NULL || *left_c == 0)
	{
		free(left_c);
		left_c = NULL;
	}
	free(line);
	line = NULL;
	return (left_c);
}

char	*read_file(int fd, char *left_over, char *buffer)
{
	ssize_t	read_char;
	char	*tmp;

	read_char = 1;
	while (read_char)
	{
		read_char = read(fd, buffer, BUFFER_SIZE);
		if (read_char < 0)
		{
			free(left_over);
			return (NULL);
		}
		if (read_char == 0)
			break ;
		buffer[read_char] = '\0';
		tmp = left_over;
		left_over = ft_strjoin(tmp, buffer);
		free(tmp);
		tmp = NULL;
		if (ft_strchr(buffer, '\n'))
			break ;
	}
	return (left_over);
}

char	*extr_line(char *line)
{
	unsigned int	i;
	char			*ext_line;

	i = 0;
	while (line[i] != '\n' && line[i] != '\0')
		i++;
	if (line[i] == '\n')
		i++;
	ext_line = malloc((i + 1) * sizeof(char));
	if (!ext_line)
		return (NULL);
	ft_strlcpy(ext_line, line, (i + 1));
	return (ext_line);
}

char	*get_next_line(int fd)
{
	static char	*left_over;
	char		*line;
	char		*buffer;
	char		*ex_line;

	buffer = (char *)malloc((size_t)(BUFFER_SIZE + 1) * sizeof(char));
	if (!buffer)
		return (NULL);
	if (fd < 0 || BUFFER_SIZE <= 0 || read(fd, 0, 0) < 0
		|| fd > OPEN_MAX || BUFFER_SIZE > 2147483647)
	{
		free(buffer);
		free(left_over);
		buffer = NULL;
		left_over = NULL;
		return (NULL);
	}
	line = read_file(fd, left_over, buffer);
	free(buffer);
	if (!line)
		return (NULL);
	ex_line = extr_line(line);
	left_over = ft_left_over(line);
	return (ex_line);
}
