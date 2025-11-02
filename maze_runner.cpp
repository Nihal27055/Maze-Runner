/*
 * 3D Maze Runner - Night Edition (C++ Version)
 * Built with SFML and OpenGL
 * Compile with: g++ maze_runner.cpp -o maze_runner -lsfml-window -lsfml-system -lGL -lGLU
 */

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <vector>
#include <random>
#include <iostream>
#include <algorithm>

// Constants
const int MAZE_SIZE = 25;
const float CELL_SIZE = 4.0f;
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const float FOV = 60.0f;
const float GRAVITY = -30.0f;
const float JUMP_HEIGHT = 8.0f;
const float MOVE_SPEED = 0.3f;
const int FOOD_COUNT = 30;

struct Food {
    float x, y, z;
    bool collected;
    float rotation;
    float original_y;
};

class MazeGame {
private:
    std::vector<std::vector<int>> maze;
    std::vector<Food> foodItems;
    int foodCollected;
    int totalFood;
    
    float playerX, playerY, playerZ;
    float velocityY;
    bool canJump;
    
    float cameraYaw;
    float cameraPitch;
    
    bool keys[256];
    float time;
    
    sf::Window* window;
    
public:
    MazeGame() : foodCollected(0), totalFood(0),
                 playerX(0), playerY(1), playerZ(0),
                 velocityY(0), canJump(true),
                 cameraYaw(0), cameraPitch(-20),
                 time(0) {
        // Initialize keys array
        for (int i = 0; i < 256; i++) {
            keys[i] = false;
        }
        
        // Create window
        sf::ContextSettings settings;
        settings.depthBits = 24;
        settings.stencilBits = 8;
        settings.antialiasingLevel = 4;
        
        window = new sf::Window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), 
                               "3D Maze Runner - Night Edition (C++)",
                               sf::Style::Default, settings);
        
        window->setFramerateLimit(60);
        window->setMouseCursorVisible(false);
        window->setMouseCursorGrabbed(true);
        
        // OpenGL setup
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        
        // Lighting
        float lightPos[] = {50, 100, 50, 1};
        float lightAmb[] = {0.2f, 0.2f, 0.3f, 1};
        float lightDiff[] = {0.8f, 0.8f, 0.9f, 1};
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiff);
        
        glClearColor(0.05f, 0.05f, 0.15f, 1.0f);
        
        // Setup projection
        glMatrixMode(GL_PROJECTION);
        gluPerspective(FOV, (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 1000.0f);
        glMatrixMode(GL_MODELVIEW);
        
        // Generate maze
        generateMaze();
        placeFood();
        
        showInstructions();
    }
    
    void showInstructions() {
        std::cout << "\n=== 3D Maze Runner - Night Edition ===\n";
        std::cout << "Controls:\n";
        std::cout << "WASD - Move\n";
        std::cout << "Space - Jump (can jump over walls)\n";
        std::cout << "Mouse - Look around\n";
        std::cout << "ESC - Exit\n";
        std::cout << "\nGoal: Collect all food items!\n";
        std::cout << "Press SPACE to start...\n";
        
        bool waiting = true;
        while (waiting) {
            sf::Event event;
            while (window->pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window->close();
                    return;
                }
                if (event.type == sf::Event::KeyPressed && 
                    event.key.code == sf::Keyboard::Space) {
                    waiting = false;
                }
            }
            sf::sleep(sf::milliseconds(10));
        }
    }
    
    void generateMaze() {
        // Initialize with walls
        maze.resize(MAZE_SIZE, std::vector<int>(MAZE_SIZE, 1));
        
        std::vector<std::pair<int, int>> stack;
        int startX = 1, startZ = 1;
        maze[startZ][startX] = 0;
        
        std::vector<std::pair<int, int>> directions = {{0, -2}, {2, 0}, {0, 2}, {-2, 0}};
        
        stack.push_back({startX, startZ});
        std::random_device rd;
        std::mt19937 gen(rd());
        
        while (!stack.empty()) {
            int x = stack.back().first;
            int z = stack.back().second;
            
            std::vector<std::tuple<int, int, int, int>> neighbors;
            
            std::shuffle(directions.begin(), directions.end(), gen);
            for (auto [dx, dz] : directions) {
                int nx = x + dx;
                int nz = z + dz;
                if (nx > 0 && nx < MAZE_SIZE - 1 && nz > 0 && nz < MAZE_SIZE - 1 && 
                    maze[nz][nx] == 1) {
                    neighbors.push_back({nx, nz, x + dx/2, z + dz/2});
                }
            }
            
            if (!neighbors.empty()) {
                auto [nx, nz, wx, wz] = neighbors[gen() % neighbors.size()];
                maze[nz][nx] = 0;
                maze[wz][wx] = 0;
                stack.push_back({nx, nz});
            } else {
                stack.pop_back();
            }
        }
        
        // Create entrance and exit
        maze[1][0] = 0;
        maze[MAZE_SIZE - 2][MAZE_SIZE - 1] = 0;
        
        // Set player start
        playerX = -MAZE_SIZE * CELL_SIZE / 2 + CELL_SIZE;
        playerZ = -MAZE_SIZE * CELL_SIZE / 2 + CELL_SIZE;
    }
    
    void placeFood() {
        foodItems.clear();
        totalFood = 0;
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, MAZE_SIZE - 1);
        std::uniform_real_distribution<> rotDis(0, 360);
        
        for (int i = 0; i < FOOD_COUNT; i++) {
            int attempts = 0;
            while (attempts < 100) {
                int x = dis(gen);
                int z = dis(gen);
                
                if (maze[z][x] == 0) {
                    float worldX = x * CELL_SIZE - (MAZE_SIZE * CELL_SIZE) / 2 + CELL_SIZE / 2;
                    float worldZ = z * CELL_SIZE - (MAZE_SIZE * CELL_SIZE) / 2 + CELL_SIZE / 2;
                    
                    Food food;
                    food.x = worldX;
                    food.y = 1.5f;
                    food.z = worldZ;
                    food.collected = false;
                    food.rotation = rotDis(gen);
                    food.original_y = 1.5f;
                    
                    foodItems.push_back(food);
                    totalFood++;
                    break;
                }
                attempts++;
            }
        }
    }
    
    bool checkCollision(float x, float z, float y) {
        int mazeX = (int)((x + MAZE_SIZE * CELL_SIZE / 2) / CELL_SIZE);
        int mazeZ = (int)((z + MAZE_SIZE * CELL_SIZE / 2) / CELL_SIZE);
        
        if (mazeX < 0 || mazeX >= MAZE_SIZE || mazeZ < 0 || mazeZ >= MAZE_SIZE) {
            return true;
        }
        
        if (maze[mazeZ][mazeX] == 1) {
            return y < 3;
        }
        
        return false;
    }
    
    void drawWalls() {
        glColor3f(0.164f, 0.164f, 0.243f);
        
        for (int z = 0; z < MAZE_SIZE; z++) {
            for (int x = 0; x < MAZE_SIZE; x++) {
                if (maze[z][x] == 1) {
                    glPushMatrix();
                    glTranslatef(
                        x * CELL_SIZE - (MAZE_SIZE * CELL_SIZE) / 2 + CELL_SIZE / 2,
                        2.75f,
                        z * CELL_SIZE - (MAZE_SIZE * CELL_SIZE) / 2 + CELL_SIZE / 2
                    );
                    glScalef(CELL_SIZE, 5.5f, CELL_SIZE);
                    
                    // Draw cube
                    glBegin(GL_QUADS);
                    // Front
                    glNormal3f(0, 0, 1);
                    glVertex3f(-0.5f, -0.5f, 0.5f);
                    glVertex3f(0.5f, -0.5f, 0.5f);
                    glVertex3f(0.5f, 0.5f, 0.5f);
                    glVertex3f(-0.5f, 0.5f, 0.5f);
                    // Back
                    glNormal3f(0, 0, -1);
                    glVertex3f(-0.5f, -0.5f, -0.5f);
                    glVertex3f(-0.5f, 0.5f, -0.5f);
                    glVertex3f(0.5f, 0.5f, -0.5f);
                    glVertex3f(0.5f, -0.5f, -0.5f);
                    // Top
                    glNormal3f(0, 1, 0);
                    glVertex3f(-0.5f, 0.5f, -0.5f);
                    glVertex3f(-0.5f, 0.5f, 0.5f);
                    glVertex3f(0.5f, 0.5f, 0.5f);
                    glVertex3f(0.5f, 0.5f, -0.5f);
                    // Bottom
                    glNormal3f(0, -1, 0);
                    glVertex3f(-0.5f, -0.5f, -0.5f);
                    glVertex3f(0.5f, -0.5f, -0.5f);
                    glVertex3f(0.5f, -0.5f, 0.5f);
                    glVertex3f(-0.5f, -0.5f, 0.5f);
                    // Left
                    glNormal3f(-1, 0, 0);
                    glVertex3f(-0.5f, -0.5f, -0.5f);
                    glVertex3f(-0.5f, -0.5f, 0.5f);
                    glVertex3f(-0.5f, 0.5f, 0.5f);
                    glVertex3f(-0.5f, 0.5f, -0.5f);
                    // Right
                    glNormal3f(1, 0, 0);
                    glVertex3f(0.5f, -0.5f, -0.5f);
                    glVertex3f(0.5f, 0.5f, -0.5f);
                    glVertex3f(0.5f, 0.5f, 0.5f);
                    glVertex3f(0.5f, -0.5f, 0.5f);
                    glEnd();
                    
                    glPopMatrix();
                }
            }
        }
    }
    
    void drawFood() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        
        for (auto& food : foodItems) {
            if (!food.collected) {
                glPushMatrix();
                glTranslatef(food.x, food.y + sinf(time * 2 + food.x) * 0.3f, food.z);
                glRotatef(food.rotation + time * 50, 0, 1, 0);
                
                glColor3f(1.0f, 0.267f, 0.267f);
                
                // Draw sphere using quadrics
                GLUquadric* quad = gluNewQuadric();
                gluSphere(quad, 0.3f, 16, 16);
                gluDeleteQuadric(quad);
                
                glPopMatrix();
            }
        }
        
        glDisable(GL_BLEND);
    }
    
    void drawPlayer() {
        glPushMatrix();
        glTranslatef(playerX, playerY, playerZ);
        
        glColor3f(0.29f, 0.56f, 0.89f);
        GLUquadric* quad = gluNewQuadric();
        glRotatef(90, 1, 0, 0);
        gluCylinder(quad, 0.5f, 0.5f, 1.2f, 32, 32);
        glTranslatef(0, 0, 1.2f);
        gluSphere(quad, 0.4f, 32, 32);
        gluDeleteQuadric(quad);
        
        glPopMatrix();
    }
    
    void drawGround() {
        glColor3f(0.1f, 0.1f, 0.18f);
        glBegin(GL_QUADS);
        float size = 500.0f;
        glNormal3f(0, 1, 0);
        glVertex3f(-size, 0, -size);
        glVertex3f(size, 0, -size);
        glVertex3f(size, 0, size);
        glVertex3f(-size, 0, size);
        glEnd();
    }
    
    void drawStars() {
        glColor3f(1.0f, 1.0f, 1.0f);
        glPointSize(2.0f);
        glBegin(GL_POINTS);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(-500, 500);
        std::uniform_real_distribution<> yDis(100, 500);
        
        for (int i = 0; i < 2000; i++) {
            glVertex3f(dis(gen), yDis(gen), dis(gen));
        }
        glEnd();
    }
    
    void handleInput(float dt) {
        float moveX = 0, moveZ = 0;
        
        if (keys[sf::Keyboard::W]) moveZ = -MOVE_SPEED * dt;
        if (keys[sf::Keyboard::S]) moveZ = MOVE_SPEED * dt;
        if (keys[sf::Keyboard::A]) moveX = -MOVE_SPEED * dt;
        if (keys[sf::Keyboard::D]) moveX = MOVE_SPEED * dt;
        
        float yawRad = cameraYaw * M_PI / 180.0f;
        float newX = moveX * cosf(yawRad) - moveZ * sinf(yawRad);
        float newZ = moveX * sinf(yawRad) + moveZ * cosf(yawRad);
        
        if (!checkCollision(playerX + newX, playerZ, playerY)) {
            playerX += newX;
        }
        if (!checkCollision(playerX, playerZ + newZ, playerY)) {
            playerZ += newZ;
        }
        
        if (keys[sf::Keyboard::Space] && canJump) {
            velocityY = JUMP_HEIGHT;
            canJump = false;
        }
        
        velocityY += GRAVITY * dt;
        playerY += velocityY * dt;
        
        if (playerY <= 1) {
            playerY = 1;
            velocityY = 0;
            canJump = true;
        }
    }
    
    void checkFoodCollection() {
        for (auto& food : foodItems) {
            if (!food.collected) {
                float dx = playerX - food.x;
                float dy = playerY - food.y;
                float dz = playerZ - food.z;
                float distance = sqrtf(dx*dx + dy*dy + dz*dz);
                
                if (distance < 1.5f) {
                    food.collected = true;
                    foodCollected++;
                    
                    if (foodCollected >= totalFood) {
                        std::cout << "Congratulations! You collected all the food!\n";
                        generateMaze();
                        placeFood();
                        foodCollected = 0;
                    }
                }
            }
        }
    }
    
    void updateCamera() {
        glLoadIdentity();
        
        float yawRad = cameraYaw * M_PI / 180.0f;
        float pitchRad = cameraPitch * M_PI / 180.0f;
        
        float distance = 8.0f;
        float height = 5.0f;
        
        float camX = playerX - sinf(yawRad) * distance * cosf(pitchRad);
        float camY = playerY + height + sinf(pitchRad) * distance;
        float camZ = playerZ - cosf(yawRad) * distance * cosf(pitchRad);
        
        gluLookAt(camX, camY, camZ,
                  playerX, playerY + 1, playerZ,
                  0, 1, 0);
    }
    
    void updateFoodAnimation() {
        for (auto& food : foodItems) {
            if (!food.collected) {
                food.rotation += 50 * 0.016f;
                food.y = food.original_y + sinf(time * 2 + food.x) * 0.3f;
            }
        }
    }
    
    void run() {
        sf::Clock clock;
        bool running = true;
        
        while (running) {
            float dt = clock.restart().asSeconds();
            time += dt;
            
            sf::Event event;
            while (window->pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    running = false;
                } else if (event.type == sf::Event::KeyPressed) {
                    keys[event.key.code] = true;
                    if (event.key.code == sf::Keyboard::Escape) {
                        running = false;
                    }
                } else if (event.type == sf::Event::KeyReleased) {
                    keys[event.key.code] = false;
                } else if (event.type == sf::Event::MouseMoved) {
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                        cameraYaw += event.mouseMove.x * 0.1f;
                        cameraPitch -= event.mouseMove.y * 0.1f;
                        if (cameraPitch > 89) cameraPitch = 89;
                        if (cameraPitch < -89) cameraPitch = -89;
                    }
                }
            }
            
            handleInput(dt);
            checkFoodCollection();
            updateFoodAnimation();
            
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            updateCamera();
            
            drawGround();
            drawStars();
            drawWalls();
            drawFood();
            drawPlayer();
            
            window->display();
        }
        
        delete window;
    }
};

int main() {
    MazeGame game;
    game.run();
    return 0;
}

