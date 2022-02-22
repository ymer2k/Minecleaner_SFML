#pragma once
#include <SFML/Graphics.hpp>
#include "TextureHolder.h"

class BaseSprite
{


public:
	BaseSprite(TextureHolder & texture);

	sf::Sprite getSprite(); // position is in here
	void setPosition(sf::Vector2f pos);
	//void setTexture(TextureHolder& texture);
	sf::Vector2u getTexturePixelSize();
	void loadSprite(TextureHolder::ID id);
	void loadTransparentSprite();
	void setScale(float x, float y);
	void setOrigin(float x, float y);

	
	enum class cellId
	{
		NO_ID
		, ONE
		, TWO
		, THREE
		, FOUR
		, FIVE
		, SIX
		, SEVEN
		, EIGHT
		, UNPRESSED
		, PRESSED
		, DETONATED
		, MINE
		, FLAG
		, FALSE

	};

	void setId(cellId id);
	cellId getId();

	void setSprite(BaseSprite::cellId type);





private:

	// member functions

	// member variables

	TextureHolder& m_textureHolder; // References Must be initilized on creation.

	sf::Image m_image;
	sf::Sprite m_sprite;
	sf::Texture m_texture;
	sf::Vector2u m_pieceTextureSize;
	sf::Vector2u m_textureSize;

	sf::Vector2f m_pos;
	cellId m_id;

};

