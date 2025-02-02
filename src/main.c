#include "../include/lem-in.h"

t_map *init_struct()
{
	t_map *map = (t_map *)malloc(sizeof(t_map));
	if (!map)
		handle_error("malloc failed", NULL, NULL);
	map->ant = 0;
	map->path = NULL;
	map->start = NULL;
	map->end = NULL;
	map->room = NULL;
	map->link = NULL;
	map->lem = NULL;
	map->path = NULL;
	map->multi_path = NULL;
	return map;
}

void draw_struct(t_map *map)
{
	t_room *room = map->room;
	t_link *link = map->link;
	t_lem *lem = map->lem;

	printf("ant = %d\n", map->ant);
	printf("start = %s\n", map->start->name);
	printf("end = %s\n", map->end->name);
	while (room)
	{
		printf("room = %s %d %d\n", room->name, room->x, room->y);
		room = room->next;
	}
	while (link)
	{
		printf("link = %s %s\n", link->room1->name, link->room2->name);
		link = link->next;
	}
	while (lem)
	{
		printf("lem = %d %s\n", lem->identityNumber, lem->room->name);
		lem = lem->next;
	}
}

void print_path(t_map *map)
{
	t_path *path = map->path;
	int j = 0;
	while (path)
	{
		int i = 0;
		printf("path :%d   score :%d\n", j, path->score);
		while (path->rooms[i])
		{
			printf("%s ", path->rooms[i]->name);
			i++;
		}
		printf("    code = %d\n", path->score);
		path = path->next;
		j++;
	}
}

void print_multi_path(t_map *map)
{
	t_multi_path *multi_path = map->multi_path;
	int j = 0;
	while (multi_path)
	{
		int i = 0;
		printf("multi_path :%d   score : %d\n", j, multi_path->score);
		while (multi_path->paths[i])
		{
			printf("\n");
			int k = 0;
			while (multi_path->paths[i]->rooms[k])
			{
				printf("%s ", multi_path->paths[i]->rooms[k]->name);
				k++;
			}
			i++;
		}
		printf("\n\n");
		multi_path = multi_path->next;
		j++;
	}
}

int main() 
{
	t_map *map = init_struct();
	parcing(map);
	// draw_struct(map);
	reset_visited(map);
	get_path(map);

	print_path(map);
	int max_path = nb_path_max(map);
	if (max_path > 1)
		get_multi_path(map, max_path);
	printf("max_path = %d\n", max_path);
	print_multi_path(map);

	

	free_all(map, NULL);
	return 0;
}
