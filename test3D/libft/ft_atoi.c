/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelbied <oelbied@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 00:34:40 by sidrissi          #+#    #+#             */
/*   Updated: 2025/08/20 10:13:54 by oelbied          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/cub3d.h"

long ft_atoi(char *str)
{
	long	res;
	int		sign;

	sign = 1;
	res = 0;
	while ((*str > 9 && *str < 13) || *str == 32)
		str++;
	if (*str == '+' || *str == '-')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	while (*str && (*str >= '0' && *str <= '9'))
	{
		res = res * 10 + (*str - '0');
		if ((sign == 1 && res > INT_MAX )||
			(sign == -1 && res > ((long)INT_MAX + 1)))
			return (1);
		str++;
	}
	return (res * sign);
}
