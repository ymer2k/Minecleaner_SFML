#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <vector>
#include "BaseSprite.h"
#include "FontHolder.h"
#include "TextureHolder.h"


class Game
{
public:
	Game();

	void update(sf::Time dt, sf::RenderWindow& window);
	void drawGame(sf::RenderWindow& window);
	void interact(sf::RenderWindow& window);


private:

	enum class gameStates
	{
		chooseDifficulty, // Easy 10x8 // medium 15,12, hard 20x16,  expert 25x20
		gamePlay,
		gameOver,
	};

	
	sf::Vector2f m_worldMousePosClick;
	sf::Vector2i m_worldMousePos;

	sf::Vector2f m_worldMousePosClickRight;

	enum gameStates m_state;

	sf::Vector2i m_boardSize;

	int m_numberOfMines;

	// Used for the floodFill to see if square has been visited by the algo. Should be OK to use this vector to decide which cells to reveal.
	std::vector<bool> m_visited;

	// 
	

	// Vector to keep all cells that will be drawn.
	std::vector<BaseSprite> m_cellVector;
	FontHolder m_fonts;
	TextureHolder m_textureHolder;
	//Hold all Text objects
	std::vector<sf::Text> m_textVector;

	sf::Text m_text;

	// Vector that hold where the mines are located
	std::vector<BaseSprite::cellId> m_cellTypeVector;

	// Shape outline to draw smiley, score and time
	sf::RectangleShape m_topBar;

	// smiley sprite
	sf::Sprite m_smileySprite;

	// mine Number sprites
	sf::Sprite m_100Number;
	sf::Sprite m_10Number;
	sf::Sprite m_1Number;

	
	// amount of flags placed
	int m_nrOfFlags;


	// METHODS

	// Check if mousecoords are within the bounds
	bool checkBounds(sf::Vector2f mouseCoords, sf::FloatRect bounds);
	bool checkBounds(sf::Vector2i mouseCoords, sf::FloatRect bounds);

	// Used to initilize the cell vector with the correct nr of Cells for the selected difficulty.
	void initCellVector();

	//init smiley
	void initSmiley();

	// Transform 2d vector to 1d
	int convertIndexFor1dVector(int x, int y);

	// Generate mines (input Boardsize, 
	void generateMines();

	// Generate other cells after mines has been generated
	void generateRemaningCells();

	// return number of mines surroinding the index (Not using this function because too much work to do boundary checks!
	int giveSurroundingMineCount(int index);

	// 2d  return number of mines surroinding the index. 2D version, easier to do boundary checks :)
	int giveSurroundingMineCount(int x, int y);

	// draw cell according to m_cellVector
	void drawClickedCell(int index);

	// FLood fill algorithm
	bool floodFill(BaseSprite::cellId target, int x, int y);

	// make all connected clicked "empty" cells visible
	void makeEmptyCellsVisible(int index);

	// reveal all mines
	void revealMines();

	// Reset for new game
	void resetForNewGame();

	// put flags on remaining cells
	void putFlagsOnRemaningCells();

	// init numbers
	void initNumbers();

	// Choose right number go








};
