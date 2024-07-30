#ifndef PROJECT_5_DRAWING_LIB_H
#define PROJECT_5_DRAWING_LIB_H

#include <GLFW/glfw3.h>

#include "../include/object.h"
#include "../include/scene.h"
#include "../include/camera.h"


class DrawingLib{
public:
    DrawingLib() = default;
    GLFWwindow* createWindow() const;
    void getWindowSize(GLFWwindow* window);

    void drawScene(GLFWwindow* window);
    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
    void scrollCallback(GLFWwindow* window, double yoffset);
    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);


    void defineCallbackFunction(GLFWwindow* window);
    void defineScene(Scene& scene){scene_ = &scene;};


private:

    Scene* scene_;
    int window_width_{1920};
    int window_height_{1080};

    bool get_color_{false};
    int selected_object_id_{-1};

    Camera camera_ = Camera(glm::vec3(0.0f, 5.0f, 5.0),glm::vec3(0.0f, 4.0f, 0.0f),  glm::vec3(0.0f, 1.0f, 0.0f));

    double cursor_pos_x_{0}, cursor_pos_y_{0};
    double current_pos_x_{0}, current_pos_y_{0}, prev_pos_x_{0}, prev_pos_y_{0};
    bool left_button_down_{false};
    bool ctrl_key_down_{false};
    double correction_{10.0};

    glm::vec3 camera_position_= glm::vec3(0.0f, 1.0f, 10.0);
    glm::vec3 target_position_ = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 up_direction_ = glm::vec3(0.0f, 1.0f, 0.0f);

    std::tuple<double, double> calculateCoordinatesOnMouseMove() const;
};

#endif //PROJECT_5_DRAWING_LIB_H
