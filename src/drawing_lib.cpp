#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../include/drawing_lib.h"

GLFWwindow *DrawingLib::createWindow() const {
    return glfwCreateWindow(window_width_, window_height_, "OpenGL Project 3", nullptr, nullptr);
}

void DrawingLib::getWindowSize(GLFWwindow *window) {
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    window_width_  = w;
    window_height_ = h;
}

void DrawingLib::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {

        if (GLFW_KEY_1 <= key <= GLFW_KEY_3){
            scene_->createObjectSet(key);
        }
        if (key == GLFW_KEY_4){
            scene_->addFlowers();
        }

        if (key == GLFW_KEY_LEFT)
        {
            camera_.move(-1,0);
        }
        if (key == GLFW_KEY_RIGHT)
        {
            camera_.move(1,0);
        }
        if (key == GLFW_KEY_UP)
        {
            camera_.move(0,-1);

        }
        if (key == GLFW_KEY_DOWN)
        {
            camera_.move(0,1);
        }
        if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL){
            ctrl_key_down_ = true;
        }

    }
    if (action == GLFW_RELEASE){
        if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL){
            ctrl_key_down_ = false;
        }
    }

}

void DrawingLib::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        glfwGetCursorPos(window, &cursor_pos_x_, &cursor_pos_y_);
        left_button_down_ = true;
        get_color_ = true;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        left_button_down_   = false;
        selected_object_id_ = -1;
    }

}

void DrawingLib::cursorPositionCallback(GLFWwindow* window,
                                        double input_cursor_pos_x,
                                        double input_cursor_pos_y)
{
    prev_pos_x_    = current_pos_x_;
    prev_pos_y_    = current_pos_y_;
    current_pos_x_ = input_cursor_pos_x;
    current_pos_y_ = input_cursor_pos_y;
    auto delta_coordinates = calculateCoordinatesOnMouseMove();

    if (left_button_down_ && selected_object_id_ < 0)
    {
        camera_.rotate((current_pos_x_ - prev_pos_x_), (current_pos_y_ - prev_pos_y_));
    }
    if (left_button_down_ && selected_object_id_ >= 0)
    {
        auto angles = camera_.angles();
        float yaw = std::get<0>(angles);
        double delta_x, delta_y;

        if (std::abs(std::cos(yaw)) >= 0.5){
            delta_x = std::get<1>(delta_coordinates);
            delta_y = std::get<0>(delta_coordinates);
            if (cos(yaw) > 0){
                delta_x *= -1;
            }
            else{
                delta_y *= -1;
            }
        }
        else{
            delta_x = std::get<0>(delta_coordinates);
            delta_y = std::get<1>(delta_coordinates);
            if (sin(yaw) >= 0){
                delta_x *= -1;
                delta_y *= -1;
            }
        }
        if (ctrl_key_down_){
            scene_-> moveObject(selected_object_id_, delta_x, -std::get<1>(delta_coordinates), 0);
        }
        else{
            scene_-> moveObject(selected_object_id_, delta_x, 0, delta_y);

        }
    }
}

void DrawingLib::defineCallbackFunction(GLFWwindow* window)
/** Sets up callback functions for a GLFW window.*/
{
    glfwSetWindowUserPointer(window, this);

    glfwSetMouseButtonCallback(window, [](GLFWwindow* win, int button, int action, int mods) {
        auto* drawing_lib = static_cast<DrawingLib*>(glfwGetWindowUserPointer(win));
        drawing_lib->mouseButtonCallback(win, button, action, mods);
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow* win, double xpos, double ypos) {
        auto* drawing_lib = static_cast<DrawingLib*>(glfwGetWindowUserPointer(win));
        drawing_lib->cursorPositionCallback(win, xpos, ypos);
    });

    glfwSetKeyCallback(window, [](GLFWwindow* win, int key, int scancode, int action, int mods) {
        auto* drawing_lib = static_cast<DrawingLib*>(glfwGetWindowUserPointer(win));
        drawing_lib->keyCallback(win, key, scancode, action, mods);
    });
    glfwSetScrollCallback(window, [](GLFWwindow* win, double xoffset, double yoffset) {
        auto* drawing_lib = static_cast<DrawingLib*>(glfwGetWindowUserPointer(win));
        drawing_lib->scrollCallback(win, yoffset);
    });
}

void DrawingLib::drawScene(GLFWwindow *window) {
    glViewport(0, 0, window_width_, window_height_);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glm::mat4 projection_mat = camera_.getProjectionMatrix(window_width_, window_height_);
    glm::mat4 view_mat = camera_.getViewMatrix();
    auto& camera_position = camera_.cameraPosition();

    if (get_color_)
    {
        double x_coord, y_coord;
        unsigned char color[3] = {0};
        scene_->draw(view_mat, projection_mat, camera_position,get_color_);
        glfwGetCursorPos(window, &x_coord, &y_coord);

        glReadPixels(x_coord, window_height_ - y_coord, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, color);

        selected_object_id_ = scene_->getObjectIdByPickColor(color);
    }
    else
    {
        scene_->draw(view_mat, projection_mat, camera_position, get_color_);
    }


    if (!get_color_)
    {
        glfwSwapBuffers(window);
    }
    get_color_ = false;
    glfwPollEvents();
}

void DrawingLib::scrollCallback(GLFWwindow* window, double yoffset) {
    if (selected_object_id_ >= 0) {
        scene_->scaleObject(selected_object_id_, float(yoffset*0.01));
    }
    else {
        camera_.zoom(float(yoffset));
    }
}



std::tuple<double, double> DrawingLib::calculateCoordinatesOnMouseMove() const
/** Calculates the change in object coordinates of mouse cursor, converting screen space coordinates to
 normalized device coordinates (NDC) and then to frustum coordinates, with depth correction applied. */
{
    // Convert the mouse movement (delta_x and delta_y) from screen space to normalized device coordinates (NDC)
    // where the range is [-1, 1].
    double ndc_delta_x = (current_pos_x_ - prev_pos_x_) / (window_width_ / 2.0f);
    double ndc_delta_y = (current_pos_y_ - prev_pos_y_) / (window_height_ / 2.0f);

    // Compute the width and height of the frustum at the near plane
    float dim_ratio = static_cast<float>(window_height_) / static_cast<float>(window_width_);
    double frustum_width = 1 - (-1);
    double frustum_height = (1 * dim_ratio) - (-1 * dim_ratio);

    // Scale normalized deltas to match the frustum coordinates
    double adjusted_delta_x = ndc_delta_x * frustum_width / 2.0f;
    double adjusted_delta_y = ndc_delta_y * frustum_height / 2.0f;

    // Apply depth correction factor (assuming linear scaling for simplicity)
    adjusted_delta_x *= correction_;
    adjusted_delta_y *= correction_;

    std::tuple<double, double> delta_coordinates = std::make_tuple(adjusted_delta_x, adjusted_delta_y);

    return delta_coordinates;
}
