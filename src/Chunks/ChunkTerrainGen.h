#pragma once

#include "Chunk.h"

struct TerrainGenOptions
{
    int octaves = 8;
    float amplitude = 230.0f;
    float smoothness = 500.0f;
    float roughness = 0.58f;
    float offset = 0;
    int seed = 0;
};

void createChunkTerrain(Chunk& chunk, int chunkX, int chunkY, int worldWidth,
                        int worldHeight, const TerrainGenOptions& terrainGenOptions);