#include "BaseSprite.h"
#include <SFML/Graphics.hpp>
#include "TextureHolder.h"

BaseSprite::BaseSprite(TextureHolder& texture):
	m_textureHolder(texture)
{
	m_id = BaseSprite::cellId::UNPRESSED;
	m_textureSize = m_textureHolder.get(TextureHolder::ID::COMBINED).getSize();
	m_pieceTextureSize.x = m_textureSize.x / 4; // combined texture has 4 rows
	m_pieceTextureSize.y = m_textureSize.y / 4; // combined texture has 4 collumns
}

sf::Sprite BaseSprite::getSprite()
{
	return m_sprite;
}

void BaseSprite::setPosition(sf::Vector2f pos)
{
	m_pos = pos;
	m_sprite.setPosition(pos.x, pos.y);
}

sf::Vector2u BaseSprite::getTexturePixelSize()
{
	return m_pieceTextureSize;
}

void BaseSprite::loadSprite(TextureHolder::ID id)
{
	//For sprite 
	m_sprite.setTexture(m_textureHolder.get(id));
}

void BaseSprite::loadTransparentSprite()
{
	m_sprite.setTexture(m_texture);
}

void BaseSprite::setScale(float x, float y)
{
	m_sprite.scale(x, y);
}

void BaseSprite::setOrigin(float x, float y)
{
	m_sprite.setOrigin(x, y);
}

void BaseSprite::setTextureRectFunc(sf::IntRect intRect)
{
	m_sprite.setTextureRect(intRect);
}

void BaseSprite::setId(cellId id)
{
	m_id = id;
}

BaseSprite::cellId BaseSprite::getId()
{
	return m_id;
}

void BaseSprite::setSprite(BaseSprite::cellId type)
{
	switch (type)
	{
	case cellId::UNPRESSED:
		m_sprite.setTextureRect(sf::IntRect(0, 0, m_pieceTextureSize.x, m_pieceTextureSize.y));
		break;
	case cellId::PRESSED:
		m_sprite.setTextureRect(sf::IntRect(32, 0, m_pieceTextureSize.x, m_pieceTextureSize.y));
		break;
	case cellId::DETONATED:
		m_sprite.setTextureRect(sf::IntRect(32*2, 0, m_pieceTextureSize.x, m_pieceTextureSize.y));
		break;
	case cellId::MINE:
		m_sprite.setTextureRect(sf::IntRect(32 * 3, 0, m_pieceTextureSize.x, m_pieceTextureSize.y));
		break;
	case cellId::FLAG:
		m_sprite.setTextureRect(sf::IntRect(32 * 0, 32, m_pieceTextureSize.x, m_pieceTextureSize.y));
		break;
	case cellId::ONE:
		m_sprite.setTextureRect(sf::IntRect(32 * 1, 32, m_pieceTextureSize.x, m_pieceTextureSize.y));
		break;
	case cellId::TWO:
		m_sprite.setTextureRect(sf::IntRect(32 * 2, 32, m_pieceTextureSize.x, m_pieceTextureSize.y));
		break;
	case cellId::THREE:
		m_sprite.setTextureRect(sf::IntRect(32 * 3, 32, m_pieceTextureSize.x, m_pieceTextureSize.y));
		break;
	case cellId::FOUR:
		m_sprite.setTextureRect(sf::IntRect(32 * 0, 32 * 2, m_pieceTextureSize.x, m_pieceTextureSize.y));
		break;
	case cellId::FIVE:
		m_sprite.setTextureRect(sf::IntRect(32 * 1, 32 * 2, m_pieceTextureSize.x, m_pieceTextureSize.y));
		break;
	case cellId::SIX:
		m_sprite.setTextureRect(sf::IntRect(32 * 2, 32 * 2, m_pieceTextureSize.x, m_pieceTextureSize.y));
		break;
	case cellId::SEVEN:
		m_sprite.setTextureRect(sf::IntRect(32 * 3, 32 * 2, m_pieceTextureSize.x, m_pieceTextureSize.y));
		break;
	case cellId::EIGHT:
		m_sprite.setTextureRect(sf::IntRect(32 * 0, 32 * 3, m_pieceTextureSize.x, m_pieceTextureSize.y));
		break;
	case cellId::FALSE:
		m_sprite.setTextureRect(sf::IntRect(32 * 1, 32 * 3, m_pieceTextureSize.x, m_pieceTextureSize.y));
		break;
	default:
		break;
	}
	setId(type); // Also set the id !
}
