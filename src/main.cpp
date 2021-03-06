#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

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
sf::Texture texture_score;
sf::Texture texture_torpedosfired;
sf::Texture texture_monsterskilled;
sf::Texture texture_minus;

sf::Sprite sprite_squid;
sf::Sprite sprite_submarine;
sf::Sprite sprite_torpedo;
sf::Sprite sprite_explosion;
sf::Sprite sprite_gameover;
sf::Sprite sprite_level;
sf::Sprite sprite_numbers;
sf::Sprite sprite_score;
sf::Sprite sprite_torpedosfired;
sf::Sprite sprite_monsterskilled;
sf::Sprite sprite_minus;

std::map<std::string, sf::SoundBuffer> sound_buffers;
std::vector<sf::Sound> sound_pool;
size_t sound_pool_current = 0;
sf::Sound beepboop_sound;

sf::RectangleShape fullscreen_rect {{800, 600}};
sf::RectangleShape pause_rect1 {{20, 100}};
sf::RectangleShape pause_rect2 {{20, 100}};

enum State {
	NEW_LEVEL,
	PLAYING,
	PAUSED,
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
float game_over_time;
float game_time_shot = 0;

size_t num_torpedos_fired = 0;
size_t num_monsters_killed = 0;

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

	if (number < 0) {
		num_digits++;
		sprite_minus.setPosition(position - sf::Vector2f(5, 6));
		window.draw(sprite_minus);
	}

	int i = 0;
	do {
		set_number(abs(number % 10));
		sprite_numbers.setPosition(position + sf::Vector2f(number_width*(num_digits-i), 0));
		window.draw(sprite_numbers);
		i++;
	} while (number /= 10);
}

void prepare_new_level()
{
	torpedos.clear();
	squids.clear();

	for (int i = 0; i < game_level; ++i) {
		Squid squid;
		do {
			squid.position = sf::Vector2f(rand()%800, rand()%600);
		} while (128 > get_distance(squid.position, sprite_submarine.getPosition()));
		squids.push_back(squid);
	}

	if (game_level == 1) {
		beepboop_sound.play();
	}

	sprite_level.setPosition(sf::Vector2f(window.getSize()/2u) - sf::Vector2f(100, 0));
}

void prepare_new_game()
{
	game_level = 1;
	num_torpedos_fired = 0;
	num_monsters_killed = 0;
	game_state = NEW_LEVEL;
	prepare_new_level();
}

void load_sound(std::string name)
{
	sf::SoundBuffer buffer;
	buffer.loadFromFile("../res/"+ name +".wav");
	sound_buffers[name] = buffer;
}

sf::Sound& get_sound_from_pool()
{
	size_t i = sound_pool_current;

	sound_pool_current++;
	sound_pool_current %= sound_pool.size();

	return sound_pool[i];
}

void play_sound(std::string name)
{
	auto& sound = get_sound_from_pool();
	sound.setBuffer(sound_buffers[name]);
	sound.play();
}

void handleEvent(sf::Event& event)
{
	switch (event.type) {
	case sf::Event::Closed:
		window.close();
		break;
	case sf::Event::LostFocus:
		if (game_state == PLAYING) {
			game_state = PAUSED;
		}
		break;
	case sf::Event::KeyPressed:
		if (game_state == NEW_LEVEL) {
			game_state = PLAYING;
			break;
		} else if (game_state == GAME_OVER) {
			if (game_time > game_over_time + 1.f) {
				prepare_new_game();
				break;
			}
		} else if (game_state == PAUSED) {
			game_state = PLAYING;
			break;
		}

		switch (event.key.code) {
		case sf::Keyboard::Escape:
			if (game_state == PLAYING) {
				game_state = PAUSED;
			}
			break;
		case sf::Keyboard::Space:
			if (game_state != PLAYING) break;
			if (game_time > game_time_shot + 0.3f) {
				Torpedo torpedo;
				torpedo.position = sprite_submarine.getPosition() + sf::Vector2f(texture_submarine.getSize().x-10, 0);
				torpedos.push_back(torpedo);
				play_sound("pew");
				game_time_shot = game_time;
				num_torpedos_fired++;
			}
			break;
		}
		break;
	}
}

void update(float dt)
{
	game_time += dt;

	if (game_state == PLAYING && squids.size() == 0) {
		game_state = NEW_LEVEL;
		game_level++;
		prepare_new_level();
	}

	if (game_state == GAME_OVER) {
		sprite_gameover.setPosition(sf::Vector2f(window.getSize() / 2u) + sf::Vector2f(sin(game_time*6)*30, 0));
		return;
	}

	if (game_state == NEW_LEVEL || game_state == PAUSED) {
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

	auto submarine_position = sprite_submarine.getPosition();
	auto submarine_half_size = sf::Vector2f(texture_submarine.getSize() / 2u);
	submarine_half_size.y /= 2.f;
	if (submarine_position.x < submarine_half_size.x) {
		sprite_submarine.setPosition({submarine_half_size.x, submarine_position.y});
	} else if (submarine_position.x > 800 - submarine_half_size.x) {
		sprite_submarine.setPosition({800 - submarine_half_size.x, submarine_position.y});
	}
	if (submarine_position.y < submarine_half_size.y) {
		sprite_submarine.setPosition({submarine_position.x, submarine_half_size.y});
	} else if (submarine_position.y > 600 - submarine_half_size.y) {
		sprite_submarine.setPosition({submarine_position.x, 600 - submarine_half_size.y});
	}

	for (auto& torpedo : torpedos) {
		torpedo.position.x += 400 * dt;
	}

	for (auto& squid : squids) {
		auto distance = get_distance(sprite_submarine.getPosition(), squid.position);
		if (distance < 32) {
			game_state = GAME_OVER;
			game_over_time = game_time;
			Explosion explosion;
			explosion.position = sprite_submarine.getPosition();
			explosion.time_started = game_time;
			explosions.push_back(explosion);
			sprite_explosion.setColor({255, 255, 255, 255});
			play_sound("explosion");
			play_sound("gameover");
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

				play_sound("explosion");
				play_sound("splash");

				num_monsters_killed++;
			}
		}
	}
}

void clean()
{
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
		if (game_state != GAME_OVER)
			sprite_explosion.setColor({255, 255, 255, static_cast<sf::Uint8>(-((scale*2-1) * 255))});
		sprite_explosion.setScale(scale, scale);
		window.draw(sprite_explosion);
	}

	if (game_state == NEW_LEVEL) {
		window.draw(sprite_level);
		draw_number(game_level, sf::Vector2f(window.getSize()/2u) + sf::Vector2f(100, 0));
	} else if (game_state == GAME_OVER) {
		sprite_level.setPosition(sf::Vector2f(window.getSize()/2u) - sf::Vector2f(140, 140));
		window.draw(sprite_gameover);
		window.draw(sprite_score);
		window.draw(sprite_level);
		window.draw(sprite_monsterskilled);
		window.draw(sprite_torpedosfired);
		draw_number(num_monsters_killed*2 - num_torpedos_fired, {450, 70});
		draw_number(game_level, {450, 170});
		draw_number(num_monsters_killed, {450, 445});
		draw_number(num_torpedos_fired, {450, 530});
	} else if (game_state == PAUSED) {
		fullscreen_rect.setFillColor({0, 0, 0, 128});
		window.draw(fullscreen_rect);
		window.draw(pause_rect1);
		window.draw(pause_rect2);
	}
}

int main()
{
	srand(time(NULL));

	window.create({800, 600}, "Submarine of Death");
	window.setFramerateLimit(60);

	texture_squid.loadFromFile("../res/squid.png");
	texture_submarine.loadFromFile("../res/submarine.png");
	texture_torpedo.loadFromFile("../res/torpedo.png");
	texture_explosion.loadFromFile("../res/explosion.png");
	texture_gameover.loadFromFile("../res/gameover.png");
	texture_level.loadFromFile("../res/level.png");
	texture_numbers.loadFromFile("../res/numbers.png");
	texture_score.loadFromFile("../res/score.png");
	texture_torpedosfired.loadFromFile("../res/torpedosfired.png");
	texture_monsterskilled.loadFromFile("../res/monsterskilled.png");
	texture_minus.loadFromFile("../res/minus.png");

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
	sprite_numbers.setTexture(texture_numbers);
	sprite_numbers.setOrigin(sf::Vector2f(texture_numbers.getSize().x / 20u, texture_numbers.getSize().y / 2u));
	sprite_score.setTexture(texture_score);
	sprite_score.setOrigin(sf::Vector2f(texture_score.getSize() / 2u));
	sprite_score.setPosition(250, 60);
	sprite_torpedosfired.setTexture(texture_torpedosfired);
	sprite_torpedosfired.setOrigin(sf::Vector2f(texture_torpedosfired.getSize().x, texture_torpedosfired.getSize().y / 2u));
	sprite_torpedosfired.setPosition(380, 520);
	sprite_monsterskilled.setTexture(texture_monsterskilled);
	sprite_monsterskilled.setOrigin(sf::Vector2f(texture_monsterskilled.getSize().x, texture_monsterskilled.getSize().y / 2u));
	sprite_monsterskilled.setPosition(380, 440);
	sprite_minus.setTexture(texture_minus);
	sprite_minus.setOrigin(sf::Vector2f(texture_minus.getSize() / 2u));

	pause_rect1.setOrigin(pause_rect1.getSize() / 2.f);
	pause_rect2.setOrigin(pause_rect2.getSize() / 2.f);
	pause_rect1.setFillColor({255, 255, 255, 200});
	pause_rect2.setFillColor({255, 255, 255, 200});
	pause_rect1.setPosition(380, 300);
	pause_rect2.setPosition(420, 300);

	sound_pool.resize(4);
	load_sound("splash");
	load_sound("explosion");
	load_sound("gameover");
	load_sound("pew");
	load_sound("beepboop");

	beepboop_sound.setBuffer(sound_buffers["beepboop"]);

	sf::Clock theclock;

	prepare_new_level();

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			handleEvent(event);
		}

		update(theclock.restart().asSeconds());
		if (game_state != GAME_OVER) {
			clean();
		}

		window.clear({20, 102, 126});

		draw();

		window.display();
	}
}
