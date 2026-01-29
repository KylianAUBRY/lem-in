#include "../include/lem-in.h"

void handle_error(const char *message, t_map *map, int num_ptrs, ...)
{
	va_list args;
    va_start(args, num_ptrs);
    for (int i = 0; i < num_ptrs; i++) {
        void *ptr = va_arg(args, void *);
        if (ptr) {
            free(ptr);
        }
    }
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

    write(2, message, ft_strlen(message));
    write(2, "\n", 1);

    exit(EXIT_FAILURE);
}