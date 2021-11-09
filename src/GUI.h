#pragma once

#include <SFML/Window.hpp>

#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/VertexArray.h"
#include <glm/glm.hpp>

#include <nuklearpp/nuklear_def.h>
#include <nuklearpp/nuklear_sfml_gl3.h>

struct Transform;
struct TerrainGenOptions;
struct GraphicsOptions;

void guiInit(sf::Window& window);
void guiShutdown();

void guiBeginFrame();
void guiProcessEvent(sf::Event& event);
void guiEndFrame();

void guiDebugScreen(const Transform& transform);

struct nk_context* getGuiContext();

struct SpriteRenderer
{
  public:
    SpriteRenderer();
    void render(const Texture2D& texture, float x, float y, float width, float height);

  private:
    VertexArray m_guiQuad;
    Shader m_guiShader;
    Texture2D m_guiBorder;
};