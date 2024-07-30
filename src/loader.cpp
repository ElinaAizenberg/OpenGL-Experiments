
#include <iostream>
#include <complex>

#include "../include/loader.h"


void ObjectLoader::loadFullObjFileData(const std::string &filepath,
                                   std::vector<float> &object_vertices)
{
    tinyobj::ObjReaderConfig reader_config;
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(filepath, reader_config))
    {
        if (!reader.Error().empty())
        {
            std::cerr << "TinyObjReader: " << reader.Error();
            throw reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty() && reader.Warning().find("Material") == std::string::npos)
    {
        std::cout << "TinyObjReader: " << reader.Warning();
        throw reader.Warning();
    }

    tinyobj::attrib_t attrib    = reader.GetAttrib();
    std::vector<tinyobj::shape_t> shapes    = reader.GetShapes();

    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
                tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
                tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];
                object_vertices.push_back(vx);
                object_vertices.push_back(vy);
                object_vertices.push_back(vz);


                // Check if `normal_index` is zero or positive. negative = no normal data
                if (idx.normal_index >= 0) {
                    tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
                    tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
                    tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];
                    object_vertices.push_back(nx);
                    object_vertices.push_back(ny);
                    object_vertices.push_back(nz);

                }

                // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                if (idx.texcoord_index >= 0) {
                    tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
                    tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
                    object_vertices.push_back(tx);
                    object_vertices.push_back(ty);

                }
            }
            index_offset += fv;
        }
    }
}

void ObjectLoader::loadLimitedObjFileData(const std::string &filepath,
                                   std::vector<float> &object_vertices,
                                   std::vector<float> &object_normals,
                                   std::vector<unsigned int> &indices_)
{
    tinyobj::ObjReaderConfig reader_config;
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(filepath, reader_config))
    {
        if (!reader.Error().empty())
        {
            std::cerr << "TinyObjReader: " << reader.Error();
            throw reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty() && reader.Warning().find("Material") == std::string::npos)
    {
        std::cout << "TinyObjReader: " << reader.Warning();
        throw reader.Warning();
    }

    tinyobj::attrib_t attrib    = reader.GetAttrib();
    std::vector<tinyobj::shape_t> shapes    = reader.GetShapes();

    for (auto const& vertex : attrib.vertices)
    {
        object_vertices.push_back(vertex);
    }

    std::vector<std::vector<unsigned int>> object_shapes;
    for (auto const& shape : shapes)
    {
        std::vector<unsigned int> shape_indices;
        for (auto const& index : shape.mesh.indices)
        {
            indices_.push_back(index.vertex_index);
            shape_indices.push_back(index.vertex_index);
        }
        object_shapes.push_back(shape_indices);
    }
    loadObjNormals(object_normals, object_vertices, object_shapes);
}

void ObjectLoader::loadObjNormals(std::vector<float>& object_normals,
                                   std::vector<float>& object_vertices,
                                   std::vector<std::vector<unsigned int>>& object_shapes)
{
    for (auto const& shape_indices : object_shapes)
    {
        std::map<unsigned int, std::vector<Vertex>> surfaces_normals;

        for (uint32_t i = 0; i < shape_indices.size(); i += 3)
        {
            unsigned int a_index = shape_indices[i];
            unsigned int b_index = shape_indices[i + 1];
            unsigned int c_index = shape_indices[i + 2];

            Vertex a = {object_vertices[a_index * 3 + 0],
                        object_vertices[a_index * 3 + 1],
                        object_vertices[a_index * 3 + 2]};
            Vertex b = {object_vertices[b_index * 3 + 0],
                        object_vertices[b_index * 3 + 1],
                        object_vertices[b_index * 3 + 2]};
            Vertex c = {object_vertices[c_index * 3 + 0],
                        object_vertices[c_index * 3 + 1],
                        object_vertices[c_index * 3 + 2]};

            Vertex surface_normal = calculateSurfaceNormal(a, b, c);

            for (auto vertex_ind : {a_index, b_index, c_index})
            {
                auto it = surfaces_normals.find(vertex_ind);

                if (it != surfaces_normals.end())
                {
                    it->second.push_back(surface_normal);
                }
                else
                {
                    std::vector<Vertex> to_add = {surface_normal};
                    surfaces_normals.insert(std::make_pair(vertex_ind, to_add));
                }
            }
        }
        for (uint32_t i = 0; i < object_vertices.size() / 3; i++)
        {
            auto it = surfaces_normals.find(i);
            if (it != surfaces_normals.end())
            {
                Vertex vertex_normal = calculateAverageSurfaceNormal(it->second);
                object_normals.push_back(vertex_normal.x);
                object_normals.push_back(vertex_normal.y);
                object_normals.push_back(vertex_normal.z);
            }
        }
    }
}

Vertex ObjectLoader::calculateSurfaceNormal(Vertex& v1, Vertex& v2, Vertex& v3)
{
    Vertex edge1 = {v2.x - v1.x, v2.y - v1.y, v2.z - v1.z};
    Vertex edge2 = {v3.x - v1.x, v3.y - v1.y, v3.z - v1.z};

    Vertex normal = {edge1.y * edge2.z - edge1.z * edge2.y,
                     edge1.z * edge2.x - edge1.x * edge2.z,
                     edge1.x * edge2.y - edge1.y * edge2.x};

    float length = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);

    if (length > 0)
    {
        normal.x /= length;
        normal.y /= length;
        normal.z /= length;
    }

    return normal;
}

Vertex ObjectLoader::calculateAverageSurfaceNormal(std::vector<Vertex> const& normals)
{
    Vertex sum = {0.0, 0.0, 0.0};

    for (auto& normal : normals)
    {
        sum.x += normal.x;
        sum.y += normal.y;
        sum.z += normal.z;
    }

    float length = std::sqrt(sum.x * sum.x + sum.y * sum.y + sum.z * sum.z);

    if (length > 0)
    {
        sum.x /= length;
        sum.y /= length;
        sum.z /= length;
    }
    return sum;
}

