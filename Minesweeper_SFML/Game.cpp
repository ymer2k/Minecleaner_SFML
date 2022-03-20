#include "Game.h"
#include "math.h"
#include <iostream>
#include <stdlib.h>
#include "Numbers.h"

Game::Game():
    m_state(Game::gameStates::chooseDifficulty)
    , m_worldMousePosClickRight(-1.0f,-1.0f)
    , m_gameRegionMousePosClick(-1.0f,-1.0f)
    , m_gameIsLost(false)
    , m_1numberMine(m_textureHolder)
    , m_10numberMine(m_textureHolder)
    , m_100numberMine(m_textureHolder)
    , m_1numberTime(m_textureHolder)
    , m_10numberTime(m_textureHolder)
    , m_100numberTime(m_textureHolder)
{
    //Load text font
    m_fonts.load(FontHolder::FontID::PixelFont, "Fonts/slkscre.ttf");

    //Huge Title text

    sf::Text easy;
    easy.setFont(m_fonts.get(FontHolder::FontID::PixelFont));
    easy.setPosition(100, 100);
    easy.setFillColor(sf::Color::White);
    easy.setString("Easy");
    easy.setCharacterSize(15);
    m_textVector.emplace_back(easy);

    sf::Text medium;
    medium.setFont(m_fonts.get(FontHolder::FontID::PixelFont));
    medium.setPosition(100, 200);
    medium.setFillColor(sf::Color::White);
    medium.setString("Medium");
    medium.setCharacterSize(15);
    m_textVector.emplace_back(medium);

    sf::Text hard;
    hard.setFont(m_fonts.get(FontHolder::FontID::PixelFont));
    hard.setPosition(100, 300);
    hard.setFillColor(sf::Color::White);
    hard.setString("Hard");
    hard.setCharacterSize(15);
    m_textVector.emplace_back(hard);

    sf::Text expert;
    expert.setFont(m_fonts.get(FontHolder::FontID::PixelFont));
    expert.setPosition(100, 400);
    expert.setFillColor(sf::Color::White);
    expert.setString("Expert");
    expert.setCharacterSize(15);
    m_textVector.emplace_back(expert);

    // Setup Cell vector
    m_textureHolder.load(TextureHolder::ID::COMBINED, "textures/combined.png");

    //Setup smiley texture
    m_textureHolder.load(TextureHolder::ID::SMILEY_COMBINED, "textures/smileyCombined.png");

    // Setup numbers texture
    m_textureHolder.load(TextureHolder::ID::NUMBERS, "textures/numbers.png");

    // Setup title texture
    m_textureHolder.load(TextureHolder::ID::TITLE, "textures/title.png");

    // set title texture
    m_titleSprite.setTexture(m_textureHolder.get(TextureHolder::ID::TITLE));
    m_titleSprite.setPosition(sf::Vector2f(250, 250));

}

void Game::update(sf::Time dt, sf::RenderWindow& window)
{
    switch (m_state)
    {
    case Game::gameStates::chooseDifficulty:
    {
        // Put into function choose difficulty
        m_worldMousePos = sf::Mouse::getPosition(window); // Current mouse position

        for (auto &text : m_textVector)
        {
            if (checkBounds(m_worldMousePos, text.getGlobalBounds()))
            {
                text.setFillColor(sf::Color::Red);
            }
            else
            {
                text.setFillColor(sf::Color::White);
            }
            if (checkBounds(m_worldMousePosClick, text.getGlobalBounds()))
            {
                m_worldMousePosClick = sf::Vector2f(-1, -1); // reset
                sf::String string =  text.getString();
                if (string == "Easy") // 10% mines i.e. 8 mines
                {
                    window.setSize(sf::Vector2u(320, 256+60));
                    m_boardSize.x = 10;
                    m_boardSize.y = 8;
                    m_numberOfMines = 8;
                }
                else if (string == "Medium") // 15% mines i.e. 27 mines
                {
                    window.setSize(sf::Vector2u(480, 384+60));
                    m_boardSize.x = 15;
                    m_boardSize.y = 12;
                    m_numberOfMines = 27;
                }
                else if (string == "Hard")
                {
                    window.setSize(sf::Vector2u(640, 512+60)); //20% mines i.e. 64 mines 
                    m_boardSize.x = 20;
                    m_boardSize.y = 16;
                    m_numberOfMines = 64;
                }
                else if (string == "Expert") // 20% mines i.e. 100 mines
                {
                    window.setSize(sf::Vector2u(800, 640+60));
                    m_boardSize.x = 25;
                    m_boardSize.y = 20;
                    m_numberOfMines = 100;
                }
                //resize view This will set the size of the current view to the whole window!
                sf::FloatRect visibleArea(0, 0, window.getSize().x, window.getSize().y);
                window.setView(sf::View(visibleArea));
                m_wholeWindowView = window.getView(); // Since the current view (the default view) is already set we take that size and copy it to m_wholeWindowView

                m_visited.resize(m_boardSize.x * m_boardSize.y);
                initCellVector();
                initSmiley();
                m_cellTypeVector.resize(m_boardSize.x * m_boardSize.y);
                generateMines();
                generateRemaningCells();
                initNumbers(window);
                initTopBar(window);
                m_timer.restart();
                m_state = Game::gameStates::gamePlay;
                return;
            }
        }
        break;
    }

    case Game::gameStates::gamePlay:
    {
        int timeElapsed = (int)m_timer.getElapsedTime().asSeconds();
        m_worldMousePos = sf::Mouse::getPosition(window); // Current mouse position

        // change smiley texture when we click
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            m_smileySprite.setTextureRect(sf::IntRect(48, 48, 48, 48));
            
        }else
            m_smileySprite.setTextureRect(sf::IntRect(0, 0, 48, 48));

        static int unpressedCells = 0;
        // Check which cell we click on 
        m_nrOfFlags = 0;
        for (int i = 0; i < (m_boardSize.x * m_boardSize.y); i++)
        {
            if (m_cellVector[i].getId() == BaseSprite::cellId::FLAG)
                m_nrOfFlags++;
            
            if (checkBounds(m_gameRegionMousePosClick, m_cellVector[i].getSprite().getGlobalBounds())) // Look for the cell we clicked on
            {
                // If we pressed on an empty square that has cell is PRESSED then we should reveal all connecting PRESSED squares!
                if(m_cellTypeVector[i] == BaseSprite::cellId::PRESSED)
                {
                    makeEmptyCellsVisible(i);
                }
                else if (m_cellTypeVector[i] == BaseSprite::cellId::MINE)
                {
                    // Game Over
                    // Set the index to detonated mine 
                    // Count final nr of flags for Numbers
                    // Reveal the remaning bombs
                    // Mark flags placed wrong
                    // move to Game over Screen
                    m_nrOfFlags = 0;
                    for (int m = 0; m < (m_boardSize.x * m_boardSize.y); m++)
                    {
                        // count nr of flags to correctly display estimated number of mines left numbers when game over.
                        if (m_cellVector[m].getId() == BaseSprite::cellId::FLAG)
                            m_nrOfFlags++;
                    }
                    revealMines();
                    m_cellTypeVector[i] = BaseSprite::cellId::DETONATED;
                    m_gameIsLost = true;
                    // Set dead smiley
                    m_smileySprite.setTextureRect(sf::IntRect(48, 0, 48, 48));
                    m_state = Game::gameStates::gameOver;
                    for (int m = 0; m < (m_boardSize.x * m_boardSize.y); m++)
                    {
                        // Look for false flags
                        if (m_cellVector[m].getId() == BaseSprite::cellId::FLAG && m_cellTypeVector[m] != BaseSprite::cellId::MINE)
                            m_cellVector[m].setSprite(BaseSprite::cellId::FALSE);
                    }                 
                }
                m_cellVector[i].setSprite(m_cellTypeVector[i]); // Set the correct sprite of the clicked cell.
                m_gameRegionMousePosClick = sf::Vector2f(-1, -1); //reset
                break; // stop the loop since we found the one we clicked on
            }
            // check if we riht clicked on a cell
            if (checkBounds(m_worldMousePosClickRight, m_cellVector[i].getSprite().getGlobalBounds()))
            {
                // If there is already a flag, just remove it and replace with the unpressed sprite.
                if (m_cellVector[i].getId() == BaseSprite::cellId::FLAG)
                {                    
                    m_cellVector[i].setSprite(BaseSprite::cellId::UNPRESSED);
                    m_worldMousePosClickRight = sf::Vector2f(-1, -1); //reset
                    break;
                }
                // Only place flags on UNPRESSED cells
                else if (m_cellVector[i].getId() == BaseSprite::cellId::UNPRESSED)
                {
                    m_cellVector[i].setSprite(BaseSprite::cellId::FLAG); // Set the correct sprite of the clicked cell.
                    m_worldMousePosClickRight = sf::Vector2f(-1, -1); //reset
                    break; // stop the loop since we found the one we clicked on
                }
            }
        }
        setCorrectMineNumberSpriteTexture(m_numberOfMines - m_nrOfFlags, m_100numberMine, m_10numberMine, m_1numberMine);
        setCorrectMineNumberSpriteTexture(timeElapsed, m_100numberTime, m_10numberTime, m_1numberTime);

        // check if game is won
        if (!m_gameIsLost) // only do this check if game has not already been lost
        {        
            for (int i = 0; i < (m_boardSize.x * m_boardSize.y); i++)
            {
                if (m_cellVector[i].getId() == BaseSprite::cellId::UNPRESSED || m_cellVector[i].getId() == BaseSprite::cellId::FLAG)
                    unpressedCells++;
            }

            if (unpressedCells == m_numberOfMines) // if we have squares left and ALL are mines, then we won!
            {
                // Victory! Move to gameOver screen
                m_smileySprite.setTextureRect(sf::IntRect(0, 48, 48, 48));
                putFlagsOnRemaningCells();
                m_state = Game::gameStates::gameOver;
                m_nrOfFlags = m_numberOfMines; // Just to make sure we display the correct nr here because we automatically set flags
                // when we finish the game when unpressed cells == numberOfMines.
            }
            else
                unpressedCells = 0;        
        }
        setCorrectMineNumberSpriteTexture(m_numberOfMines - m_nrOfFlags, m_100numberMine, m_10numberMine, m_1numberMine);
        setCorrectMineNumberSpriteTexture(timeElapsed, m_100numberTime, m_10numberTime, m_1numberTime);
        break;
    }
    case Game::gameStates::gameOver:
        if (checkBounds(m_worldMousePosClick, m_smileySprite.getGlobalBounds()))
        {
            m_worldMousePosClick.x = -1;
            m_worldMousePosClick.y = -1;
            resetForNewGame();
        }
        break;

    default:
        break;
    }
}

void Game::drawGame(sf::RenderWindow& window)
{
    window.clear();

    switch (m_state)
    {
    case Game::gameStates::chooseDifficulty:
    {
        window.draw(m_titleSprite);
        for (auto text : m_textVector)
        {
            window.draw(text);
        }
        break;
    }

    case Game::gameStates::gamePlay:
    {
        //  Here draw the top bar aka the whole view
        window.setView(m_wholeWindowView);
        window.draw(m_topBar);
        window.draw(m_smileySprite);
        window.draw(m_1numberMine.getSprite());
        window.draw(m_10numberMine.getSprite());
        window.draw(m_100numberMine.getSprite());
        window.draw(m_1numberTime.getSprite());
        window.draw(m_10numberTime.getSprite());
        window.draw(m_100numberTime.getSprite());

        float yPos = 60.0f / window.getSize().y; // equals about 0.2f the ratio we need to change the gameRegionView with

         // This works somehow, I dont understand why because we should NOT include the + 60
         //EXPLANATION:
         // I think it works because now the viewPort is actually larger than the window (yPos + 1).
         // But the size is same as the whole window so the bottom of the view is actually outside of the window I think.
        //m_gameRegionView.setSize(sf::Vector2f(320, 256 + 60));
        //m_gameRegionView.setCenter(160, 158);
        //m_gameRegionView.setViewport(sf::FloatRect(0, yPos, 1, 1));

        // Test this also works somehow but I dont understand why. we should not do 1-yPos???
        // //Explanation:
        // SO floatRect is sf::FloatRect(origin.x, origin.y, size.width, size.height) so it is correct that the first argument is 0 since we 
        // want to start from the left edge and the 2nd arugment ypos is also correct because wa want to start the view right below the topBar which is 60pixels 
        // so with a unit system we need to start from 60/window.width (about 0.19)
        // the third argument represents the width size and since we start at 0 and we use up the whole size of the x axis it should be 1.
        // the 4th argument is the height size. And if we put the 4th argument to one, which means the size of the whole window we would go too far. 
        // We would go too far because we started at 0.19 and if we then go 1, i.e. another whole window, that would be too far (1+0.19)
        // That would result in the view being stretched and some cells would be outside of the window.
        // Im pretty sure the 4th argument 1 represents 100% of the window size. so if we try to draw our minefield which is only 256 in height over (256 + 60) pixels,
        // it would have to be stretched out.
        m_gameRegionView.setViewport(sf::FloatRect(0, yPos, 1, 1 - yPos)); //we want our view to cover from below the top bar to the end of the window

        float xSize = window.getSize().x;
        float ySize = window.getSize().y * (1 - yPos);

        m_gameRegionView.setSize(sf::Vector2f(window.getSize().x, window.getSize().y*(1-yPos))); // same width but since we changed the height we need to compensate for that here
        float xPosition = window.getSize().x / 2;
        float yPosition = window.getSize().y * (1 - yPos) / 2;
        m_gameRegionView.setCenter(window.getSize().x/2, window.getSize().y*(1-yPos)/2); // but the view in the center so width/2 and half of our new height
        // This solution is more intuitive ^^.

        window.setView(m_gameRegionView); // set the view before we draw

        // Draw the m_cellVector cells        
        for (int i = 0 ; i < m_boardSize.x ;i++) 
        {
            for (int j = 0; j < m_boardSize.y; j++)
            {
                int index = convertIndexFor1dVector(i, j);
                window.draw(m_cellVector[index].getSprite());
            }
        }
        break;
    }


    case Game::gameStates::gameOver:
    {
        window.setView(m_wholeWindowView);
        // Draw the topBar
        window.draw(m_topBar);
        window.draw(m_smileySprite);
        window.draw(m_1numberMine.getSprite());
        window.draw(m_10numberMine.getSprite());
        window.draw(m_100numberMine.getSprite());
        window.draw(m_1numberTime.getSprite());
        window.draw(m_10numberTime.getSprite());
        window.draw(m_100numberTime.getSprite());

        window.setView(m_gameRegionView);
        // Draw the m_cellVector cells        
        for (int i = 0; i < m_boardSize.x; i++)
        {
            for (int j = 0; j < m_boardSize.y; j++)
            {
                int index = convertIndexFor1dVector(i, j);
                window.draw(m_cellVector[index].getSprite());
            }
        }
        break;
    }

    default:
        break;
    }

    window.display();
}

void Game::interact(sf::RenderWindow& window)
{
    sf::Event event;

    //reset variable
    m_worldMousePosClick.x = -0.0f;
    m_worldMousePosClick.y = -0.0f;

    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
        }

        switch (m_state)
        {
            case Game::gameStates::chooseDifficulty:
            {
                if (event.type == sf::Event::MouseButtonPressed)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
                        sf::Vector2i worldMousePos(window.mapPixelToCoords(mousePos));
                        m_worldMousePosClick = window.mapPixelToCoords(mousePos);
                    }
                }
                break;
            }
            

            case Game::gameStates::gamePlay:
            {
                if (event.type == sf::Event::MouseButtonPressed)
                {
                    //check which view we are in
                    m_worldMousePos = sf::Mouse::getPosition(window); // Current mouse position
                    
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        sf::FloatRect viewBounds = getViewBounds(m_gameRegionView);
                       
                        if(m_worldMousePos.y > 60.f)
                        {
                            window.setView(m_gameRegionView);
                            sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
                            sf::Vector2i worldMousePos(window.mapPixelToCoords(mousePos));
                            m_gameRegionMousePosClick = window.mapPixelToCoords(mousePos);
                        }
                        else
                        {
                            window.setView(m_wholeWindowView);
                            sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
                            sf::Vector2i worldMousePos(window.mapPixelToCoords(mousePos));
                            m_worldMousePosClick = window.mapPixelToCoords(mousePos);
                        }
                    }

                    if (event.mouseButton.button == sf::Mouse::Right)
                    {
                        sf::FloatRect viewBounds = getViewBounds(m_gameRegionView);
                        if (m_worldMousePos.y > 60.f)
                        {
                            window.setView(m_gameRegionView);
                            sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
                            sf::Vector2i worldMousePos(window.mapPixelToCoords(mousePos));
                            m_worldMousePosClickRight = window.mapPixelToCoords(mousePos);
                        }
                        else
                        {
                            window.setView(m_wholeWindowView);
                            sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
                            sf::Vector2i worldMousePos(window.mapPixelToCoords(mousePos));
                            m_worldMousePosClickRight = window.mapPixelToCoords(mousePos);
                        }
                    }
                }

                if (event.type == sf::Event::MouseButtonReleased)
                {

                }

                break;
            }
            

            case Game::gameStates::gameOver:
            {
                if (event.type == sf::Event::MouseButtonPressed)
                {
                    //check which view we are in
                    m_worldMousePos = sf::Mouse::getPosition(window); // Current mouse position

                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        window.setView(m_wholeWindowView);
                        sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
                        sf::Vector2i worldMousePos(window.mapPixelToCoords(mousePos));
                        m_worldMousePosClick = window.mapPixelToCoords(mousePos);
                    }
                }
                break;
            }
            default:
                break;
        }
    }
}

bool Game::checkBounds(sf::Vector2f mouseCoords, sf::FloatRect bounds)
{
    if (bounds.contains(mouseCoords))
    {
        return true;
    }
    else 
    {
        return false;
    }
}

bool Game::checkBounds(sf::Vector2i mouseCoords, sf::FloatRect bounds)
{
    sf::Vector2f temp(mouseCoords);
    if (bounds.contains(temp))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Game::initCellVector()
{
    int boardSize = m_boardSize.x * m_boardSize.y;
    m_cellVector.resize(boardSize,BaseSprite(m_textureHolder)); // Can not just resize like this, because it requires a default constructor, So to resize I have to do like I did in my Go code
    // I have to set the arguments for my BaseSprite Object for every object in the vector. So in this case now I only have to set the textureHolder texture!
    // Add this comment to the drive document!
    // 
    for (auto& cell : m_cellVector) // Loop through all cells to set the sprite, could also set the position here.
    {
        cell.loadSprite(TextureHolder::ID::COMBINED);
        cell.setSprite(BaseSprite::cellId::UNPRESSED); // Maybe instead set the m_id variable and the at the same time the sprite changes texture to the correct one.
    }

    // Give each Cell its correct position on the screen (we only have to do this once since the squares never move from these initial positions.
    for (int i = 0; i < m_boardSize.x; i++) 
    {
        for (int j = 0; j < m_boardSize.y; j++)
        {
            int index = convertIndexFor1dVector(i, j); //Eftersom den funkade pa 3x3 exemplet maste det vara nagot kod relaterat, tex kanske -1 kravs eller sa har jag taken the wrong x or y??
            //std::cout << index << std::endl;
            m_cellVector[index].setPosition(sf::Vector2f(i * m_cellVector[0].getTexturePixelSize().x, j * m_cellVector[0].getTexturePixelSize().y));
        }
    }
}

int Game::convertIndexFor1dVector(int x, int y)
{
    return x + m_boardSize.x * y;
}


void Game::generateMines()
{
    srand(time(0));
    for (int i = 0; i < m_numberOfMines; i++)
    {
        int randomIndex = rand() % (m_boardSize.x * m_boardSize.y); + 0;
        m_cellTypeVector[randomIndex] = BaseSprite::cellId::MINE;
    }
}

void Game::generateRemaningCells()
{
    int mineCount = 0;
    //2d
    for (int y = 0; y < m_boardSize.y; y++)
    {
        for (int x = 0; x < m_boardSize.x; x++)
        {
            mineCount = giveSurroundingMineCount(x,y);
            int index = convertIndexFor1dVector(x,y);
            if (mineCount == 0)
            {
                // Set m_cellTypeVector index to "pressed" id
                m_cellTypeVector[index] = BaseSprite::cellId::PRESSED;
            }
            else if (mineCount > 0 && mineCount < 9)
            {
                // There is at least 1 mine so set the id to the corresponding amount of mines.
                m_cellTypeVector[index] = (BaseSprite::cellId)mineCount; // recast to cellId
            }
            else
            {
                // We have reached a mine square, Do nothing since they are already set.
            }
        }
    }
}

int Game::giveSurroundingMineCount(int index)
{
    if(m_cellTypeVector[index] == BaseSprite::cellId::MINE)
        return -1; // Check if the current position is a mine, in that case return -1
    
    /*
    +-+-+-|
    |1|2|3|
    +-+-+-+
    |4|x|5|
    +-+-+-+
    |6|7|8|
    +-+-+-+
    */

    //Put surrounding indexes into an array
    int indexArray[8];
    indexArray[0] = index - (m_boardSize.x + 1);
    indexArray[1] = index - m_boardSize.x;
    indexArray[2] = index - (m_boardSize.x - 1);
    indexArray[3] = index - 1;
    indexArray[4] = index + 1;
    indexArray[5] = index + (m_boardSize.x - 1);
    indexArray[6] = index + m_boardSize.x;
    indexArray[7] = index + m_boardSize.x + 1;

    int mineCount = 0;
    for (int i = 0; i < 8; i++) // loop through all 8 positions surroinding the index and check if mines are there.
    {
        //Check if out of range
        if (indexArray[i] < 0 || indexArray[i] > (m_boardSize.x * m_boardSize.y - 1))
        {
            continue; // skip this index since it is out of range.
        }
        if (m_cellTypeVector[indexArray[i]] == BaseSprite::cellId::MINE)
            mineCount++;
    }
    return mineCount;
}

int Game::giveSurroundingMineCount(int x, int y)
{
    int index = convertIndexFor1dVector(x, y);
    if (m_cellTypeVector[index] == BaseSprite::cellId::MINE)
        return -1; // Check if the current position is a mine, in that case return -1
    
    //Put surrounding indexes into an array
    int yArr[8] = { -1,-1,-1,0,0,+1,+1,+1 };
    int xArr[8] = { -1,0,+1,-1,+1,-1,0,+1 };
    int mineCount = 0;
    // loop through all 8 positions surroinding the index and check if mines are there.
    for (int i = 0; i < 8; i++) 
    {
        int xIndexToCheck = xArr[i] + x;
        int yIndexToCheck = yArr[i] + y;
        //Check if out of range
        if (xIndexToCheck < 0 || xIndexToCheck > (m_boardSize.x - 1) || yIndexToCheck < 0 || yIndexToCheck > (m_boardSize.y - 1))
            continue; // skip this index since it is out of range.
        
        int indexToCheck = convertIndexFor1dVector(xIndexToCheck, yIndexToCheck);

        if (m_cellTypeVector[indexToCheck] == BaseSprite::cellId::MINE)
            mineCount++;
    }
    return mineCount;
}

void Game::initSmiley()
{
    int screenWidth = m_boardSize.x * 32;
    int screenHeight = m_boardSize.y * 32;
    int textureSize = 48;
    m_smileySprite.setTexture(m_textureHolder.get(TextureHolder::ID::SMILEY_COMBINED));
    m_smileySprite.setTextureRect(sf::IntRect(0, 0, textureSize, textureSize));
    m_smileySprite.setPosition(sf::Vector2f(screenWidth / 2 - textureSize/2, 6)); 
}

void Game::makeEmptyCellsVisible(int index)
{
    // Get the index of the cell we clicked on
    sf::Vector2f position(m_cellVector[index].getSprite().getPosition()); // Get the position of the clicked sprite

    // Convert the position of the clicked sprite to the index 
    int clickedIndexPosX = position.x / m_cellVector[0].getTexturePixelSize().x;
    int clickedIndexPosY = position.y / m_cellVector[0].getTexturePixelSize().y; 
    floodFill(BaseSprite::cellId::PRESSED, clickedIndexPosX, clickedIndexPosY);

    for (int i = 0; i < (m_boardSize.x * m_boardSize.y); i++)
    {
        if (m_visited[i]) // If the cell has been visited by floodfill, it means the cell should be visible. This could be done inside of the floodFill function...
        {
            if(m_cellVector[i].getId() != BaseSprite::cellId::FLAG) // Don't remove cells with flags
                m_cellVector[i].setSprite(m_cellTypeVector[i]);
        }
    }
}

void Game::revealMines()
{
    for (int i = 0; i < (m_boardSize.x * m_boardSize.y); i++)
    {
        if (m_cellTypeVector[i] == BaseSprite::cellId::MINE) // If the cell has been visited by floodfill, it means the cell should be visible. This could be done inside of the floodFill function...
        {
            m_cellVector[i].setSprite(BaseSprite::cellId::MINE);
        }
    }
}

void Game::resetForNewGame()
{
    m_state = Game::gameStates::gamePlay;
    m_smileySprite.setTextureRect(sf::IntRect(0, 0, 48, 48));
    m_gameIsLost = false;

    for (int i = 0; i < (m_boardSize.x * m_boardSize.y); i++)
    {
        m_visited[i] = false;
        m_cellVector[i].setSprite(BaseSprite::cellId::UNPRESSED);
        m_cellTypeVector[i] = BaseSprite::cellId::NO_ID;  
    }
    generateMines();
    generateRemaningCells();
    m_timer.restart();
}

void Game::putFlagsOnRemaningCells()
{
    for (int i = 0; i < (m_boardSize.x * m_boardSize.y); i++)
    {
        if (m_cellVector[i].getId() == BaseSprite::cellId::UNPRESSED)
        {
            m_cellVector[i].setSprite(BaseSprite::cellId::FLAG);
        }
    }
}

void Game::initNumbers(sf::RenderWindow& window)
{
    m_1numberMine.loadSprite(TextureHolder::ID::NUMBERS);
    m_10numberMine.loadSprite(TextureHolder::ID::NUMBERS);
    m_100numberMine.loadSprite(TextureHolder::ID::NUMBERS);

    m_1numberMine.setScale(2.5,2.5);
    m_10numberMine.setScale(2.5, 2.5);
    m_100numberMine.setScale(2.5, 2.5);

    //I Think we need -1 because we multiply the width i.e. 13 by 2.5 which equals 32.5 so it is not pixel perfect anymor, so it gets rounded!
    m_1numberMine.setPosition(sf::Vector2f(m_1numberMine.getTexturePixelSize().x * 2.5f * 2.f + -1, 2)); //2.5 because I made them 2.5 bigger. 
    m_10numberMine.setPosition(sf::Vector2f(m_1numberMine.getTexturePixelSize().x *  2.5f + -1, 2));
    m_100numberMine.setPosition(sf::Vector2f(0, 2));

    // Time Numbers
    m_1numberTime.loadSprite(TextureHolder::ID::NUMBERS);
    m_10numberTime.loadSprite(TextureHolder::ID::NUMBERS);
    m_100numberTime.loadSprite(TextureHolder::ID::NUMBERS);

    m_1numberTime.setScale(2.5, 2.5);
    m_10numberTime.setScale(2.5, 2.5);
    m_100numberTime.setScale(2.5, 2.5);
    
    m_1numberTime.setPosition(sf::Vector2f(window.getSize().x - m_1numberTime.getTexturePixelSize().x * 2.5f - 1, 2));
    m_10numberTime.setPosition(sf::Vector2f(window.getSize().x - m_1numberTime.getTexturePixelSize().x * 2 * 2.5f - 1, 2));
    m_100numberTime.setPosition(sf::Vector2f(window.getSize().x - m_1numberTime.getTexturePixelSize().x * 3 * 2.5f - 1, 2));
}

void Game::initTitleSprite()
{
    int screenWidth = m_boardSize.x * 32;
    int screenHeight = m_boardSize.y * 32;
    int textureSize = 48;
    m_smileySprite.setTexture(m_textureHolder.get(TextureHolder::ID::SMILEY_COMBINED));
    m_smileySprite.setTextureRect(sf::IntRect(0, 0, textureSize, textureSize));
    m_smileySprite.setPosition(sf::Vector2f(screenWidth / 2 - textureSize / 2, 6));
}

void Game::initTopBar(sf::RenderWindow& window)
{
    m_topBar.setSize(sf::Vector2f(window.getSize().x, 60)); // should be 60 pixels high
    m_topBar.setPosition(0, 0);
    m_topBar.setFillColor(sf::Color::White);
}

sf::FloatRect Game::getViewBounds(const sf::View& view)
{
    sf::FloatRect rt;
    rt.left = view.getCenter().x - view.getSize().x / 2.f;
    rt.top = view.getCenter().y - view.getSize().y / 2.f;
    rt.width = view.getSize().x;
    rt.height = view.getSize().y;
    return rt;
}

void Game::setCorrectMineNumberSpriteTexture(int number, Numbers& number100, Numbers& number10, Numbers& number1)
{
    std::string str = std::to_string(number);
    switch (str.length())
    {
    case 1:
        number1.setNumberSprite(convertCharNrToNumberId(str[0]));
        number10.setNumberSprite(Numbers::numberId::NO_NUMBER);
        number100.setNumberSprite(Numbers::numberId::NO_NUMBER);
        break;
    case 2:
        number1.setNumberSprite(convertCharNrToNumberId(str[1]));
        number10.setNumberSprite(convertCharNrToNumberId(str[0]));
        number100.setNumberSprite(Numbers::numberId::NO_NUMBER);
        break;
    case 3:
        number1.setNumberSprite(convertCharNrToNumberId(str[2]));
        number10.setNumberSprite(convertCharNrToNumberId(str[1]));
        number100.setNumberSprite(convertCharNrToNumberId(str[0]));
        break;
    default:
        break;
    }
}


Numbers::numberId Game::convertCharNrToNumberId(char c)
{
    if (c == '-')
        return Numbers::numberId::DASH;
    if (c == '0')
        return Numbers::numberId::ZERO;

    return (Numbers::numberId)(c - '0');
}


void Game::drawClickedCell(int index)
{
    m_cellVector[index].setSprite(m_cellTypeVector[index]);
}


bool Game::floodFill(BaseSprite::cellId target, int x, int y)
{
    // Target is the cell we are looking for
    // Check if out of bounds
    if (x > m_boardSize.x - 1 || x < 0 || y > m_boardSize.y -1 || y < 0)
        return false;

    int index = convertIndexFor1dVector(x, y);
    if (m_visited[index]) // check if visited
        return false;
    m_visited[index] = true;

    if (m_cellTypeVector[index] != target) // ignore cells that are not the target ( i.e. PRESSED)
        return false;

    // If it passes those tests then its a legit PRESSED square.
    // We should mark it and all surrounding squares as "visible"

    //Recursive search for more stones of "target" (8 directions)

    bool retVal = floodFill(target, x - 1, y - 1);
    retVal |= floodFill(target, x, y - 1);
    retVal |= floodFill(target, x + 1, y - 1);
    retVal |= floodFill(target, x - 1, y );
    retVal |= floodFill(target, x + 1, y);
    retVal |= floodFill(target, x - 1, y + 1);
    retVal |= floodFill(target, x, y + 1);
    retVal |= floodFill(target, x + 1 , y + 1);
    return retVal;
}

