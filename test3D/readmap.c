/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readmap.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oelbied <oelbied@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 14:42:07 by oelbied           #+#    #+#             */
/*   Updated: 2025/08/21 11:12:10 by oelbied          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/cub3d.h"

int chick_all_line(char *str)
{
	int i =0;
	int  j = 0;

	while(str[i])
	{

		if(str[i] == ' ')
		{
			i++;
			j++;
		}
		if( i > 1 && str[i] != '1'  && str[i - 1] != ' ' && str[i] != '\n')
		{
			return 0;
		}
		if(str[i] == ' ')
		{
			j++;
		}
		i++;
	}
	return 1 ;
} 

int ft_tchklast_laine( char *line,char *lineTow, char *linThre)
{
	int linOne = 0;
	int li = 0;
	int linethre = 0;
	printf("%s",line);
		linOne = ft_strlen(line);
		li = ft_strlen(lineTow);
		linethre = ft_strlen(linThre);
		int i = 0;

	  if(linOne < li)
	  {
		i = linOne - 1 ;
		while (i <= li)
		{
			if(lineTow[i] == '0')
			{

				return 0;
			}
			i++;
		}
		
	  }
	//   if(linOne > li)
	//   {
	// 	i = linOne - 1 ;
	// 	while (i <= li)
	// 	{
	// 		if(lineTow[i] == '0')
	// 		{

	// 			return 0;
	// 		}
	// 		i++;
	// 	}
		
	//   }
	// //   else 
	if(linOne >= li && li > linethre)
	  {
	printf("%d\n",linOne);
		
		i = linethre - 1 ;
		int j = 0;
		j =li;
		while (i <= li)
		{
			if(lineTow[i] == '0')
			{
				return 0;
			}
			i++;
	        // j--;
		}
		
	  }
	return 1;
}

int ft_tchking_player(char **maps, int contLaien)
{
	int i = 1;
	int j = 0;
	int flags = 0;

   if(!chick_all_line(maps[0]) ||!chick_all_line(maps[contLaien - 1]) )
	return 0;
	contLaien -= 1;
	while (i < contLaien)
	{
		if (ft_tchklast_laine(maps[i - 1], maps[i],maps[i + 1]) ==  0)
			return (0);
		if (maps[i][0] == '0')
			return 0;
		j = 1;
		while ((maps[i][j] != '\0'))
		{
			if (maps[i][j + 1] != '\0')
			{
				if (maps[i][j] == '0' || maps[i][j] == 'N'|| maps[i][j] == 'S' || maps[i][j] == 'W'|| maps[i][j] == 'E')
				{
					if(maps[i][j] == 'N'|| maps[i][j] == 'S' || maps[i][j] == 'W'|| maps[i][j] == 'E')
						flags++;
					if ( flags >  1 || (maps[i][j + 1] == ' ')|| (maps[i][j + 1] == '\n') || (j > 0 && maps[i][j - 1] == ' ')
						|| (i > 0 && (maps[i + 1][j] != '\0') && maps[i - 1][j] == ' ')
							|| ((i) < contLaien && maps[i + 1][j] == ' '))
						return ( 0);
				}
			}
			j++;
		}
		i++;
	}
	if (ft_tchklast_laine(maps[i - 1], maps[i],maps[i + 1]) ==  0)
			return (0);
	return 1;
}

char **men(void)
{
	char *line_reading;
	char **maps = NULL;
	int i = 0;
	int j = 0;

	int fd = open("./map/map.cub", O_RDONLY);
	while ((line_reading = get_next_line(fd)) != NULL)
	{
		i++;
		free(line_reading);
	}
	close(fd);
	fd = open("./map/map.cub", O_RDONLY);
	maps = malloc(sizeof(char *) * (i ));
	while ((line_reading = get_next_line(fd)) != NULL)
	{
		maps[j] = ft_strdup(line_reading);
		free(line_reading);
		j++;
	}
	printf("%s",maps[j - 1]);
		maps[j ] = NULL;
	// j = 0;
	if(ft_tchking_player(maps , i))
	{
		printf("this maps is good\n");
		return maps;
	}
	else
	{
		printf("this maps is open\n");
		return NULL;
	}

	return NULL;
}



















// int men()
// {
// 	char *line_reading;
// 	// char *line_readi ;
// 	char **maps = NULL;
// 	int i = 0;
// 	int j = 0;
// 	int k = 1;
// 	int d = 0;
// 	int fd = open("./map/map.cub", O_RDONLY);
// 	while ((line_reading = get_next_line(fd)) != NULL)
// 	{
// 		i++;
// 		free(line_reading);
// 	}
// 	close(fd);
// 	fd = open("./map/map.cub", O_RDONLY);
// 	maps = malloc(sizeof(char *) * (i + 1));
// 	while ((line_reading = get_next_line(fd)) != NULL)
// 	{
// 		maps[j] = ft_strdup(line_reading);
// 		free(line_reading);
// 		j++;
// 	}
	
// 	// if (!chick_all_line(maps[0]) || !chick_all_line(maps[i - 1]))
// 	// {
// 	// 	printf("kddkkdk\n");
// 	// 	return 0;
// 	// }
// 	// int ii = 0;

// 	// while(ii < i - 1)
// 	// {
//     // d  =  ft_tchking_espass(maps[ii]) ;
	
//     //  printf ("%d====\n",d);
// 	// if ( d > 1 && maps[ii + 1] != NULL ){
// 	// 	// if(ft_tchklast_laine(d, maps[ii]) ==  0)
// 	// 	// {
// 	// 	// 	printf("%s++",maps[ii]);
// 	// 	// 	printf("last line is open\n");
// 	// 	// 	return 0;
// 	// 	// }
// 	// 	if(	tchk_out_plyr(maps[ii + 1], d) == 1)
// 	// 	{
// 	// 		printf("this line is open\n");
// 	// 		return 0;
// 	// 	}
// 	// 		// printf("%s+++",maps[ii]);
// 	// 		d  =  ft_tchking_espass(maps[ii]) ;
//     //  		// printf ("%d====\n",d);

// 	// 		if(	tchk_out_plyr(maps[ii + 1], d ) == 1)
// 	// 		{
// 	// 				printf("this line is ++ open\n");
// 	// 		return 0;
// 	// 		}
// 	// }
// 	// 		ii++;
// 	// }
// 	// // printf("%s+E+E",maps[i - 3]);
// 	// int sd =  i - 1;
// 	// while(sd >= 1)
// 	// {
		
// 	//   d  =  ft_tchking_espass(maps[sd]) ;
//     //  printf ("%d====\n",d);
// 	// if ( d > 1  ){
		
// 	// 	if(	 tchk_out_plyr(maps[sd - 1], d) == 1)
// 	// 	{
// 	// 		printf("this line is open\n");
// 	// 		return 0;
// 	// 	}
// 	// 		printf("%s+++",maps[sd]);
// 	// 		d  =  ft_tchking_espass(maps[sd]) ;
//     //  		printf ("%d====\n",d);

// 	// 		if(tchk_out_plyr(maps[sd - 1], d ) == 1)
// 	// 		{
// 	// 				printf("this line is ++ open\n");
// 	// 		return 0;
// 	// 		}
// 	// }
// 	// 		sd--;
// 	// }
// 	// while (k < i - 1)
// 	// {
// 	// int dd  = frst_finl_lien(maps[k]) ;
// 	// 	if (dd == 0)
// 	// 	{
// 	// 		printf("kddkkdk+++++\n");
// 	// 		return 0;
// 	// 	}
// 	// 	k++;
// 	// }

// 	// maps[j] = NULL;
// 	// j = 0;
// 	// i = 0;
// 	// while (maps && maps[j] != NULL)
// 	// {
// 	// 	printf("%s", maps[j]);
// 	// 	printf("%d", index(maps[j]));
// 	// 	free(maps[j]);
// 	// 	j++;
// 	// 	i++;
// 	// }
// 	free(maps);

// 	return 0;
// }