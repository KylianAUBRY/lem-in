#include "lem-in.h"

int ft_strlen(const char *str)
{
	int i;

	i = 0;
	while (str && str[i])
		i++;
	return i;
}

char *ft_strjoin_chr(char *s1, const char *s2, char c)
{
	char *dest;
	int i;
	int j;

	dest = malloc(sizeof(char) * (ft_strlen(s1) + ft_strlen(s2) + 1));
	if (!dest)
	{
		free(s1);
		return NULL;
	}
	i = 0;
	while (s1 && s1[i])
	{
		dest[i] = s1[i];
		i++;
	}
	j = 0;
	while (s2 && s2[j] && s2[j] != c)
	{
		dest[i + j] = s2[j];
		j++;
	}
	dest[i + j] = '\0';
	free(s1);
	return dest;
}

char *ft_strdup(const char *str)
{
	char *dest;
	int i;

	dest = malloc(sizeof(char) * (ft_strlen(str) + 1));
	if (!dest)
		return NULL;
	i = 0;
	while (str && str[i])
	{
		dest[i] = str[i];
		i++;
	}
	dest[i] = '\0';
	return dest;
}

char *ft_strdup_char(const char *str, char c)
{
	char *dest;
	int i;

	dest = malloc(sizeof(char) * (ft_strlen(str) + 1));
	if (!dest)
		return NULL;
	i = 0;
	while (str && str[i] && str[i] != c)
	{
		dest[i] = str[i];
		i++;
	}
	dest[i] = '\0';
	return dest;
}

int	ft_strcmp(const char *s1, const char *s2)
{
	int i;

	i = 0;
	while (s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	return (unsigned char)s1[i] - (unsigned char)s2[i];
}

int ft_atoi(const char *str)
{
	int i;
	int res;

	i = 0;
	res = 0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		res = res * 10 + str[i] - '0';
		i++;
	}
	if (str[i] != '\0')
		return -1;
	return res;
}

char	*ft_strchr(const char *s, int c)
{
	int	i;

	i = 0;
	while (s[i] && s != NULL)
	{
		if (s[i] == (const char)c)
			return ((char *)s + i);
		i++;
	}
	if (s[i] == (const char)c)
		return ((char *)s + i);
	return (NULL);
}

void ft_buf_cut(char *dest)
{
	int i;
	int j;

	i = 0;
	j = 0;
	while(dest[i] != '\n' && dest[i])
		i++;
	if (dest[i] == '\n')
		i++;
	while (dest[j + i])
	{
		dest[j] = dest[i + j];
		j++;
	}
	dest[j] = '\0';
}

char *get_next_line(int fd)
{
	static char buff[1024];
	char *dest;
	int i;

	i = 1;
	dest = NULL;
	if (ft_strchr(buff, '\n') != NULL)
	{
		ft_buf_cut(buff);
		dest = ft_strjoin_chr(dest, buff, '\n');
		if (!dest)
			return NULL;
	}
	while (i > 0 && ft_strchr(buff, '\n') == NULL)
	{
		i = read(fd, buff, 1024);
		if (i == -1)
		{
			free(dest);
			return NULL;
		}
		else if (i == 0)
			return dest;
		buff[i] = '\0';
		dest = ft_strjoin_chr(dest, buff, '\n');
		if (!dest)
			return NULL;
	}
	return dest;
}