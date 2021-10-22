#pragma once

#include "Chunks/ChunkMap.h"
#include "Graphics/GLWrappers.h"

class Camera;

class World
{
  public:
    World();

    void update();
    void render(const Camera& camera);

    void breakBlock(int x, int y);
    void placeBlock(int x, int y, VoxelID id);

    int getLightLevel(int x, int y);

  private:
    ChunkMap m_chunkMap;

    TextureArray2D m_chunkTextures;
    std::unordered_map<ChunkPosition, VertexArray, ChunkPositionHash> m_chunkRendersList;
    std::vector<VertexArray> m_chunkVertexArrays;
    Shader m_voxelShader;
};
