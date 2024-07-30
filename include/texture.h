#ifndef PROJECT_5_TEXTURE_H
#define PROJECT_5_TEXTURE_H

#include <iostream>
#include <GL/gl.h>
#include <vector>


class Texture2D{
public:
    explicit Texture2D(const std::string& filepath);
    GLuint getTexture() const;

private:
    GLuint texture_id_{0};
};


class Texture3D{
public:
    explicit Texture3D(std::vector<std::string> filepaths);
    GLuint getTexture() const;

private:
    GLuint texture_id_{0};
};


#endif //PROJECT_5_TEXTURE_H
