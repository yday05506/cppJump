#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <random>
#include <ctime>

int main() {
	sf::RenderWindow window(sf::VideoMode(500, 700), "Jump", sf::Style::Close);
	window.setFramerateLimit(60);
	sf::Texture backgroundTexture;	// ���
	sf::Texture playerTexture;	// ĳ����
	sf::Texture platformTexture;	// ����
	sf::Texture obstacleTexture;	// ��ֹ�

	backgroundTexture.loadFromFile("images/ground_back.png");	// ��� �̹���
	playerTexture.loadFromFile("images/character.png");	// ĳ���� �̹���
	platformTexture.loadFromFile("images/step2.png");	// ���� �̹���
	obstacleTexture.loadFromFile("images/obstacle.png");	// ��ֹ� �̹���

	sf::Sprite background(backgroundTexture);	// ���
	sf::Sprite player(playerTexture);	// ĳ����
	sf::Sprite plat(platformTexture);	// ����
	sf::Sprite obstacle(obstacleTexture);	// ��ֹ�

	sf::RectangleShape gameoverBackground(sf::Vector2f(500, 700));
	gameoverBackground.setFillColor(sf::Color::White);

	sf::Font font;
	font.loadFromFile("font/EF_Diary.ttf");	// ��Ʈ
	sf::Text scoreText;
	scoreText.setFont(font);
	scoreText.setCharacterSize(50);
	scoreText.setFillColor(sf::Color::Red);
	
	sf::Text gameoverText;
	gameoverText.setFont(font);
	gameoverText.setString("Game Over");
	gameoverText.setCharacterSize(80);
	gameoverText.setFillColor(sf::Color::Red);

	//sound
	//sf::SoundBuffer buffer;
	//buffer.loadFromFile("sound/jump.wav");
	//sf::Sound sound;
	//sound.setBuffer(buffer);

	// ���� �ʱ�ȭ
	sf::Vector2u platformPosition[8];
	std::uniform_int_distribution<unsigned> platX(0, 500 - platformTexture.getSize().x);
	std::uniform_int_distribution<unsigned> platY(100, 700);
	std::default_random_engine platE(time(0));
	for (size_t i = 0; i < 8; ++i) {
		platformPosition[i].x = platX(platE);
		platformPosition[i].y = platY(platE);
	}

	// ��ֹ� �ʱ�ȭ
	sf::Vector2u obstaclePosition[3];
	std::uniform_int_distribution<unsigned> obX(0, 500 - obstacleTexture.getSize().x);
	std::uniform_int_distribution<unsigned> obY(100, 700);
	std::default_random_engine obE(time(0));
	for (size_t i = 0; i < 3; i++) {
		obstaclePosition[i].x = obX(obE);
		obstaclePosition[i].y = obY(obE);
	}

	// �÷��̾� ��ġ & �ӵ� ���̱�
	int playerX = 250;
	int playerY = 151;
	float dy = 0;
	int height = 150;
	int score = 0;

	// �÷��̾� �ٿ�� �ڽ�
	const int PLAYER_LEFT_BOUNDING_BOX = 20;
	const int PLAYER_RIGHT_BOUNDING_BOX = 60;
	const int PLAYER_BOTTOM_BOUNDING_BOX = 70;
	
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			playerX -= 4;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			playerX += 4;

		if (playerX > 500)
			playerX = 0;
		if (playerX < -40)
			playerX = window.getSize().x - playerTexture.getSize().x;

		if (playerY == height && dy < (-1.62))
		{
			score += 1;
			scoreText.setString("Score: " + std::to_string(score));
		}

		// ��ֹ��� �浹 -> ��ī���̵� �� ��� ������ �浹? ���ñ�. �װ� ������
		

		// �÷��̾� ���� ��Ŀ����
		dy += 0.2;
		playerY += dy;

		if (playerY < height) {
			for (size_t i = 0; i < 8; ++i) {
				playerY = height;
				platformPosition[i].y -= dy;
				if (platformPosition[i].y > 700)	// ���ο� �÷����� �� ���� �α�
				{
					platformPosition[i].y = 0;
					platformPosition[i].x = platX(platE);
				}
			}
			for (size_t i = 0; i < 3; ++i) {
				playerY = height;
				obstaclePosition[i].y -= dy;
				if (obstaclePosition[i].y > 700)	// ���ο� ��ֹ��� �� ���� �α�
				{
					obstaclePosition[i].y = 0;
					obstaclePosition[i].x = obX(obE);
				}
			}
		}

		for (size_t i = 0; i < 10; ++i)
		{
			if ((playerX + PLAYER_RIGHT_BOUNDING_BOX > platformPosition[i].x) && (playerX + PLAYER_LEFT_BOUNDING_BOX < platformPosition[i].x + platformTexture.getSize().x)
				&& (playerY + PLAYER_BOTTOM_BOUNDING_BOX > platformPosition[i].y) && (playerY + PLAYER_BOTTOM_BOUNDING_BOX < platformPosition[i].y + platformTexture.getSize().y)
				&& (dy > 0))	// �÷��̾ ������ �� 
			{
				//sound.play();
				dy = -10;
			}
		}
		player.setPosition(playerX, playerY);

		window.draw(background);
		window.draw(player);

		for (size_t i = 0; i < 8; ++i) {
			plat.setPosition(platformPosition[i].x, platformPosition[i].y);
			window.draw(plat);
		}

		for (size_t i = 0; i < 3; ++i) {
			obstacle.setPosition(obstaclePosition[i].x, obstaclePosition[i].y);
			window.draw(obstacle);
		}

		// game over
		if (playerY > 700) 
		{
			gameoverText.setPosition(30, 200);
			scoreText.setPosition(150, 400);
			goto gameover;
		}
		window.draw(scoreText);
		window.display();
 	}

	// Game Over
gameover:
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}
		window.draw(gameoverBackground);
		window.draw(gameoverText);
		window.draw(scoreText);
		window.display();
	}

	return 0;
}