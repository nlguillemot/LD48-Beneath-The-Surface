#include "geometry.hpp"

bool RayParallelogramIntersect(glm::vec3 origin, glm::vec3 direction,
                               glm::vec3 corner, glm::vec3 across, glm::vec3 upward,
                               float& t)
{
    glm::vec3 d = glm::normalize(direction);
    glm::vec3 n = glm::normalize(glm::cross(across, upward));

    // check denominator to see if there's a solution
    float denom = glm::dot(d,n);
    if (glm::abs(denom) < 1e-6) return false;

    // find distance to plane
    float tt = glm::dot(glm::normalize(corner - origin), n) / denom;

    // see if the hit point was within the parallelogram
    glm::vec3 hitPoint = origin + tt * direction;
    glm::vec3 offset = hitPoint - corner;

    // check if it's within the across reach
    float u = glm::dot(glm::normalize(across), glm::normalize(offset));
    if (u < 0 || u > 1) return false;

    float v = glm::dot(glm::normalize(upward), glm::normalize(offset));
    if (v < 0 || v > 1) return false;

    // collided!
    t = tt;
    return true;
}
