#pragma once
#include <iostream>
#include <map>
#include <memory>
#include <SFML/Graphics.hpp>
// class structure taken from SFML Game Development book

class TextureHolder
{

public:
	TextureHolder();

	enum class ID
	{
		COMBINED
		,SMILEY_COMBINED
		, NUMBERS
	};

	// given the ID and filename we "load" the map with the requested texture.
	void load(ID id, const std::string& filename);
	// overloading load to take a image as a 2nd parameter
	void load(ID id, const sf::Image image);
	// A get function to get the approprite texture depending on the ID we pass in.
	sf::Texture& get(ID id);
	//a const qualified overload 
	const sf::Texture& get(ID id) const;

private:
	// a map that contains pointers to sf::Textures (key is the ID)
	std::map<ID, std::unique_ptr<sf::Texture>> m_textureMap;
};

