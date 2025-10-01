#include "../include/lem-in.h"

void free_all(t_map* map, ...)
{
	va_list args;

	va_start(args, map);
	for(void *ptr = va_arg(args, void*); ptr; ptr = va_arg(args, void *))
		free(ptr);
	va_end(args);
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
}

void handle_error(const char *message, void (*cleanup)(t_map *, ...), ...)
{
	//write(2, "Error : ", 8);
	write(1, "ERROR\n", 6);
	(void) message;
	//write(2, message, ft_strlen(message));
	//write(2, "\n", 1);
	va_list args;
	va_start(args, cleanup);
    cleanup(va_arg(args, void *), va_arg(args, void *), NULL);
    va_end(args);
	exit(1);
}