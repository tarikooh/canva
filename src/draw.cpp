#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <cmath>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <ctime>
#include <string>



struct Point {
    float x, y;
};

struct Stroke {
    std::vector<Point> points;
    float r, g, b;
    float size;
};

std::vector<Stroke> strokes;
Stroke currentStroke;

bool drawing = false;

// Predefined colors
std::vector<std::tuple<float, float, float>> colorPalette = {
    {1.0f, 0.0f, 0.0f},  // red
    {0.0f, 1.0f, 0.0f},  // green
    {0.0f, 0.0f, 1.0f},  // blue
    {1.0f, 1.0f, 0.0f},   // yellow
    {0.0f, 0.0f, 0.0f},  // black
    {1.0f, 0.5f, 0.0f},  // Orange
    {0.5f, 0.0f, 1.0f},  // Purple
    {1.0f, 1.0f, 1.0f},  // white
};
int currentColorIndex = 0;
float currentBrushSize = 3.0f;


std::string generate_timestamped_filename() {
    std::time_t now = std::time(nullptr);
    std::tm* t = std::localtime(&now);
    std::string filename = "drawing_" +
    std::to_string(t->tm_year + 1900) + "-" +
    std::to_string(t->tm_mon + 1) + "-" +
    std::to_string(t->tm_mday) + "_" +
    std::to_string(t->tm_hour) + "-" +
    std::to_string(t->tm_min) + "-" +
    std::to_string(t->tm_sec) + ".png";
    
    return filename;
}

void save_screenshot(const char* filename, int width, int height) {
    std::vector<unsigned char> pixels(width * height * 3);
    
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // flip kalhone ayseram
    for (int y = 0; y < height / 2; ++y) {
        for (int x = 0; x < width * 3; ++x) {
            std::swap(pixels[y * width * 3 + x], pixels[(height - 1 - y) * width * 3 + x]);
        }
    }

    stbi_write_png(filename, width, height, 3, pixels.data(), width * 3);
}

void draw_color_preview(float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, 1.0f);
    glVertex2f(-0.95f, 1.0f);
    glVertex2f(-0.95f, 0.95f);
    glVertex2f(-1.0f, 0.95f);
    glEnd();
}

void addInterpolatedPoints(Point a, Point b, std::vector<Point>& out) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    float dist = std::sqrt(dx * dx + dy * dy);
    int steps = static_cast<int>(dist / 0.01f);
    
    for (int i = 1; i < steps; ++i) {
        float t = static_cast<float>(i) / steps;
        out.push_back({a.x + t * dx, a.y + t * dy});
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (!drawing) return;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    float normX = (2.0f * xpos) / width - 1.0f;
    float normY = 1.0f - (2.0f * ypos) / height;

    
    Point newPoint = {normX, normY};
    if (!currentStroke.points.empty()) {
        addInterpolatedPoints(currentStroke.points.back(), newPoint, currentStroke.points);
    }
    currentStroke.points.push_back(newPoint);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            drawing = true;
            currentStroke = Stroke{};
            currentStroke.r = std::get<0>(colorPalette[currentColorIndex]);
            currentStroke.g = std::get<1>(colorPalette[currentColorIndex]);
            currentStroke.b = std::get<2>(colorPalette[currentColorIndex]);
            currentStroke.size = currentBrushSize;
        } else if (action == GLFW_RELEASE) {
            drawing = false;
            if (!currentStroke.points.empty())
                strokes.push_back(currentStroke);
        }
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS) return;

    switch (key) {
        case GLFW_KEY_C:
            currentColorIndex = (currentColorIndex + 1) % colorPalette.size();
            break;
        case GLFW_KEY_E:
	    currentColorIndex = 7;
            break;
        case GLFW_KEY_J:{
	    currentBrushSize = std::max(1.0f, currentBrushSize - 1.0f);
	    //	    std::cout<<"Brush size: "<<currentBrushSize<<std::endl;
            break;}
        case GLFW_KEY_K:
            currentBrushSize = std::min(50.0f, currentBrushSize + 1.0f);
	    // std::cout<<"Brush size: "<<currentBrushSize<<std::endl;
            break;
        case GLFW_KEY_R:
            strokes.clear();
            break;
       case GLFW_KEY_S:{
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
	    std::string filename = generate_timestamped_filename();
            save_screenshot(filename.c_str(), width, height);
            break;}
        case GLFW_KEY_Q:
	    glfwSetWindowShouldClose(window, true);
	    break;
    }
}

void draw_background(float r, float g, float b) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f( 1.0f, -1.0f);
    glVertex2f( 1.0f,  1.0f);
    glVertex2f(-1.0f,  1.0f);
    glEnd();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(800, 600, "Paint on Canvas", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetKeyCallback(window, key_callback);

    glLineWidth(2.0f);
    glPointSize(3.0f);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
 
    while (!glfwWindowShouldClose(window)) {
      glClear(GL_COLOR_BUFFER_BIT);
      draw_background(1.0f, 1.0f, 1.0f);

        for (const auto& stroke : strokes) {
            glColor3f(stroke.r, stroke.g, stroke.b);
            glPointSize(stroke.size);

	    glLineWidth(stroke.size);
	    glBegin(GL_LINE_STRIP);
	    for (const auto& p : stroke.points) {
	      glVertex2f(p.x, p.y);
	    }
	    glEnd();

	    glPointSize(stroke.size);
	    glBegin(GL_POINTS);
	    for (const auto& p : stroke.points) {
	      glVertex2f(p.x, p.y);
	    }
	    glEnd();

        }

        if (drawing) {
            glColor3f(currentStroke.r, currentStroke.g, currentStroke.b);
            glPointSize(currentStroke.size);

	    glLineWidth(1.0f);
	    glBegin(GL_LINE_STRIP);
	    for (const auto& p : currentStroke.points) {
	      glVertex2f(p.x, p.y);
	    }
	    glEnd();

	    glPointSize(currentStroke.size);
	    glBegin(GL_POINTS);
	    for (const auto& p : currentStroke.points) {
	      glVertex2f(p.x, p.y);
	    }
	    glEnd();
	    
        }
	auto& color = colorPalette[currentColorIndex];
	draw_color_preview(std::get<0>(color), std::get<1>(color), std::get<2>(color));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
