#include <SFML/Graphics.hpp>

sf::RenderWindow window;

int main()
{
	window.create({800, 600}, "Beneath the Surface");

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code) {
				case sf::Keyboard::Escape:
					window.close();
					break;
				}
				break;
			}
		}

		window.clear();

		window.display();
	}
}
