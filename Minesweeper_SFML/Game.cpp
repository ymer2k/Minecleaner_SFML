#include "Game.h"
#include "math.h"
#include <iostream>
#include <stdlib.h>




Game::Game():
    m_state(Game::gameStates::chooseDifficulty)
    , m_worldMousePosClickRight(-1.0f,-1.0f)
{
    //Load text font
    m_fonts.load(FontHolder::FontID::PixelFont, "Fonts/slkscre.ttf");
    //Create and put text in array  
    // Put this shit in the Game constructor in a init() function



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

    // Setup top Rectangle
    m_topBar.setSize(sf::Vector2f(320, 60));
    m_topBar.setPosition(0, 256);
    m_topBar.setFillColor(sf::Color::White);

    // Setup smiley
    //m_smiley.loadSprite(TextureHolder::ID::SMILEY_COMBINED);
    //m_smiley.setPosition(sf::Vector2f(m_boardSize.x * 32 / 2, m_boardSize.y + 6));


    //m_smileySprite.setTexture(m_textureHolder.get(TextureHolder::ID::SMILEY_COMBINED));
    //m_smileySprite.setTextureRect(sf::IntRect(0, 0, 48, 48));
    //m_smileySprite.setPosition(sf::Vector2f(320 / 2-24, 256 + 6));
}

void Game::update(sf::Time dt, sf::RenderWindow& window)
{
    switch (m_state)
    {
    case Game::gameStates::chooseDifficulty:
    {
        // Put into function choose difficulty
        m_worldMousePos = sf::Mouse::getPosition(window); // Current mouse position
        //std::cout << m_worldMousePos.x << " - " << m_worldMousePos.y << std::endl;

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
                m_visited.resize(m_boardSize.x * m_boardSize.y);
                m_state = Game::gameStates::gamePlay;
                initCellVector();
                initSmiley();
                // Generate mine positions randomly
                m_cellTypeVector.resize(m_boardSize.x * m_boardSize.y);
                generateMines();
                generateRemaningCells();
                initNumbers();
                // Generate Numbers below each cell.
                // check index (0,0) check squares all around that index and count number of mines it touches. put the corresponding number on that index.
                return;
                // MOve to Gameplay, set the window size and game size and everything depending on which difficulty we chose! window.setSize() etc
            }
        }
        break;
    }


    case Game::gameStates::gamePlay:
    {
        //if (checkBounds(m_worldMousePosClick, m_smileySprite.getGlobalBounds()))
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            m_smileySprite.setTextureRect(sf::IntRect(48, 48, 48, 48));
            
        }else
            m_smileySprite.setTextureRect(sf::IntRect(0, 0, 48, 48));

    
        static int unpressedCells = 0;
        // CHeck which cell we click on 
        //TODO: (only do this when we actually click)
        m_nrOfFlags = 0;
        for (int i = 0; i < (m_boardSize.x * m_boardSize.y); i++)
        {
            if (m_cellVector[i].getId() == BaseSprite::cellId::FLAG) // Count number of flags
            {
                m_nrOfFlags++;
            }

            if (checkBounds(m_worldMousePosClick, m_cellVector[i].getSprite().getGlobalBounds())) // Look for the cell we clicked on
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
                    // Reveal the remaning bombs
                    // Mark flags placed wrong
                    // move to Game over Screen
                    revealMines();
                    m_cellTypeVector[i] = BaseSprite::cellId::DETONATED;
                    // Set dead smiley
                    m_smileySprite.setTextureRect(sf::IntRect(48, 0, 48, 48));
                    m_state = Game::gameStates::gameOver;
                    // Look for false Flags
                    for (int m = 0; m < (m_boardSize.x * m_boardSize.y); m++)
                    {
                        if (m_cellVector[m].getId() == BaseSprite::cellId::FLAG && m_cellTypeVector[m] != BaseSprite::cellId::MINE)
                        {
                            m_cellVector[m].setSprite(BaseSprite::cellId::FALSE);
                        }
                    }

                    
                }
                m_cellVector[i].setSprite(m_cellTypeVector[i]); // Set the correct sprite of the clicked cell.
                m_worldMousePosClick = sf::Vector2f(-1, -1); //reset
                break; // stop the loop since we found the one we clicked on
            }
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

        // check if game is won
        for (int i = 0; i < (m_boardSize.x * m_boardSize.y); i++)
        {
            if (m_cellVector[i].getId() == BaseSprite::cellId::UNPRESSED || m_cellVector[i].getId() == BaseSprite::cellId::FLAG)
            {
                unpressedCells++;
            }
        }

        if (unpressedCells == m_numberOfMines)
        {
            // Victory! Move to gameOver screen
            m_smileySprite.setTextureRect(sf::IntRect(0, 48, 48, 48));
            putFlagsOnRemaningCells();
            m_state = Game::gameStates::gameOver;

        }
        else
            unpressedCells = 0;
        




        break;
    }
    case Game::gameStates::gameOver:
        if (checkBounds(m_worldMousePosClick, m_smileySprite.getGlobalBounds()))
        {
            //Reset everything
            // generate new mines
            // Change back to happy smiley
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

    //Draw everything
    switch (m_state)
    {
    case Game::gameStates::chooseDifficulty:
    {
        for (auto text : m_textVector)
        {
            window.draw(text);
        }
        break;
    }




    case Game::gameStates::gamePlay:
    {
        // Draw the topBar
        window.draw(m_topBar);
        window.draw(m_smileySprite);
        

        // Draw the m_cellVector cells        
        for (int i = 0 ; i < m_boardSize.x ;i++) 
        {
            for (int j = 0; j < m_boardSize.y; j++)
            {
                int index = convertIndexFor1dVector(i, j);
                window.draw(m_cellVector[index].getSprite());
            }
        }
        window.draw(m_100Number);
        //std::cout << m_nrOfFlags << std::endl;

       //  Draw what should appear when we click a square (debugging)
        //for (int i = 0 ; i < (m_boardSize.x * m_boardSize.y) ; i ++)
        //{
        //     //BaseSprite::cellId element = m_cellTypeVector[i];
        //    //if (element == BaseSprite::cellId::MINE)
        //    //{
        //    //    m_cellVector[i].setSprite(BaseSprite::cellId::MINE);
        //    //    window.draw(m_cellVector[i].getSprite());
        //    //}
        //     m_cellVector[i].setSprite(m_cellTypeVector[i]);
        //     window.draw(m_cellVector[i].getSprite());
        //    
        //}

        //Draw the cell that we click on.


        break;
    }

    case Game::gameStates::gameOver:
    {
        // Draw the topBar
        window.draw(m_topBar);
        window.draw(m_smileySprite);

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

    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
        }

        if (event.type == sf::Event::Resized)
        {
            sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
            window.setView(sf::View(visibleArea));
        }

        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
                sf::Vector2i worldMousePos(window.mapPixelToCoords(mousePos));
                m_worldMousePosClick = window.mapPixelToCoords(mousePos);
                //std::cout << "left Mouse Button Pressed" << std::endl;
            }
            

            if (event.mouseButton.button == sf::Mouse::Right)
            {
                sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
                sf::Vector2i worldMousePos(window.mapPixelToCoords(mousePos));
                m_worldMousePosClickRight = window.mapPixelToCoords(mousePos);
            }
        }

        if (event.type == sf::Event::MouseButtonReleased)
        {
           
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
    //srand(time(0));
    for (int i = 0; i < m_numberOfMines; i++)
    {
        
        int randomIndex = rand() % (m_boardSize.x * m_boardSize.y); + 0;
        m_cellTypeVector[randomIndex] = BaseSprite::cellId::MINE;
    }
}

void Game::generateRemaningCells()
{
    //for (int i = 0; i < m_boardSize.x; i++)
    //{
    //    for (int j = 0; i < m_boardSize.y; j++)
    //    {
    //        int index = 
    //    }
    //}
    int mineCount = 0;
    // 1D
    /*
    for (int i = 0; i < m_boardSize.x * m_boardSize.y; i++)
    {
        mineCount = giveSurroundingMineCount(i);

        if (mineCount == 0)
        {
            // Set m_cellTypeVector index to "pressed" id
            m_cellTypeVector[i] = BaseSprite::cellId::PRESSED;

        }
        else if(mineCount > 0 && mineCount < 9) 
        {
            // There is at least 1 mine so set the id to the corresponding amount of mines.
            m_cellTypeVector[i] = (BaseSprite::cellId)mineCount; // recast to cellId
        }
        else
        {
            // We have reached a mine square, Do nothing since they are already set.
        }

    }
    */

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



    // loop throough all cells,
    // at each cell check the cells around it if it has any mines around it,
    // if it has mark the curren cell with the amount of mines that surrounds it.
    // If no mines around leave it as a "pressed" cell and move on.
    // SKIPP cells that already has a mine!
}

int Game::giveSurroundingMineCount(int index)
{
    if(m_cellTypeVector[index] == BaseSprite::cellId::MINE)
    {
        return -1; // Check if the current position is a mine, in that case return -1
    }
    /*
    
    +-+-+-|
    |1|2|3|
    +-+-+-+
    |4|x|5|
    +-+-+-+
    |6|7|8|
    +-+-+-+
    
    */

    // get surroinding indexs
    int topLeft = index - (m_boardSize.x + 1);
    int top = index - m_boardSize.x;
    int topRight = index - (m_boardSize.x - 1);
    int left = index - 1;
    int right = index + 1;
    int lowerLeft = index + (m_boardSize.x - 1);
    int lower = index + m_boardSize.x;
    int lowerRight = index + m_boardSize.x + 1;
    //

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
    {
        return -1; // Check if the current position is a mine, in that case return -1
    }

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
        {
            continue; // skip this index since it is out of range.
        }
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
    m_smileySprite.setPosition(sf::Vector2f(screenWidth / 2 - textureSize/2, screenHeight + 6));
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

    for (int i = 0; i < (m_boardSize.x * m_boardSize.y); i++)
    {
        m_visited[i] = false;
        m_cellVector[i].setSprite(BaseSprite::cellId::UNPRESSED);
        m_cellTypeVector[i] = BaseSprite::cellId::NO_ID;
        
    }
    generateMines();
    generateRemaningCells();
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

void Game::initNumbers()
{
    m_100Number.setTexture(m_textureHolder.get(TextureHolder::ID::NUMBERS));
    m_100Number.scale(2.5, 2.5);
    m_100Number.setTextureRect(sf::IntRect(0, 0, 13, 23));
    m_100Number.setPosition(0, m_boardSize.y*32+2);
    

    m_10Number.setTexture(m_textureHolder.get(TextureHolder::ID::NUMBERS));
    m_10Number.scale(4, 4);


    m_1Number.setTexture(m_textureHolder.get(TextureHolder::ID::NUMBERS));
    m_1Number.scale(4, 4);

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