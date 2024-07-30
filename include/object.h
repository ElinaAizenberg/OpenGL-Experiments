#ifndef PROJECT_5_OBJECT_H
#define PROJECT_5_OBJECT_H

#include <string>
#include <utility>
#include <vector>

#include "../include/texture.h"
#include "../include/shader.h"

struct Material {
    glm::vec3 ambient = glm::vec3(1.000000 ,1.000000, 1.000000);
    glm::vec3 diffuse = glm::vec3(0.800000, 0.800000, 0.800000);
    glm::vec3 specular = glm::vec3(0.928571, 0.928571, 0.928571);
    float shininess{32.0f};
};


class Object{
public:
    Object(const std::string& obj_filepath, const std::string& shader_vert, const std::string& shader_frag, int pick_id=0, float pick_r=0, float pick_g=0, float pick_b=0, float scale = 1.0f, std::vector<float> rotate = {}, const std::vector<std::string>& textures = {});

    void loadObjectBuffers();
    void drawObject(glm::mat4& view, glm::mat4& projection, glm::vec3& camera_position, bool get_pick_color, bool reflection = false);


    bool checkPickColor(int pick_color_id) const;

    void move(float delta_x, float delta_y, float delta_z);
    void scale(float offset){scale_ += offset;}

private:

    std::vector<GLfloat> vertices_{};
    std::vector<GLfloat> normals_{};
    std::vector<GLuint> indices_{};
    std::vector<GLfloat> texture_coordinates_{};
    Material material_;

    GLuint VAO_{};
    GLuint VBO_{};

    int pick_id_;
    float pick_rgb_[3] = {0,0,0};
    float scale_;
    float xyz_[3] = {0,0,0};
    std::vector<float> rotate_xyz_{0,0,0};

    glm::vec3 translate_vec = glm::vec3(0, 0, 0);

    ShaderProgram shaderProgram_;

    std::vector<Texture2D> object_textures_;

    void loadObjectFile(const std::string& filepath);


};

class Skybox {
public:
    Skybox(const std::string& shader_vert, const std::string& shader_frag);
    void draw(glm::mat4 &view, glm::mat4 &projection);


private:
    GLuint VAO_{};
    GLuint VBO_{};
    ShaderProgram shaderProgram_;

    Texture3D skybox_texture_ = Texture3D({"/home/elina/MyProjects/skyboxes/sky/right.jpg",
                                           "/home/elina/MyProjects/skyboxes/sky/left.jpg",
                                           "/home/elina/MyProjects/skyboxes/sky/top.jpg",
                                           "/home/elina/MyProjects/skyboxes/sky/bottom.jpg",
                                           "/home/elina/MyProjects/skyboxes/sky/back.jpg",
                                           "/home/elina/MyProjects/skyboxes/sky/front.jpg"});


    std::vector<GLfloat> vertices_ ={
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };

};

class GroundFloor {
public:
    GroundFloor(const std::string& obj_filepath, const std::string& shader_vert, const std::string& shader_frag);
    void draw(glm::mat4& view, glm::mat4& projection, float alpha = 1.0);


private:
    std::vector<GLfloat> vertices_{};

    GLuint VAO_{};
    GLuint VBO_{};

    ShaderProgram shaderProgram_;

    float scale_{15};
    Texture2D floor_texture_ = Texture2D("/home/elina/MyProjects/textures/metal_floor_3.jpg");

    void loadObjectFile(const std::string& filepath);
    void loadObjectBuffers();
};

class BillBoard{
public:
    explicit BillBoard(const std::string& texture_filepath);
    void draw(glm::mat4& view, glm::mat4& projection, glm::vec3& translation_vec);

private:
    std::vector<GLfloat> vertices_{
            0.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
            1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
            1.0f,  1.0f,  0.0f,  1.0f,  1.0f,
            1.0f,  0.0f,  0.0f,  1.0f,  0.0f
    };
    float scale_{2};

    GLuint VAO_{};
    GLuint VBO_{};

    ShaderProgram shaderProgram_ = ShaderProgram("/home/elina/MyProjects/OpenGl/project_5/shaders/billboard_shader.vert","/home/elina/MyProjects/OpenGl/project_5/shaders/billboard_shader.frag");
    Texture2D billboard_texture_;
    void loadObjectBuffers();
};

class TransparentCube{
public:
    explicit TransparentCube(const std::string& texture_filepath);
    void draw(glm::mat4 &view, glm::mat4 &projection);

private:
    std::vector<GLfloat> vertices_ ={
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };
    GLuint VAO_{};
    GLuint VBO_{};
    ShaderProgram shaderProgram_ = ShaderProgram("/home/elina/MyProjects/OpenGl/project_5/shaders/cube_shader.vert","/home/elina/MyProjects/OpenGl/project_5/shaders/skybox_shader.frag");

    Texture3D texture_3D_;
    void loadObjectBuffers();
    void loadObjectFile(const std::string &filepath);
};

#endif //PROJECT_5_OBJECT_H
