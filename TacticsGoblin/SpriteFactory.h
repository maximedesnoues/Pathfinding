#pragma once
#include <memory>
#include <string>
#include <map>
#include <SDL_render.h>

class Sprite;
class Font;

class SpriteFactory
{
private:
	//only static funcs mean we can remove all
	SpriteFactory() = delete;
	~SpriteFactory() = delete;
	SpriteFactory(const SpriteFactory&) = delete;
	SpriteFactory(SpriteFactory&&) = delete;
	SpriteFactory& operator=(const SpriteFactory&) = delete;

	static std::map<std::string, std::shared_ptr<Sprite>> _loadedSprites;
	static std::map<std::string, std::shared_ptr<Font>> _loadedFonts;
	static std::string _resourcePath;
	static SDL_Renderer* _globalRenderer;

public:
	static void SetResourcePath(const std::string& path);
	static void SetSDLRenderer(SDL_Renderer* globalRenderer);

	static std::shared_ptr<Sprite> GetSprite(const std::string& name);
	static std::shared_ptr<Font> GetFont(const std::string& name, int size);

	static void ReleaseAll();
};
