#include "Player.h"
#include "Chunks/Voxels.h"
#include "GUI.h"
#include "Utility.h"
#include "World.h"
#include <cmath>
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

        auto tryDigHole = [&](int offsetX, int offsetY) {
            if (m_isOnGround && getVoxelType((VoxelType)m_pWorld->getVoxel(
                                                 voxelX + offsetX, voxelY + offsetY))
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

    if (!m_isOnGround)
    {
        m_velocity.y -= 20 * delta;
    }
    m_isOnGround = false;
    m_isTouchingWall = false;

    if (m_state == PlayerState::Exploring)
    {

        m_transform.position.x += m_velocity.x * delta;
        resolveCollisions({m_velocity.x * delta, 0, 0});

        m_transform.position.y += m_velocity.y * delta;
        resolveCollisions({0, m_velocity.y * delta, 0});

        m_velocity.x *= 0.95;
    }
    else if (m_state == PlayerState::Digging)
    {

        m_velocity = glm::vec3{0.0f};
        m_transform.position.x += m_digDirection.x * m_digSpeed * dt.asSeconds();
        m_transform.position.y += m_digDirection.y * m_digSpeed * dt.asSeconds();
        m_digProgress +=
            std::abs((m_digDirection.x + m_digDirection.y) * m_digSpeed * dt.asSeconds());

        for (int i = 0; i < 5; i++)
        {
            Particle p;
            p.transform.position =
                m_transform.position +
                glm::vec3{Player::box.x / 2.0f, -Player::box.y / 2.0f, Player::box.z};
            float angle = (std::rand() % 360) * 3.14f / 180.f;
            p.direction = glm::vec3{std::cos(angle), std::sin(angle), rand() % 2 + 2};
            p.lifetime = rand() % 5;

            particles.push_back(p);
        }

        if (m_digProgress > 0.95)
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

        nk_labelf(ctx, NK_STATIC, "Player Dig: (%.*f, %.*f)", 2, m_digDirection[0], 2,
                  m_digDirection[1]);

        nk_labelf(ctx, NK_STATIC, "Digging Progress: %.*f", 2, m_digProgress);
    }
    nk_end(ctx);
}

const Transform& Player::getTransform() const
{
    return m_transform;
}

void Player::beginDig(int offsetX, int offsetY)
{

    int voxelX = static_cast<int>(m_transform.position.x);
    int voxelY = static_cast<int>(m_transform.position.y);

    m_digDirection = {offsetX, offsetY};
    m_state = PlayerState::Digging;
    m_digProgress = 0;
    if (offsetY)
    {

        if (m_transform.position.x - std::floor(m_transform.position.x) < 0.5)
        {
            m_transform.position.x = std::floor(m_transform.position.x);
        }
        else
        {
            m_transform.position.x = std::ceil(m_transform.position.x);
            offsetX += 1;
        }
    }
    m_digLocation = {voxelX + offsetX, voxelY + offsetY};
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
