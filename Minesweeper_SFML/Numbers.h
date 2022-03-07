#pragma once
#include "BaseSprite.h"


class Numbers
{
public:
	Numbers(TextureHolder& texture);

	sf::Sprite getSprite(); // position is in here
	void setPosition(sf::Vector2f pos);
	//void setTexture(TextureHolder& texture);
	sf::Vector2u getTexturePixelSize();
	void loadSprite(TextureHolder::ID id);
	void loadTransparentSprite();
	void setScale(float x, float y);
	void setOrigin(float x, float y);
	void setTextureRectFunc(sf::IntRect intRect);

	enum class numberId
	{
		
		 ZERO
		, ONE
		, TWO
		, THREE
		, FOUR
		, FIVE
		, SIX
		, SEVEN
		, EIGHT
		, NINE
		, DASH
		, NO_NUMBER

	};
	void setNumberSprite(numberId type);

private:
	// member variables
	TextureHolder& m_textureHolder; // References Must be initilized on creation.
	sf::Image m_image;
	sf::Sprite m_sprite;
	sf::Texture m_texture;
	sf::Vector2u m_pieceTextureSize;
	sf::Vector2u m_textureSize;
	sf::Vector2f m_pos;

	// member functions
};

