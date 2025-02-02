#include "../include/lem-in.h"

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
		handle_error("the start room is already defined", free_all, map, NULL);
	char *line = get_next_line(0);
	if (links_or_rooms(line) == 0)
	{
		t_room *room = add_room(map, line);
		map->start = room;
	}
	else
	{
		handle_error("the line after ##start must be a room", free_all, map, line, NULL);
	}
	free(line);
}

void add_end(t_map *map)
{
	if (map->end)
		handle_error("the end room is already defined", free_all, map, NULL);
	char *line = get_next_line(0);
	if (links_or_rooms(line) == 0)
	{
		t_room *room = add_room(map, line);
		map->end = room;
	}
	else
	{
		handle_error("the line after ##end must be a room", free_all, map, line, NULL);
	}
	free(line);
}

int check_exit_path(t_map *map, t_room *room)
{
	room->visited = 1;
	if (room == map->end)
		return 0;
	int i = 0;
	while (room->links[i])
	{
		if (room->links[i]->visited == 0)
		{
			if (check_exit_path(map, room->links[i]) == 0)
				return 0;
		}
		i++;
	}
	return 1;
}

void verif_map(t_map *map)
{
	if(map->ant == 0)
		handle_error("the number of ants is not defined", free_all, map, NULL);
	if (!map->start)
		handle_error("the start room is not defined", free_all, map, NULL);
	if (!map->end)
		handle_error("the end room is not defined", free_all, map, NULL);
	t_room *tmpRoom = map->room;
	while(tmpRoom)
	{
		t_room *room = tmpRoom->next;
		while (room)
		{
			if (ft_strcmp(tmpRoom->name, room->name) == 0)
				handle_error("the rooms must have different names", free_all, map, NULL);
			if (tmpRoom->x == room->x && tmpRoom->y == room->y)
				handle_error("the rooms must have different coordinates", free_all, map, NULL);
			room = room->next;
		}
		tmpRoom = tmpRoom->next;
	}
	if (check_exit_path(map, map->start) == 1)
		handle_error("the start and end rooms are not connected", free_all, map, NULL);
}

void add_link_to_room(t_map *map)
{
	t_link *link;
	int i;
	t_room *room = map->room;
	while (room)
	{
		i = 0;
		link = map->link;
		while (link)
		{
			if (ft_strcmp(room->name, link->room1->name) == 0)
				i++;
			else if (ft_strcmp(room->name, link->room2->name) == 0)
				i++;
			link = link->next;
		}
		room->links = (t_room **)malloc(sizeof(t_room *) * (i + 1));
		if (!room->links)
			handle_error("malloc failed", free_all, map, NULL);
		i = 0;
		link = map->link;
		while (link)
		{
			if (ft_strcmp(room->name, link->room1->name) == 0)
			{
				room->links[i] = link->room2;
				i++;
			}
			else if (ft_strcmp(room->name, link->room2->name) == 0)
			{
				room->links[i] = link->room1;
				i++;
			}
			link = link->next;
		}
		
		room->links[i] = NULL;
		room = room->next;
	}
}

void parcing(t_map *map)
{
	errno = 0;
	char *line = get_next_line(0);
	if (!line)
	{
		if (errno == 0)
			handle_error("empty standard output, please respect the format : \nnumber_of_ants\nthe_rooms\nthe_links", free_all, map, NULL);
		else
			handle_error(strerror(errno), free_all, map, NULL);
	}
	map->ant = ft_atoi(line);
	free(line);
	if (map->ant < 0)
		handle_error("the first line (number of ants) must contain only numbers", free_all, map, NULL);
	else if (map->ant == 0)
		handle_error("the first line (number of ants) must contain a number greater than 0", free_all, map, NULL);
	for (line = get_next_line(0); line; line = get_next_line(0))
	{
		// printf("line = %s\n", line);
		if (line[0] == 'L')
			handle_error("the line must not start with 'L'", free_all, map, line, NULL);
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
				handle_error("the line must contain a room or a link", free_all, map, line, NULL);
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
		handle_error(strerror(errno), free_all, map, NULL);
	
	add_link_to_room(map);
	verif_map(map);
}
