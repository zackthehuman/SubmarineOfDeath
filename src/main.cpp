#include <SFML/Graphics.hpp>

sf::RenderWindow window;

sf::Texture texture_squid;
sf::Texture texture_submarine;

sf::Sprite sprite_squid;
sf::Sprite sprite_submarine;

int main()
{
	window.create({800, 600}, "Beneath the Surface");

	texture_squid.loadFromFile("../res/squid.png");
	texture_submarine.loadFromFile("../res/submarine.png");

	sprite_squid.setTexture(texture_squid);
	sprite_squid.setPosition({600, 400});
	sprite_submarine.setTexture(texture_submarine);

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

		window.clear({20, 102, 126});

		window.draw(sprite_squid);
		window.draw(sprite_submarine);

		window.display();
	}
}
