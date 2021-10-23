#pragma once

#include "Maths.h"
#include <SFML/Window/Window.hpp>

class Keyboard;
class World;

class Player
{
  public:
    Player();

    void input(const Keyboard& keyboard, const sf::Window& window);

    void update(const sf::Time& dt, const World& world);

    const Transform& getTransform() const;

  private:
    void resolveCollisions(const World& world, const glm::vec3& vel);

    Transform m_transform;
    glm::vec3 m_velocity;
    bool m_isOnGround = false;
};
