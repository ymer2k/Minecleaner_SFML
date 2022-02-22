#include "FontHolder.h"

FontHolder::FontHolder()
{
}

void FontHolder::load(FontID id, const std::string& filename)
{
	std::unique_ptr<sf::Font> font(new sf::Font()); // Create a unique pointer of type sf::Texture and allocate space for a sf::Texture
	font->loadFromFile(filename); // load the texture from the given filename

	//Use this to remove the blurriness  (make sure the font size is exactly 10).
	// getTexture(10) is the font size.
	const_cast<sf::Texture&>(font->getTexture(15)).setSmooth(false); 

	// insert into map, move actually moves the texture pointer into the map and texture should become "empty".
	// we do this instead of copying.
	m_fontMap.insert(std::make_pair(id, std::move(font)));
}

sf::Font& FontHolder::get(FontID id)
{
	auto found = m_fontMap.find(id);
	return *found->second;
}

const sf::Font& FontHolder::get(FontID id) const
{
	auto found = m_fontMap.find(id);
	return *found->second;
}