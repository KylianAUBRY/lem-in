# include "../include/lem-in.h"

int is_sufficient(int score, t_multi_path *multi_path, int nb_ants)
{
	int ants = 0;
	for (int i = 0; multi_path->paths[i]; i++)
	{
		int path_on_path = score - multi_path->paths[i]->size + 1;
		if (path_on_path > 0)
			ants += path_on_path;
	}
	return ants >= nb_ants;
	
}

int calculate_score(t_multi_path *multi_path, int nb_ants)
{
	int score = 1;
	while (!is_sufficient(score, multi_path, nb_ants))
	{
		score++;
	}
	return score;
}

int no_shared_room(t_multi_path *multi_path, t_path *path)
{
	for(int i = 0; multi_path->paths[i]; i++)
	{
		int j = 0;
		while (multi_path->paths[i]->rooms[j + 1])
		{
			int k = 0;
			while (path->rooms[k + 1])
			{
				if (multi_path->paths[i]->rooms[j] == path->rooms[k])
					return 0;
				k++;
			}
			j++;
		}
	}
	return 1;
}

void chr_multi_path(t_map *map, t_multi_path *multi_path, int max_path, t_path *path)
{
	path = path->next;
	if (multi_path->size >= max_path)
		return ;
	while (path)
	{
		if (no_shared_room(multi_path, path) == 1)
		{
			multi_path->size++;
			t_multi_path *new_multi_path = copy_multi_path(map, multi_path);
			multi_path->size--;
			new_multi_path->paths[multi_path->size] = path;
			new_multi_path->paths[new_multi_path->size] = NULL;
			new_multi_path->score = calculate_score(new_multi_path, map->ant);
			if (new_multi_path->score >= multi_path->score)
			{
				free_multi_path(new_multi_path);
				path = path->next;
				continue;
			}
			if (map->multi_path == NULL)
				map->multi_path = new_multi_path;
			else
			{
				t_multi_path *tmp = map->multi_path;
				t_multi_path *tmp2 = NULL;
				if(!tmp)
				{
					map->multi_path = new_multi_path;
				}
				while (tmp)
				{
					if(tmp->score > new_multi_path->score)
					{
						new_multi_path->next = tmp;
						if (tmp == map->multi_path)
							map->multi_path = new_multi_path;
						else
							tmp2->next = new_multi_path;
						break ;
					}
					tmp2 = tmp;
					tmp = tmp2->next;
					if (!tmp)
					{
						tmp2->next = new_multi_path;
						break ;
					}
				}
			}
			chr_multi_path(map, new_multi_path, max_path, path);
		}
		path = path->next;
	}
}

void get_multi_path(t_map *map, int max_path)
{
	t_path *path = map->path;

	while (path)
	{
		t_multi_path *multi_path = malloc(sizeof(t_multi_path));
		if (!multi_path)
			handle_error("malloc failed", free_all, map, NULL);
		multi_path->size = 1;
		multi_path->score = path->score;
		multi_path->paths = (t_path **)malloc(sizeof(t_path *) * (2));
		if (!multi_path->paths)
			handle_error("malloc failed", free_all, map, multi_path, NULL);
		multi_path->paths[0] = path;
		multi_path->paths[1] = NULL;
		chr_multi_path(map, multi_path, max_path, path);
		free_multi_path(multi_path);
		path = path->next;
	}
}