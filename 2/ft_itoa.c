extern char	*ft_strdup(const char *s1);

static char	*to_string(long long_nb, int size, int *string_index)
{
	int		value[10];
	int		value_index;
	char	string[12];

	if (long_nb < 0)
	{
		long_nb *= -1;
		string[(*string_index)++] = '-';
	}
	value_index = 0;
	value[value_index++] = long_nb % size;
	while (value_index < 10 && (long_nb / size) != 0)
	{
		if (value_index < 9)
			value[value_index++] = (long_nb % (size * 10)) / size;
		else
			value[value_index++] = long_nb / size;
		size *= 10;
	}
	while (value_index > 0)
	{
		string[(*string_index)++] = value[--value_index] + 48;
	}
	string[(*string_index)] = '\0';
	return (ft_strdup(string));
}

char	*ft_itoa(int n)
{
	int		size;
	int		string_index;
	char	*s;
	long	long_nb;

	long_nb = n;
	size = 10;
	string_index = 0;
	s = to_string(long_nb, size, &string_index);
	return (s);
}
