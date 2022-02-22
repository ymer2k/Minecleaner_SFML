#pragma once
#include <iostream>
#include <map>
#include <memory>
#include <SFML/Graphics.hpp>


class FontHolder
{
public :
	FontHolder();

	enum class FontID
	{
		PixelFont
	};

	// given the ID and filename we "load" the map with the requested texture.
	void load(FontID id, const std::string& filename);
	// A get function to get the approprite texture depending on the ID we pass in.
	sf::Font& get(FontID id);
	//a const qualified overload 
	const sf::Font& get(FontID id) const;


private:
	// a map that contains pointers to sf::Textures (key is the ID)
	std::map<FontID, std::unique_ptr<sf::Font>> m_fontMap;
};

