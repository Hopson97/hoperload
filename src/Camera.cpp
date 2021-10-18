#include "Camera.h"
#include "Chunks/Chunk.h"
#include "Utility.h"

namespace
{
    int worldHeight = 14;
    int worldWidth = 4;
} // namespace

Camera::Camera()
{
    float aspect = (float)WIDTH / (float)HEIGHT;

    m_projectionMatrix = createProjectionMatrix(aspect, 110.0f);
    m_transform = {{50, worldHeight * CHUNK_SIZE - CHUNK_SIZE / 2, 10}, {0, 270, 0}};
}

void Camera::update()
{
    auto viewMatrix = createViewMartix(m_transform, {0, 1, 0});
    m_pvMatrix = m_projectionMatrix * viewMatrix;
    m_frustum.update(m_pvMatrix);
}
