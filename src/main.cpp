#include "GUI.h"
#include "Graphics/GLWrappers.h"
#include "Hoperload.h"
#include "Utility.h"
#include <SFML/GpuPreference.hpp>

#include <ostream>
#include <iostream>

namespace
{
    void GLAPIENTRY glDebugCallback(GLenum source, GLenum type, GLuint, GLenum severity, GLsizei,
                                    const GLchar* message, const void*)
    {
        // clang-format off
    const char* type_string = "?";
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:                   type_string = "error";               break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:     type_string = "deprecated behavior"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:      type_string = "undefined behavior";  break;
        case GL_DEBUG_TYPE_PORTABILITY:             type_string = "portability";         break;
        case GL_DEBUG_TYPE_MARKER:                  type_string = "marker";              break;
        case GL_DEBUG_TYPE_PUSH_GROUP:              type_string = "push group";          break;
        case GL_DEBUG_TYPE_POP_GROUP:               type_string = "pop group";           break;
        case GL_DEBUG_TYPE_OTHER:                   type_string = "other";               break;
    }

    const char* severity_string = "?";
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:            severity_string = "high";    break;
        case GL_DEBUG_SEVERITY_MEDIUM:          severity_string = "medium";  break;
        case GL_DEBUG_SEVERITY_LOW:             severity_string = "low";     break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:                                return;
    }

    const char* source_string = "?";
    switch (source) {
        case GL_DEBUG_SOURCE_API:               source_string = "API";               break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:     source_string = "window system";     break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:   source_string = "shader compiler";   break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:       source_string = "third party";       break;
        case GL_DEBUG_SOURCE_APPLICATION:       source_string = "app";               break;
        case GL_DEBUG_SOURCE_OTHER:             source_string = "other";             break;
    }
        // clang-format on
        std::println(std::cerr,
                     "OpenGL message.\n  Type: {}\n  Severity: {}\n  Source: {}\n  Message: {}",
                     type_string, severity_string, source_string, message);
    }

    void enableGLDebugging()
    {
#ifndef __APPLE__
#ifndef NDEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif
        glDebugMessageCallback(glDebugCallback, NULL);

        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
#endif
    }
} // namespace


int main()
{
    bool isMouseActive = true;

    sf::ContextSettings context_settings;
    context_settings.depthBits = 24;
    context_settings.stencilBits = 8;
    context_settings.antiAliasingLevel = 4;
    context_settings.majorVersion = 4;
    context_settings.minorVersion = 5;
    context_settings.attributeFlags = sf::ContextSettings::Debug;

    sf::Window window(sf::VideoMode{{1600, 900}}, "Hoperload", sf::Style::Default,
                      sf::State::Windowed, context_settings);

    window.setVerticalSyncEnabled(true);
    if (!window.setActive(true))
    {
        std::println(std::cerr, "Failed to activate the window.");
        return EXIT_FAILURE;
    }
    if (!gladLoadGL())
    {
        std::println(std::cerr, "Failed to initialise OpenGL - Is OpenGL linked correctly?");
        return EXIT_FAILURE;
    }
    glClearColor(50.0f / 255.0f, 205.0f / 255.0f, 250.0f / 255.0f, 0);
    glViewport(0, 0, window.getSize().x, window.getSize().y);
    glCullFace(GL_BACK);
    enableGLDebugging();

    //guiInit(window);

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
        //guiBeginFrame();

        while (auto event = window.pollEvent())
        {
            keyboard.update(*event);
            //guiProcessEvent(e);

            game.onEvent(*event);
            if (auto key = event->getIf<sf::Event::KeyReleased>())
            {
                if (key->code == sf::Keyboard::Key::Escape)
                {
                    window.close();
                }
                else if (key->code == sf::Keyboard::Key::L)
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

        game.onRender();
        game.onGUI();
        //guiEndFrame();

        window.display();
    }
    //guiShutdown();
}