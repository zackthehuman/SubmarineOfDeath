#include <SFML/Graphics.hpp>

#include <cmath>

constexpr float submarine_speed_x = 200.f;
constexpr float submarine_speed_y = 100.f;

sf::RenderWindow window;

sf::Texture texture_squid;
sf::Texture texture_submarine;
sf::Texture texture_torpedo;
sf::Texture texture_explosion;

sf::Sprite sprite_squid;
sf::Sprite sprite_submarine;
sf::Sprite sprite_torpedo;
sf::Sprite sprite_explosion;

struct Torpedo {
	sf::Vector2f position;
};
std::vector<Torpedo> torpedos;

struct Squid {
	sf::Vector2f position;
};
std::vector<Squid> squids;

bool game_over = false;

float dot(const sf::Vector2f& a, const sf::Vector2f& b) {
	return a.x*b.x + a.y*b.y;
}
float length(const sf::Vector2f& vector) {
	return sqrt(dot(vector, vector));
}
sf::Vector2f normalize(const sf::Vector2f& vector) {
	return vector / length(vector);
}
float get_distance(const sf::Vector2f& a, const sf::Vector2f& b) {
	return length(a - b);
}

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
		case sf::Keyboard::Space:
			if (game_over) break;
			Torpedo torpedo;
			torpedo.position = sprite_submarine.getPosition() + sf::Vector2f(texture_submarine.getSize().x-10, 0);
			torpedos.push_back(torpedo);
			break;
		}
		break;
	}
}

void update(float dt)
{
	if (game_over) return;

	bool down = sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S);
	bool up = sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W);
	bool right = sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D);
	bool left = sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A);

	for (auto& squid : squids) {
		auto delta = normalize(sprite_submarine.getPosition() - squid.position);
		squid.position += dt * 80.f * delta;
	}

	if (down ^ up) {
		sprite_submarine.move(dt * submarine_speed_y * sf::Vector2f(0, down - up));
	}
	if (right ^ left) {
		sprite_submarine.move(dt * submarine_speed_x * sf::Vector2f(right - left, 0));
	}

	for (auto& torpedo : torpedos) {
		torpedo.position.x += 400 * dt;
	}

	for (auto& squid : squids) {
		auto distance = get_distance(sprite_submarine.getPosition(), squid.position);
		if (distance < 32) {
			game_over = true;
			break;
		}
	}
}

void draw()
{
	for (auto& squid : squids) {
		sprite_squid.setPosition(squid.position);
		window.draw(sprite_squid);
	}

	window.draw(sprite_submarine);

	for (auto& torpedo : torpedos) {
		sprite_torpedo.setPosition(torpedo.position);
		window.draw(sprite_torpedo);
	}
}

int main()
{
	window.create({800, 600}, "Beneath the Surface");
	window.setFramerateLimit(60);

	texture_squid.loadFromFile("../res/squid.png");
	texture_submarine.loadFromFile("../res/submarine.png");
	texture_torpedo.loadFromFile("../res/torpedo.png");
	texture_explosion.loadFromFile("../res/explosion.png");

	sprite_squid.setTexture(texture_squid);
	sprite_submarine.setTexture(texture_submarine);
	sprite_torpedo.setTexture(texture_torpedo);
	sprite_explosion.setTexture(texture_explosion);

	sf::Clock theclock;

	Squid squid;
	squid.position = {600, 400};
	squids.push_back(squid);

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
