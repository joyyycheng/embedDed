#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>
#include "maze.h"

Stack posStack;
ShortestPath shortestpath;
int navigationPhase = 0;

int main(int argc, char** argv) {

    initialize(&posStack);  // Initialize the stack
    initializeMaze();

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL initialization failed. SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    // Create window
    window = SDL_CreateWindow("Maze UI", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window creation failed. SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer creation failed. SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    // Main loop
    int quit = 0;
    int drawMaze = 0; // Flag to track whether to draw the maze
    while (!quit) {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_SPACE && !drawMaze) {
                    drawMaze = 1; // Set the flag when the space bar is pressed (and not already drawing)
                    drawBox(renderer, 480, 260);
                } else if (e.key.keysym.sym == SDLK_ESCAPE) {
                    // Stop drawing the maze if the Esc key is pressed
                    drawMaze = 0;
                } else if (e.key.keysym.sym == SDLK_UP) {
                    // Stop drawing the maze if the Esc key is pressed
                    currentDirection = 8;
                }
                else if (e.key.keysym.sym == SDLK_RIGHT) {
                    // Stop drawing the maze if the Esc key is pressed
                    currentDirection = 4;
                }
                else if (e.key.keysym.sym == SDLK_DOWN) {
                    // Stop drawing the maze if the Esc key is pressed
                    currentDirection = 2;
                }
                else if (e.key.keysym.sym == SDLK_LEFT) {
                    // Stop drawing the maze if the Esc key is pressed
                    currentDirection = 1;
                }
                handleUIEvents(&e);
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    printf("Left mouse button clicked\n");
                    scanWalls(currentDirection);
                    // Handle left click
                } else if (e.button.button == SDL_BUTTON_MIDDLE) {
                    if (navigateValue < 0)
                    {
                        navigateValue = storeShortestPathLength - 1; //Restart navigation
                        navigate(navigateValue);
                        currentDirection = startDirection;
                    }
                    else{
                        navigate(navigateValue);
                    }
                } else if (e.button.button == SDL_BUTTON_RIGHT) {
                    // Find the next unexplored cell
                    if (findNextUnexploredCell(&rows, &cols)) {
                        // Explore the maze from the next unexplored cell
                        exploreMaze(currentRow,currentCol);
                        //exploreMaze(currentRow, currentCol);
                        // Print the maze after exploration
                        printf("Maze after exploration:\n");
                    // Handle right click
                    }

                    else // When mapping phase is over, right click triggers navigation phase
                    {   
                        navigationPhase = 1;
                        currentDirection = startDirection;
                        printf("Car goes back to the start\n");

                        Point endOfExplore = {currentRow, currentCol};
                        printf("%d, %d\n", currentRow, currentCol);
                        Point start = {startPos[0], startPos[1]};

                        bfs(maze, start, endOfExplore);
                        for (int i = storeShortestPathLength - 1; i >= 0; i--)
                        {
                            printf("(Map back to start %d, %d)", shortestpath.point[i].row, shortestpath.point[i].col);
                        }

                        Point destination = {endPos[0], endPos[1]};

                        currentRow = startPos[0];
                        currentCol = startPos[1];
                        
                        printf("(Start location and destination %d, %d)", start, destination);
                        bfs(maze, start, destination);
                        for (int i = storeShortestPathLength - 1; i >= 0; i--)
                        {
                            printf("(Map fastest path %d, %d)", shortestpath.point[i].row, shortestpath.point[i].col);
                        }
                        // for (int i = shortestpath.length - 1; i >= 0; i--) {
                        //     printf("does it enter here?");
                        //     drawBox(renderer,shortestpath.point[i].row,shortestpath.point[i].col);
                        // }
                        
                        
                    }
                }
            }
        }
        

        // Set background color to white
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        // Clear the screen
        SDL_RenderClear(renderer);

        // Draw the maze grid, light grey
        drawGrid(renderer);

        // Draw the current player position, red
        drawPlayerPosition(renderer, currentDirection);

        // Draw the UI
        drawUI(renderer);

        if (navigationPhase) {
            drawShortestPath(renderer);
        }
        // Set line color to black
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        // Calculate the position to center the maze
        int offsetX = (SCREEN_WIDTH - (cols * CELL_SIZE)) / 2;
        int offsetY = (SCREEN_HEIGHT - (rows * CELL_SIZE)) / 2;

        // Draw the maze
        // for (int i = 0; i < rows; ++i) {
        //     for (int j = 0; j < cols; ++j) {
        //         int x = offsetX + j * CELL_SIZE;
        //         int y = offsetY + i * CELL_SIZE;

        //         // Debug print to check the position
        //         //printf("Drawing at (%d, %d)\n", x, y);

        //         // Draw walls based on the cell value
        //         drawWalls(renderer, x, y, maze[i][j]);
        //     }
        // }

        if (drawMaze) {
            // Draw the maze as long as the space bar is held down
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

            int offsetX = (SCREEN_WIDTH - (cols * CELL_SIZE)) / 2;
            int offsetY = (SCREEN_HEIGHT - (rows * CELL_SIZE)) / 2;
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {

                    int x = offsetX + j * CELL_SIZE;
                    int y = offsetY + i * CELL_SIZE;

                    // Debug print to check the position
                    //printf("Drawing at (%d, %d)\n", x, y);

                    // Draw walls based on the cell value
                    drawWalls(renderer, x, y, maze[i][j]);
                }
            }
        }
        // After drawing, update the screen
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();





    return 0;
}