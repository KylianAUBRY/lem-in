#include "lem-in.h"

t_map *init_struct()
{
	t_map *map = (t_map *)malloc(sizeof(t_map));
	if (!map)
		handle_error("malloc failed", NULL, NULL);
	map->ant = 0;
	map->start = NULL;
	map->end = NULL;
	map->room = NULL;
	map->link = NULL;
	map->lem = NULL;
	return map;
}


int main() 
{
	t_map *map = init_struct();
	parcing(map);
	free(map);
	return 0;
}