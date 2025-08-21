/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sidrissi <sidrissi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 18:07:40 by sidrissi          #+#    #+#             */
/*   Updated: 2025/05/22 18:08:53 by sidrissi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/cub3d.h"

char	*ft_strdup(char *s)
{
	char	*array;
	size_t	len;
	size_t	i;

	if (!s)
		return (NULL);
	len = ft_strlen(s);
	i = 0;
	array = (char *)malloc((len + 1) * sizeof(char));
	if (array == NULL)
		return (NULL);
	while (s[i])
	{
		array[i] = s[i];
		i++;
	}
	array[i] = '\0';
	return (array);
}