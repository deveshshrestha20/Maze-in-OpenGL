#include<windows.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stack>
#include <mmsystem.h>

#define CELL_SIZE 20
#define ANIMATION_DELAY_MS 2000 // Animation delay in milliseconds

#define WALL 0
#define PATH 1
#define VISITED 2

time_t startTime = 0;

int elapsedTime = 0;
int MAZE_WIDTH=41;
int MAZE_HEIGHT=25;
int maze[100][100];
int pathWidth = 3;
int nextX, nextY;
int startX, startY;
int endX, endY;
int bar =0;
int playerX =1, playerY =1;
int states =0;
std::stack<std::pair<int, int> > backtrackStack;
std::pair<int, int> lastPoppedCell;
int window_width = GetSystemMetrics(SM_CXSCREEN);
int window_height = GetSystemMetrics(SM_CYSCREEN);

enum Mode {
    MODE_OPTIONS,
    MODE_GAME_MENU
};

Mode currentMode = MODE_GAME_MENU;

// Function to initialize the maze with walls
void initializeMaze() {
	glColor3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < MAZE_HEIGHT; i++) {
        for (int j = 0; j < MAZE_WIDTH; j++) {
            if (i % 2 == 0 || j % 2 == 0)
                maze[i][j] = WALL;
            else
                maze[i][j] = PATH;
        }
    }
    // Set starting point at one end
    startX = 1;
    startY = 1;
    maze[startY][startX] = VISITED;

    // Set endpoint at another corner
    endX = MAZE_WIDTH - 2;
    endY = MAZE_HEIGHT - 1;
    maze[endY][endX] = PATH;
}

// Function to generate maze using depth-first search
void generateMaze(int value) {
    int x = nextX;
    int y = nextY;

    int dx[] = {0, 0, 1, -1};
    int dy[] = {1, -1, 0, 0};

    maze[y][x] = VISITED;

    // Randomize direction
    int dirs[4] = {0, 1, 2, 3};
    for (int i = 0; i < 4; i++) {
        int r = rand() % 4;
        int temp = dirs[i];
        dirs[i] = dirs[r];
        dirs[r] = temp;
    }

    // Visit neighbors
    bool found = false;
    for (int i = 0; i < 4; i++) {
        int nx = x + dx[dirs[i]] * 2;
        int ny = y + dy[dirs[i]] * 2;

        if (nx > 0 && nx < MAZE_WIDTH && ny > 0 && ny < MAZE_HEIGHT && maze[ny][nx] == PATH) {
            maze[y + dy[dirs[i]]][x + dx[dirs[i]]] = VISITED;
            backtrackStack.push(std::make_pair(x, y));
            nextX = nx;
            nextY = ny;
            found = true;
            break; // Exit the loop as we found a valid neighbor
        }
    }

    // If no valid neighbor was found, backtrack
    if (!found && !backtrackStack.empty()) {
        std::pair<int, int> prevCell = backtrackStack.top();
        backtrackStack.pop();
        nextX = prevCell.first;
        nextY = prevCell.second;
        found = true;
    }

    // Redraw the maze to show animation after each step
    glutPostRedisplay();

    // If backtracked or found a valid neighbor, continue generating maze
    if (found)
        glutTimerFunc(ANIMATION_DELAY_MS, generateMaze, 0);
}

// Function to draw the maze
void drawMaze() {
	bar = 1;
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.5, 0.5, 0.5, 0.0);

    glLoadIdentity();
    if (states == 1 && startTime == 0) {
    	startTime = time(NULL); // Start the timer when maze generation begins
	}

	if (states == 1) {
	    time_t currentTime = time(NULL);
	    elapsedTime = difftime(currentTime, startTime);
	}

	// Display elapsed time on the screen
	glColor3f(0.0f, 0.0f, 0.0f);
	glRasterPos2f(window_width - 200, window_height - 50);
	char timeString[50];
	sprintf(timeString, "Time: %d seconds", elapsedTime);
	for (int i = 0; timeString[i] != '\0'; i++) {
	    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, timeString[i]);
	}

    int offsetX = (glutGet(GLUT_WINDOW_WIDTH) - MAZE_WIDTH * CELL_SIZE) / 2;
    int offsetY = (glutGet(GLUT_WINDOW_HEIGHT) - MAZE_HEIGHT * CELL_SIZE) / 2;

    glTranslatef(offsetX, offsetY, 0);

    glColor3f(0.0, 0.0, 0.0);
    for (int i = 0; i < MAZE_HEIGHT; i++) {
        for (int j = 0; j < MAZE_WIDTH; j++) {
            if (maze[i][j] == WALL) {
                glBegin(GL_QUADS);
                glVertex2i(j * CELL_SIZE, i * CELL_SIZE);
                glVertex2i(j * CELL_SIZE + CELL_SIZE, i * CELL_SIZE);
                glVertex2i(j * CELL_SIZE + CELL_SIZE, i * CELL_SIZE + CELL_SIZE);
                glVertex2i(j * CELL_SIZE, i * CELL_SIZE + CELL_SIZE);
                glEnd();
            }
        }
    }

    // Draw starting point
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_QUADS);
    glVertex2i(startX * CELL_SIZE, startY * CELL_SIZE);
    glVertex2i(startX * CELL_SIZE + CELL_SIZE, startY * CELL_SIZE);
    glVertex2i(startX * CELL_SIZE + CELL_SIZE, startY * CELL_SIZE + CELL_SIZE);
    glVertex2i(startX * CELL_SIZE, startY * CELL_SIZE + CELL_SIZE);
    glEnd();

    // Draw endpoint
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_QUADS);
    glVertex2i(endX * CELL_SIZE, endY * CELL_SIZE);
    glVertex2i(endX * CELL_SIZE + CELL_SIZE, endY * CELL_SIZE);
    glVertex2i(endX * CELL_SIZE + CELL_SIZE, endY * CELL_SIZE + CELL_SIZE);
    glVertex2i(endX * CELL_SIZE, endY * CELL_SIZE + CELL_SIZE);
    glEnd();

	// Draw player
    glColor3f(0.0, 0.0, 1.0); // Blue color for player
    glBegin(GL_QUADS);
    glVertex2i(playerX * CELL_SIZE, playerY * CELL_SIZE);
    glVertex2i(playerX * CELL_SIZE + CELL_SIZE, playerY * CELL_SIZE);
    glVertex2i(playerX * CELL_SIZE + CELL_SIZE, playerY * CELL_SIZE + CELL_SIZE);
    glVertex2i(playerX * CELL_SIZE, playerY * CELL_SIZE + CELL_SIZE);
    glEnd();

    glutSwapBuffers();
}

void mainpage()
{
    glClear(GL_COLOR_BUFFER_BIT);
    bar = 0;
    glClearColor(0.5, 0.5, 0.5, 0.0);

    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(2.3*window_width / 5, window_height / 2 + 75);
    const char *startOption = "New Game";
    for (int i = 0; startOption[i] != '\0'; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, startOption[i]);
    }

    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(2.3*window_width / 5, window_height / 2 + 25);
    const char *InstructionOption = "Instruction";
    for (int i = 0; InstructionOption[i] != '\0'; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, InstructionOption[i]);
    }

    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(2.3*window_width / 5, window_height / 2 - 25);
    const char *exitOption = "Option";
    for (int i = 0; exitOption[i] != '\0'; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, exitOption[i]);
    }

    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(2.3*window_width / 5, window_height / 2 - 75);
    const char *optionOption = "EXIT";

    for (int i = 0; optionOption[i] != '\0'; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, optionOption[i]);
    }

    glutSwapBuffers();
    glutPostRedisplay();
}
// Function to initialize OpenGL
void init() {
    glClearColor(0.5, 0.5, 0.5, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void optionpage() {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.5, 0.5, 0.5, 0.0);

	glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(2.3*window_width / 5, window_height / 2 + 100);
    const char *Option = "Options";
    for (int i = 0; Option[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, Option[i]);
    }

    // Draw the "Easy" button with red box
    glColor3f(1.0, 0.0, 0.0); // Red color
    glBegin(GL_QUADS);
    glVertex2f(2.29*window_width / 5+100, window_height / 2 + 70); // Top-left corner
    glVertex2f(2.29*window_width / 5, window_height / 2 +70); // Top-right corner
    glVertex2f(2.29*window_width / 5, window_height / 2+40);     // Bottom-right corner
    glVertex2f(2.29*window_width / 5+100, window_height / 2+40);     // Bottom-left corner
    glEnd();

    // Draw the "Medium" button with red box
    glBegin(GL_QUADS);
    glVertex2f(2.29*window_width / 5+100, window_height / 2 + 20); // Top-left corner
    glVertex2f(2.29*window_width / 5, window_height / 2 + 20); // Top-right corner
    glVertex2f(2.29*window_width / 5, window_height / 2 - 10 ); // Bottom-right corner
    glVertex2f(2.29*window_width / 5+100, window_height / 2 -10 ); // Bottom-left corner
    glEnd();

    // Draw the "Hard" button with red box
    glBegin(GL_QUADS);
    glVertex2f(2.29*window_width / 5+100, window_height / 2 -30); // Top-left corner
    glVertex2f(2.29*window_width / 5, window_height / 2 -30); // Top-right corner
    glVertex2f(2.29*window_width / 5, window_height / 2 - 60); // Bottom-right corner
    glVertex2f(2.29*window_width / 5+100, window_height / 2 - 60); // Bottom-left corner
    glEnd();

    // Draw text for each button
    glColor3f(1.0, 1.0, 1.0); // White color for text
    glRasterPos2f(2.37*window_width / 5, window_height / 2 + 50);
    const char *easyOption = "Easy";
    for (int i = 0; easyOption[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, easyOption[i]);
    }

    glRasterPos2f(2.37*window_width / 5, window_height / 2 );
    const char *medOption = "Medium";
    for (int i = 0; medOption[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, medOption[i]);
    }

    glRasterPos2f(2.37*window_width / 5, window_height / 2 - 50);
    const char *hardOption = "Hard";
    for (int i = 0; hardOption[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, hardOption[i]);
    }

    glutSwapBuffers();
}

void instructionpage()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.5, 0.5, 0.5, 0.0);
    glColor3f(0.0, 0.0, 0.0);

    glRasterPos2f(window_width / 2 - 100, window_height / 2 +200);
    const char *instruction = "Instructions:";
    for (int i = 0; instruction[i] != '\0'; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, instruction[i]);
    }

    glRasterPos2f(window_width / 2 - 250, window_height / 2 +125);
    const char *line1 = "The green sqaure is the starting sqaure and the red square denotes exit.";
    for (int i = 0; line1[i] != '\0'; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, line1[i]);
    }

    glRasterPos2f(window_width / 2 - 250, window_height / 2 +75);
    const char *line2 = "Direct the player sqaure (Blue) using the arrow keys in four directions: up, down, left, and right.";
    for (int i = 0; line2[i] != '\0'; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, line2[i]);
    }

    glRasterPos2f(window_width / 2 - 250, window_height / 2 +25);
    const char *line3 = "The maze walls block your movement. Find a path to reach the exit.";
    for (int i = 0; line3[i] != '\0'; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, line3[i]);
    }

    glRasterPos2f(window_width / 2 - 250, window_height / 2 -25);
    const char *line4 = "Choose the complexity level from the option menu. Harder the level,larger is the maze size!";
    for (int i = 0; line4[i] != '\0'; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, line4[i]);
    }

    glRasterPos2f(window_width / 2 - 250, window_height / 2 -75);
    const char *line5 = "Reach exit in as much less time as you can!";
    for (int i = 0; line5[i] != '\0'; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, line5[i]);
    }

    glutSwapBuffers();
}

void winpage(){
	init();
	glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.5, 0.5, 0.5, 0.0);

	glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(2.25*window_width / 5, window_height/2);
    const char* startOption = "Congratulations!!! You have completed the maze.";
    for (int i = 0; startOption[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, startOption[i]);
    }

	glRasterPos2f(2.25*window_width / 5, window_height/2-50);
    char timing[50];
	sprintf(timing, "It took you %d seconds", elapsedTime);
	for (int i = 0; timing[i] != '\0'; i++) {
    	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, timing[i]);
	}
	glutSwapBuffers();
}

void initi(){
	initializeMaze();
	nextX = 1;
	nextY = 1;
	playerX = 1;
	playerY = 1;
	startTime = 0;
	elapsedTime = 0;
}

void handleOptionsClick(int x, int y) {
    if (x >= (2.29 * window_width / 5) && x <= 2.29 * window_width / 5 + 100) {
        if (y >= (window_height / 2 + 40) && y <= (window_height / 2 + 70)) {
            MAZE_WIDTH = 55;
            MAZE_HEIGHT = 35;
            states = 0;
            initi();
            // Switch back to the main menu
            currentMode = MODE_GAME_MENU;
            mainpage(); // Navigate back to the main page
        } else if (y >= (window_height / 2 - 10) && y <= (window_height / 2 + 20)) {
            MAZE_WIDTH = 51;
            MAZE_HEIGHT = 31;
            states = 0;
            initi();
            // Switch back to the main menu
            currentMode = MODE_GAME_MENU;
            mainpage(); // Navigate back to the main page
        } else if (y >= (window_height / 2 - 60) && y <= window_height / 2 - 30) {
            MAZE_WIDTH = 41;
            MAZE_HEIGHT = 25;
            states = 0;
            initi();
            // Switch back to the main menu
            currentMode = MODE_GAME_MENU;
            mainpage(); // Navigate back to the main page
        }
    }
}

void handleMenuClick(int x, int y) {
    if (x >= window_width / 2 - 100 && x <= window_width / 2 + 100) {
        if (y <= window_height / 2 + 75 && y >= window_height / 2 +35) {
            exit(0);

        } else if (y <= window_height / 2 + 25 && y >= window_height / 2 - 15) {
        	states = 2; // option clicked
            glutPostRedisplay();
        }
		else if (y <= window_height / 2 - 25 && y >= window_height / 2 - 65) {
            states = 3; // Instruction option clicked
            glutPostRedisplay(); // Redraw the window to switch to the instruction mode

        }
		else if (y <= window_height / 2 - 75 && y >= window_height / 2 - 105) {
            bar =1;
			states = 1; // New Game option clicked
            initi();
            glutPostRedisplay(); // Redraw the window to switch to the game mode
        }
    }
}

// Function to handle arrow key input
void specialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_DOWN:
        if (maze[playerY - 1][playerX] != WALL)
            playerY--;
        break;
    case GLUT_KEY_UP:
        if (maze[playerY + 1][playerX] != WALL)
            playerY++;
        break;
    case GLUT_KEY_LEFT:
        if (maze[playerY][playerX - 1] != WALL)
            playerX--;
        break;
    case GLUT_KEY_RIGHT:
        if (maze[playerY][playerX + 1] != WALL)
            playerX++;
        break;
    }
    glutPostRedisplay();

    // Check if the player reached the endpoint
    if (playerX == endX && playerY == endY) {
        states =4;
    }
}

void handleEscapeKey(unsigned char key, int x, int y) {
    if (key == 27) { // 27 is the ASCII code for ESC
        // Reset game state
        states = 0;
        // Switch back to the main menu
        currentMode = MODE_GAME_MENU;
        // Redraw the window
        glutPostRedisplay();
    }
}

void mouseCallback (int button, int state, int x, int y){
	if (button == GLUT_LEFT_BUTTON &&state == GLUT_DOWN){
		switch (currentMode){
			case MODE_OPTIONS:
				handleOptionsClick(x, y);
				break;
			case MODE_GAME_MENU:
				handleMenuClick(x,y);
				break;
			default:
				break;
		}
	}
}

void display() {
    if (states == 0) {
    	currentMode=MODE_GAME_MENU;
    	init();
        mainpage();
    } else if (states == 1) {
        drawMaze();
        generateMaze(0);
    }
    else if (states == 2 &&bar == 0){
    	currentMode = MODE_OPTIONS;
    	optionpage();
	}
	else if (states == 3 && bar == 0){
		instructionpage();
	}
	else if (states == 4){
		glutSwapBuffers();
		winpage();
	}
}

void myreshape(int w, int h) {
    window_width = w; // Update global window width
    window_height = h; // Update global window height

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLfloat)w, 0.0, (GLfloat)h); // Set the orthographic projection to cover the entire window
    glMatrixMode(GL_MODELVIEW);
    glutPostRedisplay();
}

// Main function
int main(int argc, char** argv) {
    srand(time(NULL));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(window_width, window_height);
    glutCreateWindow("Maze Game");
    glutDisplayFunc(display);
    initializeMaze();
	nextX = 1;
    nextY = 1;
    glutMouseFunc(mouseCallback);
    glutSpecialFunc(specialKeys); // Register the specialKeys function for arrow key input
	glutKeyboardFunc(handleEscapeKey);
	init();
sndPlaySound("littleroot town",SND_ASYNC | SND_LOOP);
    glutMainLoop();
    return 0;
}
