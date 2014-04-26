#include <SFML/Graphics.hpp>

constexpr float submarine_speed_x = 200.f;
constexpr float submarine_speed_y = 100.f;

sf::RenderWindow window;

sf::Texture texture_squid;
sf::Texture texture_submarine;

sf::Sprite sprite_squid;
sf::Sprite sprite_submarine;

void handleEvent(sf::Event& event)
{
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

void update(float dt)
{
	bool down = sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S);
	bool up = sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W);
	bool right = sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D);
	bool left = sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A);

	if (down ^ up) {
		sprite_submarine.move(dt * submarine_speed_y * sf::Vector2f(0, down - up));
	}
	if (right ^ left) {
		sprite_submarine.move(dt * submarine_speed_x * sf::Vector2f(right - left, 0));
	}
}

void draw()
{
	window.draw(sprite_squid);
	window.draw(sprite_submarine);
}

int main()
{
	window.create({800, 600}, "Beneath the Surface");
	window.setFramerateLimit(60);

	texture_squid.loadFromFile("../res/squid.png");
	texture_submarine.loadFromFile("../res/submarine.png");

	sprite_squid.setTexture(texture_squid);
	sprite_squid.setPosition({600, 400});
	sprite_submarine.setTexture(texture_submarine);

	sf::Clock theclock;

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			handleEvent(event);
		}

		update(theclock.restart().asSeconds());

		window.clear({20, 102, 126});

		draw();

		window.display();
	}
}
