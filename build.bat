@echo off
g++ -o AzTankTrouble.exe src/main.cpp src/Game/Game.cpp src/Maze/Maze.cpp src/Bullet/Bullet.cpp src/Tank/Tank.cpp src/AI/AI.cpp src/UI.cpp -Isrc -IC:/msys64/mingw64/include -LC:/msys64/mingw64/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
pause
