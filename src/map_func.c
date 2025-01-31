# include "../include/lem-in.h"

t_room *get_room(t_map *map, char *name)
{
	t_room *room = map->room;
	while (room)
	{
		if (ft_strcmp(room->name, name) == 0)
			return room;
		room = room->next;
	}
	return NULL;
}

t_link *get_link(t_map *map, t_room *room1, t_room *room2)
{
	t_link *link = map->link;
	while (link)
	{
		if ((link->room1 == room1 && link->room2 == room2) || (link->room1 == room2 && link->room2 == room1))
			return link;
		link = link->next;
	}
	return NULL;
}

t_lem *get_lem(t_map *map, int identityNumber)
{
	t_lem *lem = map->lem;
	while (lem)
	{
		if (lem->identityNumber == identityNumber)
			return lem;
		lem = lem->next;
	}
	return NULL;
}

t_room *add_room(t_map *map, char *line)
{
	t_room *room = (t_room *)malloc(sizeof(t_room));
	if (!room)
		return NULL;
	room->name = ft_strdup_char(line, ' ');
	if (!room->name)
		handle_error("malloc failed", free_all, map, line);
	while (line && *line && *line != ' ')
		line++;
	room->x = ft_atoi(line);
	while (line && *line && *line != ' ')
		line++;
	room->y = ft_atoi(line);
	room->next = map->room;
	map->room = room;
	if (room->x < 0 || room->y < 0)
		handle_error("the coordinates of the rooms must be numbers and positive", free_all, map, line);
	return room;
}

t_link *add_link(t_map *map, char *line)
{
	t_link *link = (t_link *)malloc(sizeof(t_link));
	if (!link)
		return NULL;
	t_room *room1 = get_room(map, ft_strdup_char(line, '-'));
	if (!room1)
		handle_error("the room of the link does not exist", free_all, map, line, link);
	while (line && *line && *line != '-')
		line++;
	line++;
	t_room *room2 = get_room(map, ft_strdup(line));
	if (!room2)
		handle_error("the room of the link does not exist", free_all, map, line, link);
	link->room1 = room1;
	link->room2 = room2;
	link->next = map->link;
	map->link = link;
	return link;
}

t_lem *add_lem(t_map *map, int identityNumber, t_room *room)
{
	t_lem *lem = (t_lem *)malloc(sizeof(t_lem));
	if (!lem)
		return NULL;
	lem->identityNumber = identityNumber;
	lem->room = room;
	lem->next = map->lem;
	map->lem = lem;
	return lem;
}

void free_link(t_link *link)
{
	if (link)
	{
		free_link(link->next);
		free(link);
	}
}

void free_room(t_room *room)
{
	if (room)
	{
		free_room(room->next);
		free(room->name);
		free(room);
	}
}

void free_lem(t_lem *lem)
{
	if (lem)
	{
		free_lem(lem->next);
		free(lem);
	}
}