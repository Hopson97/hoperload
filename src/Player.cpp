#include "Player.h"
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
    m_transform.position += m_velocity * dt.asSeconds();
    m_velocity *= 0.98;
}

const Transform& Player::getTransform() const
{
    return m_transform;
}
