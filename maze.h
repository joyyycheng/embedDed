#include <SDL2/SDL.h>
#define MAZE_H
#ifdef MAZE_H

    #define SCREEN_WIDTH 960
    #define SCREEN_HEIGHT 640
    #define CELL_SIZE 40
    #define SMALL_BOX_SIZE 5
    #define PLAYER_RADIUS 10

    // Define directions
    #define NORTH 8
    #define EAST 4
    #define SOUTH 2
    #define WEST 1

    // Wall flags
    #define LEFT_WALL 1
    #define FRONT_WALL 8
    #define RIGHT_WALL 4

    // Wall flags
    #define TOP 8
    #define RIGHT 4
    #define BOTTOM 2
    #define LEFT 1

    #define UI_BOX_SIZE 30
    #define UI_MARGIN 10

    #define MAX_ROWS 100
    #define MAX_COLS 100

    extern int wallsBinary;
    extern int uiBoxes[3];
    extern int** maze;
    extern int rows, cols;
    extern int currentRow;
    extern int currentCol;
    extern int currentDirection;
    extern int startDirection;
    extern int fullmaze[4][6];
    extern int printthis[4][6];
    extern int startPos[2];
    extern int endPos[2];
    extern int pathLength;
    extern int navigationPhase;
    extern int storeShortestPathLength;
    extern int navigateValue;

    typedef struct {
        int arr[MAX_ROWS][MAX_COLS];
        int top;  // Index of the top element
    } Stack;

    extern Stack posStack;
        
    typedef struct {
        int row;
        int col;
    } Point;

    typedef struct QueueNode {
        Point point;
        struct QueueNode* next;
    } QueueNode;

    typedef struct {
        QueueNode* front;
        QueueNode* back;
    } Queue;

    typedef struct
    {
        Point point[20];
        int length;
    } ShortestPath;
            
    extern ShortestPath shortestpath;
    
    void initialize(Stack *stack);
    int isEmpty(Stack *stack);
    int isFull(Stack *stack, int rows, int cols);
    int push(Stack *stack, int row, int col);
    void printStack(Stack *stack);
    void pop(Stack *stack, int *row, int *col);
    void peek(Stack *stack, int *row, int *col);

    int isEndingPoint(int row, int col);
    void drawUI(SDL_Renderer* renderer);
    void handleUIEvents(SDL_Event* e);
    int rotateWallFlags(int direction, int walls);
    void handleUIEvents(SDL_Event* e);
    int rotateWallFlags(int direction, int walls);
    void scanWalls(int direction);
    void initializeMaze();
    void drawPlayerPosition(SDL_Renderer* renderer, int direction);
    void updateMaze(SDL_Event *e);
    void drawWalls(SDL_Renderer *renderer, int x, int y, int cell);
    void drawGrid(SDL_Renderer* renderer);
    void nextMove();
    void exploreMaze(int row, int col);
    int findNextUnexploredCell(int *row, int *col);
    int isValidMove(int nextRow, int nextCol);
    char* intToBinaryString(int num);
    int getBackwardDirection(int currentDirection);
    void bfs(int** maze, Point start, Point destination);
    void drawShortestPath(SDL_Renderer* renderer);
    void drawBox(SDL_Renderer* renderer, int x, int y);
    void navigate(int value);
    
    
#endif