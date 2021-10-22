#pragma once

#include "Chunks/ChunkMap.h"
#include "Graphics/GLWrappers.h"
#include <atomic>
#include <thread>
#include <mutex>
#include <set>

class Camera;

class World
{
  public:
    World();
    ~World();

    void update();
    void render(const Camera& camera);

    void breakBlock(int x, int y);
    void placeBlock(int x, int y, VoxelID id);

    int getLightLevel(int x, int y);

  private:
    void doChunkBuildThread();

    ChunkMap m_chunkMap;

    TextureArray2D m_chunkTextures;
    std::unordered_map<ChunkPosition, VertexArray, ChunkPositionHash> m_chunkRendersList;
    std::vector<VertexArray> m_chunkVertexArrays;
    std::unordered_set<ChunkPosition, ChunkPositionHash> m_chunkBuildQueue;
    std::unordered_map<ChunkPosition, ChunkMesh, ChunkPositionHash> m_chunkBufferQueue;
    Shader m_voxelShader;

    std::atomic<bool> m_isRunning{true};
    std::thread m_chunkBuildThread;
    std::mutex m_lock;
};
