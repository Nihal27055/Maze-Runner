"""
3D Maze Runner - Night Edition (Python Version)
Built with Pygame and PyOpenGL
"""

import pygame
import random
import math
from pygame.locals import *
from OpenGL.GL import *
from OpenGL.GLU import *
import numpy as np

# Game constants
MAZE_SIZE = 25
CELL_SIZE = 4
WINDOW_WIDTH = 1280
WINDOW_HEIGHT = 720
FOV = 60
GRAVITY = -30
JUMP_HEIGHT = 8
MOVE_SPEED = 0.3
FOOD_COUNT = 30

class MazeGame:
    def __init__(self):
        pygame.init()
        pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT), DOUBLEBUF | OPENGL)
        pygame.display.set_caption("3D Maze Runner - Night Edition (Python)")
        
        # OpenGL setup
        glEnable(GL_DEPTH_TEST)
        glEnable(GL_LIGHTING)
        glEnable(GL_LIGHT0)
        glEnable(GL_COLOR_MATERIAL)
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE)
        
        # Lighting
        glLightfv(GL_LIGHT0, GL_POSITION, [50, 100, 50, 1])
        glLightfv(GL_LIGHT0, GL_AMBIENT, [0.2, 0.2, 0.3, 1])
        glLightfv(GL_LIGHT0, GL_DIFFUSE, [0.8, 0.8, 0.9, 1])
        
        glClearColor(0.05, 0.05, 0.15, 1.0)  # Dark blue night sky
        
        # Camera setup
        gluPerspective(FOV, WINDOW_WIDTH / WINDOW_HEIGHT, 0.1, 1000.0)
        glTranslatef(0, 0, -5)
        
        # Game state
        self.maze = []
        self.food_items = []
        self.food_collected = 0
        self.total_food = 0
        
        # Player state
        self.player_x = 0
        self.player_y = 1
        self.player_z = 0
        self.velocity_y = 0
        self.can_jump = True
        
        # Camera angles
        self.camera_yaw = 0
        self.camera_pitch = -20
        
        # Controls
        self.keys = {}
        self.mouse_sensitivity = 0.1
        
        # Generate maze
        self.generate_maze()
        self.place_food()
        
        # Show instructions
        self.show_instructions()
    
    def show_instructions(self):
        """Display game instructions"""
        screen = pygame.Surface((WINDOW_WIDTH, WINDOW_HEIGHT))
        screen.fill((0, 0, 0))
        font_large = pygame.font.Font(None, 72)
        font_med = pygame.font.Font(None, 36)
        font_small = pygame.font.Font(None, 28)
        
        title = font_large.render("3D Maze Runner - Night Edition", True, (74, 144, 226))
        title_rect = title.get_rect(center=(WINDOW_WIDTH//2, 100))
        screen.blit(title, title_rect)
        
        instructions = [
            "Controls:",
            "WASD - Move",
            "Space - Jump (can jump over walls)",
            "Mouse - Look around",
            "ESC - Exit",
            "",
            "Goal: Collect all food items!",
            "",
            "Press SPACE to start"
        ]
        
        y_offset = 200
        for line in instructions:
            if line.startswith("Controls:") or line.startswith("Goal:"):
                text = font_med.render(line, True, (255, 215, 0))
            else:
                text = font_small.render(line, True, (255, 255, 255))
            text_rect = text.get_rect(center=(WINDOW_WIDTH//2, y_offset))
            screen.blit(text, text_rect)
            y_offset += 40
        
        # Display instructions
        pygame.display.flip()
        waiting = True
        while waiting:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    pygame.quit()
                    exit()
                if event.type == pygame.KEYDOWN and event.key == pygame.K_SPACE:
                    waiting = False
    
    def generate_maze(self):
        """Generate random maze using recursive backtracking"""
        # Initialize with walls
        self.maze = [[1 for _ in range(MAZE_SIZE)] for _ in range(MAZE_SIZE)]
        
        stack = []
        start_x, start_z = 1, 1
        self.maze[start_z][start_x] = 0
        
        directions = [(0, -2), (2, 0), (0, 2), (-2, 0)]
        
        stack.append((start_x, start_z))
        
        while stack:
            x, z = stack[-1]
            neighbors = []
            
            random.shuffle(directions)
            for dx, dz in directions:
                nx, nz = x + dx, z + dz
                if (0 < nx < MAZE_SIZE - 1 and 0 < nz < MAZE_SIZE - 1 and 
                    self.maze[nz][nx] == 1):
                    neighbors.append((nx, nz, x + dx//2, z + dz//2))
            
            if neighbors:
                nx, nz, wx, wz = random.choice(neighbors)
                self.maze[nz][nx] = 0
                self.maze[wz][wx] = 0
                stack.append((nx, nz))
            else:
                stack.pop()
        
        # Create entrance and exit
        self.maze[1][0] = 0
        self.maze[MAZE_SIZE - 2][MAZE_SIZE - 1] = 0
        
        # Set player start position
        self.player_x = -MAZE_SIZE * CELL_SIZE / 2 + CELL_SIZE
        self.player_z = -MAZE_SIZE * CELL_SIZE / 2 + CELL_SIZE
    
    def place_food(self):
        """Place food items randomly in open spaces"""
        self.food_items = []
        self.total_food = 0
        
        for _ in range(FOOD_COUNT):
            attempts = 0
            while attempts < 100:
                x = random.randint(0, MAZE_SIZE - 1)
                z = random.randint(0, MAZE_SIZE - 1)
                
                if self.maze[z][x] == 0:
                    world_x = x * CELL_SIZE - (MAZE_SIZE * CELL_SIZE) / 2 + CELL_SIZE / 2
                    world_z = z * CELL_SIZE - (MAZE_SIZE * CELL_SIZE) / 2 + CELL_SIZE / 2
                    self.food_items.append({
                        'x': world_x,
                        'y': 1.5,
                        'z': world_z,
                        'collected': False,
                        'rotation': random.random() * 360,
                        'original_y': 1.5
                    })
                    self.total_food += 1
                    break
                attempts += 1
    
    def check_collision(self, x, z, y):
        """Check if position collides with wall"""
        maze_x = int((x + MAZE_SIZE * CELL_SIZE / 2) / CELL_SIZE)
        maze_z = int((z + MAZE_SIZE * CELL_SIZE / 2) / CELL_SIZE)
        
        if maze_x < 0 or maze_x >= MAZE_SIZE or maze_z < 0 or maze_z >= MAZE_SIZE:
            return True
        
        if self.maze[maze_z][maze_x] == 1:
            # Can pass if jumping high enough
            return y < 3
        
        return False
    
    def draw_walls(self):
        """Draw maze walls"""
        glPushMatrix()
        
        glColor3f(0.164, 0.164, 0.243)  # Dark blue-gray
        
        for z in range(MAZE_SIZE):
            for x in range(MAZE_SIZE):
                if self.maze[z][x] == 1:
                    glPushMatrix()
                    glTranslatef(
                        x * CELL_SIZE - (MAZE_SIZE * CELL_SIZE) / 2 + CELL_SIZE / 2,
                        2.75,
                        z * CELL_SIZE - (MAZE_SIZE * CELL_SIZE) / 2 + CELL_SIZE / 2
                    )
                    glScalef(CELL_SIZE, 5.5, CELL_SIZE)
                    
                    # Draw cube
                    glBegin(GL_QUADS)
                    # Front
                    glNormal3f(0, 0, 1)
                    glVertex3f(-0.5, -0.5, 0.5)
                    glVertex3f(0.5, -0.5, 0.5)
                    glVertex3f(0.5, 0.5, 0.5)
                    glVertex3f(-0.5, 0.5, 0.5)
                    # Back
                    glNormal3f(0, 0, -1)
                    glVertex3f(-0.5, -0.5, -0.5)
                    glVertex3f(-0.5, 0.5, -0.5)
                    glVertex3f(0.5, 0.5, -0.5)
                    glVertex3f(0.5, -0.5, -0.5)
                    # Top
                    glNormal3f(0, 1, 0)
                    glVertex3f(-0.5, 0.5, -0.5)
                    glVertex3f(-0.5, 0.5, 0.5)
                    glVertex3f(0.5, 0.5, 0.5)
                    glVertex3f(0.5, 0.5, -0.5)
                    # Bottom
                    glNormal3f(0, -1, 0)
                    glVertex3f(-0.5, -0.5, -0.5)
                    glVertex3f(0.5, -0.5, -0.5)
                    glVertex3f(0.5, -0.5, 0.5)
                    glVertex3f(-0.5, -0.5, 0.5)
                    # Left
                    glNormal3f(-1, 0, 0)
                    glVertex3f(-0.5, -0.5, -0.5)
                    glVertex3f(-0.5, -0.5, 0.5)
                    glVertex3f(-0.5, 0.5, 0.5)
                    glVertex3f(-0.5, 0.5, -0.5)
                    # Right
                    glNormal3f(1, 0, 0)
                    glVertex3f(0.5, -0.5, -0.5)
                    glVertex3f(0.5, 0.5, -0.5)
                    glVertex3f(0.5, 0.5, 0.5)
                    glVertex3f(0.5, -0.5, 0.5)
                    glEnd()
                    
                    glPopMatrix()
        
        glPopMatrix()
    
    def draw_food(self, time):
        """Draw food items"""
        glPushMatrix()
        
        for food in self.food_items:
            if not food['collected']:
                glPushMatrix()
                glTranslatef(food['x'], food['y'] + math.sin(time * 2 + food['x']) * 0.3, food['z'])
                glRotatef(food['rotation'] + time * 50, 0, 1, 0)
                
                # Glowing red sphere
                glColor3f(1.0, 0.267, 0.267)
                glEnable(GL_BLEND)
                glBlendFunc(GL_SRC_ALPHA, GL_ONE)
                
                # Draw sphere
                quad = gluNewQuadric()
                gluSphere(quad, 0.3, 16, 16)
                gluDeleteQuadric(quad)
                
                glDisable(GL_BLEND)
                glPopMatrix()
        
        glPopMatrix()
    
    def draw_player(self):
        """Draw player"""
        glPushMatrix()
        glTranslatef(self.player_x, self.player_y, self.player_z)
        
        # Player body (cylinder)
        glColor3f(0.29, 0.56, 0.89)  # Blue
        quad = gluNewQuadric()
        glRotatef(90, 1, 0, 0)
        gluCylinder(quad, 0.5, 0.5, 1.2, 32, 32)
        
        # Player head (sphere)
        glTranslatef(0, 0, 1.2)
        gluSphere(quad, 0.4, 32, 32)
        gluDeleteQuadric(quad)
        
        glPopMatrix()
    
    def draw_ground(self):
        """Draw ground plane"""
        glPushMatrix()
        glColor3f(0.1, 0.1, 0.18)
        glBegin(GL_QUADS)
        size = 500
        glNormal3f(0, 1, 0)
        glVertex3f(-size, 0, -size)
        glVertex3f(size, 0, -size)
        glVertex3f(size, 0, size)
        glVertex3f(-size, 0, size)
        glEnd()
        glPopMatrix()
    
    def draw_stars(self):
        """Draw stars in sky"""
        glPushMatrix()
        glColor3f(1.0, 1.0, 1.0)
        glPointSize(2.0)
        glBegin(GL_POINTS)
        for _ in range(2000):
            x = (random.random() - 0.5) * 1000
            y = random.random() * 400 + 100
            z = (random.random() - 0.5) * 1000
            glVertex3f(x, y, z)
        glEnd()
        glPopMatrix()
    
    def handle_input(self, dt):
        """Handle keyboard and mouse input"""
        # Movement
        move_x = 0
        move_z = 0
        
        if self.keys.get(pygame.K_w):
            move_z = -MOVE_SPEED * dt
        if self.keys.get(pygame.K_s):
            move_z = MOVE_SPEED * dt
        if self.keys.get(pygame.K_a):
            move_x = -MOVE_SPEED * dt
        if self.keys.get(pygame.K_d):
            move_x = MOVE_SPEED * dt
        
        # Apply rotation to movement
        yaw_rad = math.radians(self.camera_yaw)
        new_x = move_x * math.cos(yaw_rad) - move_z * math.sin(yaw_rad)
        new_z = move_x * math.sin(yaw_rad) + move_z * math.cos(yaw_rad)
        
        # Check collision
        next_x = self.player_x + new_x
        next_z = self.player_z + new_z
        
        if not self.check_collision(next_x, self.player_z, self.player_y):
            self.player_x = next_x
        if not self.check_collision(self.player_x, next_z, self.player_y):
            self.player_z = next_z
        
        # Jump
        if self.keys.get(pygame.K_SPACE) and self.can_jump:
            self.velocity_y = JUMP_HEIGHT
            self.can_jump = False
        
        # Gravity
        self.velocity_y += GRAVITY * dt
        self.player_y += self.velocity_y * dt
        
        if self.player_y <= 1:
            self.player_y = 1
            self.velocity_y = 0
            self.can_jump = True
    
    def check_food_collection(self):
        """Check if player collected food"""
        for food in self.food_items:
            if not food['collected']:
                dx = self.player_x - food['x']
                dy = self.player_y - food['y']
                dz = self.player_z - food['z']
                distance = math.sqrt(dx*dx + dy*dy + dz*dz)
                
                if distance < 1.5:
                    food['collected'] = True
                    self.food_collected += 1
                    
                    if self.food_collected >= self.total_food:
                        print("Congratulations! You collected all the food!")
                        # Regenerate game
                        self.generate_maze()
                        self.place_food()
                        self.food_collected = 0
    
    def update_camera(self):
        """Update camera based on player position and rotation"""
        glLoadIdentity()
        
        # Calculate camera position (third-person)
        yaw_rad = math.radians(self.camera_yaw)
        pitch_rad = math.radians(self.camera_pitch)
        
        distance = 8
        height = 5
        
        cam_x = self.player_x - math.sin(yaw_rad) * distance * math.cos(pitch_rad)
        cam_y = self.player_y + height + math.sin(pitch_rad) * distance
        cam_z = self.player_z - math.cos(yaw_rad) * distance * math.cos(pitch_rad)
        
        gluLookAt(
            cam_x, cam_y, cam_z,  # Camera position
            self.player_x, self.player_y + 1, self.player_z,  # Look at player
            0, 1, 0  # Up vector
        )
    
    def run(self):
        """Main game loop"""
        clock = pygame.time.Clock()
        running = True
        time = 0
        
        while running:
            dt = clock.tick(60) / 1000.0  # Delta time in seconds
            time += dt
            
            # Handle events
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    running = False
                elif event.type == pygame.KEYDOWN:
                    self.keys[event.key] = True
                    if event.key == pygame.K_ESCAPE:
                        running = False
                elif event.type == pygame.KEYUP:
                    self.keys[event.key] = False
                elif event.type == pygame.MOUSEMOTION:
                    if pygame.mouse.get_pressed()[0]:
                        dx, dy = event.rel
                        self.camera_yaw += dx * self.mouse_sensitivity
                        self.camera_pitch -= dy * self.mouse_sensitivity
                        self.camera_pitch = max(-89, min(89, self.camera_pitch))
            
            # Handle input
            self.handle_input(dt)
            
            # Check food collection
            self.check_food_collection()
            
            # Clear screen
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
            
            # Update camera
            self.update_camera()
            
            # Draw scene
            self.draw_ground()
            self.draw_stars()
            self.draw_walls()
            self.draw_food(time)
            self.draw_player()
            
            # Update display
            pygame.display.flip()
        
        pygame.quit()

if __name__ == "__main__":
    game = MazeGame()
    game.run()

