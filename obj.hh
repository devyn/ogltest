#ifndef OBJ_HH
#define OBJ_HH

#include <vector>

#include <glm/glm.hpp>

bool loadOBJ(const char *path,
    std::vector<glm::vec3> &outVertices,
    std::vector<glm::vec2> &outUVs,
    std::vector<glm::vec3> &outNormals);

#endif
