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

void print_best_path(t_map *map)
{
	t_multi_path *multi_path = map->multi_path;
	printf("Best Path score: %d\n", multi_path->score);
	for (int i = 0; multi_path->paths[i]; i ++)
	{
		int j = 0;
		while (multi_path->paths[i]->rooms[j])
		{
			printf("%s ", multi_path->paths[i]->rooms[j]->name);
			j++;
		}
		printf("\n");
	}
}

void room_allocation(t_map *map, t_lem *lem, int link_number, int remaining_ants)
{
	if (link_number == 0)
	{
		lem->indice = 0;
		lem->path = map->multi_path->paths[0];
		return ;
	}
	else if ((float)remaining_ants / link_number > map->multi_path->paths[link_number]->size - map->multi_path->paths[0]->size)
	{
		lem->indice = 0;
		lem->path = map->multi_path->paths[link_number];
	}
	else
	{
		lem->indice = -1;
	}
}

void print_and_change_of_room(t_map *map, t_lem *lem, int *remaning_ants)
{
	if (lem->indice == 0)
	{
		write(1, "L", 1);
		ft_putnbr(lem->identityNumber);
		write(1, "-", 1);
		write(1, lem->path->rooms[lem->indice]->name, ft_strlen(lem->path->rooms[lem->indice]->name));
		write(1, " ", 1);
		lem->indice++;
		*remaning_ants -= 1;
	}
	else if (lem->path->rooms[lem->indice])
	{
		write(1, "L", 1);
		ft_putnbr(lem->identityNumber);
		write(1, "-", 1);
		write(1, lem->path->rooms[lem->indice]->name, ft_strlen(lem->path->rooms[lem->indice]->name));
		write(1, " ", 1);
		lem->indice++;
	}
	else
	{
		dell_lem(map, lem->identityNumber);
	}
}

void run_simulation(t_map *map)
{
	int remaining_ants = map->ant;
	for(int i = map->ant; i > 0; i--)
		add_lem(map, i);
	while(map->lem)
	{
		t_lem *lem = map->lem;
		int link_number = 0;
		while (lem && remaining_ants > 0 && map->multi_path->paths[link_number])
		{
			if (lem->indice == -1)
			{
				room_allocation(map, lem, link_number, remaining_ants);
				link_number++;
			}
			lem = lem->next;
		}
		lem = map->lem;
		while (lem && lem->indice >= 0)
		{
			t_lem *tmp = lem->next;
			print_and_change_of_room(map, lem, &remaining_ants);
			lem = tmp;
		}
		write(1, "\n", 1);
	}

	// while (map->lem)
	// {
	// 	t_lem *lem = map->lem;
	// 	for (int linker_number = 0; remaining_ants > 0 && map->multi_path->paths[linker_number]; linker_number++)
	// 	{
	// 		while (lem && lem->indice == -1)
	// 		{
	// 			if ()
	// 			room_allocation(map, lem, linker_number, remaining_ants);
	// 			lem = lem->next;
	// 		}
	// 	}
	// 	lem = map->lem;
	// 	while (lem && lem->indice >= 0)
	// 	{
	// 		t_lem *tmp = lem->next;
	// 		print_and_change_of_room(map, lem, &remaining_ants);
	// 		lem = tmp;
	// 	}
	// 	write (1, "\n", 1);
	// }
}

int main() 
{
	t_map *map = init_struct();
	parcing(map);
	// draw_struct(map);
	reset_visited(map);
	get_path(map);

	// print_path(map);
	int max_path = nb_path_max(map);
	if (max_path > 1)
		get_multi_path(map, max_path);
	// printf("max_path = %d\n", max_path);
	// print_multi_path(map);

	printf("\nant : %d\n\n", map->ant);
	print_best_path(map);
	write(1, "\n", 1);
	run_simulation(map);

	free_all(map, NULL);
	return 0;
}
