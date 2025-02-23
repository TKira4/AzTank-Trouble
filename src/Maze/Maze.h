#ifndef MAZE_H
#define MAZE_H

#include <SDL2/SDL.h>
#include <vector>

//forward declare
class Wall;

/// Lớp Cell: đại diện cho mỗi ô của mê cung.
class Cell {
public:
    int row, col;
    bool isInMaze;
    bool top, bottom, left, right;  
    Wall* walls[4];                // 0: top, 1: bottom, 2: left, 3: right

    Cell(int r = 0, int c = 0);
    void setInMaze();
};

/// Lớp Wall: đại diện cho một bức tường giữa 2 ô.
class Wall {
public:
    bool isEdge;   
    bool isOpen;   
    Cell* c1;
    Cell* c2;

    Wall();
    void setIsEdge();
    void toOpenWall();
};

//Lớp Maze: quản lý việc sinh mê cung, collision và render.
class Maze {
public:
    int rows, cols, cellSize;
    std::vector<std::vector<Cell>> grid;    
    std::vector<Wall*> wallList;            // danh sách các đối tượng Wall (để dễ dọn dẹp)
    std::vector<SDL_Rect> wallRects;        // danh sách các SDL_Rect của tường (dùng cho collision & render)

    Maze(int r, int c, int cs);
    ~Maze();

    //tao me cung
    void generateWalls();
    void generateMaze();
    void openMaze();
    void buildWallsFromGrid();

    //kiem tra va cham
    bool collides(const SDL_Rect &rect);

    //ve me cung
    void render(SDL_Renderer* renderer);

    //utils method cho AI
    int getRows() const { return rows; }
    int getCols() const { return cols; }
    int getCellSize() const { return cellSize; }
    
    bool canMove(int cx, int cy, int nx, int ny) const;

    friend class AITank;
};

#endif // MAZE_H
