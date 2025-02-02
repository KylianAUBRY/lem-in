#include "../include/lem-in.h"

void chr_path(t_map *map, t_path *path)
{
	int i = path->size - 1;
	if (path->rooms[i] == map->end)
	{
		t_path *new_path = copy_path(map, path);
		new_path->score = path->size + map->ant;
		t_path *tmp = map->path;
		t_path *tmp2 = NULL;
		while (tmp)
		{
			if (tmp->score > new_path->score)
			{
				new_path->next = tmp;
				if (tmp == map->path)
					map->path = new_path;
				else
					tmp2->next = new_path;
				return ;
			}
			tmp2 = tmp;
			tmp = tmp->next;
		}
		new_path->next = map->path;
		map->path = new_path;
		return ;
	}
	int j = 0;
	while (path->rooms[i]->links[j])
	{
		if (path->rooms[i]->links[j]->visited == 0)
		{
			path->rooms[i]->links[j]->visited = 1;
			add_room_to_path(map, path, path->rooms[i]->links[j]);
			chr_path(map, path);
			path->rooms[i]->links[j]->visited = 0;
			dell_room_to_path(path);
		}
		j++;
	}
}

void get_path(t_map *map)
{
	map->start->visited = 1;
	for (int i = 0; map->start->links[i]; i++)
		map->start->links[i]->visited = 1;
	map->end->visited = 0; // security if start and end are directly connected
	for (int i = 0; map->start->links[i]; i++)
	{
		t_path *new_path = malloc(sizeof(t_path));
		if (!new_path)
			handle_error("malloc failed", free_all, map, NULL);
		new_path->size = 0;
		add_room_to_path(map, new_path, map->start->links[i]);
		
		chr_path(map, new_path);
		free_path(new_path);
	}
}

int nb_path_max(t_map *map)
{
	int score = 0, score2 = 0;

	for (int i = 0; map->start->links[i] && score <= map->ant; i++)
	{
		t_path *path = map->path;
		while (path)
		{
			if (path->rooms[0] == map->start->links[i])
			{
				score ++;
				break ;
			}
			path = path->next;
		}
	}
	for (int i = 0; map->end->links[i] && score2 < score; i++)
	{
		t_path *path = map->path;
		while (path)
		{
			if (path->size <= 1 || path->rooms[path->size - 2] == map->end->links[i])
			{
				score2 ++;
				break ;
			}
			path = path->next;
		}
	}
	return (score < score2) ? score : score2;
}

