#include "lem-in.h"

int links_or_rooms(char *line) // -1 == error, 0 == rooms, 1 == links
{
	int j;
	for (int i = 0; line[i]; i++)
	{
		if (line[i] == '-')
		{
			if (i == 0)
				return -1;
			j = i;
			while (line[i])
			{
				i++;
				if (line[i] == '-')
					return -1;
				else if (line[i] == ' ')
					return -1;
			}
			if (j == i)
				return -1;
			return 1; // links
		} else if (line[i] == ' ')
		{
			if (i == 0)
				return -1;
			i++;
			j = i;
			while (line[i] && line[i] != ' ')
				i++;
			if (j == i || line[i] == '\0')
				return -1;
			j = i;
			while (line[i])
			{
				i++;
				if (line[i] == '-')
					return -1;
				else if (line[i] == ' ')
					return -1;
			}
			if (j == i)
				return -1;
			return 0; // rooms
		}
	}
	return -1;
}

void add_start(t_map *map)
{
	if (map->start)
		handle_error("the start room is already defined", free_all, map);
	char *line = get_next_line(0);
	if (links_or_rooms(line) == 0)
	{
		t_room *room = add_room(map, line);
		map->start = room;
	}
	else
	{
		handle_error("the line after ##start must be a room", free_all, map, line);
	}
}

void add_end(t_map *map)
{
	if (map->end)
		handle_error("the end room is already defined", free_all, map);
	char *line = get_next_line(0);
	if (links_or_rooms(line) == 0)
	{
		t_room *room = add_room(map, line);
		map->start = room;
	}
	else
	{
		handle_error("the line after ##end must be a room", free_all, map, line);
	}
}


void parcing(t_map *map)
{
	errno = 0;
	char *line = get_next_line(0);
	if (!line)
	{
		if (errno == 0)
			handle_error("empty standard output, please respect the format : \nnumber_of_ants\nthe_rooms\nthe_links", free_all, map);
		else
			handle_error(strerror(errno), free_all, map);
	}
	map->ant = ft_atoi(line);
	free(line);
	if (map->ant < 0)
		handle_error("the first line (number of ants) must contain only numbers", free_all, map);
	else if (map->ant == 0)
		handle_error("the first line (number of ants) must contain a number greater than 0", free_all, map);
	printf("ant = %d\n", map->ant);
	for (line = get_next_line(0); line; line = get_next_line(0))
	{
		printf("%s\n", line);
		if (line[0] == 'L')
			handle_error("the line must not start with 'L'", free_all, map, line);
		else if (line[0] == '\0' || line[0] == '\n')
		{
			free (line);
			break;
		} else if (line[0] == '#') // commande
		{
			if (ft_strcmp(line, "##start") == 0)
			{
				free (line);
				add_start(map);
			}
			else if (ft_strcmp(line, "##end") == 0)
			{
				free (line);
				add_end(map);
			}
			else
				free (line);
			continue;
		}
		switch (links_or_rooms(line))
		{
			case -1: //error
				handle_error("the line must contain a room or a link", free_all, map, line);
				break;
			case 0: //rooms
				add_room(map, line);
				break;
			case 1: //links
				add_link(map, line);
				break;
		}
		free(line);
	}
	if (errno)
		handle_error(strerror(errno), free_all, map);
}