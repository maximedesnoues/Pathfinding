#include <time.h>


#include "MapGenerator.h"
#include "Map.h"
#include "Tile.h"
#include <algorithm>


void MapGenerator::GenerateMap(std::shared_ptr<Map> map)
{
	//srand(time(NULL));
	int width = map->GetWidth();
	int height = map->GetHeight();
	Fill(map, 0, 0, width, height, TileType::Grass);

	int tileBlockCount = 5 + rand() % 15;
	for (int i = 0; i < tileBlockCount; ++i)
	{
		AddWalkingRandomDir(map, rand() % width, rand() % height, 5 + rand() % 15,1 + rand() % 3, TileType::HighGrass);
	}
	
	tileBlockCount = 5 + rand() % 15;
	for (int i = 0; i < tileBlockCount; ++i)
	{
		AddWalkingRandomDir(map, rand() % width, rand() % height, 5 + rand() % 15, 1 + rand() % 3, TileType::Dirt);
	}

	tileBlockCount = 2 + rand() % 5;
	for (int i = 0; i < tileBlockCount; ++i)
	{
		AddWalkingRandomDir(map, rand() % width, rand() % height, 2 + rand() % 12, 1 + rand() % 3, TileType::Sand);
	}

	tileBlockCount = 2 + rand() % 5;
	for (int i = 0; i < tileBlockCount; ++i)
	{
		AddWalkingRandomDir(map, rand() % width, rand() % height, 2 + rand() % 3, 1 + rand() % 3, TileType::Swamp);
	}

	for (int i = 0; i < 5; ++i)
	{
		AddWalkingRandomDir(map, rand() % width, rand() % height, 3 + rand() % 6, 1, TileType::Trees);
		AddWalkingRandomDir(map, rand() % width, rand() % height, 1 + rand() % 3, 1, TileType::Rock);
		AddWalkingRandomDir(map, rand() % width, rand() % height, 1 + rand() % 3, 1, TileType::Mountain);
	}

	//river
	GenerateRiver(map);
	AddBridges(map);
	AddRiverBanks(map);
	
}

void MapGenerator::GenerateRiver(std::shared_ptr<Map> map)
{

	int x = map->GetWidth() / 2;
	int y = 0;

	while (y < map->GetHeight() - 1)
	{
		int nextY = std::min(
			y + 5 + rand() % 5,
			map->GetHeight() - 1
		);

		int nextX = std::clamp(
			x + rand() % 11 - 5,
			4,
			map->GetWidth() - 5
		);

		AddWalkingTo(map, x, y, nextX, nextY, 3, TileType::Water);

		x = nextX;
		y = nextY;
	}
}

void MapGenerator::AddBridges(std::shared_ptr<Map> map)
{
	int count = 2 + rand() % 2;
	int delta = map->GetHeight() / (count + 1);
	int width = map->GetWidth();

	for (int j = 1; j <= count; ++j)
	{
		int y = j * delta - 2 + rand() % 5;
		for (int i = 0; i < width; ++i)
		{
			if (map->GetTile(i, y)->GetType() == TileType::Water)
				map->SetTileType(i, y, TileType::Bridge);
		}

	}
}

void MapGenerator::AddRiverBanks(std::shared_ptr<Map> map)
{
	for (int y = 0; y < map->GetHeight(); ++y)
	{
		for (int x = 0; x < map->GetWidth(); ++x)
		{
			if (map->GetTileType(x, y) != TileType::Grass)
				continue;

			bool nearWater =
				map->GetTileType(x - 1, y) == TileType::Water ||
				map->GetTileType(x + 1, y) == TileType::Water ||
				map->GetTileType(x, y - 1) == TileType::Water ||
				map->GetTileType(x, y + 1) == TileType::Water;

			if (nearWater && rand() % 100 < 60)
				map->SetTileType(x, y, TileType::HighGrass);
		}
	}
}

void MapGenerator::Fill(std::shared_ptr<Map> map, int startX, int startY, int endX, int endY, TileType tileType)
{
	for (int i = startX; i < endX; ++i)
	{
		for (int j = startY; j < endY; ++j)
		{
			map->SetTileType(i, j, tileType);
		}
	}
}

void MapGenerator::AddWalkingRandomDir(std::shared_ptr<Map> map, int startX, int startY, int length, int thickness, TileType tileType)
{
	int deltaX = 0;
	int deltaY = 0;
	for (int i = 0; i < length; ++i)
	{
		for (int dy = -thickness/2; dy <= thickness/2; ++dy)
		{
			for (int dx = -thickness/2; dx <= thickness/2; ++dx)
			{
				int x = startX + dx;
				int y = startY + dy;

				if (x >= 0 && x < map->GetWidth() &&
					y >= 0 && y < map->GetHeight())
				{
					map->SetTileType(x, y, tileType);
				}
			}
		}
		if (rand() % 100 < 35 || (deltaX == 0 && deltaY == 0))
		{
			int val = rand() % 4;
			switch (val)
			{
			case 0:
				deltaX = 1;
				deltaY = 0;
				break;
			case 1:
				deltaX = -1;
				deltaY = 0;
				break;
			case 2:
				deltaX = 0;
				deltaY = 1;
				break;
			case 3:
				deltaX = 0;
				deltaY = -1;
				break;
			}
		}
		startX = std::clamp(startX + deltaX, 0, map->GetWidth() - 1);
		startY = std::clamp(startY + deltaY, 0, map->GetHeight() - 1);
	}
}

void MapGenerator::AddWalkingTo(std::shared_ptr<Map> map, int startX, int startY, int endX, int endY, int thickness, TileType tileType)
{
	int currentX = startX;
	int currentY = startY;

	int steps = 0;
	int maxRandomSteps = 2 * (
		abs(endX - startX) + abs(endY - startY)
		);

	while (currentX != endX || currentY != endY)
	{
		for (int dy = -thickness / 2; dy <= thickness / 2; ++dy)
		{
			for (int dx = -thickness / 2; dx <= thickness / 2; ++dx)
			{
				int x = currentX + dx;
				int y = currentY + dy;

				if (x >= 0 && x < map->GetWidth() &&
					y >= 0 && y < map->GetHeight())
				{
					map->SetTileType(x, y, tileType);
				}
			}
		}
		
		int moveX = 0;
		int moveY = 0;
		if(currentX != endX)
			moveX = (endX < currentX)? -1 : 1;
		if (currentY != endY)
			moveY = (endY < currentY) ? -1 : 1;

		if (rand() % 100 < 25 && steps < maxRandomSteps)
		{
			int val = rand() % 4;
			switch (val)
			{
			case 0:
				moveX = 1;
				break;
			case 1:
				moveX = -1;
				break;
			case 2:
				moveY = 1;
				break;
			case 3:
				moveY = -1;
				break;
			}
		}
		
		currentX = std::clamp(currentX + moveX, 0, map->GetWidth() - 1);
		currentY = std::clamp(currentY + moveY, 0, map->GetHeight() - 1);

		++steps;

	}
}