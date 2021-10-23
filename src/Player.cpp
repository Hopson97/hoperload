#include "Player.h"
#include "Chunks/Voxels.h"
#include "Utility.h"
#include "World.h"

int worldHeight = 14;

Player::Player()
{
    m_transform = {{50, worldHeight * CHUNK_SIZE - CHUNK_SIZE / 2 + 1, 1}, {0, 0, 0}};
}

void Player::input(const Keyboard& keyboard, const sf::Window& window)
{
    float PLAYER_SPEED = 0.5f;

    if (keyboard.isKeyDown(sf::Keyboard::W))
    {
        m_velocity.y += PLAYER_SPEED;
    }
    else if (keyboard.isKeyDown(sf::Keyboard::S))
    {
        m_velocity.y -= PLAYER_SPEED;
    }
    if (keyboard.isKeyDown(sf::Keyboard::A))
    {
        m_velocity.x -= PLAYER_SPEED;
    }
    else if (keyboard.isKeyDown(sf::Keyboard::D))
    {
        m_velocity.x += PLAYER_SPEED;
    }
}

void Player::update(const sf::Time& dt, const World& world)
{
    float delta = dt.asSeconds();

    if (!m_isOnGround)
    {
        m_velocity.y -= 20 * delta;
    }
    m_isOnGround = false;

    m_transform.position.x += m_velocity.x * delta;
    resolveCollisions(world, {m_velocity.x * delta, 0, 0});

    m_transform.position.y += m_velocity.y * delta;
    resolveCollisions(world, {0, m_velocity.y * delta, 0});

    m_velocity.x *= 0.98;
}

const Transform& Player::getTransform() const
{
    return m_transform;
}

struct
{
    glm::vec3 dimensions{0.5f, 0.5f, 0.5f};
} box;

void Player::resolveCollisions(const World& world, const glm::vec3& vel)
{
    auto& position = m_transform.position;
    for (int x = position.x - box.dimensions.x; x < position.x + box.dimensions.x; x++)
    {
        for (int y = position.y - box.dimensions.y; y < position.y + 0.7; y++)
        {
            for (int z = position.z - box.dimensions.z; z < position.z + box.dimensions.z;
                 z++)
            {
                auto voxel = world.getVoxel(x, y);

                if (voxel != AIR)
                {
                    if (vel.y > 0)
                    {
                        position.y = y - box.dimensions.y;
                        m_velocity.y = 0;
                    }
                    else if (vel.y < 0)
                    {
                        m_isOnGround = true;
                        position.y = y + box.dimensions.y + 1;
                        m_velocity.y = 0;
                    }

                    if (vel.x > 0)
                    {
                        position.x = x - box.dimensions.x;
                    }
                    else if (vel.x < 0)
                    {
                        position.x = x + box.dimensions.x + 1;
                    }

                    if (vel.z > 0)
                    {
                        position.z = z - box.dimensions.z;
                    }
                    else if (vel.z < 0)
                    {
                        position.z = z + box.dimensions.z + 1;
                    }
                }
            }
        }
    }
}
