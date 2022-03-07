#include "Numbers.h"
#include<string>  

Numbers::Numbers(TextureHolder& texture):
	m_textureHolder(texture)
{
}

void Numbers::setNumberSprite(numberId type)
{
	int width = m_pieceTextureSize.x; //13
	int height = m_pieceTextureSize.y; //23
	int offset = 0;
	switch (type)
	{
	case numberId::ONE:
		setTextureRectFunc(sf::IntRect(width * 0, 0, width, height));
		break;
	case numberId::TWO:
		setTextureRectFunc(sf::IntRect(width * 1 + 1, 0, width, height));
		break;
	case numberId::THREE:
		setTextureRectFunc(sf::IntRect(width * 2 + 2, 0, width, height));
		break;
	case numberId::FOUR:
		setTextureRectFunc(sf::IntRect(width * 3 + 3, 0, width, height));
		break;
	case numberId::FIVE:
		setTextureRectFunc(sf::IntRect(width * 4 + 4, 0, width, height));
		break;
	case numberId::SIX:
		setTextureRectFunc(sf::IntRect(width * 5 + 5, 0, width, height));
		break;
	case numberId::SEVEN:
		setTextureRectFunc(sf::IntRect(width * 6 + 6, 0, width, height));
		break;
	case numberId::EIGHT:
		setTextureRectFunc(sf::IntRect(width * 7 + 7, 0, width, height));
		break;
	case numberId::NINE:
		setTextureRectFunc(sf::IntRect(width * 8 + 8, 0, width, height));
		break;
	case numberId::ZERO:
		setTextureRectFunc(sf::IntRect(width * 9 + 9, 0, width, height));
		break;
	case numberId::DASH:
		setTextureRectFunc(sf::IntRect(width * 10 + 10, 0, width, height));
		break;
	case numberId::NO_NUMBER:
		setTextureRectFunc(sf::IntRect(width * 11 + 11, 0, width, height));
		break;
	default:
		break;
	}
}

sf::Sprite Numbers::getSprite()
{
	return m_sprite;
}

void Numbers::setPosition(sf::Vector2f pos)
{
	m_pos = pos;
	m_sprite.setPosition(pos.x, pos.y);
}

sf::Vector2u Numbers::getTexturePixelSize()
{
	return m_pieceTextureSize;
}

void Numbers::loadSprite(TextureHolder::ID id)
{
	//For sprite 
	m_sprite.setTexture(m_textureHolder.get(id));
	m_textureSize = m_textureHolder.get(TextureHolder::ID::NUMBERS).getSize();
	m_pieceTextureSize.x = (m_textureSize.x - 11) / 12; // 11 one pixels columns.12 textures
	m_pieceTextureSize.y = m_textureSize.y; // combined texture has 4 collumns
}

void Numbers::loadTransparentSprite()
{
	m_sprite.setTexture(m_texture);
}

void Numbers::setScale(float x, float y)
{
	m_sprite.scale(x, y);
}

void Numbers::setOrigin(float x, float y)
{
	//std::cout << m_sprite.getOrigin().x << std::endl;
	m_sprite.setOrigin(x, y);
}

void Numbers::setTextureRectFunc(sf::IntRect intRect)
{
	m_sprite.setTextureRect(intRect);
}
