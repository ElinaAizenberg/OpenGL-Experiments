#include "../include/camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>


Camera::Camera(glm::vec3 camera_position, glm::vec3 target_position, glm::vec3 up_direction) {
    camera_position_ = camera_position;
    target_position_ = target_position;
    up_direction_ = up_direction;

   // updateCameraVectors();

}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(camera_position_, target_position_, up_direction_);
}

glm::mat4 Camera::getProjectionMatrix(float window_width, float window_height) const {
    return glm::perspective(glm::radians(fov_), static_cast<float>(window_width) / static_cast<float>(window_height), near_, far_);
}

void Camera::zoom(float yoffset) {
    fov_ -= yoffset;
    if (fov_ < 10.0f)
        fov_ = 10.0f;
    if (fov_ > 100.0f)
        fov_ = 100.0f;
}
void Camera::move(float delta_x, float delta_z) {
    if (std::abs(cos(yaw_)) >= 0.5){

        if (cos(yaw_) < 0){
            delta_x *= -1;
        }
        else{
            delta_z *= -1;
        }
        camera_position_.x += delta_z;
        target_position_.x += delta_z;

        camera_position_.z += delta_x;
        target_position_.z += delta_x;
    }
    else{
        if (sin(yaw_) >= 0){
            delta_x *= -1;
            delta_z *= -1;
        }
        camera_position_.x += delta_x;
        target_position_.x += delta_x;

        camera_position_.z += delta_z;
        target_position_.z += delta_z;
    }

}


void Camera::rotate(float delta_x, float delta_y, float delta_z) {
    yaw_ -= (delta_x*mouse_sensitivity_);
    pitch_ += (delta_y*mouse_sensitivity_);

    // Limit the pitch angle to avoid flipping
    if (pitch_ > glm::radians(89.0)) {
        pitch_ = glm::radians(89.0);
    }
    if (pitch_ < glm::radians(-89.0)) {
        pitch_ = glm::radians(-89.0);
    }

    updateCameraVectors();
}

void Camera::updateCameraVectors()
{
    // Compute the camera's front direction
    glm::vec3 front;
    front.x = cos(yaw_) * cos(pitch_);
    front.y = sin(pitch_);
    front.z = sin(yaw_) * cos(pitch_);
    front = glm::normalize(front);

    // Compute the right direction
    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));

    // Compute the new up vector with the roll angle
    up_direction_ = glm::normalize(glm::cross(right, front));

    target_position_ = camera_position_ +front;



}
