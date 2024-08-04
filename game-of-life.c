#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#define COMMAND_LENGTH 20

#define RESET		0
#define BRIGHT 		1
#define DIM		    2
#define UNDERLINE 	3
#define BLINK		4
#define REVERSE		7
#define HIDDEN		8

#define YELLOW		3
#define	WHITE		7
#define BLACK       8

typedef struct CELL{
	int alive;
	int death_marked;
	int life_marked;
} cell;


void textcolor(int attr, int fg, int bg)
{	char command[100];

	/* Command is the control command to the terminal */
	sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
	printf("%s", command);
}


cell** initGridEmpty(int n){
	cell** grid;
	grid = (cell**)calloc(n + 2, sizeof(cell*));

	for(int i = 0; i < n + 2; i++){
		grid[i] = (cell*)calloc(n + 2, sizeof(cell));
	}

	return grid;
}

void printGrid(cell** grid, int n){
	for(int i = 1; i < n + 1; i++){
		for(int j = 1; j < n + 1; j++){
			if(j == n){
				if(grid[i][j].alive == 1){
					textcolor(BRIGHT, YELLOW, BLACK);
					printf("%d\n", grid[i][j].alive);
					textcolor(RESET, WHITE, BLACK);
				}
				else
					printf("%d\n", grid[i][j].alive);
			}
			else{
				if(grid[i][j].alive == 1){
					textcolor(BRIGHT, YELLOW, BLACK);		
					printf("%d ", grid[i][j].alive);
					textcolor(RESET, WHITE, BLACK);
				}
				else
					printf("%d ", grid[i][j].alive);
			}
		}
	}
}

void printGridMarkedDeath(cell** grid, int n){
	for(int i = 1; i < n + 1; i++){
        for(int j = 1; j < n + 1; j++){
            if(j == n)
                printf("%d\n", grid[i][j].death_marked);
            else
                printf("%d ", grid[i][j].death_marked);
        }
    }
}

void printGridMarkedLife(cell** grid, int n){
    for(int i = 1; i < n + 1; i++){
        for(int j = 1; j < n + 1; j++){
            if(j == n)
                printf("%d\n", grid[i][j].life_marked);
            else
                printf("%d ", grid[i][j].life_marked);
        }
    }
}

int scan_number(){
	int n;
	char number_input[COMMAND_LENGTH];

	fgets(number_input, sizeof(number_input), stdin);
    number_input[strcspn(number_input, "\n")] = '\0';
    sscanf(number_input, "%d", &n);

	return n;
}

void check_neighbours_cell(int posX, int posY, cell** grid){
	int i, j;
	int count_alive = 0;
	for(i = posY - 1; i <= posY + 1; i++){
		for(j = posX - 1; j <= posX + 1; j++){
			if(i == posY && j == posX) continue;
			else if(grid[i][j].alive == 1) count_alive++;
		}
	}

	if (grid[posY][posX].alive == 1 && count_alive < 2)
		grid[posY][posX].death_marked = 1;
	else if (grid[posY][posX].alive == 1 && (count_alive == 2 || count_alive == 3))
		grid[posY][posX].life_marked = 1;
	else if (grid[posY][posX].alive == 1 && count_alive > 3)
		grid[posY][posX].death_marked = 1;
	else if (grid[posY][posX].alive == 0 && count_alive == 3)
		grid[posY][posX].life_marked = 1;

	//printf("posX = %d, posY = %d, count_alive = %d\n", posX, posY, count_alive);
}

void check_neighbours_grid(cell** grid, int n){
	int x, y;
	for(y = 1; y < n + 1; y++){
		for(x = 1; x < n + 1; x++){
			check_neighbours_cell(x, y, grid);
		}
	}
}

void update_cell(int posX, int posY, cell** grid){
	if(grid[posY][posX].death_marked == 1){ 
		grid[posY][posX].alive = 0;
		grid[posY][posX].death_marked = 0;
	}
	else if(grid[posY][posX].life_marked == 1){
		grid[posY][posX].alive = 1;
		grid[posY][posX].life_marked = 0;
	}
}

void update_grid(cell** grid, int n){
	int x, y;
	for(y = 1; y < n + 1; y++){
 	   for(x = 1; x < n + 1; x++){
	       update_cell(x, y, grid);
		}
	}
}

int main(){
	printf("Welcome to the Game of Life! The grid is padded with dead cells, not a wrap-around!\n\n");
	printf("Input size of grid side-length: ");
	
	int n = scan_number();

	cell** grid = initGridEmpty(n);

	printGrid(grid, n);

	printf("Fill in spaces, in the (x,y) coordinates format (ex. 1 1 to fill out first column, first row):\n");

	int posX, posY, result;
	char remainder[10];
	char commandString[COMMAND_LENGTH];

	while(1){
		fgets(commandString, sizeof(commandString), stdin);
		commandString[strcspn(commandString, "\n")] = '\0';
		result = sscanf(commandString, "%d %d %9s", &posX, &posY, remainder);

		if(strcmp(commandString, "!") == 0){
			printGrid(grid, n);
			break;
		}

		else if(posX < 0 || posX > n || posY < 0 || posY > n){
			printf("Bad input! Retry:\n");
			continue;
		}

		else if(commandString != NULL && result == 2){
				if ((posX == 0 || posX == n + 1) || (posY == 0 || posY == n + 1)) continue;
				else if (grid[posY][posX].alive == 1) grid[posY][posX].alive = 0;
				else if (grid[posY][posX].alive == 0) grid[posY][posX].alive = 1;
				printGrid(grid, n);
		}

		else{
			printf("Bad input! Retry:\n");
			continue;
		}
	}
	
	int counter = 0;

	while(1){
		printf("Phase %d\n", ++counter);
		check_neighbours_grid(grid, n);

		//printf("Check phase %d\n", counter);
		//printGrid(grid, n);

		//printf("Check death %d\n", counter);
		//printGridMarkedDeath(grid, n);

		//printf("Check life %d\n", counter);
		//printGridMarkedLife(grid, n);

		//printf("Update phase %d\n", counter);
		update_grid(grid, n);
		printGrid(grid, n);

		sleep(1);
	}
}
