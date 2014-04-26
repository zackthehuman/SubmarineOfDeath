#include <SFML/Graphics.hpp>

#include <cmath>

constexpr float submarine_speed_x = 200.f;
constexpr float submarine_speed_y = 120.f;

sf::RenderWindow window;

sf::Texture texture_squid;
sf::Texture texture_submarine;
sf::Texture texture_torpedo;
sf::Texture texture_explosion;
sf::Texture texture_gameover;
sf::Texture texture_level;
sf::Texture texture_numbers;

sf::Sprite sprite_squid;
sf::Sprite sprite_submarine;
sf::Sprite sprite_torpedo;
sf::Sprite sprite_explosion;
sf::Sprite sprite_gameover;
sf::Sprite sprite_level;
sf::Sprite sprite_numbers;

enum State {
	NEW_LEVEL,
	PLAYING,
	GAME_OVER,
};

struct Torpedo {
	sf::Vector2f position;
	bool dead = false;
};
std::vector<Torpedo> torpedos;

struct Squid {
	sf::Vector2f position;
	bool dead = false;
};
std::vector<Squid> squids;

struct Explosion {
	sf::Vector2f position;
	float time_started;
};
std::vector<Explosion> explosions;

float game_time = 0.f;
State game_state = NEW_LEVEL;
int game_level = 1;

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

void set_number(int number)
{
	auto size = texture_numbers.getSize();
	sf::IntRect rect = {static_cast<int>(size.x/10.f*number), 0, static_cast<int>(size.x/10.f), static_cast<int>(size.y)};
	sprite_numbers.setTextureRect(rect);
}

void draw_number(int number, sf::Vector2f position)
{
	int number_width = texture_numbers.getSize().x / 10.f;
	int tmp_number = number;
	int num_digits = -1; do { tmp_number /= 10; num_digits++; } while (tmp_number != 0);

	int i = 0;
	do {
		set_number(number % 10);
		sprite_numbers.setPosition(position + sf::Vector2f(number_width*(num_digits-i), 0));
		window.draw(sprite_numbers);
		i++;
	} while (number /= 10);
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
			if (game_state != PLAYING) break;
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
	game_time += dt;

	if (game_state == GAME_OVER) {
		sprite_gameover.setPosition(sf::Vector2f(window.getSize() / 2u) + sf::Vector2f(sin(game_time*6)*30, 0));
		return;
	}

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
			game_state = GAME_OVER;
			Explosion explosion;
			explosion.position = sprite_submarine.getPosition();
			explosion.time_started = game_time;
			explosions.push_back(explosion);
			break;
		}

		for (auto& torpedo : torpedos) {
			distance = get_distance(torpedo.position, squid.position);
			if (distance < 52) {
				torpedo.dead = true;
				squid.dead = true;

				Explosion explosion;
				explosion.position = torpedo.position;
				explosion.time_started = game_time;
				explosions.push_back(explosion);
			}
		}
	}

	squids.erase(std::remove_if(squids.begin(), squids.end(), [](const Squid& squid) {
		return squid.dead;
	}), squids.end());

	torpedos.erase(std::remove_if(torpedos.begin(), torpedos.end(), [](const Torpedo& torpedo) {
		return torpedo.dead || torpedo.position.x > 800;
	}), torpedos.end());

	explosions.erase(std::remove_if(explosions.begin(), explosions.end(), [](const Explosion& explosion) {
		return game_time > explosion.time_started + 0.5f;
	}), explosions.end());
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

	for (auto& explosion : explosions) {
		sprite_explosion.setPosition(explosion.position);
		float scale = (game_time - explosion.time_started) + 0.5f;
		if (game_state == PLAYING)
			sprite_explosion.setColor({255, 255, 255, static_cast<sf::Uint8>(-((scale*2-1) * 255))});
		sprite_explosion.setScale(scale, scale);
		window.draw(sprite_explosion);
	}

	if (game_state == NEW_LEVEL) {
		window.draw(sprite_level);
		draw_number(game_level, sf::Vector2f(window.getSize()/2u) + sf::Vector2f(100, 0));
	} else if (game_state == GAME_OVER) {
		window.draw(sprite_gameover);
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
	texture_gameover.loadFromFile("../res/gameover.png");
	texture_level.loadFromFile("../res/level.png");
	texture_numbers.loadFromFile("../res/numbers.png");

	sprite_squid.setTexture(texture_squid);
	sprite_squid.setOrigin(sf::Vector2f(texture_squid.getSize() / 2u));
	sprite_submarine.setTexture(texture_submarine);
	sprite_submarine.setOrigin(sf::Vector2f(texture_submarine.getSize() / 2u));
	sprite_submarine.setPosition({50, 300});
	sprite_torpedo.setTexture(texture_torpedo);
	sprite_torpedo.setOrigin(sf::Vector2f(texture_torpedo.getSize() / 2u));
	sprite_explosion.setTexture(texture_explosion);
	sprite_explosion.setOrigin(sf::Vector2f(texture_explosion.getSize() / 2u));
	sprite_gameover.setTexture(texture_gameover);
	sprite_gameover.setOrigin(sf::Vector2f(texture_gameover.getSize() / 2u));
	sprite_level.setTexture(texture_level);
	sprite_level.setOrigin(sf::Vector2f(texture_level.getSize() / 2u));
	sprite_level.setPosition(sf::Vector2f(window.getSize()/2u) - sf::Vector2f(100, 0));
	sprite_numbers.setTexture(texture_numbers);

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
