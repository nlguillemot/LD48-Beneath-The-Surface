#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <glm/glm.hpp>

bool RayParallelogramIntersect(glm::vec3 origin, glm::vec3 direction,
                               glm::vec3 corner, glm::vec3 across, glm::vec3 upward,
                               float& t);

#endif // GEOMETRY_HPP
