#include "GUI.h"
#include "Hoperload.h"
#include "Graphics/GLWrappers.h"
#include "Utility.h"
#include <SFML/GpuPreference.hpp>

int main()
{
    bool isMouseActive = true;

    sf::Window window;
    if (!initWindow(&window))
    {
        return 1;
    }
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);

    guiInit(window);

    // Time step vars, 60 ticks per second
    const sf::Time timePerUpdate = sf::seconds(1.0f / 60.0f);
    sf::Clock timer;
    sf::Time lastTime = sf::Time::Zero;
    sf::Time lag = sf::Time::Zero;
    sf::Clock updateClock;

    Hoperload game;
    Keyboard keyboard;
    while (window.isOpen())
    {
        guiBeginFrame();
        sf::Event e;
        while (window.pollEvent(e))
        {
            keyboard.update(e);
            guiProcessEvent(e);
            game.onEvent(e);
            if (e.type == sf::Event::KeyReleased)
            {
                if (e.key.code == sf::Keyboard::Escape)
                {
                    window.close();
                }
                else if (e.key.code == sf::Keyboard::L)
                {
                    isMouseActive = !isMouseActive;
                    window.setMouseCursorVisible(!isMouseActive);
                }
            }
        }

        // Get times
        sf::Time time = timer.getElapsedTime();
        sf::Time elapsed = time - lastTime;
        lastTime = time;
        lag += elapsed;
        game.onInput(keyboard, window, isMouseActive);

        // Fixed time update
        while (lag >= timePerUpdate)
        {
            lag -= timePerUpdate;
            game.onUpdate(elapsed);
        }

        glEnable(GL_DEPTH_TEST);
        game.onRender();
        game.onGUI();
        guiEndFrame();

        window.display();
    }
    guiShutdown();
}