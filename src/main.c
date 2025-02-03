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

void room_allocation(t_map *map, t_lem *lem, int link_number, int remaining_ants)
{
	if (link_number == 0)
	{
		lem->indice = 0;
		lem->path = map->multi_path->paths[0];
	}
	else if ((float)remaining_ants / link_number > map->multi_path->paths[link_number]->size - map->multi_path->paths[0]->size)
	{
		lem->indice = 0;
		lem->path = map->multi_path->paths[link_number];
	}
	else
		lem->indice = -1;
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
}

int main() 
{
	t_map *map = init_struct();
	parcing(map);
	reset_visited(map);
	get_path(map);
	int max_path = nb_path_max(map);
	if (max_path > 1)
		get_multi_path(map, max_path);
	run_simulation(map);
	free_all(map, NULL);
	return 0;
}
