#include "Maze.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iostream>

// Sử dụng không gian tên std cho vector, v.v.
using namespace std;

static const int WALL_THICKNESS = 4;

//////////////////////
// Cell implementation
//////////////////////
Cell::Cell(int r, int c)
    : row(r), col(c), isInMaze(false), top(true), bottom(true), left(true), right(true)
{
    for (int i = 0; i < 4; i++) {
        walls[i] = nullptr;
    }
}

void Cell::setInMaze() { isInMaze = true; }

//////////////////////
// Wall implementation
//////////////////////
Wall::Wall() : isEdge(false), isOpen(false), c1(nullptr), c2(nullptr) { }

void Wall::setIsEdge() { isEdge = true; }

void Wall::toOpenWall() { isOpen = true; }

//////////////////////
// Maze implementation
//////////////////////
Maze::Maze(int r, int c, int cs)
    : rows(r), cols(c), cellSize(cs)
{
    srand((unsigned)time(nullptr));

    grid.resize(rows, vector<Cell>(cols, Cell(0,0)));
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            grid[i][j] = Cell(i, j);
        }
    }
    generateWalls();
    generateMaze();
    openMaze();
    buildWallsFromGrid();
}

Maze::~Maze() {
    vector<Wall*> deleted;
    for (Wall* w : wallList) {
        if (find(deleted.begin(), deleted.end(), w) == deleted.end()){
            deleted.push_back(w);
            delete w;
        }
    }
}

void Maze::generateWalls() {
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            //Top wall
            if (i == 0) {
                grid[i][j].walls[0] = new Wall();
                grid[i][j].walls[0]->c1 = &grid[i][j];
                grid[i][j].walls[0]->setIsEdge();
            } else {
                grid[i][j].walls[0] = grid[i-1][j].walls[1];
                grid[i][j].walls[0]->c2 = &grid[i][j];
            }
            //Bottom wall
            grid[i][j].walls[1] = new Wall();
            grid[i][j].walls[1]->c1 = &grid[i][j];
            if (i == rows - 1)
                grid[i][j].walls[1]->setIsEdge();
            //Left wall
            if (j == 0) {
                grid[i][j].walls[2] = new Wall();
                grid[i][j].walls[2]->c1 = &grid[i][j];
                grid[i][j].walls[2]->setIsEdge();
            } else {
                grid[i][j].walls[2] = grid[i][j-1].walls[3];
                grid[i][j].walls[2]->c2 = &grid[i][j];
            }
            //Right wall
            grid[i][j].walls[3] = new Wall();
            grid[i][j].walls[3]->c1 = &grid[i][j];
            if (j == cols - 1)
                grid[i][j].walls[3]->setIsEdge();
        }
    }
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            if (i == 0)
                wallList.push_back(grid[i][j].walls[0]);
            if (j == 0)
                wallList.push_back(grid[i][j].walls[2]);
            wallList.push_back(grid[i][j].walls[1]);
            wallList.push_back(grid[i][j].walls[3]);
        }
    }
}

void Maze::generateMaze() {
    int startRow = rand() % rows;
    int startCol = rand() % cols;
    grid[startRow][startCol].setInMaze();

    vector<Wall*> frontier;
    for (int i = 0; i < 4; i++){
        frontier.push_back(grid[startRow][startCol].walls[i]);
    }
    
    while (!frontier.empty()) {
        int idx = rand() % frontier.size();
        Wall* currentWall = frontier[idx];
        if (!currentWall->isEdge) {
            bool in1 = currentWall->c1->isInMaze;
            bool in2 = (currentWall->c2 != nullptr) ? currentWall->c2->isInMaze : false;
            if (in1 ^ in2) {
                currentWall->toOpenWall();
                Cell* newCell = in1 ? currentWall->c2 : currentWall->c1;
                if (newCell != nullptr && !newCell->isInMaze) {
                    newCell->setInMaze();
                    for (int i = 0; i < 4; i++){
                        frontier.push_back(newCell->walls[i]);
                    }
                }
            }
        }
        frontier.erase(frontier.begin() + idx);
    }
    do {
        int endRow = rand() % rows;
        int endCol = rand() % cols;
        if (&grid[endRow][endCol] != &grid[startRow][startCol])
            break;
    } while (true);
}

void Maze::openMaze() {
    //tranh truong hop duong cut
    for (int i = 1; i < rows - 1; i++) {
        for (int j = 1; j < cols - 1; j++) {
            Cell &cell = grid[i][j];
            int r = rand() % 8;
            if (r == 0) {
                for (int k = 0; k < 4; k++) {
                    int r2 = rand() % 3;
                    if (r2 != 0)
                        cell.walls[k]->toOpenWall();
                }
            }
        }
    }
    // Đảm bảo viền ngoài luôn đóng
    for (int j = 0; j < cols; j++){
        grid[0][j].walls[0]->isOpen = false;
        grid[rows-1][j].walls[1]->isOpen = false;
    }
    for (int i = 0; i < rows; i++){
        grid[i][0].walls[2]->isOpen = false;
        grid[i][cols-1].walls[3]->isOpen = false;
    }
}

void Maze::buildWallsFromGrid() {
    vector<SDL_Rect> rects;
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            int x = j * cellSize;
            int y = i * cellSize;
            Cell &cell = grid[i][j];
            if (!cell.walls[0]->isOpen)
                rects.push_back(SDL_Rect{ x, y, cellSize, WALL_THICKNESS });
            if (!cell.walls[1]->isOpen)
                rects.push_back(SDL_Rect{ x, y + cellSize - WALL_THICKNESS, cellSize, WALL_THICKNESS });
            if (!cell.walls[2]->isOpen)
                rects.push_back(SDL_Rect{ x, y, WALL_THICKNESS, cellSize });
            if (!cell.walls[3]->isOpen)
                rects.push_back(SDL_Rect{ x + cellSize - WALL_THICKNESS, y, WALL_THICKNESS, cellSize });
        }
    }
    wallRects = rects;
}

bool Maze::collides(const SDL_Rect &rect) {
    for (auto &w : wallRects) {
        if (SDL_HasIntersection(&rect, &w))
            return true;
    }
    return false;
}

void Maze::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (auto &w : wallRects) {
        SDL_RenderFillRect(renderer, &w);
    }
}

bool Maze::canMove(int cx, int cy, int nx, int ny) const {
    if (nx < 0 || nx >= cols || ny < 0 || ny >= rows)
        return false;
    if (ny == cy - 1 && grid[cy][cx].top == false) return true;
    if (ny == cy + 1 && grid[cy][cx].bottom == false) return true;
    if (nx == cx - 1 && grid[cy][cx].left == false) return true;
    if (nx == cx + 1 && grid[cy][cx].right == false) return true;
    return false;
}
