
#include <random>
#include "../include/scene.h"


Scene::Scene() {
    keyMap_.insert({49, {"/home/elina/MyProjects/object_files/kettle/Chaynik.obj", 10}});
    keyMap_.insert({50, {"/home/elina/MyProjects/object_files/lantern/lantern_obj.obj", 0.05, {}, {"/home/elina/MyProjects/object_files/lantern/textures/lantern_Base_Color.jpg"}}});

    billboards_.emplace_back("/home/elina/MyProjects/textures/flowers_1.png");
    billboards_.emplace_back("/home/elina/MyProjects/textures/flowers_2.png");
    billboards_.emplace_back("/home/elina/MyProjects/textures/window.png");

    cubes_.emplace_back("/home/elina/MyProjects/textures/window.png");

}

void Scene::draw(glm::mat4 &view, glm::mat4 &projection, glm::vec3& camera_position, bool get_color) {
  //

    /* Don't update color or depth. */
    glDisable(GL_DEPTH_TEST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    /* Draw 1 into the stencil buffer. */
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xffffffff);

    /* Now render floor; floor pixels just get their stencil set to 1. */
    floor_.draw(view, projection);
    /* Re-enable update of color and depth. */
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_DEPTH_TEST);

    /* Now, only render where stencil is set to 1. */
    glStencilFunc(GL_EQUAL, 1, 0xffffffff);  /* draw if ==1 */
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    for (auto& obj: objects_){
        obj.drawObject(view, projection, camera_position, get_color, true);
    }
    glDisable(GL_STENCIL_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    floor_.draw(view, projection, 0.7);
    glDisable(GL_BLEND);

    for (auto& obj: objects_){
          obj.drawObject(view, projection, camera_position, get_color, false);
      }

    skybox_.draw(view, projection);

    // flowers
    int ind = 0;
    for (auto & flower : flowers_)
    {
        billboards_[ind].draw(view, projection, flower);
        ind ^= 1;
    }

    // windows
    auto vec = glm::vec3(0.0, 0.0, -3.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    billboards_[2].draw(view, projection, vec);
    glDisable(GL_BLEND);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    cubes_[0].draw(view, projection);
    glDisable(GL_BLEND);


}

void Scene::addObjectSet(const std::string &filepath, float scale, const std::vector<float>& rotate, const std::vector<std::string>& textures) {
    generateNewPickColor_();
    auto pick_color_id = generatePickColorID_();
    auto new_obj = Object(filepath, "/home/elina/MyProjects/OpenGl/project_5/shaders/obj_shader.vert", "/home/elina/MyProjects/OpenGl/project_5/shaders/obj_shader.frag", pick_color_id, current_pick_color_[0], current_pick_color_[1], current_pick_color_[2], scale, rotate, textures);
    new_obj.loadObjectBuffers();
    objects_.push_back(std::move(new_obj));
}

int Scene::generatePickColorID_() {
    int pick_color_id = current_pick_color_[0] * (256 * 256) + current_pick_color_[1] * 256 +
                        current_pick_color_[2];
    return pick_color_id;
}

void Scene::generateNewPickColor_() {
    for (int i = 2; i >= 0; --i)
    {
        if (current_pick_color_[i] < 255)
        {
            current_pick_color_[i] += 10;
            return;
        }
        if (current_pick_color_[i] == 255)
        {
            current_pick_color_[i] = 0;
            current_pick_color_[i - 1]++;
            return;
        }
    }
}

int Scene::getObjectIdByPickColor(const unsigned char *pick_color) {
    int pick_color_id = pick_color[0] * (256 * 256) + pick_color[1] * 256 + pick_color[2];

    for (std::vector<Object>::size_type i = 0; i < objects_.size(); i++)
    {
        if (objects_[i].checkPickColor(pick_color_id))
        {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void Scene::moveObject(int objectSet_id, float delta_x, float delta_y, float delta_z) {
    objects_[objectSet_id].move(delta_x, delta_y, delta_z);
}

void Scene::createObjectSet(int id) {
    for ( auto it = keyMap_.begin(); it != keyMap_.end(); ++it  )
    {
        if (it->first == id){
            addObjectSet(it->second.filepath, it->second.scale, it->second.rotate, it->second.textures);
        }
    }

}


void Scene::scaleObject(int objectSet_id, float offset) {
    objects_[objectSet_id].scale(offset);
}

void Scene::addFlowers() {
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(-14, 14); // define the range

    flowers_.emplace_back(static_cast<float>(distr(gen)),0, static_cast<float>(distr(gen)));
}


