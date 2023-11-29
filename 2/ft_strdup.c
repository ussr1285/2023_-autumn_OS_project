#include<stdlib.h>

char	*ft_strdup(const char *s1)
{
	char	*new_str;
	int		src_size;
	int		i;

	src_size = 0;
	while (s1[src_size] != '\0')
		src_size++;
	new_str = (char *)malloc((src_size + 1) * sizeof(char));
	if (new_str == 0)
		return (0);
	i = -1;
	while (++i < src_size)
	{
		new_str[i] = s1[i];
	}
	new_str[i] = '\0';
	return (new_str);
}
