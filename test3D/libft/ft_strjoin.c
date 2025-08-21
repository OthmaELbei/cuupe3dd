/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sidrissi <sidrissi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 18:06:09 by sidrissi          #+#    #+#             */
/*   Updated: 2025/05/22 18:09:06 by sidrissi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/cub3d.h"

char	*ft_strjoin(char *s1, char *s2)
{
	char	*array;
	size_t	len1;
	size_t	len2;

	if (!s1 && !s2)
		return (NULL);
	if (!s1)
		return (ft_strdup(s2));
	if (!s2)
		return (ft_strdup(s1));
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	array = (char *)malloc((len1 + len2 + 1));
	if (array == NULL)
		return (NULL);
	ft_strlcpy(array, s1, len1 + 1);
	ft_strlcpy(array + len1, s2, len2 + 1);
	return (array);
}