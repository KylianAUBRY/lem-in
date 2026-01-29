#include "../include/lem-in.h"

t_map *init_struct()
{
	t_map *map = (t_map *)malloc(sizeof(t_map));
	if (!map)
		handle_error("malloc failed", NULL, 0);
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

void draw_room_recursive(t_room *room, t_map *map)
{
	if (!room)
		return ;
	draw_room_recursive(room->next, map);
	if (room->name == map->start->name)
		write(1, "##start\n", 8);
	else if (room->name == map->end->name)
		write(1, "##end\n", 6);
	ft_putstr(room->name);
	write(1, " ", 1);
	ft_putnbr(room->x); 
	write (1, " ", 1);
	ft_putnbr(room->y);
	write(1, "\n", 1);
}

void draw_room(t_map *map)
{
	draw_room_recursive(map->room, map);
}

void draw_links_recursive(t_link *link)
{
	if (!link)
		return ;
	draw_links_recursive(link->next);
	ft_putstr(link->room1->name);
	write(1, "-", 1);
	ft_putstr(link->room2->name);
	write(1, "\n", 1);
}

void draw_links(t_map *map)
{
	draw_links_recursive(map->link);
}

void run_simulation(t_map *map)
{
	ft_putnbr(map->ant);
	write(1, "\n", 1);
	draw_room(map);
	draw_links(map);
	write(1, "\n", 1);

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
	//char *c = malloc(sizeof (char) * 1000); (void) c ; return 0;
	t_map *map = init_struct();
	parcing(map);
	reset_visited(map);
	get_path(map);
	int max_path = nb_path_max(map);
	if (max_path > 1)
		get_multi_path(map, max_path);
	else {
		t_multi_path *multi_path = malloc(sizeof(t_multi_path));
		if (!multi_path)
			handle_error("malloc failed", map, 0);
		multi_path->size = 1;
		multi_path->score = map->path->score;
		multi_path->paths = (t_path **)malloc(sizeof(t_path *) * 2);
		if (!multi_path->paths)
			handle_error("malloc failed", map, 1, multi_path);
		multi_path->paths[0] = map->path;
		multi_path->paths[1] = NULL;
		multi_path->next = NULL;
		map->multi_path = multi_path;
	}
	write(1, "run simulation\n", 15);
	run_simulation(map);
	// Remove free_all call, handle_error does the cleanup
	if (map)
	{
		if (map->path)
			free_path(map->path);
		if (map->link)
			free_link(map->link);
		if (map->room)
			free_room(map->room);
		if (map->lem)
			free_lem(map->lem);
		if (map->multi_path)
			free_multi_path(map->multi_path);
		free(map);
	}
	return 0;
}
