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
		handle_error("the number of ants is not defined", map, 0);
	if (!map->start)
		handle_error("the start room is not defined", map, 0);
	if (!map->end)
		handle_error("the end room is not defined", map, 0);
	t_room *tmpRoom = map->room;
	while(tmpRoom)
	{
		t_room *room = tmpRoom->next;
		while (room)
		{
			if (ft_strcmp(tmpRoom->name, room->name) == 0)
				handle_error("the rooms must have different names", map, 0);
			if (tmpRoom->x == room->x && tmpRoom->y == room->y)
				handle_error("the rooms must have different coordinates", map, 0);
			room = room->next;
		}
		tmpRoom = tmpRoom->next;
	}
	if (check_exit_path(map, map->start) == 1)
		handle_error("the start and end rooms are not connected", map, 0);
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
			handle_error("malloc failed", map, 0);
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

char *get_red_0()
{
	static char buff[1025];
	static char buff_tmp[1025] = "";
	char *buff_final;
	static int i_final = 0;
	int i;
	buff_final = malloc(sizeof(char) * 1024 + i_final + 1);
	if (!buff_final)
		return NULL;
	i_final = 0;
	while (buff_tmp[i_final])
	{
		buff_final[i_final] = buff_tmp[i_final];
		buff_tmp[i_final] = '\0';
		i_final++;
	}
	buff_final[i_final] = '\0';
	i = 1;
	i = read(0, buff, 1024);
	if (i <= 0)
	{
		buff[0] = '\0';
		return buff_final;
	}
	buff[i] = '\0';
	if(i < 1024)
	{
		i = 0;
		while(buff[i])
		{
			buff_final[i_final] = buff[i];
			i_final++;
			i++;
		}
		buff_final[i_final] = '\0';
		return buff_final;
	}
	while(i >= 0 && buff[i] != '\n')
	{
		i--;
	}
	int j = 0;
	buff[i] = '\0';
	i++;
	while (buff[i])
	{
		buff_tmp[j] = buff[i];
		i++;
		j++;
	}
	buff_tmp[j] = '\0';
	i = 0;
	while(buff[i])
	{
		buff_final[i_final] = buff[i];
		i_final++;
		i++;
	}
	buff_final[i_final] = '\0';
	if (i >= 0)
		return buff_final;
	else
	{
		free (buff_final);
		return get_red_0();
	}
	return buff_final;
}




void parcing(t_map *map)
{
	int i = 0;
	char *line = get_red_0();
	char *line_free = line;
	if (!line || line[0] == '\0')
	{
		if(line[0])
			free(line_free);
		handle_error("empty standard output, please respect the format : \nnumber_of_ants\nthe_rooms\nthe_links", map, 0);
	}
	while (line[0] == '#')
	{
		while(line[i] && line[i] != '\n')
		{
			i++;
		}
		if (line[i] == '\n')
		{
			i++;
			line += i;
			i = 0;
		}
		else
		{
			free (line_free);
			line = get_red_0();
			line_free = line;
		}
		
	}
	while(line[i] >= '0' && line[i] <= '9')
	{
		map->ant = map->ant * 10 + (line[i] - '0');
		i++;
	}
	if (map->ant < 0)
		handle_error("the first line (number of ants) must contain only numbers", map, 1, line_free);
	else if (map->ant == 0)
		handle_error("the first line (number of ants) must contain a number greater than 0", map, 1, line_free);
	else if (map->ant > 10000)
		handle_error("the number of ants is too high", map, 0);
	while(line[i] == ' ')
		i++;
	if(line[i] != '\n')
		handle_error("the first line (number of ants) must contain only numbers", map, 1, line_free);
	while (line[i] == '\n' && line[i] == ' ')
		i++;
	if (line[i] == '\0')
		handle_error("the map is not defined", map, 1, line_free);
	i++;
	line += i;
	int is_start = 0;
	int is_end = 0;
	int link = 0;
	while(1)
	{
		while (line[i])
		{
			while (line[i] && line[i] != '\n')
				i++;
			if (line[i] != '\0')
			{
				line[i] = '\0';
				i++;
			}
			if (line[0] == 'L')
				handle_error("the line must not start with 'L'", map, 1, line_free);
			else if (line[0] == '#')
			{
				if (ft_strcmp(line, "##start") == 0)
					is_start = 1;
				else if (ft_strcmp(line, "##end") == 0)
					is_end = 1;
				line += i;
				i = 0;
				if (!line || line[0] == '\0')
					break ;
				continue;
			}
			if (link == 0) //c'est donc une room
			{
				t_room *room = add_room(map, line, line_free);
				if (room == NULL)
				{
					if (is_start == 1 || is_end == 1)
						handle_error("the ##start or ##end command must be followed by a room", map, 1, line_free);
					link = 1;
				}
				else
				{
					if (is_start == 1)
					{
						if (map->start)
							handle_error("the start room is already defined", map, 1, line_free);
						map->start = room;
						is_start = 0;
					}
					else if (is_end == 1)
					{
						if (map->end)
							handle_error("the end room is already defined", map, 1, line_free);
						map->end = room;
						is_end = 0;
					}
				}
			}
			if (link == 1)//c'est un lien
			{
				add_link(map, line, line_free);
			}
			line += i;
			i = 0;
			if (!line || line[0] == '\0')
				break ;
		}
		free(line_free);
		line = get_red_0();
		line_free = line;
		if (!line || line[0] == '\0')
		{
			if(line)
				free(line_free);
			break ;
		}
	}
	if (errno)
		handle_error(strerror(errno), map, 0);
	add_link_to_room(map);
	verif_map(map);
}