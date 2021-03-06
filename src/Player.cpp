#include "Player.h"
#include "Chunks/Voxels.h"
#include "GUI.h"
#include "Utility.h"
#include "World.h"
#include <cmath>
#include <iostream>
#include <random>

int worldHeight = 14;

Player::Player(World& world)
    : m_pWorld(&world)
{
    m_transform = {{50, worldHeight * CHUNK_SIZE - CHUNK_SIZE / 2 + 1, 1}, {0, 0, 0}};
}

void Player::input(const Keyboard& keyboard, const sf::Window& window)
{
    if (m_state == PlayerState::Exploring)
    {
        float PLAYER_SPEED = 0.5f;

        int voxelX = static_cast<int>(m_transform.position.x);
        int voxelY = static_cast<int>(m_transform.position.y);

        auto tryDigHole = [&](int offsetX, int offsetY)
        {
            int voxelXOffset = offsetX;
            if (offsetY && m_transform.position.x - floorf(m_transform.position.x) > 0.5f)
            {
                voxelX += 1;
            }

            if (m_isOnGround && getVoxelType((VoxelType)m_pWorld->getVoxel(
                                                 voxelX + voxelXOffset, voxelY + offsetY))
                                    .collidable)
            {
                beginDig(offsetX, offsetY);
                return true;
            }
            return false;
        };

        if (keyboard.isKeyDown(sf::Keyboard::W))
        {
            m_velocity.y += PLAYER_SPEED;
        }
        else if (keyboard.isKeyDown(sf::Keyboard::S) && m_isOnGround)
        {
            if (tryDigHole(0, -1))
            {
                return;
            }
        }
        if (keyboard.isKeyDown(sf::Keyboard::A))
        {
            if (m_isTouchingWall && tryDigHole(-1, 0))
            {
                return;
            }
            m_velocity.x -= PLAYER_SPEED;
        }
        else if (keyboard.isKeyDown(sf::Keyboard::D))
        {

            if (m_isTouchingWall && tryDigHole(1, 0))
            {
                return;
            }
            m_velocity.x += PLAYER_SPEED;
        }
    }
}

void Player::update(const sf::Time& dt, std::vector<Particle>& particles)
{
    float delta = dt.asSeconds();

    auto& pos = m_transform.position;
    if (m_state == PlayerState::Exploring)
    {
        if (!m_isOnGround)
        {
            m_velocity.y -= 20 * delta;
        }
        m_isOnGround = false;
        m_isTouchingWall = false;
        pos.x += m_velocity.x * delta;
        resolveCollisions({m_velocity.x * delta, 0, 0});

        pos.y += m_velocity.y * delta;
        resolveCollisions({0, m_velocity.y * delta, 0});

        m_velocity.x *= 0.95f;
    }
    else if (m_state == PlayerState::Digging)
    {

        m_digProgress +=
            std::abs((m_digDirection.x + m_digDirection.y) * m_digSpeed * dt.asSeconds());

        pos.x = lerp(m_digStartPosition.x, m_digTarget.x, m_digProgress);
        pos.y = lerp(m_digStartPosition.y, m_digTarget.y, m_digProgress);
        if (rand() % 5 >= 2)
        {
            Particle p;
            p.transform.position = pos + glm::vec3{Player::box.x / 2.0f,
                                                   -Player::box.y / 2.0f, Player::box.z};
            p.direction = {m_digDirection.x == 0
                               ? rand() % 4 - 2
                               : m_digDirection.x * 2.5f * m_digSpeed * -(rand() % 2 - 1),
                           m_digDirection.y == 0
                               ? rand() % 4 - 2
                               : m_digDirection.y * 2.5f * m_digSpeed * -(rand() % 2 - 1),
                           rand() % 3 + 2};
            p.lifetime = static_cast<float>(rand() % 5);

            particles.push_back(p);
        }

        if (m_digProgress > 1)
        {
            m_state = PlayerState::Exploring;
            m_pWorld->breakBlock(m_digLocation.x, m_digLocation.y);
        }
    }
}

void Player::gui()
{
    auto ctx = getGuiContext();
    if (nk_begin(ctx, "Player", nk_rect(10, 150, 300, 130), 0))
    {
        nk_layout_row_dynamic(ctx, 12, 1);
        nk_labelf(ctx, NK_STATIC, "Player Velcoity: (%.*f, %.*f, %.*f)", 2, m_velocity[0],
                  2, m_velocity[1], 2, m_velocity[2]);

        nk_labelf(ctx, NK_STATIC, "Dig Direction: (%.*f, %.*f)", 2, m_digDirection[0], 2,
                  m_digDirection[1]);

        nk_labelf(ctx, NK_STATIC, "Dig Target: (%.*f, %.*f)", 2, m_digTarget[0], 2,
                  m_digTarget[1]);

        nk_labelf(ctx, NK_STATIC, "Digging Progress: %.*f", 2, m_digProgress);
    }
    nk_end(ctx);
}

const Transform& Player::getTransform() const
{
    return m_transform;
}

bool Player::isDigging() const
{
    return m_state == PlayerState::Digging;
}

void Player::beginDig(int offsetX, int offsetY)
{
    m_velocity = glm::vec3{0.0f};

    int voxelX = static_cast<int>(m_transform.position.x);
    int voxelY = static_cast<int>(m_transform.position.y);

    m_digDirection = {offsetX, offsetY};
    m_state = PlayerState::Digging;
    m_digProgress = 0;

    if (offsetY && m_transform.position.x - floorf(m_transform.position.x) > 0.5)
    {
        voxelX += 1;
    }

    // The voxel being mined
    m_digLocation = {voxelX + offsetX, voxelY + offsetY};

    // Player's final position after mining - for linear interpolation between current and
    // final location
    m_digStartPosition = m_transform.position.xy();
    m_digTarget =
        glm::vec2(voxelX + offsetX + Player::box.x / 4, voxelY + offsetY + Player::box.y);
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
                        m_isTouchingWall = true;
                        position.x = x - Player::box.x;
                        m_velocity.x = 0;
                    }
                    else if (vel.x < 0)
                    {
                        m_isTouchingWall = true;
                        position.x = x + 1;
                        m_velocity.x = 0;
                    }
                }
            }
        }
    }
}
