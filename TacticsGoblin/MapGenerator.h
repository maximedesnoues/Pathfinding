#pragma once
#include <memory>

class Map;
enum TileType;

class MapGenerator
{
protected:
	static void Fill(std::shared_ptr<Map> map, int startX, int startY, int endX, int endY, TileType tileType);
	static void AddWalkingRandomDir(std::shared_ptr<Map> map, int startX, int startY, int length, int thickness, TileType tileType);
	static void AddWalkingTo(std::shared_ptr<Map> map, int startX, int startY, int endX, int endY, int thickness, TileType tileType);

	static void GenerateRiver(std::shared_ptr<Map> map);
	static void AddBridges(std::shared_ptr<Map> map);
	static void AddRiverBanks(std::shared_ptr<Map> map);
public:
	static void GenerateMap(std::shared_ptr<Map> map);
	
};

