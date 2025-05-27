#include <GL/glut.h>
#include <string>
#include <vector>
#include <cmath>

// Car positions and speeds
float car1X = -60, car2X = -180, car3X = 860;
float carSpeed1 = 2.0f, carSpeed2 = 1.0f, carSpeed3 = 0.5f;

enum LightState { RED, GREEN };
LightState lightBottom = GREEN, lightTop = RED;
bool isDay = true;
int frameCounter = 0, lightTimer = 30, dayNightCounter = 0;

std::vector<float> moreCarsBottom = {-300, -500};
std::vector<float> moreCarsTop = {960, 1100};

// App State
enum ScreenState { MENU, INSTRUCTIONS, SIMULATION };
ScreenState currentScreen = MENU;

// Utility: Draw text
void drawText(float x, float y, const std::string& str, void* font = GLUT_BITMAP_HELVETICA_18) {
    glRasterPos2f(x, y);
    for (char c : str) glutBitmapCharacter(font, c);
}

// ========== BUTTONS ========== //
struct Button {
    float x, y, width, height;
    std::string label;
};

std::vector<Button> menuButtons = {
    {300, 350, 200, 50, "Start Simulation"},
    {300, 270, 200, 50, "Instructions"},
    {300, 190, 200, 50, "Exit"}
};

// ========== SCREENS ========== //
void drawMenuScreen() {
    glClearColor(0.3f, 0.6f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    drawText(320, 460, "Smart Traffic Simulation", GLUT_BITMAP_TIMES_ROMAN_24);

    for (const auto& btn : menuButtons) {
        glColor3f(0.2f, 0.2f, 0.2f);
        glBegin(GL_QUADS);
        glVertex2f(btn.x, btn.y);
        glVertex2f(btn.x + btn.width, btn.y);
        glVertex2f(btn.x + btn.width, btn.y + btn.height);
        glVertex2f(btn.x, btn.y + btn.height);
        glEnd();

        glColor3f(1, 1, 1);
        drawText(btn.x + 20, btn.y + 18, btn.label);
    }

    glutSwapBuffers();
}

void drawInstructionsScreen() {
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    drawText(280, 500, "Instructions", GLUT_BITMAP_TIMES_ROMAN_24);

    drawText(150, 440, "- Press 'G' to turn bottom light GREEN");
    drawText(150, 410, "- Press 'R' to turn bottom light RED");
    drawText(150, 380, "- Press 'D' for Day mode, 'N' for Night mode");
    drawText(150, 350, "- Day/Night switches every 30s automatically");
    drawText(150, 300, "- Press 'M' to return to the Menu");

    glutSwapBuffers();
}

// ========== SIMULATION CODE ========== //
void drawUrbanBackground() {
    if (isDay) glClearColor(0.8f, 0.9f, 1.0f, 1.0f);
    else glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0.7f, 0.7f, 0.8f); glBegin(GL_QUADS);
    glVertex2f(50, 500); glVertex2f(150, 500); glVertex2f(150, 600); glVertex2f(50, 600); glEnd();

    glColor3f(0.5f, 0.5f, 0.6f); glBegin(GL_QUADS);
    glVertex2f(650, 450); glVertex2f(750, 450); glVertex2f(750, 600); glVertex2f(650, 600); glEnd();
}

void drawRoad() {
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS); glVertex2f(0, 150); glVertex2f(800, 150); glVertex2f(800, 350); glVertex2f(0, 350); glEnd();

    glColor3f(1, 1, 1);
    for (int i = 0; i < 800; i += 80)
        glBegin(GL_QUADS), glVertex2f(i + 10, 245), glVertex2f(i + 40, 245), glVertex2f(i + 40, 255), glVertex2f(i + 10, 255), glEnd();
}

void drawCar(float x, float y, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS); glVertex2f(x, y); glVertex2f(x + 50, y); glVertex2f(x + 50, y + 20); glVertex2f(x, y + 20); glEnd();

    glColor3f(0, 0, 0);
    for (int i = 0; i < 30; ++i) {
        float t = i * 2 * 3.14159f / 30;
        glBegin(GL_POINTS);
        glVertex2f(x + 10 + cos(t) * 4, y - 4 + sin(t) * 4);
        glVertex2f(x + 40 + cos(t) * 4, y - 4 + sin(t) * 4);
        glEnd();
    }
}

void drawTrafficLight(float x, float y, LightState state) {
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_QUADS); glVertex2f(x, y); glVertex2f(x + 20, y); glVertex2f(x + 20, y + 60); glVertex2f(x, y + 60); glEnd();

    glColor3f(state == RED ? 1 : 0.2f, 0, 0);
    glBegin(GL_POLYGON); for (int i = 0; i < 30; ++i) {
        float t = i * 2 * 3.14159f / 30;
        glVertex2f(x + 10 + cos(t) * 6, y + 50 + sin(t) * 6);
    } glEnd();

    glColor3f(0, state == GREEN ? 1 : 0.2f, 0);
    glBegin(GL_POLYGON); for (int i = 0; i < 30; ++i) {
        float t = i * 2 * 3.14159f / 30;
        glVertex2f(x + 10 + cos(t) * 6, y + 10 + sin(t) * 6);
    } glEnd();
}

void drawTimerCountdown() {
    glColor3f(1, 1, 1);
    drawText(20, 560, "Signal Change In: " + std::to_string(lightTimer) + "s");
}

void drawSimulationScreen() {
    drawUrbanBackground();
    drawRoad();
    drawTrafficLight(390, 120, lightBottom);
    drawTrafficLight(390, 360, lightTop);
    drawTimerCountdown();

    drawCar(car1X, 170, 0.8f, 0.1f, 0.1f);
    drawCar(car2X, 210, 1.0f, 0.3f, 0.3f);
    drawCar(car3X, 290, 0.1f, 0.1f, 0.9f);

    for (float x : moreCarsBottom) drawCar(x, 210, 1.0f, 0.3f, 0.3f);
    for (float x : moreCarsTop) drawCar(x, 290, 0.1f, 0.1f, 0.9f);

    glutSwapBuffers();
}

// ========== LOGIC ========== //
void updateLights() {
    frameCounter++;
    if (frameCounter >= 60) {
        frameCounter = 0;
        lightTimer--;
        dayNightCounter++;

        if (lightTimer <= 0) {
            lightBottom = (lightBottom == GREEN) ? RED : GREEN;
            lightTop = (lightTop == GREEN) ? RED : GREEN;
            lightTimer = 30;
        }

        if (dayNightCounter >= 30) {
            isDay = !isDay;
            dayNightCounter = 0;
        }
    }
}

void timer(int) {
    if (currentScreen == SIMULATION) {
        if (!(lightBottom == RED && car1X + 50 >= 390 && car1X <= 440)) car1X += carSpeed1;
        if (!(lightBottom == RED && car2X + 50 >= 390 && car2X <= 440)) car2X += carSpeed2;
        if (!(lightTop == RED && car3X >= 370 && car3X <= 420)) car3X -= carSpeed3;

        if (car1X > 860) car1X = -60;
        if (car2X > 860) car2X = -180;
        if (car3X < -60) car3X = 860;

        for (size_t i = 0; i < moreCarsBottom.size(); ++i) {
            if (!(lightBottom == RED && moreCarsBottom[i] + 50 >= 390 && moreCarsBottom[i] <= 440))
                moreCarsBottom[i] += 1.0f;
            if (moreCarsBottom[i] > 860) moreCarsBottom[i] = -200;
        }

        for (size_t i = 0; i < moreCarsTop.size(); ++i) {
            if (!(lightTop == RED && moreCarsTop[i] >= 370 && moreCarsTop[i] <= 420))
                moreCarsTop[i] -= 0.5f;
            if (moreCarsTop[i] < -60) moreCarsTop[i] = 960;
        }

        updateLights();
    }

    glutPostRedisplay();
    glutTimerFunc(1000 / 60, timer, 0);
}

void display() {
    switch (currentScreen) {
        case MENU: drawMenuScreen(); break;
        case INSTRUCTIONS: drawInstructionsScreen(); break;
        case SIMULATION: drawSimulationScreen(); break;
    }
}

void mouseClick(int button, int state, int x, int y) {
    if (currentScreen == MENU && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        int windowY = 600 - y;
        for (const auto& btn : menuButtons) {
            if (x >= btn.x && x <= btn.x + btn.width && windowY >= btn.y && windowY <= btn.y + btn.height) {
                if (btn.label == "Start Simulation") currentScreen = SIMULATION;
                else if (btn.label == "Instructions") currentScreen = INSTRUCTIONS;
                else if (btn.label == "Exit") exit(0);
            }
        }
    }
}

void keyPress(unsigned char key, int x, int y) {
    if (key == 'g' || key == 'G') lightBottom = GREEN, lightTop = RED, lightTimer = 30;
    else if (key == 'r' || key == 'R') lightBottom = RED, lightTop = GREEN, lightTimer = 30;
    else if (key == 'd' || key == 'D') isDay = true;
    else if (key == 'n' || key == 'N') isDay = false;
    else if (key == 'm' || key == 'M') currentScreen = MENU;
}

void init() {
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, 800, 0, 600);
}

// ========== MAIN ========== //
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Urban Traffic Simulation - Menu");
    init();
    glutDisplayFunc(display);
    glutMouseFunc(mouseClick);
    glutKeyboardFunc(keyPress);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}
