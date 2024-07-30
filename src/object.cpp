#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>


#include "../include/object.h"
#include "../include/loader.h"




Object::Object(const std::string &obj_filepath, const std::string &shader_vert, const std::string &shader_frag, int pick_id, float pick_r, float pick_g, float pick_b, float scale, std::vector<float> rotate, const std::vector<std::string>& textures): shaderProgram_(shader_vert.c_str(), shader_frag.c_str()), pick_id_(pick_id), scale_(scale) {

    if (!obj_filepath.empty()){
        loadObjectFile(obj_filepath);
    }

    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);

    pick_rgb_[0] = pick_r/255;
    pick_rgb_[1] = pick_g/255;
    pick_rgb_[2] = pick_b/255;

    if (!rotate.empty()){
        rotate_xyz_[0] = rotate[0];
        rotate_xyz_[1] = rotate[1];
        rotate_xyz_[2] = rotate[2];
    }

    if (!textures.empty()){
        for (auto& texture_filepath: textures){
            object_textures_.emplace_back(texture_filepath);
        }
    }
}

void Object::loadObjectBuffers()
{
    glBindVertexArray(VAO_);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(GLfloat) * vertices_.size(),
                 vertices_.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Object::loadObjectFile(const std::string &filepath) {
    if (filepath.empty()){
        return;
    }

    try{
        ObjectLoader::loadFullObjFileData(filepath, vertices_);
    }
    catch(...) {
        std::cerr << "Error: Unable to load file: " << filepath;
        return;
    }

}

void Object::drawObject(glm::mat4 &view, glm::mat4 &projection, glm::vec3& camera_position, bool get_pick_color, bool reflection) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDepthFunc(GL_LESS);

    shaderProgram_.use();
    if (object_textures_.empty()){
        shaderProgram_.setBool("isTexture1Provided", false); // Bind to texture unit 0
    } else{
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, object_textures_[0].getTexture());
        shaderProgram_.setBool("isTexture1Provided", true); // Bind to texture unit 0
        shaderProgram_.setInt("texture1", 0); // Bind to texture unit 0
    }

    if (get_pick_color){
        shaderProgram_.setInt("buffer_id", 1);
        shaderProgram_.setVec3("ObjectColor", pick_rgb_[0], pick_rgb_[1], pick_rgb_[2]);
    }
    else{
        shaderProgram_.setInt("buffer_id", 0);
        shaderProgram_.setVec3("light.position", glm::vec3(5.0f, 10.0f, 2.0f));
        shaderProgram_.setVec3("viewPos", camera_position);

        shaderProgram_.setVec3("light.ambient", glm::vec3(0.2,0.2,0.2));
        shaderProgram_.setVec3("light.diffuse", glm::vec3(0.859,0.859,0.859));
        shaderProgram_.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    }

    glm::mat4 model = glm::mat4(1.0f);
    translate_vec = glm::vec3(xyz_[0], xyz_[1], xyz_[2]);

    model = glm::translate(model, translate_vec);
    model = glm::rotate(model, glm::radians(rotate_xyz_[0]), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotate_xyz_[1]), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotate_xyz_[2]), glm::vec3(0.0f, 0.0f, 1.0f));

    model = glm::scale(model, glm::vec3(scale_, scale_, scale_));
    if (reflection){
        model = glm::scale(model, glm::vec3(1.0, -1.0, 1.0));
        shaderProgram_.setVec3("light.position", glm::vec3(5.0f, -10.0f, 2.0f));
        glm::vec3 scaledCameraPosition = camera_position * glm::vec3(1.0, -1.0, 1.0);
        shaderProgram_.setVec3("viewPos", scaledCameraPosition);
    }


    shaderProgram_.setMat4("projection", projection);
    shaderProgram_.setMat4("view", view);
    shaderProgram_.setMat4("model", model);

    shaderProgram_.setVec3("material.ambient", material_.ambient);
    shaderProgram_.setVec3("material.diffuse_vec", material_.diffuse);
    shaderProgram_.setVec3("material.specular", material_.specular);
    shaderProgram_.setFloat("material.shininess", material_.shininess);

    glBindVertexArray(VAO_);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices_.size()/8));
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture


}

bool Object::checkPickColor(int pick_color_id) const {
    if (pick_color_id == pick_id_)
    {
        return true;
    }
    return false;
}

void Object::move(float delta_x, float delta_y, float delta_z) {
    xyz_[0] += delta_x;
    xyz_[1] += delta_y;
    xyz_[2] += delta_z;
}


Skybox::Skybox(const std::string& shader_vert, const std::string& shader_frag): shaderProgram_(shader_vert.c_str(), shader_frag.c_str()){
    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);

    glBindVertexArray(VAO_);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(GLfloat) * vertices_.size(),
                 vertices_.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

}

void Skybox::draw(glm::mat4 &view, glm::mat4 &projection) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDepthFunc(GL_LEQUAL);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture_.getTexture());

    shaderProgram_.use();
    shaderProgram_.setInt("skybox", 0);

    glm::mat4 new_view = glm::mat4(glm::mat3(view)); // remove translation from the view matrix

    glm::mat4 model = glm::mat4(1.0f);

    shaderProgram_.setMat4("projection", projection);
    shaderProgram_.setMat4("view", new_view);

    glBindVertexArray(VAO_);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices_.size()/3));

    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0); // Unbind texture


}

GroundFloor::GroundFloor(const std::string& obj_filepath, const std::string &shader_vert, const std::string &shader_frag) : shaderProgram_(shader_vert.c_str(), shader_frag.c_str()) {
    loadObjectFile(obj_filepath);

    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);

    loadObjectBuffers();
}

void GroundFloor::loadObjectFile(const std::string &filepath) {
    if (filepath.empty()){
        return;
    }
    try{
        ObjectLoader::loadFullObjFileData(filepath, vertices_);
    }
    catch(...) {
        std::cerr << "Error: Unable to load file: " << filepath;
        return;
    }
}

void GroundFloor::loadObjectBuffers() {
    glBindVertexArray(VAO_);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(GLfloat) * vertices_.size(),
                 vertices_.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

}

void GroundFloor::draw(glm::mat4 &view, glm::mat4 &projection, float alpha) {

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    shaderProgram_.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, floor_texture_.getTexture());
    shaderProgram_.setInt("texture1", 0); // Bind to texture unit 0

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::scale(model, glm::vec3(scale_, scale_, scale_));

    shaderProgram_.setMat4("projection", projection);
    shaderProgram_.setMat4("view", view);
    shaderProgram_.setMat4("model", model);

    shaderProgram_.setFloat("alpha", alpha);

    glBindVertexArray(VAO_);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices_.size()/3));

    glBindVertexArray(0); // Unbind VAO
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture
}


BillBoard::BillBoard(const std::string &texture_filepath): billboard_texture_(texture_filepath) {
    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);
    loadObjectBuffers();
}

void BillBoard::loadObjectBuffers() {
    glBindVertexArray(VAO_);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(GLfloat) * vertices_.size(),
                 vertices_.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void BillBoard::draw(glm::mat4 &view, glm::mat4 &projection, glm::vec3& translation_vec) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    shaderProgram_.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, billboard_texture_.getTexture());
    shaderProgram_.setInt("texture1", 0);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, translation_vec);
    model = glm::scale(model, glm::vec3(scale_, scale_, scale_));

    shaderProgram_.setMat4("projection", projection);
    shaderProgram_.setMat4("view", view);
    shaderProgram_.setMat4("model", model);

    glBindVertexArray(VAO_);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices_.size()/5));

    glBindVertexArray(0); // Unbind VAO
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture
}

void TransparentCube::loadObjectBuffers() {
    glBindVertexArray(VAO_);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(GLfloat) * vertices_.size(),
                 vertices_.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}



void TransparentCube::loadObjectFile(const std::string &filepath) {
    if (filepath.empty()){
        return;
    }
    try{
        ObjectLoader::loadFullObjFileData(filepath, vertices_);
    }
    catch(...) {
        std::cerr << "Error: Unable to load file: " << filepath;
        return;
    }
}

void TransparentCube::draw(glm::mat4 &view, glm::mat4 &projection) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_3D_.getTexture());

    shaderProgram_.use();
    shaderProgram_.setInt("skybox", 0);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0, 1.0, -7.0));

    shaderProgram_.setMat4("projection", projection);
    shaderProgram_.setMat4("view", view);
    shaderProgram_.setMat4("model", model);


    glBindVertexArray(VAO_);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices_.size()/3));


    glBindTexture(GL_TEXTURE_CUBE_MAP, 0); // Unbind texture

    glBindVertexArray(0);

}

TransparentCube::TransparentCube(const std::string &texture_filepath): texture_3D_(Texture3D({texture_filepath,
                                                                                              texture_filepath,
                                                                                              texture_filepath,
                                                                                              texture_filepath,
                                                                                              texture_filepath,
                                                                                              texture_filepath})) {
    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);
    loadObjectBuffers();
}
