#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <math.h>
#include "maze.h"
#include <stdbool.h>

int wallsBinary = {0};

// UI state
int uiBoxes[3] = {1, 1, 1}; // Initial state: all boxes are initially on

// Define maze parameters
int** maze;
int rows, cols;
int currentRow = 0;
int currentCol = 0;
int currentDirection;
int startDirection;
const int directionMapping[] = {8, 4, 2, 1};
int startPos[2];
int endPos[2];
int storeShortestPathLength;
int navigateValue;


// Define maze parameters
int fullmaze[4][6] = {
{13, 9, 10, 8, 10, 12},
{1, 2, 12, 5, 11, 0},
{0, 14, 5, 5, 9, 6},
{7, 11, 6, 7, 3, 14}
};

// Define a structure for the stack
typedef struct {
    int row;
    int col;
} Position;


int pathLength = 0;


// Function to initialize a stack
void initialize(Stack *stack) {
    stack->top = -1;  // Initialize top to -1 to indicate an empty stack
}

// Function to check if the stack is empty
int isEmpty(Stack *stack) {
    return stack->top == -1;
}

// Function to check if the stack is full
int isFull(Stack *stack, int rows, int cols) {
    return stack->top == MAX_ROWS - 1 || stack->top == MAX_COLS - 1;
}

// Function to push row and col onto the stack
int push(Stack *stack, int row, int col) {
    if (isFull(stack, 1, 2)) {
        printf("Stack overflow\n");
        return 0;
    }

    stack->top++;
    stack->arr[stack->top][0] = row;
    stack->arr[stack->top][1] = col;

    return 1;
}

// Function to print the contents of the stack
void printStack(Stack *stack) {
    if (isEmpty(stack)) {
        printf("Stack is empty\n");
        return;
    }

    printf("Stack contents:\n");
    for (int i = stack->top; i >= 0; i--) {
        printf("(%d, %d)\n", stack->arr[i][0], stack->arr[i][1]);
    }
}

// Function to pop row and col from the stack
void pop(Stack *stack, int *row, int *col) {
    if (isEmpty(stack)) {
        printf("Stack underflow\n");
    }

    *row = stack->arr[stack->top][0];
    *col = stack->arr[stack->top][1];

    stack->top--;
}

// Function to peek row and col from the stack without popping
void peek(Stack *stack, int *row, int *col) {
    if (isEmpty(stack)) {
        printf("Stack is empty\n");
    }

    *row = stack->arr[stack->top][0];
    *col = stack->arr[stack->top][1];
}

char* intToBinaryString(int num) {
    static char binaryString[5];  // Adjust size based on the number of bits
    for (int i = 3; i >= 0; i--) {
        binaryString[3 - i] = ((num >> i) & 1) ? '1' : '0';
    }
    binaryString[4] = '\0'; // Null-terminate the string
    return binaryString;
}

int isEndingPoint(int row, int col) {
    int walls = maze[row][col];
    const char* binaryWalls = intToBinaryString(walls);

    printf("Binary representation of walls: %s\n", binaryWalls);

    // Starting position cannot be set to end point
    if (row == startPos[0] && col == startPos[1]){ 
        return 0;
    }
    // Check if there is no wall on any side (excluding the start position)
    if (row == 0 && (binaryWalls[0] == '0')) {
        printf("Top side has no wall");
        return 1; // Top side has no wall
    }
    if (row == rows - 1 && (binaryWalls[2] == '0')) {
        printf("Bot side has no wall");
        return 1; // Bottom side has no wall
    }
    if (col == 0 && (binaryWalls[3] == '0')) {
        printf("Left side has no wall");
        printf("binaryWalls of 0,%c\n",binaryWalls[0]);
        printf("binaryWalls of 1,%c\n",binaryWalls[1]);
        printf("binaryWalls of 2,%c\n",binaryWalls[2]);
        printf("binaryWalls of 3,%c\n",binaryWalls[3]);
        return 1; // Left side has no wall
    }
    if (col == cols - 1 && (binaryWalls[1] == '0')) {
        printf("Right side has no wall");
        return 1; // Right side has no wall
    }

    return 0; // Not the ending point
}

int getBackwardDirection(int currentDirection) {
    switch (currentDirection) {
        case 1: return 4;  // Facing left, move to right
        case 2: return 8;  // Facing bottom, move to top
        case 4: return 1;  // Facing right, move to left
        case 8: return 2;  // Facing top, move to bottom
        default: return currentDirection;  // Invalid direction, stay in place
    }
}

void drawUI(SDL_Renderer* renderer) {
    SDL_Rect boxRect;
    boxRect.w = UI_BOX_SIZE;
    boxRect.h = UI_BOX_SIZE;

    // 'W' box at the top
    boxRect.x = SCREEN_WIDTH - 5 * (UI_BOX_SIZE - UI_MARGIN);
    boxRect.y = SCREEN_HEIGHT - 1 * (UI_BOX_SIZE + UI_MARGIN);
    if (uiBoxes[0]) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    }
    SDL_RenderFillRect(renderer, &boxRect);

    // 'A' box at the bottom left
    boxRect.x = SCREEN_WIDTH - 4 * (UI_BOX_SIZE - UI_MARGIN);
    boxRect.y = SCREEN_HEIGHT - 2 * (UI_BOX_SIZE + UI_MARGIN);
    if (uiBoxes[1]) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    }
    SDL_RenderFillRect(renderer, &boxRect);

    // 'D' box at the bottom right
    boxRect.x = SCREEN_WIDTH - 3 * (UI_BOX_SIZE - UI_MARGIN);
    boxRect.y = SCREEN_HEIGHT - 1 * (UI_BOX_SIZE + UI_MARGIN);
    if (uiBoxes[2]) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    }
    SDL_RenderFillRect(renderer, &boxRect);
}

// Function to handle UI events (Toggling boxes)
void handleUIEvents(SDL_Event* e) {
    if (e->type == SDL_KEYDOWN) {
        switch (e->key.keysym.sym) {
            case SDLK_a: // Press 'a' to toggle the left box
                uiBoxes[0] = !uiBoxes[0];
                break;
            case SDLK_w: // Press 'w' to toggle the middle box
                uiBoxes[1] = !uiBoxes[1];
                break;
            case SDLK_d: // Press 'd' to toggle the right box
                uiBoxes[2] = !uiBoxes[2];
                break;
            default:
                break;
        }
    }
}

void nextMove(){
    
}

void exploreMaze(int row, int col) {
    // Explore all possible neighbors (up, right, down, left)
    int dr[] = {-1, 0, 1, 0}; // Changes in row for each direction
    int dc[] = {0, 1, 0, -1}; // Changes in column for each direction

    // Scan the walls around the current cell
    int walls = maze[row][col];
    const char* binaryWalls = intToBinaryString(walls);
    int newRow, newCol;
    int moveForwardRow, moveForwardCol;

    for (int i = 0; i < 4; ++i) {
        // Check if there is a wall in the current direction
        if (binaryWalls[i] == '1') {
            // There is a wall in this direction, so we cannot move in that direction
            int wallRow = currentRow + dr[i];
            int wallCol = currentCol + dc[i];
            printf("There is a wall at (%d, %d)\n", wallRow, wallCol);
            continue;
        }
        moveForwardRow = dr[i];
        moveForwardCol = dc[i];
        newRow = currentRow + dr[i];
        newCol = currentCol + dc[i];
        printf("No wall at(%d,%d)\n", newRow, newCol);

        // Check if the neighbor is a valid move and an unexplored area
        if (isValidMove(newRow, newCol) && maze[newRow][newCol] == -1) {
            printf("Priority Move to unexplored cell (%d, %d)\n", newRow, newCol);
            currentRow = newRow;
            currentCol = newCol;
            // if (i == 0) {
            //     currentDirection = 8;
            // } else if(i == 1) {
            //     currentDirection = 4;
            // } else if(i == 2) {
            //     currentDirection = 2;
            // } else if(i == 3) {
            //     currentDirection = 1;
            // }
            currentDirection = directionMapping[i];

            // Try to push the current position onto the stack
            if (push(&posStack, currentRow, currentCol)) {
                printf("Pushed successfully!\n");
            } else {
                printf("Failed to push (stack overflow)\n");
            }
            printStack(&posStack);
            return;  // Exit the function after the first valid move
        }
    }
    printStack(&posStack);
    if (!isEmpty(&posStack)){
        pop(&posStack, &currentRow, &currentCol);
    }
    if (!isEmpty(&posStack)){
        int topRow, topCol;
        peek(&posStack, &topRow, &topCol);
            printf("Top values: (%d, %d)\n", topRow, topCol);
            int rowDiff = topRow - currentRow;
            int colDiff = topCol - currentCol;
            currentRow = topRow;
            currentCol = topCol;
            // Determine the new direction based on the move

            if (rowDiff == -1) {
                currentDirection = 8;
            } else if (rowDiff == 1) {
                currentDirection = 2;
            } else if (colDiff == -1) {
                currentDirection = 1;
            } else if (colDiff == 1) {
                currentDirection = 4;
            }
    }
    else {
        printf("Maze Completed Stack no more");
    }
    // If no unexplored areas found, move to the first valid neighbor
    // for (int i = 0; i < 4; ++i) {
    //     newRow = currentRow + dr[i];
    //     newCol = currentCol + dc[i];

    //     // Check if the neighbor is a valid move
    //     if (isValidMove(newRow, newCol)) {
    //         printf("Move to cell (%d, %d)\n", newRow + 1, newCol + 1);
    //         currentRow = newRow;
    //         currentCol = newCol;
    //         currentDirection = i;
    //         return;  // Exit the function after the first valid move
    //     }
    // }
    // // Check if the neighbor is a valid move honestly might not need this
    // if (isValidMove(newRow, newCol)) {
    //     printf("Move to cell (%d, %d)\n", newRow +1, newCol+1);
    //     currentRow = newRow;
    //     currentCol = newCol;
    //     return;  // Exit the function after the first valid move
    // }

    // If all directions are blocked, consider backtracking
    // if (stackSize > 0) {
    //     printf
    //     // Pop the last position from the stack
    //     stackSize--;
    //     int prevRow = stack[stackSize].row;
    //     int prevCol = stack[stackSize].col;

    //     // Determine the relative movement direction
    //     if (prevRow < currentRow) {
    //         // Move up
    //         currentDirection = 8;
    //     } else if (prevRow > currentRow) {
    //         // Move down
    //         currentDirection = 2;
    //     } else if (prevCol < currentCol) {
    //         // Move left
    //         currentDirection = 1;
    //     } else if (prevCol > currentCol) {
    //         // Move right
    //         currentDirection = 4;
    //     }

    //     currentRow = prevRow;
    //     currentCol = prevCol;
    //     printf("Backtrack to cell (%d, %d)\n", currentRow + 1, currentCol + 1);
    // } else {
    //     printf("No more positions to backtrack. Maze exploration complete.\n");
    //     // Handle completion logic or exit the function
    // }

    // If all directions are blocked, consider moving backward (180-degree turn)
    // printf("All directions blocked, should move back\n");
    // int backwardDirection = getBackwardDirection(currentDirection);
    // printf("MoveForwardRow,%d\n",moveForwardRow);
    // printf("MoveForwardCol,%d\n",moveForwardCol);
    // newRow = currentRow + moveForwardRow;
    // newCol = currentCol + moveForwardCol;

    // // Check if moving backward is a valid move
    // if (isValidMove(newRow, newCol)) {
    //     printf("Move backward to cell (%d, %d)\n", newRow+1, newCol+1);
    //     currentRow = newRow;
    //     currentCol = newCol;
    //     currentDirection = getBackwardDirection(currentDirection);
    //     // Change current direction to the opposite
    //     //currentDirection = (currentDirection + 2) % 4;
    // }
}

// Function to find the next unexplored cell
int findNextUnexploredCell(int *row, int *col) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (maze[i][j] == -1) {
                printf("Found unexplored cell");
                return 1; // Found an unexplored cell
            }
        }
    }
    printf("No more unexplored cells");
    // Print the entire maze array
    printf("Maze array:\n");
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            printf("%d ", maze[i][j]);
        }
        printf("\n");
    }

    return 0; // No unexplored cell found
}

//Function to rotate the wall flags in the circular buffer
int rotateWallFlags(int direction, int walls) {
    int temp = walls;

    // Rotate the wall flags based on the car's direction
    switch (direction) {
        case EAST:
            walls = ((temp << 3) | (temp >> 1)) & 0xF;
            break;
        case SOUTH:
            walls = ((temp << 2) | (temp >> 2)) & 0xF;
            break;
        case WEST:
            walls = ((temp << 1) | (temp >> 3)) & 0xF;
            break;
        // For NORTH, no rotation is needed
    }
    return walls;
}

// Function to scan wall into array
void scanWalls(int direction){

    int walls = 0;

    // Check left box
    if (uiBoxes[0]) {
        walls |= LEFT_WALL;
    }

    // Check front box
    if (uiBoxes[1]) {
        walls |= FRONT_WALL;
    }

    // Check right box
    if (uiBoxes[2]) {
        walls |= RIGHT_WALL;
    }

    // Rotate wall flags based on car's direction
    int rotatedWalls = rotateWallFlags(direction, walls);

    printf("Wall number: %d\n", walls);
    printf("Rotated wall number: %d\n", rotatedWalls);
    maze[currentRow][currentCol] = rotatedWalls;

    // After scanning walls, check if it is the ending location
    if (isEndingPoint(currentRow, currentCol)) {
        endPos[0] = currentRow;
        endPos[1] = currentCol;
        printf("Maze completed! Ending point found at box(%d, %d)\n", currentRow + 1, currentCol + 1);
        printf("Continue mapping phase...\n");
        // Perform any other actions you want when the maze is completed
        //exit(0); // Exit the program or add your completion logic
    }
}

// Function to initialize the maze (Change size of maze col x rows)
void initializeMaze() {

    printf("Enter number of rows: ");
    scanf("%d", &rows);

    printf("Enter number of columns: ");
    scanf("%d", &cols);

    printf("Enter starting row: ");
    scanf("%d", &currentRow);
    startPos[0] = currentRow;

    printf("Enter starting column: ");
    scanf("%d", &currentCol);
    startPos[1] = currentCol;

    printf("Enter starting direction (8 = up, 4 = right, 2 = down, 1 = left): ");
    scanf("%d", &currentDirection);
    startDirection = currentDirection;

    // printf("Enter direction car faces(8 for up, 4 for right, 2 for down and 1 for left): ");
    // scanf("%d", &currentDirection);

    // Allocate memory for the maze
    maze = (int**)malloc(rows * sizeof(int*));
    for (int i = 0; i < rows; ++i) {
        maze[i] = (int*)malloc(cols * sizeof(int));
    }

    // Initialize the maze with default values
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            maze[i][j] = -1;
        }
    }

    if (push(&posStack, currentRow, currentCol)) {
        printf("Start of maze pushed successfully!\n");
        } else {
            printf("Failed to push (stack overflow)\n");
        }
    printStack(&posStack);
}

int isValidMove(int nextRow, int nextCol) {
    return nextRow >= 0 && nextRow < rows && nextCol >= 0 && nextCol < cols;
}

void drawPlayerPosition(SDL_Renderer* renderer, int direction) {
    int offsetX = (SCREEN_WIDTH - (cols * CELL_SIZE)) / 2;
    int offsetY = (SCREEN_HEIGHT - (rows * CELL_SIZE)) / 2;
    
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int x = offsetX + j * CELL_SIZE;
            int y = offsetY + i * CELL_SIZE;

            // Highlight the current position with a triangle
            if (i == currentRow && j == currentCol) {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color for highlight

                int x1, y1, x2, y2, x3, y3;

                if (direction == 8) // Facing up
                {
                    x1 = x + CELL_SIZE / 2;  // Tip of the triangle
                    y1 = y + 10;

                    x2 = x + 10;                  // Bottom-left corner
                    y2 = y + CELL_SIZE - 10;
                    
                    x3 = x + CELL_SIZE - 10;      // Bottom-right corner
                    y3 = y + CELL_SIZE - 10;
                }
 
                else if (direction == 4) // Facing right
                {
                    x1 = x + CELL_SIZE - 10; // Tip of the arrow
                    y1 = y + CELL_SIZE / 2;

                    x2 = x + 10;             // Top-left corner
                    y2 = y + 10;

                    x3 = x + 10;             // Bottom-left corner
                    y3 = y + CELL_SIZE - 10;
                }

                else if (direction == 2) // Facing down
                {
                    x1 = x + CELL_SIZE / 2; // Tip of the arrow
                    y1 = y + CELL_SIZE - 10;

                    x2 = x + 10;             // Top-left corner
                    y2 = y + 10;

                    x3 = x + CELL_SIZE - 10; // Top-right corner
                    y3 = y + 10;
                }

                else if (direction == 1) // Facing left
                {
                    x1 = x + 10;             // Tip of the arrow
                    y1 = y + CELL_SIZE / 2;

                    x2 = x + CELL_SIZE - 10; // Top-right corner
                    y2 = y + 10;

                    x3 = x + CELL_SIZE - 10; // Bottom-right corner
                    y3 = y + CELL_SIZE - 10;
                }
                // Draw the triangle
                SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
                //SDL_RenderDrawLine(renderer, x2, y2, x3, y3);
                SDL_RenderDrawLine(renderer, x3, y3, x1, y1);
            }
        }
    }
}

void navigate(int value) {
    int previousRow = currentRow;
    int previousCol = currentCol;

    currentRow = shortestpath.point[value].row;
    currentCol = shortestpath.point[value].col;

    int rowDiff = previousRow - currentRow;
    int colDiff = previousCol - currentCol;

    if (rowDiff == -1) {
        currentDirection = 2;
    } else if (rowDiff == 1) {
        currentDirection = 8;
    } else if (colDiff == -1) {
        currentDirection = 4;
    } else if (colDiff == 1) {
        currentDirection = 1;
    }

    navigateValue--;
}
//Not called right now
void updateMaze(SDL_Event *e) {
    if (e->type == SDL_KEYDOWN) {
        switch (e->key.keysym.sym) {
            // case SDLK_UP:
            //     if (currentRow > 0) {
            //         currentRow--;
            //     }
            //     break;
            // case SDLK_DOWN:
            //     if (currentRow < 3) {
            //         currentRow++;
            //     }
            //     break;
            // case SDLK_LEFT:
            //     if (currentCol > 0) {
            //         currentCol--;
            //     }
            //     break;
            // case SDLK_RIGHT:
            //     if (currentCol < 5) {
            //         currentCol++;
            //     }
            //     break;
            case SDLK_SPACE:
                // Toggle wall configuration for the current cell
                maze[currentRow][currentCol] = (maze[currentRow][currentCol] + 1) % 16;
                break;
            default:
                break;
        }
    }
}

void drawBox(SDL_Renderer* renderer, int x, int y) {
    // Calculate the center of the cell
    int centerX = x + CELL_SIZE / 2 - SMALL_BOX_SIZE / 2;
    int centerY = y + CELL_SIZE / 2 - SMALL_BOX_SIZE / 2;

    // Set the color for the box
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green color for the fill

    // Draw a filled rectangle at the center of the cell
    SDL_Rect boxRect = {centerX, centerY, SMALL_BOX_SIZE, SMALL_BOX_SIZE};
    SDL_RenderFillRect(renderer, &boxRect);
}

void drawShortestPath(SDL_Renderer* renderer) {
    // Calculate the position to center the maze
    int offsetX = (SCREEN_WIDTH - (cols * CELL_SIZE)) / 2;
    int offsetY = (SCREEN_HEIGHT - (rows * CELL_SIZE)) / 2;

    // Set the color for the shortest path
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green color for the fill

    // Draw rectangles and connect them with lines in the shortest path
    for (int i = storeShortestPathLength - 1; i > 0; i--) {
        int x1 = offsetX + shortestpath.point[i].col * CELL_SIZE + CELL_SIZE / 2;
        int y1 = offsetY + shortestpath.point[i].row * CELL_SIZE + CELL_SIZE / 2;

        int x2 = offsetX + shortestpath.point[i - 1].col * CELL_SIZE + CELL_SIZE / 2;
        int y2 = offsetY + shortestpath.point[i - 1].row * CELL_SIZE + CELL_SIZE / 2;

        // Draw a small green box in the center of the cell
        int centerX = x1 - SMALL_BOX_SIZE / 2;
        int centerY = y1 - SMALL_BOX_SIZE / 2;

        SDL_Rect smallBoxRect = {centerX, centerY, SMALL_BOX_SIZE, SMALL_BOX_SIZE};
        SDL_RenderFillRect(renderer, &smallBoxRect);

        // Draw a line connecting the current and previous points
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }

    // Draw the last box (no need to draw a line after the last point)
    int x = offsetX + shortestpath.point[0].col * CELL_SIZE + CELL_SIZE / 2;
    int y = offsetY + shortestpath.point[0].row * CELL_SIZE + CELL_SIZE / 2;
    int centerX = x - SMALL_BOX_SIZE / 2;
    int centerY = y - SMALL_BOX_SIZE / 2;
    SDL_Rect smallBoxRect = {centerX, centerY, SMALL_BOX_SIZE, SMALL_BOX_SIZE};
    SDL_RenderFillRect(renderer, &smallBoxRect);
}

// Function to draw walls based on the cell value (When press spacebar)
void drawWalls(SDL_Renderer *renderer, int x, int y, int cell) {
    if (cell == 0) {
        return;  // No walls, nothing to draw
    }

    // Draw left wall
    if (cell == 1 || cell == 3 || cell == 5 || cell == 7 || cell == 9 || cell == 11 || cell == 13) {
        SDL_RenderDrawLine(renderer, x, y, x, y + CELL_SIZE);
    }

    // Draw bottom wall
    if (cell == 2 || cell == 3 || cell == 6 || cell == 7 || cell == 10 || cell == 11 || cell == 14) {
        SDL_RenderDrawLine(renderer, x, y + CELL_SIZE, x + CELL_SIZE, y + CELL_SIZE);
    }

    // Draw right wall
    if (cell == 4 || cell == 5 || cell == 6 || cell == 7 || cell == 12 || cell == 13 || cell == 14) {
        SDL_RenderDrawLine(renderer, x + CELL_SIZE, y, x + CELL_SIZE, y + CELL_SIZE);
    }

    // Draw top wall
    if (cell == 8 || cell == 9 || cell == 10 || cell == 11 || cell == 12 || cell == 13 || cell == 14) {
        SDL_RenderDrawLine(renderer, x, y, x + CELL_SIZE, y);
    }
}

// Function to draw the maze grid (Grey lines)
void drawGrid(SDL_Renderer* renderer) {
    // Calculate the position to center the maze
    int offsetX = (SCREEN_WIDTH - (cols * CELL_SIZE)) / 2;
    int offsetY = (SCREEN_HEIGHT - (rows * CELL_SIZE)) / 2;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int x = offsetX + j * CELL_SIZE;
            int y = offsetY + i * CELL_SIZE;

            // Draw a filled white rectangle for each cell
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color for the fill
            SDL_Rect cellRect = {x, y, CELL_SIZE, CELL_SIZE};
            SDL_RenderFillRect(renderer, &cellRect);

            // Draw grey lines on the top, right, bottom, and left edges
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // Light grey color for the lines

            SDL_RenderDrawLine(renderer, x, y, x + CELL_SIZE, y);                        // Top
            SDL_RenderDrawLine(renderer, x + CELL_SIZE, y, x + CELL_SIZE, y + CELL_SIZE); // Right
            SDL_RenderDrawLine(renderer, x, y + CELL_SIZE, x + CELL_SIZE, y + CELL_SIZE); // Bottom
            SDL_RenderDrawLine(renderer, x, y, x, y + CELL_SIZE);                        // Left

            // Draw a small green box in the center of the cell
            // int centerX = x + CELL_SIZE / 2 - SMALL_BOX_SIZE / 2;
            // int centerY = y + CELL_SIZE / 2 - SMALL_BOX_SIZE / 2;

            // SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green color for the small box
            // SDL_Rect smallBoxRect = {centerX, centerY, SMALL_BOX_SIZE, SMALL_BOX_SIZE};
            // SDL_RenderFillRect(renderer, &smallBoxRect);
            //printf("(Centre coordinate%d, %d\n)", centerX, centerY);
        }
    }
}


Queue* createQueue() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->front = queue->back = NULL;
    return queue;
}

// Function to check if the queue is empty
bool isEmpty(Queue* queue) {
    return queue->front == NULL;
}

// Function to enqueue a grid
void enqueue(Queue* queue, int row, int col) {
    QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
    newNode->point.row = row;
    newNode->point.col = col;
    newNode->next = NULL;

    if (isEmpty(queue)) {
        queue->front = queue->back = newNode;
    } else {
        queue->back->next = newNode;
        queue->back = newNode;
    }
}

// Function to dequeue a grid
Point dequeue(Queue* queue) {
    // if queue is empty
    if (isEmpty(queue)) {
        Point emptyPoint = {-1, -1}; // Placeholder for an invalid point
        return emptyPoint;
    }

    // create a temp queue to add to the front
    QueueNode* temp = queue->front;
    Point point = temp->point;
    //replace it
    queue->front = temp->next;

    // if front is empty, the back will be empty
    if (queue->front == NULL) {
        queue->back = NULL;
    }

    free(temp);
    return point;
}

// Function to perform BFS and print the path
void bfs(int** maze, Point start, Point destination) {
    // Directions for moving up, down, left, and right
    int row[] = {-1, 0, 0, 1};
    int col[] = {0, -1, 1, 0};

    // Initialize visited array
    bool** visited= (bool**)malloc(rows * sizeof(bool*));
    for (int i = 0; i < rows; i++) {
        visited[i] = (bool*)malloc(cols * sizeof(bool));
        for (int j = 0; j < cols; j++) {
            // all the array will be false first, once they have been visited it will change to true
            visited[i][j] = false;
        }
    }

    // Create a queue for BFS
    Queue* queue = createQueue();

    // Enqueue the starting point
    enqueue(queue, start.row, start.col);
    //change the starting position to visited
    visited[start.row][start.col] = true;

    // Create a 2D array to store the path
    Point parent[rows][cols];
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            parent[i][j] = (Point){-1, -1}; // Initialize to an invalid point
        }
    }
    // Perform BFS
    while (!isEmpty(queue)) {     
        // dequeue the current grid  
        Point current = dequeue(queue);
        printf("Processing (%d, %d)\n", current.row, current.col);

        // Check if the destination is reached
        if (current.row == destination.row && current.col == destination.col) {
            printf("Destination reached! Path found.\n");
            // Print the path
            printf("Path: ");
            while (parent[current.row][current.col].row != -1) {
                shortestpath.point[shortestpath.length++] = current;
                printf("(%d, %d) <- ", current.row, current.col);
                current = parent[current.row][current.col];
            }
            // add the starting position
            shortestpath.point[shortestpath.length++] = current;
            printf("(%d, %d)\n", start.row, start.col);
            for (int i = shortestpath.length - 1; i >= 0; i--)
            {
                printf("(%d, %d)", shortestpath.point[i].row, shortestpath.point[i].col);
            }
            printf("Shortest path length in maze: %d\n", shortestpath.length);
            // reset the path to prevent it from being overwritten
            storeShortestPathLength = shortestpath.length;
            navigateValue = storeShortestPathLength - 1; // Store in navigate value for navigation
            printf("Shortestsssss path length in maze: %d\n", storeShortestPathLength);
            shortestpath.length = 0;

            return;
        }


        // Explore neighbors
        for (int i = 0; i < 4; i++) {
            int newRow = current.row + row[i];
            int newCol = current.col + col[i];

            // 0 up
            // 1 left
            // 2 right
            // 3 down

        if (newRow >= 0 && newRow < rows && newCol >= 0 && newCol < cols && !visited[newRow][newCol]) {
                if(maze[current.row][current.col] == 1) {
                    if(i == 1) //  cannot move left
                    {
                        continue;
                    }
                } else if(maze[current.row][current.col] == 2) {
                    if(i == 3) // cannot move down
                    {
                        continue;
                    }
                }else if(maze[current.row][current.col] == 3) {
                    if(i == 1 || i == 3) // cannot move left and down
                    {
                        continue;
                    }
                } else if(maze[current.row][current.col] == 4) {
                    if(i == 2) // cannot move right
                    {
                        continue;
                    }
                } else if(maze[current.row][current.col] == 5) {
                    if(i == 1 || i == 2) // cannot move left and right
                    {
                        continue;
                    }
                } else if(maze[current.row][current.col] == 6) {
                    if(i == 2 || i == 3) // cannot move down and right
                    {
                        continue;
                    }
                } else if(maze[current.row][current.col] == 7) {

                    if (i == 1 || i == 2 || i == 3) // cannot move left, right and down
                    {
                        continue;
                    }
                } else if(maze[current.row][current.col] == 8) {
                    if(i == 0) // cannot move up
                    {
                        continue;
                    }
                } else if(maze[current.row][current.col] == 9) {
                    if(i == 0 || i == 1) // cannot move up and left
                    {
                        continue;
                    }
                } else if(maze[current.row][current.col] == 10) {
                    if(i == 0 ||i == 3) // cannot move up and down
                    {
                        continue;
                    }
                } else if(maze[current.row][current.col] == 11) {
                    if(i == 3 || i == 0 || i == 1) // only can move right
                    {
                        continue;
                    }
                } else if(maze[current.row][current.col] == 12) {
                    if(i == 0 || i == 2) // cannot move up and right
                    {
                        continue;
                    }
                } else if(maze[current.row][current.col] == 13) {
                    if(i == 2 || i == 0 || i == 1) // only can move down
                    {
                        continue;
                    }
                } else if(maze[current.row][current.col] == 14) {

                    if (i == 0 || i == 2|| i == 3) // only can move left
                    {
                        continue;
                    }
                }
                // Enqueue the valid neighbor
                enqueue(queue, newRow, newCol);
                //change the grid to visited
                visited[newRow][newCol] = true;
                // Store the parent for backtracking the path
                parent[newRow][newCol] = current;
            }
        }
    }
    // If the queue is empty and destination is not reached
    printf("No path found to the destination.\n");
}