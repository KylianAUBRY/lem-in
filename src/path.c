#include "../include/lem-in.h"

void chr_path(t_map *map, t_path *path)
{
	int i = path->size - 1;
	if (path->rooms[i] == map->end)
	{
		t_path *new_path = copy_path(map, path);
		new_path->next = map->path;
		map->path = new_path;
		return ;
	}
	int j = 0;
	while (path->rooms[i]->links[j]) // ici on parcour tout les enfant de notre rooms pour touverla plus loin dans la list
	{
		if (path->rooms[i]->links[j]->visited == 0) // si on a pas encore visité cette room
		{
			path->rooms[i]->links[j]->visited = 1; // on la visite
			add_room_to_path(map, path, path->rooms[i]->links[j]);
			chr_path(map, path); // on relance la fonction pour voir si on peut encore ajouter des rooms
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
