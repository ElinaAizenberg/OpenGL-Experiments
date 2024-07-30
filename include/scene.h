
#ifndef PROJECT_5_SCENE_H
#define PROJECT_5_SCENE_H

#include <map>
#include <unordered_map>
#include "../include/object.h"

struct ObjectInput{
    std::string filepath;
    float scale;
    std::vector<float> rotate{};
    std::vector<std::string> textures{};

};

class Scene{
public:
    Scene();
    void addObjectSet(const std::string& filepath, float scale = 1,  const std::vector<float>& rotate = {}, const std::vector<std::string>& textures = {});
    void draw(glm::mat4 &view, glm::mat4 &projection, glm::vec3& camera_position, bool get_color=false);
    int getObjectIdByPickColor(const unsigned char* pick_color);
    void moveObject(int objectSet_id, float delta_x, float delta_y, float delta_z);
    void scaleObject(int objectSet_id, float offset);
    void createObjectSet(int id);
    void addFlowers();


private:
    int current_object_id_{0};
    int current_pick_color_[3]{0, 0, 0};

    std::map<std::string, int> objects_pick_ids_;

    int generatePickColorID_();
    void generateNewPickColor_();

    std::vector<Object> objects_;

    Skybox skybox_ = Skybox("/home/elina/MyProjects/OpenGl/project_5/shaders/skybox_shader.vert", "/home/elina/MyProjects/OpenGl/project_5/shaders/skybox_shader.frag");
    GroundFloor floor_ = GroundFloor("/home/elina/MyProjects/object_files/square.obj", "/home/elina/MyProjects/OpenGl/project_5/shaders/floor_shader.vert", "/home/elina/MyProjects/OpenGl/project_5/shaders/floor_shader.frag");

    std::unordered_map<int, ObjectInput> keyMap_;

    std::vector<BillBoard> billboards_;
    std::vector<TransparentCube> cubes_;
    std::vector<glm::vec3> flowers_{};

};

#endif //PROJECT_5_SCENE_H
