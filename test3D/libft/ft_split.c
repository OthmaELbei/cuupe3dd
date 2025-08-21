#include "../include/cub3d.h"

static void	*ft_free(char **strs, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		free(strs[i]);
		i++;
	}
	free(strs);
	return (NULL);
}

int	is_sep(char s, char *sep)
{
	int	i;

	i = 0;
	while (sep[i])
	{
		if (sep[i] == s)
			return (1);
		i++;
	}
	return (0);
}

char	*fill_word(char const *s, int *preffix, char *sep)
{
	char	*word;
	int		i;
	int		len_word;

	len_word = 0;
	i = *preffix;
	while (s[i] && is_sep(s[i], sep))
		i++;
	*preffix = i;
	while (s[i] && !(is_sep(s[i], sep)))
	{
		len_word++;
		i++;
	}
	word = malloc((len_word + 1) * sizeof(char));
	if (!word)
		return (NULL);
	i = 0;
	while (i < len_word)
		word[i++] = s[(*preffix)++];
	word[i] = '\0';
	return (word);
}

int	word_count(char const *s, char *sep)
{
	int	i;
	int	words;

	words = 0;
	i = 0;
	while (s[i])
	{
		if (!is_sep(s[i], sep))
		{
			words++;
			while (s[i] && !is_sep(s[i], sep))
				i++;
		}
		else
			i++;
	}
	return (words);
}

char	**ft_split(char const *s, char *sep)
{
	char	**word_arr;
	int		number_words;
	int		i;
	int		preffix;

	if (!s)
		return (NULL);
	preffix = 0;
	number_words = word_count(s, sep);
	word_arr = (char **)malloc((number_words + 1) * sizeof(char *));
	if (!word_arr)
		return (NULL);
	i = 0;
	while (i < number_words)
	{
		word_arr[i] = fill_word(s, &preffix, sep);
		if (!word_arr[i])
			return (ft_free(word_arr, i));
		i++;
	}
	word_arr[i] = NULL;
	return (word_arr);
}