#pragma once
#include <SFML/Graphics.hpp>


class Render
{
public:
	Render();

	
	sf::RenderWindow m_window;
	sf::View m_view;


	sf::RenderWindow& getWindow();
	// Set a new window size (and also change the view)
	void setNewWindowSize(int width, int height);

private:
	

};

