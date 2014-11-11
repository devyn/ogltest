#include <cstdio>
#include <cstring>

#include "obj.hh"

template<typename T>
bool mapIndices(std::vector<T> &in, std::vector<unsigned int> &indices,
    std::vector<T> &out) {

  for (unsigned int index : indices) {
    if (index > 0 && index <= in.size()) {
      out.push_back(in[index - 1]);
    }
    else {
      fprintf(stderr, "mapIndices error: index %d out of bounds %ld\n",
          index, in.size());
      return false;
    }
  }

  return true;
}

bool loadOBJ(const char *path,
    std::vector<glm::vec3> &outVertices,
    std::vector<glm::vec2> &outUVs,
    std::vector<glm::vec3> &outNormals) {

  FILE *file = fopen(path, "rb");
  if (file == NULL) {
    fprintf(stderr, "Failed to open '%s'!", path);
    return false;
  }

  std::vector<glm::vec3> inVertices;
  std::vector<glm::vec2> inUVs;
  std::vector<glm::vec3> inNormals;

  std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;

  // For each line:
  while (!feof(file)) {
    char command[128];
    fscanf(file, "%127s", command);

    if (command[0] == '#') {
      // Comment; skip this line.
      while (!feof(file) && fgetc(file) != '\n');
    }
    else if (strcmp(command, "v") == 0) {
      // This is a vertex.
      glm::vec3 vertex;
      fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
      inVertices.push_back(vertex);
    }
    else if (strcmp(command, "vt") == 0) {
      // This is a UV coordinate.
      glm::vec2 uv;
      fscanf(file, "%f %f\n", &uv.x, &uv.y);
      inUVs.push_back(uv);
    }
    else if (strcmp(command, "vn") == 0) {
      // This is a surface normal.
      glm::vec3 normal;
      fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
      inNormals.push_back(normal);
    }
    else if (strcmp(command, "f") == 0) {
      // This is a face (hopefully triangular).
      unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];

      int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
          &vertexIndex[0], &uvIndex[0], &normalIndex[0],
          &vertexIndex[1], &uvIndex[1], &normalIndex[1],
          &vertexIndex[2], &uvIndex[2], &normalIndex[2]);

      if (matches != 9) {
        fprintf(stderr, "OBJ format error: invalid (or unsupported) "
            "face command in '%s'\n", path);
        fclose(file);
        return false;
      }

      for (int i = 0; i < 3; i++) {
        vertexIndices.push_back(vertexIndex[i]);
        uvIndices    .push_back(uvIndex[i]);
        normalIndices.push_back(normalIndex[i]);
      }
    }
    else {
      fprintf(stderr, "OBJ format warning: unknown command '%s' in '%s'\n",
          command, path);
      while (!feof(file) && fgetc(file) != '\n');
    }
  }

  fclose(file);

  // Generate proper output data by mapping the indices manually.
  if (!mapIndices(inVertices, vertexIndices, outVertices)) return false;
  if (!mapIndices(inUVs,      uvIndices,     outUVs))      return false;
  if (!mapIndices(inNormals,  normalIndices, outNormals))  return false;

  return true;
}
