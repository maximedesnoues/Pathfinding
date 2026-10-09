#include "GameConfig.h"
#include "Map.h"
#include <random>
#include "MapGenerator.h"
#include "EntityType.h"

Map::~Map()
{
	Release();
}

std::shared_ptr<Map> Map::Create()
{
	std::shared_ptr<Map> newShared = std::make_shared<Map>();
	newShared->thisWeakPtr = newShared;
	return newShared;
}

std::shared_ptr<IGameEntity> Map::GetSharedPtr()
{
	return thisWeakPtr.lock();
}

void Map::Init(const IGameEntityConfig* config)
{
	spriteSheet = SpriteFactory::GetSprite("Basis.png");
	width = MAP_WIDTH;
	height = MAP_HEIGHT;
	tileset = new std::shared_ptr<Tile>*[height];
	//generate a random map	
	for (int i = 0; i < height; ++i)
	{
		tileset[i] = new std::shared_ptr<Tile>[width];
		for (int j = 0; j < width; ++j)
		{
			TileType kind = TileType::Destroyed;
			int val = rand() %100;
			if (val < 40)
				kind = TileType::Grass;
			else if (val < 60)
				kind = TileType::Dirt;
			else if (val < 80)
				kind = TileType::HighGrass;

			tileset[i][j] = std::make_shared<Tile>(tileConfigs[kind], j, i);
		}
	}
	MapGenerator::GenerateMap(thisWeakPtr.lock());
}

void Map::Release()
{
	if (tileset != nullptr)
	{
		for (int i = 0; i < height; ++i)
		{
			delete[] tileset[i];
		}
		delete[] tileset;
		tileset = nullptr;
	}
}

void Map::Update(int deltaTime)
{
}

void Map::Render(SDL_Renderer* screenRenderer)
{
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			tileset[i][j]->Render(screenRenderer, spriteSheet.get(), j * TILE_SIZE + (TILE_SIZE >> 1), i * TILE_SIZE + (TILE_SIZE >> 1));
		}
	}
}

EntityType Map::GetEntityType() const
{
	return EntityType::Map;
}

unsigned int Map::GetTileCost(int x, int y)
{
	if (x < 0 || y < 0 || x >= width || y >= height)
	{
		return 0xFFFFFFFF;
	}
	return tileset[y][x]->GetCost();
}

TileType Map::GetTileType(int x, int y)
{
	if (x < 0 || y < 0 || x >= width || y >= height)
	{
		return TileType::Destroyed;
	}
	return tileset[y][x]->GetType();
}

void Map::SetTileType(int x, int y, TileType type)
{
	if (x < 0 || y < 0 || x >= width || y >= height)
		return;

	tileset[y][x] = std::make_shared<Tile>(
		tileConfigs.at(type), x, y
	);
}