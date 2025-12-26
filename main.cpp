#include <iostream>
#include <array>
#include <random>
#include <vector>
#include <algorithm>
#include <cstdint>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/* ============================================================
   Utility
   ============================================================ */

short randomShort(short min, short max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<short> dist(min, max);
    return dist(gen);
}

/* ============================================================
   Cube model
   ============================================================ */

enum class Color {
    WHITE,
    YELLOW,
    RED,
    ORANGE,
    BLUE,
    GREEN
};

enum Face {
    UP = 0,
    DOWN,
    LEFT,
    RIGHT,
    FRONT,
    BACK
};

enum class Move {
    U, U_PRIME, U2,
    D, D_PRIME, D2,
    L, L_PRIME, L2,
    R, R_PRIME, R2,
    F, F_PRIME, F2,
    B, B_PRIME, B2
};

glm::vec3 colorToRGB(Color c) {
    switch (c) {
        case Color::WHITE: return {1, 1, 1};
        case Color::YELLOW: return {1, 1, 0};
        case Color::RED: return {1, 0, 0};
        case Color::ORANGE: return {1, 0.5f, 0};
        case Color::BLUE: return {0, 0, 1};
        case Color::GREEN: return {0, 1, 0};
    }
    return {0, 0, 0};
}

class Cube {
public:
    Color faces[6][9];

    Cube() {
        short cnt[6] = {0};
        for (int f = 0; f < 6; ++f) {
            for (int i = 0; i < 9; ++i) {
                short c = randomShort(0, 5);
                while (cnt[c] == 9)
                    c = randomShort(0, 5);
                faces[f][i] = static_cast<Color>(c);
                cnt[c]++;
            }
        }
    }

    void applyMove(Move m) {
        auto rotateFaceCW = [](Color f[9]) {
            Color tmp[9];
            tmp[0] = f[6];
            tmp[1] = f[3];
            tmp[2] = f[0];
            tmp[3] = f[7];
            tmp[4] = f[4];
            tmp[5] = f[1];
            tmp[6] = f[8];
            tmp[7] = f[5];
            tmp[8] = f[2];
            for (int i = 0; i < 9; i++) f[i] = tmp[i];
        };

        auto rotateFaceCCW = [](Color f[9]) {
            Color tmp[9];
            tmp[0] = f[2];
            tmp[1] = f[5];
            tmp[2] = f[8];
            tmp[3] = f[1];
            tmp[4] = f[4];
            tmp[5] = f[7];
            tmp[6] = f[0];
            tmp[7] = f[3];
            tmp[8] = f[6];
            for (int i = 0; i < 9; i++) f[i] = tmp[i];
        };

        auto swap3 = [this](int aFace, int a0, int a1, int a2,
                            int bFace, int b0, int b1, int b2,
                            int cFace, int c0, int c1, int c2,
                            int dFace, int d0, int d1, int d2) {
            Color tmp[3] = {faces[aFace][a0], faces[aFace][a1], faces[aFace][a2]};
            faces[aFace][a0] = faces[dFace][d0];
            faces[aFace][a1] = faces[dFace][d1];
            faces[aFace][a2] = faces[dFace][d2];
            faces[dFace][d0] = faces[cFace][c0];
            faces[dFace][d1] = faces[cFace][c1];
            faces[dFace][d2] = faces[cFace][c2];
            faces[cFace][c0] = faces[bFace][b0];
            faces[cFace][c1] = faces[bFace][b1];
            faces[cFace][c2] = faces[bFace][b2];
            faces[bFace][b0] = tmp[0];
            faces[bFace][b1] = tmp[1];
            faces[bFace][b2] = tmp[2];
        };

        switch (m) {
            case Move::U: rotateFaceCW(faces[UP]);
                swap3(FRONT, 0, 1, 2, RIGHT, 0, 1, 2, BACK, 0, 1, 2, LEFT, 0, 1, 2);
                break;
            case Move::U_PRIME: rotateFaceCCW(faces[UP]);
                swap3(FRONT, 0, 1, 2, LEFT, 0, 1, 2, BACK, 0, 1, 2, RIGHT, 0, 1, 2);
                break;
            case Move::U2: applyMove(Move::U);
                applyMove(Move::U);
                break;

            case Move::D: rotateFaceCW(faces[DOWN]);
                swap3(FRONT, 6, 7, 8, LEFT, 6, 7, 8, BACK, 6, 7, 8, RIGHT, 6, 7, 8);
                break;
            case Move::D_PRIME: rotateFaceCCW(faces[DOWN]);
                swap3(FRONT, 6, 7, 8, RIGHT, 6, 7, 8, BACK, 6, 7, 8, LEFT, 6, 7, 8);
                break;
            case Move::D2: applyMove(Move::D);
                applyMove(Move::D);
                break;

            case Move::F: rotateFaceCW(faces[FRONT]);
                swap3(UP, 6, 7, 8, LEFT, 8, 5, 2, DOWN, 2, 1, 0, RIGHT, 0, 3, 6);
                break;
            case Move::F_PRIME: rotateFaceCCW(faces[FRONT]);
                swap3(UP, 6, 7, 8, RIGHT, 0, 3, 6, DOWN, 2, 1, 0, LEFT, 8, 5, 2);
                break;
            case Move::F2: applyMove(Move::F);
                applyMove(Move::F);
                break;

            case Move::B: rotateFaceCW(faces[BACK]);
                swap3(UP, 0, 1, 2, RIGHT, 2, 5, 8, DOWN, 8, 7, 6, LEFT, 6, 3, 0);
                break;
            case Move::B_PRIME: rotateFaceCCW(faces[BACK]);
                swap3(UP, 0, 1, 2, LEFT, 6, 3, 0, DOWN, 8, 7, 6, RIGHT, 2, 5, 8);
                break;
            case Move::B2: applyMove(Move::B);
                applyMove(Move::B);
                break;

            case Move::L: rotateFaceCW(faces[LEFT]);
                swap3(UP, 0, 3, 6, BACK, 8, 5, 2, DOWN, 0, 3, 6, FRONT, 0, 3, 6);
                break;
            case Move::L_PRIME: rotateFaceCCW(faces[LEFT]);
                swap3(UP, 0, 3, 6, FRONT, 0, 3, 6, DOWN, 0, 3, 6, BACK, 8, 5, 2);
                break;
            case Move::L2: applyMove(Move::L);
                applyMove(Move::L);
                break;

            case Move::R: rotateFaceCW(faces[RIGHT]);
                swap3(UP, 2, 5, 8, FRONT, 2, 5, 8, DOWN, 2, 5, 8, BACK, 6, 3, 0);
                break;
            case Move::R_PRIME: rotateFaceCCW(faces[RIGHT]);
                swap3(UP, 2, 5, 8, BACK, 6, 3, 0, DOWN, 2, 5, 8, FRONT, 2, 5, 8);
                break;
            case Move::R2: applyMove(Move::R);
                applyMove(Move::R);
                break;
        }
    }
};

/* ============================================================
   Heuristic
   ============================================================ */

uint64_t findClosestToPerfectSide(const Cube &cb) {
    uint64_t bestSide = 0, best = 0;
    for (uint64_t s = 0; s < 6; ++s) {
        uint64_t cnt[6] = {0};
        for (int i = 0; i < 9; ++i)
            cnt[static_cast<int>(cb.faces[s][i])]++;
        uint64_t mx = *std::max_element(cnt, cnt + 6);
        if (mx > best) {
            best = mx;
            bestSide = s;
        }
    }
    return bestSide;
}

uint64_t remainingPixels(uint64_t side, const Cube &cb) {
    uint64_t cnt[6] = {0};
    for (int i = 0; i < 9; ++i)
        cnt[static_cast<int>(cb.faces[side][i])]++;
    return 9 - *std::max_element(cnt, cnt + 6);
}

uint64_t h(const Cube &cb) {
    uint64_t t = remainingPixels(findClosestToPerfectSide(cb), cb);
    return t / 3 + t % 3;
}

/* ============================================================
   A* support
   ============================================================ */


uint64_t moveCost(Move m) {
    return (static_cast<int>(m) % 3 == 2) ? 2 : 1;
}

struct Player {
    uint64_t cost = 0;
};

struct Node {
    Cube cube;
    uint64_t gCost;
};

// Count how many stickers are in correct positions relative to center
uint64_t heuristicAllFaces(const Cube &cb) {
    uint64_t score = 0;

    // For each face, check if stickers match the center
    for (int face = 0; face < 6; ++face) {
        Color center = cb.faces[face][4]; // Center sticker
        for (int i = 0; i < 9; ++i) {
            if (i == 4) continue; // Skip center
            if (cb.faces[face][i] != center) {
                score++;
            }
        }
    }

    return score / 12; // Approximate moves needed (each move affects ~12 stickers)
}

struct AStarState {
    Cube cube;
    Player player;
    bool initialized = false;
};

// Modified f function
uint64_t f(const Cube &cb, uint64_t gCost) {
    return gCost + h(cb);
}

/* ============================================================
   One-step A* state + step
   ============================================================ */

void AstarStep(AStarState &state, const std::vector<Move> &moves) {
    std::vector<Node> frontier;
    frontier.reserve(moves.size());

    // Track best f value and all nodes with that value
    uint64_t bestF = UINT64_MAX;
    std::vector<uint64_t> bestIndices;

    for (uint64_t i = 0; i < moves.size(); ++i) {
        Move m = moves[i];
        Node n;
        n.cube = state.cube;
        n.cube.applyMove(m);
        n.gCost = state.player.cost + moveCost(m);

        uint64_t fi = f(n.cube, n.gCost);
        frontier.push_back(n);

        if (fi < bestF) {
            bestF = fi;
            bestIndices.clear();
            bestIndices.push_back(i);
        } else if (fi == bestF) {
            bestIndices.push_back(i);
        }
    }

    // Random tie-breaking instead of always picking first
    static std::random_device rd;
    static std::mt19937 gen(rd());

    if (!bestIndices.empty()) {
        std::uniform_int_distribution<> dist(0, bestIndices.size() - 1);
        uint64_t chosenIndex = bestIndices[dist(gen)];

        state.cube = frontier[chosenIndex].cube;
        state.player.cost = frontier[chosenIndex].gCost;

        // Print which move was chosen
        Move chosenMove = moves[chosenIndex];
        const char *moveNames[] = {
            "U", "U'", "U2", "D", "D'", "D2",
            "L", "L'", "L2", "R", "R'", "R2",
            "F", "F'", "F2", "B", "B'", "B2"
        };
        printf("Solver chose: %s (h=%lu, g=%lu)\n",
               moveNames[static_cast<int>(chosenMove)],
               heuristicAllFaces(state.cube),
               state.player.cost);
    }
}

/* ============================================================
   Input (mouse + solver step)
   ============================================================ */

float rotX = 25.f, rotY = -35.f;
double lastX = 0, lastY = 0;
bool dragging = false;
bool nextStepRequested = false;

void mouseButton(GLFWwindow *, int button, int action, int) {
    if (button == GLFW_MOUSE_BUTTON_LEFT)
        dragging = (action == GLFW_PRESS);
}

void mouseMove(GLFWwindow *, double x, double y) {
    if (!dragging) {
        lastX = x;
        lastY = y;
        return;
    }
    rotY += float(x - lastX) * 0.4f;
    rotX += float(y - lastY) * 0.4f;
    lastX = x;
    lastY = y;
}

void keyCallback(GLFWwindow *, int key, int, int action, int) {
    if (key == GLFW_KEY_N && action == GLFW_PRESS) {
        nextStepRequested = true;
        printf("N pressed\n");
    }
}

/* ============================================================
   OpenGL helpers
   ============================================================ */

GLuint compile(GLenum t, const char *src) {
    GLuint s = glCreateShader(t);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    return s;
}

GLuint makeProgram(const char *vs, const char *fs) {
    GLuint p = glCreateProgram();
    GLuint v = compile(GL_VERTEX_SHADER, vs);
    GLuint f = compile(GL_FRAGMENT_SHADER, fs);
    glAttachShader(p, v);
    glAttachShader(p, f);
    glLinkProgram(p);
    glDeleteShader(v);
    glDeleteShader(f);
    return p;
}

/* ============================================================
   Rendering
   ============================================================ */

float quad[] = {
    -0.45f, -0.45f, 0, 0.45f, -0.45f, 0, 0.45f, 0.45f, 0,
    0.45f, 0.45f, 0, -0.45f, 0.45f, 0, -0.45f, -0.45f, 0
};

void drawFace(const Cube &cube, Face face,
              GLuint vao, GLuint program,
              const glm::mat4 &proj,
              const glm::mat4 &view) {
    for (int i = 0; i < 9; ++i) {
        int r = i / 3, c = i % 3;
        glm::mat4 model(1);
        float s = 1.05f, d = 1.6f;

        if (face == FRONT) model = glm::translate(model, {(c - 1) * s, (1 - r) * s, d});
        if (face == BACK)
            model = glm::rotate(glm::translate(model, {(1 - c) * s, (1 - r) * s, -d}),
                                glm::radians(180.f), {0, 1, 0});
        if (face == UP)
            model = glm::rotate(glm::translate(model, {(c - 1) * s, d, (r - 1) * s}), glm::radians(-90.f),
                                {1, 0, 0});
        if (face == DOWN)
            model = glm::rotate(glm::translate(model, {(c - 1) * s, -d, (1 - r) * s}), glm::radians(90.f),
                                {1, 0, 0});
        if (face == RIGHT)
            model = glm::rotate(glm::translate(model, {d, (1 - r) * s, (1 - c) * s}),
                                glm::radians(-90.f), {0, 1, 0});
        if (face == LEFT)
            model = glm::rotate(glm::translate(model, {-d, (1 - r) * s, (c - 1) * s}), glm::radians(90.f),
                                {0, 1, 0});

        glm::mat4 mvp = proj * view * model;
        glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1,GL_FALSE, glm::value_ptr(mvp));

        glm::vec3 col = colorToRGB(cube.faces[face][i]);
        glUniform3fv(glGetUniformLocation(program, "color"), 1, glm::value_ptr(col));

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

/* ============================================================
   Main
   ============================================================ */

int main() {
    glfwInit();
    GLFWwindow *win = glfwCreateWindow(900, 700, "Rubik Cube + A*", nullptr, nullptr);
    glfwMakeContextCurrent(win);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    glfwSetMouseButtonCallback(win, mouseButton);
    glfwSetCursorPosCallback(win, mouseMove);
    glfwSetKeyCallback(win, keyCallback);

    glEnable(GL_DEPTH_TEST);

    const char *vs =
            "#version 330 core\n"
            "layout(location=0)in vec3 p;"
            "uniform mat4 mvp;"
            "void main(){gl_Position=mvp*vec4(p,1);}";

    const char *fs =
            "#version 330 core\n"
            "out vec4 f;"
            "uniform vec3 color;"
            "void main(){f=vec4(color,1);}";

    GLuint prog = makeProgram(vs, fs);

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad,GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3,GL_FLOAT,GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    Cube cube;
    AStarState solver;
    solver.cube = cube;

    std::vector<Move> moves = {
        Move::U, Move::U_PRIME, Move::U2,
        Move::D, Move::D_PRIME, Move::D2,
        Move::L, Move::L_PRIME, Move::L2,
        Move::R, Move::R_PRIME, Move::R2,
        Move::F, Move::F_PRIME, Move::F2,
        Move::B, Move::B_PRIME, Move::B2
    };

    while (!glfwWindowShouldClose(win)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (nextStepRequested) {
            AstarStep(solver, moves);
            cube = solver.cube;
            nextStepRequested = false;
        }

        glm::mat4 proj = glm::perspective(glm::radians(45.f), 900.f / 700.f, 0.1f, 100.f);
        glm::mat4 view = glm::translate(glm::mat4(1), {0, 0, -10});
        view = glm::rotate(view, glm::radians(rotX), {1, 0, 0});
        view = glm::rotate(view, glm::radians(rotY), {0, 1, 0});

        glUseProgram(prog);

        drawFace(cube, FRONT, vao, prog, proj, view);
        drawFace(cube, BACK, vao, prog, proj, view);
        drawFace(cube, UP, vao, prog, proj, view);
        drawFace(cube, DOWN, vao, prog, proj, view);
        drawFace(cube, LEFT, vao, prog, proj, view);
        drawFace(cube, RIGHT, vao, prog, proj, view);

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
