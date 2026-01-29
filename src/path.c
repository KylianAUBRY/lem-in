#include "../include/lem-in.h"

int chr_path(t_map *map, t_path *path)
{
	int i = path->size - 1;
	if (path->rooms[i] == map->end)
	{
		t_path *new_path = copy_path(map, path);
		new_path->score = path->size + (map->ant - 1);
		t_path *tmp = map->path;
		t_path *tmp2 = NULL;
		if (!tmp)
		{
			map->path = new_path;
			return 1;
		}
		while (tmp)
		{
			if (new_path->score < tmp->score)
			{
				new_path->next = tmp;
				if (tmp == map->path)
					map->path = new_path;
				else
					tmp2->next = new_path;
				return 1;
			}
			tmp2 = tmp;
			tmp = tmp->next;
			if (!tmp)
			{
				tmp2->next = new_path;
				return 1;
			}
		}
		return 1;
	}
	int j = 0;
	int res = -2;
	while (path->rooms[i]->links[j])
	{
		if (path->rooms[i]->links[j]->visited == 0)
		{
			path->rooms[i]->links[j]->visited = 1;
			if (add_room_to_path(map, path, path->rooms[i]->links[j]) == -1)
			{
				return 0;
			}

			switch (chr_path(map, path))
			{
				case -1 : //cas cus de sac
					if (res < 0)
						res = -1;
					break;
				case 0 : //cas normal
					res = 0;
					break;
				case 1 : //explorer par l'instance actuelle
					res = 0;
					break;
				case 2: //cas ou on a trouve un chemin et qu'on veut arreter
					if (res < 0)
					res = 2;
					break;
					//case a refaire suivant note : 
			}
			//path->rooms[i]->links[j]->visited = 0;
			dell_room_to_path(path);
		}
		j++;
	}
	return 0;
	return (res == -2) ? 0 : res;
}

// quand on arrive a la dernière nœud (celle qui ce nomme fin) on explore pas les chemin qui lentoure (ca ne sert a rien) 


// il y a une value dans chaque noeud, 

// quand elle est a -1:
// c'est que c'est un cus de sac, pour quelle soit cus de sac il faut que toute les neud suivant soit aussi des cus de sac OU que la neud n'est pas de lien sauf celui qui nous a permis d'y parvenir, 

// quand elle est a 0 c'est que le chemin n'est pas explorer par l'instance actuelle de l'algo,

// quand elle est a 1 c'est quelle est explorer par l'instance actuelle

// quand elle est a 2;
// c'est quelle mène a la fin, pour quelle soit a 2 il faut que tout les nœud suivant on ( au moins 1 suivant a 1 et aucun 1 ou 0) OU que un de c'est suivant soit a 1, ATTENTION SI L'ALGO ARRIVE A UN POINT DE DEPART CA SERA JAMAIS 0


void get_path(t_map *map)
{
	map->start->visited = 1;
	for (int i = 0; map->start->links[i]; i++)
		map->start->links[i]->visited = 1;
	map->end->visited = 0;
	for (int i = 0; map->start->links[i]; i++)
	{
		t_path *new_path = malloc(sizeof(t_path));
		if (!new_path)
			handle_error("malloc failed", map, 0);
		new_path->next = NULL;
		new_path->size = 0;
		add_room_to_path(map, new_path, map->start->links[i]);
		chr_path(map, new_path);
		if (new_path)
			free_path(new_path);
	}
}

int nb_path_max(t_map *map)
{
	int score = 0, score2 = 0;

	for (int i = 0; map->start->links[i] && score <= map->ant; i++)
	{
		t_path *path = map->path;
		while (path)
		{
			if (path->rooms[0] == map->start->links[i])
			{
				score ++;
				break ;
			}
			path = path->next;
		}
	}
	for (int i = 0; map->end->links[i] && score2 < score; i++)
	{
		t_path *path = map->path;
		while (path)
		{
			if (path->size <= 1 || path->rooms[path->size - 2] == map->end->links[i])
			{
				score2 ++;
				break ;
			}
			path = path->next;
		}
	}
	return (score < score2) ? score : score2;
}
