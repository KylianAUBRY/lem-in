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

### Pathfinding Strategy
Concerning the algorithm used, everything is custom-built.

1. **Finding All Possible Paths:**  
   I start by identifying all possible paths from the start room to the exit. This ensures we have a complete overview of potential routes. (path.c)

2. **Path Scoring System:**  
   Each path is assigned a score calculated as follows:
   ```
   score = number_of_rooms_in_the_path + (number_of_ants - 1)
   ```
   This formula helps balance between shorter paths and the distribution of ants to minimize congestion.

### Resolution Algorithm
- **In Progress:**  
  The resolution algorithm is currently under development. The goal is to efficiently assign ants to paths based on their scores, optimizing for the least number of turns required for all ants to reach the exit.
