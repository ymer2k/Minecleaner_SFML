#include <iostream>
#include <SFML/Graphics.hpp>
#include "Render.h"
#include "Game.h"

const sf::Time TimePerFrame = sf::seconds(1.0f / 60.0f);

int main()
{

    Game game;
    Render world;

    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;

    while (world.m_window.isOpen())
    {
        sf::Time dt = clock.restart();
        timeSinceLastUpdate += dt; // add the time it took to update the game + render the game;

        while (timeSinceLastUpdate > TimePerFrame)
        {
            //We stay in this loop and update the game without rendering until until 
            timeSinceLastUpdate -= TimePerFrame;

            game.interact(world.getWindow());
            game.update(TimePerFrame, world.getWindow()); // Since we have fixed time per frame now, send in that into the update function (or since it global we dont have to :))
        }


        game.drawGame(world.getWindow());
        // If its very fast to draw the game, we will draw it several times WITHOUT calling update().
        // This is to prevent speed up and slow down if the game draws slowly. We want to update the game in even time steps to prevent
        // speed ups and speed downs. If we only had dt (time it takes to update and render) then if render was slow and we passed dt into the
        // update function then we would perhaps move the player a very big distance, mahybe through walls etc.
        //
        // Now since we have Fixed Time frames. IF timeSinceLastUpdate is 3x TimePerFrame, we would just update the game three times with that time step
        // and then render the game after that! This way we would not jump huge distances in one "cycle" of game.update()


    }
}


