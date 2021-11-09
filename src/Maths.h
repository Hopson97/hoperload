#pragma once

#define GLM_FORCE_SWIZZLE

#include <array>
#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Transform
{
    glm::vec3 position{0.0f};
    glm::vec3 rotation{0.0f};
};

class ViewFrustum
{
    struct Plane
    {
        float distanceToPoint(const glm::vec3& point) const;

        float distanceToOrigin = 0;
        glm::vec3 normal{0.0f};
    };

  public:
    void update(const glm::mat4& projViewMatrix) noexcept;

  private:
    std::array<Plane, 6> m_planes{0.0f};
};

glm::mat4 createModelMatrix(const Transform& transform);
glm::mat4 createOrbitModelMatrix(const Transform& transform);
glm::mat4 createViewMartix(const Transform& transform, const glm::vec3& up);
glm::mat4 createProjectionMatrix(float aspectRatio, float fov);

glm::vec3 forwardsVector(const glm::vec3& rotation);
glm::vec3 backwardsVector(const glm::vec3& rotation);
glm::vec3 leftVector(const glm::vec3& rotation);
glm::vec3 rightVector(const glm::vec3& rotation);

float lerp(float v0, float v1, float t);