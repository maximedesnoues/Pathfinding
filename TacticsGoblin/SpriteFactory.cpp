#include "SpriteFactory.h"
#include "Sprite.h"
#include "Font.h"
#include <SDL_image.h>
#include <iostream>

std::map<std::string, std::shared_ptr<Sprite>> SpriteFactory::_loadedSprites;
std::map<std::string, std::shared_ptr<Font>> SpriteFactory::_loadedFonts;
std::string SpriteFactory::_resourcePath = "";
SDL_Renderer* SpriteFactory::_globalRenderer = nullptr;

void SpriteFactory::SetResourcePath(const std::string& path)
{
    _resourcePath = path;
}

void SpriteFactory::SetSDLRenderer(SDL_Renderer* globalRenderer)
{
    _globalRenderer = globalRenderer;
}

std::shared_ptr<Sprite> SpriteFactory::GetSprite(const std::string& name)
{
	//Search for an already loaded one
	auto searchResult = _loadedSprites.find(name);
	if (searchResult != _loadedSprites.end())
		return searchResult->second;

    //Load image at specified path
    SDL_Texture* spriteImage;
    SDL_Surface* loadedSurface = IMG_Load((_resourcePath +name).c_str());
    if (loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n", name.c_str(), IMG_GetError());
    }
    else
    {
        //Create texture from surface pixels
        spriteImage = SDL_CreateTextureFromSurface(_globalRenderer, loadedSurface);
        if (spriteImage == NULL)
        {
            printf("Unable to create texture from %s! SDL Error: %s\n", name.c_str(), SDL_GetError());
        }

        //Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }



	std::shared_ptr<Sprite> newSprite = std::make_shared<Sprite>(spriteImage);
	_loadedSprites[name] = newSprite;

	return newSprite;
}

std::shared_ptr<Font> SpriteFactory::GetFont(const std::string& name, int size)
{
    std::string key = name + "#" + std::to_string(size);
    //Search for an already loaded one
    auto searchResult = _loadedFonts.find(key);
    if (searchResult != _loadedFonts.end())
        return searchResult->second;

    std::shared_ptr<Font> newFont = std::make_shared<Font>(name, size);
    _loadedFonts[key] = newFont;

    return newFont;

}

void SpriteFactory::ReleaseAll()
{
    for (auto& element : _loadedSprites)
    {
        if (element.second.use_count() > 1)
        {
            std::cout << "Sprite released while still being used: " + element.first << " (" << element.second.use_count() << ")" << std::endl;
        }
    }
    _loadedSprites.clear();

    for (auto& element : _loadedFonts)
    {
        if (element.second.use_count() > 1)
        {
            std::cout << "Font released while still being used: " + element.first << " (" << element.second.use_count() << ")" << std::endl;
        }
    }
    _loadedFonts.clear();
}

