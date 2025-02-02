#pragma once

/* ==================== include ==================== */

# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <errno.h>
# include <stdarg.h>


/* ==================== structure ==================== */

typedef struct s_multi_path
{
	struct s_path	**paths;
	int size;
	int score;

	struct s_multi_path	*next;
}				t_multi_path;


typedef struct s_path
{
	struct s_room	**rooms;
	int size;
	int score;

	struct s_path	*next;
}				t_path;


typedef struct s_room
{
	char	*name;
	int		x;
	int		y;
	struct s_room	*next;

	int visited;
	struct s_room	**links;
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
	t_path	*path;
	t_multi_path	*multi_path;
}				t_map;

/* ==================== function utils ==================== */

int ft_strlen(const char *str);
char *ft_strdup(const char *str);
char *ft_strdup_char(const char *str, char c);
char *get_next_line(int fd);
int	ft_strcmp(const char *s1, const char *s2);
int ft_atoi(const char *str);


/* ==================== function error ==================== */

void free_all(t_map* map, ...);
void handle_error(const char *message, void (*cleanup)(t_map *, ...), ...);

/* ==================== function parcing ==================== */

void parcing(t_map *map);

/* ==================== function path ==================== */

void chr_path(t_map *map, t_path *path);
void get_path(t_map *map);
int nb_path_max(t_map *map);

/* ==================== function algo ==================== */

int no_shared_room(t_multi_path *multi_path, t_path *path);
void chr_multi_path(t_map *map, t_multi_path *multi_path, int max_path, t_path *path);
void get_multi_path(t_map *map, int max_path);

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
void free_path(t_path *path);
void free_multi_path(t_multi_path *multi_path);
t_path *copy_path(t_map *map, t_path *path);
void reset_visited(t_map *map);
void add_room_to_path(t_map *map, t_path *path, t_room *room);
void dell_room_to_path(t_path *path);
t_multi_path *copy_multi_path(t_map *map, t_multi_path *multi_path);