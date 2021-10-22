#include "ChunkTerrainGen.h"
#include "Voxels.h"

void createChunkTerrain(Chunk& chunk, int chunkX, int chunkY, int worldWidth,
                        int worldHeight, const TerrainGenOptions& terrainGenOptions)
{
    int BASE_HEIGHT = worldHeight * CHUNK_SIZE - CHUNK_SIZE / 2;

    if (chunkY == worldHeight)
    {
    }

    for (int y = 0; y < CHUNK_SIZE; y++)
    {
        auto voxelHeight = CHUNK_SIZE * chunkY + y;

        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            // Top level
            if (voxelHeight > BASE_HEIGHT)
            {
                chunk.qSetVoxel({x, y, 0}, AIR);
                chunk.qSetVoxel({x, y, 1}, AIR);
            }

            // Base
            else if (voxelHeight == BASE_HEIGHT)
            {
                chunk.qSetVoxel({x, y, 0}, GRASS);
                chunk.qSetVoxel({x, y, 1}, GRASS);
            }

            // Upper dirt layer
            else if (voxelHeight > BASE_HEIGHT - 2)
            {
                chunk.qSetVoxel({x, y, 0}, DIRT);
                chunk.qSetVoxel({x, y, 1}, DIRT);
            }

            // Lower dirt layer
            else if (voxelHeight > BASE_HEIGHT - CHUNK_SIZE / 2)
            {
                chunk.qSetVoxel({x, y, 0}, DIRT);
                if (rand() % 64 < 40)
                {
                    chunk.qSetVoxel({x, y, 1}, DIRT);
                }
            }
            else if (voxelHeight > BASE_HEIGHT - CHUNK_SIZE)
            {
                chunk.qSetVoxel({x, y, 0}, DIRT);
                if (rand() % 64 > 30)
                {
                    chunk.qSetVoxel({x, y, 1}, DIRT);
                }
                else if (rand() % 64 > 10) {
                    chunk.qSetVoxel({x, y, 1}, STONE);
                }
            }

            // Stoney
            else
            {
                chunk.qSetVoxel({x, y, 0}, STONE);
                if (rand() % 64 < 40)
                {
                    chunk.qSetVoxel({x, y, 1}, STONE);
                }
            }
        }
    }
}
