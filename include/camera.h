
#ifndef PROJECT_5_CAMERA_H
#define PROJECT_5_CAMERA_H

#include <glm/glm.hpp>
#include <tuple>

class Camera{
public:
    Camera(glm::vec3 camera_position, glm::vec3 target_position, glm::vec3 up_direction);
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix(float window_width, float window_height) const;
    void move(float delta_x, float delta_z);
    void zoom(float yoffset);
    void rotate(float delta_x=0, float delta_y=0, float delta_z = 0);
    glm::vec3& cameraPosition(){return camera_position_;}

    std::tuple<float, float> angles(){return std::make_tuple(yaw_, pitch_);}


private:
    glm::mat4 view_          = glm::mat4(1.0f);

    glm::vec3 camera_position_{};
    glm::vec3 target_position_{};
    glm::vec3 up_direction_{};

    float near_{0.1};
    float far_{100};
    float fov_{75};

    float yaw_   = glm::radians(-90.0f);
    float pitch_ = glm::radians(0.0f);

    float mouse_sensitivity_ = 0.01;

    void updateCameraVectors();
};

#endif //PROJECT_5_CAMERA_H
