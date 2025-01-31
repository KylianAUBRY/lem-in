#pragma once

/* ==================== include ==================== */

# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <errno.h>
#include <stdarg.h>


/* ==================== structure ==================== */

typedef struct s_room
{
	char	*name;
	int		x;
	int		y;
	struct s_room	*next;
}				t_room;

typedef struct s_link
{
	t_room	*room1;
	t_room	*room2;
	struct s_link	*next;
}				t_link;

typedef struct s_lem
{
	int		identityNumber;
	t_room	*room;
	struct s_lem	*next;
}				t_lem;

typedef struct s_map
{
	int		ant;
	t_room	*start;
	t_room	*end;
	t_room	*room;
	t_link	*link;
	t_lem	*lem;
}				t_map;

/* ==================== function utils ==================== */

int ft_strlen(const char *str);
char *ft_strdup(const char *str);
char *get_next_line(int fd);
int	ft_strcmp(const char *s1, const char *s2);
int ft_atoi(const char *str);


/* ==================== function error ==================== */

void free_all(t_map* map, ...);
void handle_error(const char *message, void (*cleanup)(t_map *, ...), ...);

/* ==================== function parcing ==================== */

char *read_file();
void parcing(t_map *map);

/* ==================== function map func ==================== */

t_room *get_room(t_map *map, char *ligne);
t_link *get_link(t_map *map, t_room *room1, t_room *room2);
t_lem *get_lem(t_map *map, int identityNumber);
t_room *add_room(t_map *map, char *line);
t_link *add_link(t_map *map, char *line);
t_lem *add_lem(t_map *map, int identityNumber, t_room *room);
void free_link(t_link *link);
void free_room(t_room *room);
void free_lem(t_lem *lem);