#include <SFML/Graphics.hpp>

sf::RenderWindow window;

sf::Texture texture_squid;

sf::Sprite sprite_squid;

int main()
{
	window.create({800, 600}, "Beneath the Surface");

	texture_squid.loadFromFile("../res/squid.png");

	sprite_squid.setTexture(texture_squid);

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

		window.draw(sprite_squid);

		window.display();
	}
}
