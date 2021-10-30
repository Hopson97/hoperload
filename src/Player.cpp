#include "Player.h"
#include "Chunks/Voxels.h"
#include "GUI.h"
#include "Utility.h"
#include "World.h"
#include <cmath>

int worldHeight = 14;

Player::Player(World& world)
    : m_pWorld(&world)
{
    m_transform = {{50, worldHeight * CHUNK_SIZE - CHUNK_SIZE / 2 + 1, 1}, {0, 0, 0}};
}

void Player::input(const Keyboard& keyboard, const sf::Window& window)
{
    float PLAYER_SPEED = 0.5f;

    int voxelX = m_transform.position.x;
    int voxelY = m_transform.position.y;

    if (keyboard.isKeyDown(sf::Keyboard::W))
    {
        m_velocity.y += PLAYER_SPEED;
    }
    else if (keyboard.isKeyDown(sf::Keyboard::S))
    {
        if (getVoxelType((VoxelType)m_pWorld->getVoxel(voxelX, voxelY - 1)).collidable)
        {
            m_pWorld->breakBlock(voxelX, voxelY - 1);
        }
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

void Player::update(const sf::Time& dt)
{
    float delta = dt.asSeconds();

    if (!m_isOnGround)
    {
        m_velocity.y -= 20 * delta;
    }
    m_isOnGround = false;

    m_transform.position.x += m_velocity.x * delta;
    resolveCollisions({m_velocity.x * delta, 0, 0});

    m_transform.position.y += m_velocity.y * delta;
    resolveCollisions({0, m_velocity.y * delta, 0});

    m_velocity.x *= 0.95;
}

void Player::gui()
{
    auto ctx = getGuiContext();
    if (nk_begin(ctx, "Player", nk_rect(10, 150, 300, 130), 0))
    {
        nk_layout_row_dynamic(ctx, 12, 1);
        nk_labelf(ctx, NK_STATIC, "Player Velcoity: (%.*f, %.*f, %.*f)", 2, m_velocity[0],
                  2, m_velocity[1], 2, m_velocity[2]);
    }
    nk_end(ctx);
}

const Transform& Player::getTransform() const
{
    return m_transform;
}

void Player::resolveCollisions(const glm::vec3& vel)
{
    auto& position = m_transform.position;
    for (int x = position.x; x < position.x + Player::box.x; x++)
    {
        for (int y = position.y - Player::box.y; y < position.y; y++)
        {
            for (int z = position.z; z < position.z + Player::box.z; z++)
            {
                const auto& voxel = getVoxelType((VoxelType)m_pWorld->getVoxel(x, y));

                if (voxel.collidable)
                {
                    if (vel.y > 0)
                    {
                        position.y = y;
                        m_velocity.y = 0;
                    }
                    else if (vel.y < 0)
                    {
                        m_isOnGround = true;
                        position.y = y + Player::box.y + 1;
                        m_velocity.y = 0;
                    }

                    if (vel.x > 0)
                    {
                        position.x = x - Player::box.x;
                        m_velocity.x = 0;
                    }
                    else if (vel.x < 0)
                    {
                        position.x = x + 1;
                        m_velocity.x = 0;
                    }
                }
            }
        }
    }
}
