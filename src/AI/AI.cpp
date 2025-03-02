#include "AI.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <queue>
#include <limits>

static const float PI = 3.14159f;

struct Node {
    int x, y;
    float g, h;
    Node* parent;
};

static float heuristic(int x1, int y1, int x2, int y2) {
    return std::abs(x1 - x2) + std::abs(y1 - y2);
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

AITank::AITank(float x, float y, SDL_Color color, int owner)
    : Tank(x, y, color), currentWaypoint(0), pathUpdateTimer(0), shootCooldown(0), aiOwner(owner)
{ }

void AITank::updateAI(const std::vector<Tank*>& targets, Maze &maze, float deltaTime, std::vector<Bullet> &bullets) {
    shootCooldown -= deltaTime;
    pathUpdateTimer -= deltaTime;

    //finding the nearest target
    Tank* nearestTarget = nullptr;
    float minDistance = std::numeric_limits<float>::max();
    SDL_Point myCenter = getCenter();
    for (Tank* t : targets) {
        if (t && t->alive) {
            SDL_Point tCenter = t->getCenter();
            float dx = tCenter.x - myCenter.x;
            float dy = tCenter.y - myCenter.y;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist < minDistance) {
                minDistance = dist;
                nearestTarget = t;
            }
        }
    }
    if (!nearestTarget)
        return; 

    int aiCellX = static_cast<int>(myCenter.x / maze.cellSize);
    int aiCellY = static_cast<int>(myCenter.y / maze.cellSize);
    int targetCellX = static_cast<int>(nearestTarget->getCenter().x / maze.cellSize);
    int targetCellY = static_cast<int>(nearestTarget->getCenter().y / maze.cellSize);

    if (pathUpdateTimer <= 0) {
        path = computeAStar(maze, aiCellX, aiCellY, targetCellX, targetCellY);
        currentWaypoint = 0;
        pathUpdateTimer = 1.0f;
        //neu khong tim thay duong di se di chuyen ngau nhien
        if (path.empty()) {
            float randomAngle = ((float)rand() / RAND_MAX) * 2.f * PI;
            x += cos(randomAngle) * speed * deltaTime;
            y += sin(randomAngle) * speed * deltaTime;
        }
    }

    if (!path.empty() && currentWaypoint < static_cast<int>(path.size())) {
        SDL_Point wp = path[currentWaypoint];
        float targetX = wp.x * maze.cellSize + maze.cellSize / 2 - width / 2;
        float targetY = wp.y * maze.cellSize + maze.cellSize / 2 - height / 2;
        float dx = targetX - x;
        float dy = targetY - y;
        float dist = std::sqrt(dx * dx + dy * dy);
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
            angle = std::atan2(moveY, moveX);
        }
    }

    if (shootCooldown <= 0) {
        SDL_Point a = getCenter();
        SDL_Point tCenter = nearestTarget->getCenter();
        float dx = tCenter.x - a.x;
        float dy = tCenter.y - a.y;
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist < 300) {
            shoot(bullets);
            shootCooldown = 1.0f;
        }
    }
}


void AITank::shoot(std::vector<Bullet> &bullets) {
    float bulletSpeed = 200.0f;
    SDL_Point c = getCenter();
    float bx = c.x - 3;
    float by = c.y - 3;
    // Sử dụng aiOwner đã được thiết lập để đánh dấu viên đạn của AI này
    bullets.push_back(Bullet(bx, by, std::cos(angle) * bulletSpeed, std::sin(angle) * bulletSpeed, aiOwner));
}

void AITank::render(SDL_Renderer* renderer) {
    Tank::render(renderer);
}
