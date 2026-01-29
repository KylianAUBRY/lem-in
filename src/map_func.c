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
		if ((link->room1 == room1 && link->room2 == room2))// || (link->room1 == room2 && link->room2 == room1))
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
	int i = 0;
	t_room *room = (t_room *)malloc(sizeof(t_room));
	if (!room)
		handle_error("malloc failed", map, 1, line);
	room->name = malloc(sizeof(char) * (ft_strlen(line) + 1));
	if(!room->name)
		handle_error("malloc failed", map, 2, room, line);
	while (line && *line && *line != ' ' && *line != '-')
	{
		room->name[i] = *line;
		line++;
		i++;
	}
	room->name[i] = '\0';
	// room->name = ft_strdup_char(line, ' ');
	// if (!room->name)
	// 	handle_error("malloc failed", map, 1, line);
	// while (line && *line && *line != ' ')
	// 	line++;
	if(*line == '-')
	{
		free(room->name);
		free(room);
		return NULL;
	}
	if (line == NULL || *line == '\0')
		handle_error("the coordinates of the rooms must be numbers", map, 2, line, room);
	line++;
	i = 0;
	while (line[i] && line[i] != ' ')
		i++;
	if (line[i] == '\0')
		handle_error("the coordinates of the rooms must be numbers", map, 2, line, room);
	line[i] = '\0';
	room->x = ft_atoi(line);
	line += i + 1;
	room->y = ft_atoi(line);
	if (room->x < 0 || room->y < 0)
		handle_error("the coordinates of the rooms must be numbers and positive", map, 2, line, room);
	room->visited = 0;
	room->next = map->room;
	room->links = NULL;
	map->room = room;
	return room;
}

t_link *add_link(t_map *map, char *line)
{
	char *tmpLine = line;
	int i = 0;
	t_link *link = (t_link *)malloc(sizeof(t_link));
	if (!link)
		return NULL;
	while (line[i] && line[i] != '-')
		i++;
	tmpLine[i] = '\0';
	t_room *room1 = get_room(map, tmpLine);
	if (!room1)
		handle_error("the room of the link does not exist1", map, 2, line, link);
	tmpLine += i + 1;
	t_room *room2 = get_room(map, tmpLine);
	if (!room2)
		handle_error("the room of the link does not exist", map, 2, line, link);
	if (get_link(map, room1, room2))
	{
		// write (2, room1->name , ft_strlen(room1->name));
		// write (2, "-", 1);
		// write (2, room2->name , ft_strlen(room2->name));
		// write (1, "\n", 1);
		handle_error("the link already exists", map, 2, line, link);
	}
	if (room1 == room2)
		handle_error("the link must connect two different rooms3", map, 2, line, link);
	link->room1 = room1;
	link->room2 = room2;
	link->next = map->link;
	map->link = link;
	return link;
}

t_lem *add_lem(t_map *map, int identityNumber)
{
	t_lem *lem = (t_lem *)malloc(sizeof(t_lem));
	if (!lem)
		handle_error("malloc failed", map, 0);
	lem->identityNumber = identityNumber;
	lem->indice = -1;
	lem->path = NULL;
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
		link = NULL;
	}
}

void free_room(t_room *room)
{
	if (room)
	{
		if (room->links)
		{
			free(room->links);
			room->links = NULL;
		}
		free_room(room->next);
		free(room->name);
		room->name = NULL;
		free(room);
		room = NULL;
	}
}

void free_lem(t_lem *lem)
{
	if (lem)
	{
		free_lem(lem->next);
		lem->next = NULL;
		free(lem);
		lem = NULL;
	}
}

void free_path(t_path *path)
{
	if (path)
	{
		free_path(path->next);
		if (path->size > 0)
		{
			free(path->rooms);
			path->rooms = NULL;
		}
		free(path);
		path = NULL;
	}
}

void free_multi_path(t_multi_path *multi_path)
{
	if (multi_path)
	{
		free_multi_path(multi_path->next);
		if (multi_path->paths)
		{
			free(multi_path->paths);
			multi_path->paths = NULL;
		}
		free(multi_path);
		multi_path = NULL;
	}
}

void dell_lem(t_map *map, int identityNumber)
{
	t_lem *lem = map->lem;
	t_lem *tmp = NULL;
	while (lem)
	{
		if (lem->identityNumber == identityNumber)
		{
			if (tmp)
				tmp->next = lem->next;
			else
				map->lem = lem->next;
			free(lem);
			return;
		}
		tmp = lem;
		lem = lem->next;
	}
}

t_path *copy_path(t_map *map, t_path *path)
{
	t_path *new_path = (t_path *)malloc(sizeof(t_path));
	if (!new_path)
		handle_error("malloc failed", map, 0);
	new_path->size = path->size;
	new_path->rooms = (t_room **)malloc(sizeof(t_room *) * (5000));
	if (!new_path->rooms)
		handle_error("malloc failed", map, 1, new_path);
	for (int i = 0; i <= path->size; i++)
		new_path->rooms[i] = path->rooms[i];
	new_path->next = NULL;
	return new_path;
}

int add_room_to_path(t_map *map, t_path *path, t_room *room)
{
	if (path->size == 0)
	{
		path->rooms = (t_room **)malloc(sizeof(t_room *) * PATH_SIZE);
		if (!path->rooms)
			handle_error("malloc failed", map, 1, path);
	}
	else if (path->size == (PATH_SIZE - 1))
	{
		return -1;
		handle_error("the path is too long (is a bit too much for me)", map, 0);
	}
	path->rooms[path->size] = room;
	path->size++;
	path->rooms[path->size] = NULL;
	return 0;
}

void dell_room_to_path(t_path *path)
{
	path->size--;
	if (path->size == 0)
	{
		free(path->rooms);
	}
	else
		path->rooms[path->size] = NULL;
}

void reset_visited(t_map *map)
{
	t_room *room = map->room;
	while (room)
	{
		room->visited = 0;
		room = room->next;
	}
}

t_multi_path *copy_multi_path(t_map *map, t_multi_path *multi_path)
{
	t_multi_path *new_multi_path = (t_multi_path *)malloc(sizeof(t_multi_path));
	if (!new_multi_path)
		handle_error("malloc failed", map, 0);
	new_multi_path->size = multi_path->size;
	new_multi_path->paths = (t_path **)malloc(sizeof(t_path *) * (multi_path->size + 1));
	if (!new_multi_path->paths)
		handle_error("malloc failed", map, 1, new_multi_path);
	for (int i = 0; i < multi_path->size; i++)
		new_multi_path->paths[i] = multi_path->paths[i];
	new_multi_path->paths[multi_path->size] = NULL;
	new_multi_path->next = NULL;
	return new_multi_path;
}
