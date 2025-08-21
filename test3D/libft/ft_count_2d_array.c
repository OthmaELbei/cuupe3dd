/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_count_2d_array.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sidrissi <sidrissi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 00:22:55 by sidrissi          #+#    #+#             */
/*   Updated: 2025/07/13 17:45:08 by sidrissi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/cub3d.h"

int	ft_count_2d_array(char **arr)
{
	int	i;

	i = 0;
	if (!arr)
		return (i);
	while (arr[i] != NULL)
		i++;
	return (i);
}
