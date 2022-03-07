#include "Render.h"

Render::Render()
{
	m_window.create(sf::VideoMode(800, 600), "MineCleaner"); // The create  function is used when we create the window AFTER initilization.
	m_window.setVerticalSyncEnabled(true);
	sf::View view = m_window.getView();
	view.setSize(800, 600);
	m_window.setView(view);
}

sf::RenderWindow& Render::getWindow()
{
	return m_window;
}

void Render::setNewWindowSize(int width, int height)
{
	m_window.setSize(sf::Vector2u(width, height));
	m_view.setSize(width,height);
	m_window.setView(m_view);
}
