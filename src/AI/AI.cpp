#include "AI.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <queue>

static const float PI = 3.14159f;

struct Node {
    int x, y;
    float g, h;
    Node* parent;
};

static float heuristic(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

struct CompareNode {
    bool operator()(Node* a, Node* b) {
        return (a->g + a->h) > (b->g + b->h);
    }
};

static std::vector<SDL_Point> computeAStar(Maze &maze, int startX, int startY, int goalX, int goalY) {
    std::vector<SDL_Point> path;
    std::vector<std::vector<bool>> closed(maze.rows, std::vector<bool>(maze.cols, false));
    std::vector<std::vector<Node*>> allNodes(maze.rows, std::vector<Node*>(maze.cols, nullptr));

    std::priority_queue<Node*, std::vector<Node*>, CompareNode> openSet;

    Node* startNode = new Node{ startX, startY, 0, heuristic(startX, startY, goalX, goalY), nullptr };
    openSet.push(startNode);
    allNodes[startY][startX] = startNode;

    int dx[4] = {0, 0, -1, 1};
    int dy[4] = {-1, 1, 0, 0};

    Node* goalNode = nullptr;
    while (!openSet.empty()) {
        Node* current = openSet.top();
        openSet.pop();
        if (current->x == goalX && current->y == goalY) {
            goalNode = current;
            break;
        }
        closed[current->y][current->x] = true;
        for (int i = 0; i < 4; i++) {
            int nx = current->x + dx[i];
            int ny = current->y + dy[i];
            if (nx < 0 || nx >= maze.cols || ny < 0 || ny >= maze.rows)
                continue;
            if (closed[ny][nx])
                continue;

            Cell &currCell = maze.grid[current->y][current->x];
            if (i == 0 && !currCell.walls[0]->isOpen) continue;
            if (i == 1 && !currCell.walls[1]->isOpen) continue;
            if (i == 2 && !currCell.walls[2]->isOpen) continue;
            if (i == 3 && !currCell.walls[3]->isOpen) continue;

            float tentative_g = current->g + 1;
            if (allNodes[ny][nx] == nullptr) {
                Node* neighbor = new Node{ nx, ny, tentative_g, heuristic(nx, ny, goalX, goalY), current };
                allNodes[ny][nx] = neighbor;
                openSet.push(neighbor);
            } else if (tentative_g < allNodes[ny][nx]->g) {
                allNodes[ny][nx]->g = tentative_g;
                allNodes[ny][nx]->parent = current;
                openSet.push(allNodes[ny][nx]);
            }
        }
    }

    if (goalNode != nullptr) {
        Node* current = goalNode;
        while (current != nullptr) {
            path.push_back(SDL_Point{ current->x, current->y });
            current = current->parent;
        }
        std::reverse(path.begin(), path.end());
    }

    for (auto &row : allNodes) {
        for (auto &node : row) {
            delete node;
        }
    }

    return path;
}

AITank::AITank(float x, float y, SDL_Color color)
    : Tank(x, y, color), currentWaypoint(0), pathUpdateTimer(0), shootCooldown(0)
{
}

void AITank::updateAI(Tank &player, Maze &maze, float deltaTime, std::vector<Bullet> &bullets) {
    shootCooldown -= deltaTime;
    pathUpdateTimer -= deltaTime;

    int aiCellX = (int)(getCenter().x / maze.cellSize);
    int aiCellY = (int)(getCenter().y / maze.cellSize);
    int pCellX = (int)(player.getCenter().x / maze.cellSize);
    int pCellY = (int)(player.getCenter().y / maze.cellSize);

    //cap nhat duong di
    if (pathUpdateTimer <= 0) {
        path = computeAStar(maze, aiCellX, aiCellY, pCellX, pCellY);
        currentWaypoint = 0;
        pathUpdateTimer = 1.0f;
    }

    //AI di chuyen theo path
    if (!path.empty() && currentWaypoint < (int)path.size()) {
        SDL_Point wp = path[currentWaypoint];
        float targetX = wp.x * maze.cellSize + maze.cellSize / 2 - width / 2;
        float targetY = wp.y * maze.cellSize + maze.cellSize / 2 - height / 2;
        float dx = targetX - x;
        float dy = targetY - y;
        float dist = sqrt(dx * dx + dy * dy);

        if (dist < 5) {
            currentWaypoint++;
        } else {
            float moveX = dx / dist;
            float moveY = dy / dist;
            float moveDist = speed * deltaTime;

            float oldX = x, oldY = y;
            x += moveX * moveDist;
            y += moveY * moveDist;

            if (collidesWithMaze(getCollisionRect(), maze)) {
                x = oldX;
                y = oldY;
            }

            angle = atan2(moveY, moveX);  
        }
    }

    //AI ban khi trong pham vi
    if (shootCooldown <= 0) {
        SDL_Point a = getCenter();
        SDL_Point p = player.getCenter();
        float dx = p.x - a.x, dy = p.y - a.y;
        float dist = sqrt(dx * dx + dy * dy);
        if (dist < 300) {
            shoot(bullets);
            shootCooldown = 1.0f;   //khoang thoi gian cho
        }
    }
}


void AITank::shoot(std::vector<Bullet> &bullets) {
    float bulletSpeed = 200.0f;
    SDL_Point c = getCenter();
    float bx = c.x - 3;
    float by = c.y - 3;
    bullets.push_back(Bullet(bx, by, cos(angle)*bulletSpeed, sin(angle)*bulletSpeed, 1));
}

void AITank::render(SDL_Renderer* renderer) {
    Tank::render(renderer);
}