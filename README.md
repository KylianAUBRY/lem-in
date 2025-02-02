# Lem-in

## 🏗️ 42 Project - Lem-in
Lem-in is an algorithmic project from 42 school that focuses on finding the optimal path for moving a set number of ants through a network of rooms and tunnels while respecting specific constraints.

## 📜 Description
The goal of this project is to create a program that analyzes a given map input and determines the best way to move the ants from the start to the exit.

### 🔹 Constraints:
- Only one ant can move through a tunnel at a time.
- All ants must reach the exit room as quickly as possible.
- Multiple paths can be used in parallel.

## 🚀 Installation & Compilation
Clone the repository:
```sh
 git clone https://github.com/your-username/lem-in.git
 cd lem-in
```

Compile the program using `make`:
```sh
make
```

## 📝 Usage
Run the program by providing a map as input:
```sh
./lem-in < maps/map.txt
```

## 📂 Project Structure
```
lem-in/
│── src/        # Project source files
│── includes/   # Header files
│── maps/       # Example maps
│── Makefile    # Project compilation
│── README.md   # Documentation
```

## 📊 Algorithm

### Pathfinding Strategy (path.c)
Concerning the algorithm used, everything is custom-built.

1. **Finding All Possible Paths:**  
   I start by identifying all possible paths from the start room to the exit. This ensures we have a complete overview of potential routes. I start by identifying all possible paths from the start room to the exit. This ensures we have a complete overview of potential routes. Once identified, I sort these paths in ascending order based on their scores.

2. **Path Scoring System:**  
   Each path is assigned a score calculated as follows:
   ```
   score = number_of_rooms_in_the_path + (number_of_ants - 1)
   ```
   This formula helps balance between shorter paths and the distribution of ants to minimize congestion.

### Resolution Algorithm (algo.c)
- **Calculating the Maximum Number of Simultaneous Paths:**  
  I begin by determining the maximum number of paths that can be used simultaneously. This is based on:
  - The number of rooms connected directly to the **start** and **end** rooms. For example, if only 2 rooms are connected to the end but 10 are connected to the start, the maximum number of simultaneous paths is 2. There's no point in starting 10 ants at once if only 2 can enter the end room simultaneously.
  - Additionally, if the number of ants is greater than or equal to the smaller of the two counts (rooms connected to start or end), this also limits the effective number of simultaneous paths.

- **Grouping Paths:**  
   I then create "groups of paths" that do not share any rooms in common. Each group is assigned a score:


	$$
	score = \min \left( T \mid \sum_{i=0}^{P-1} \max(0, T - p[i] + 1) \geq X \right)
	$$

	### Explication des termes :
	- \( T_{\text{min}} \) : Le nombre minimal de tours nécessaire pour que toutes les fourmis atteignent la fin.
	- \( P \) : Le nombre total de chemins disponibles.
	- \( p[i] \) : La longueur du chemin \( i \) (en nombre de rooms).
	- \( X \) : Le nombre total de fourmis à déplacer.
	- \( \max(0, T - p[i] + 1) \) : Cela représente le nombre de fourmis qui peuvent arriver à la fin sur le chemin \( i \) en \( T \) tours :
	- \( p[i] \) est le nombre de tours que prend la première fourmi pour arriver à la fin.
	- Chaque fourmi suivante peut arriver après un tour supplémentaire.
	- Si \( T \) est inférieur à \( p[i] \), aucune fourmi ne pourra arriver sur ce chemin.
	- L'objectif est de trouver la plus petite valeur de \( T \) qui satisfait la condition : la somme des fourmis arrivant sur tous les chemins doit être égale ou supérieure à \( X \).

	### Remarque :
	La fonction \(\max(0, T - p[i] + 1)\) est utilisée pour s'assurer qu'on ne compte pas de fourmis avant le moment où elles peuvent atteindre la fin (c'est-à-dire avant que \( T \) soit supérieur ou égal à \( p[i] \)).

   
   This approach helps optimize the movement by avoiding conflicts between ants on different paths.

- **In Progress:**  
  The resolution algorithm is currently under development. The goal is to efficiently assign ants to paths based on their scores, optimizing for the least number of turns required for all ants to reach the exit.






