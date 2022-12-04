#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <random>
#include <ctime>

#define WIDTH 500
#define HEIGHT 700

int main() {
	sf::RenderWindow window(sf::VideoMode(500, 700), "Jump", sf::Style::Close);
	window.setFramerateLimit(60);
	sf::Texture backgroundTexture;	// 배경
	sf::Texture playerTexture;	// 캐릭터
	sf::Texture platformTexture;	// 발판
	sf::Texture obstacleTexture;	// 장애물
	sf::Texture potionTexture;	// 포션

	backgroundTexture.loadFromFile("images/ground_back.png");	// 배경 이미지
	playerTexture.loadFromFile("images/character.png");	// 캐릭터 이미지
	platformTexture.loadFromFile("images/step2.png");	// 발판 이미지
	obstacleTexture.loadFromFile("images/rock.png");	// 장애물 이미지
	potionTexture.loadFromFile("images/hp_potion.png");	// 포션 이미지

	sf::Sprite background(backgroundTexture);	// 배경
	sf::Sprite player(playerTexture);	// 캐릭터
	sf::Sprite plat(platformTexture);	// 발판
	sf::Sprite obstacle(obstacleTexture);	// 장애물
	sf::Sprite potion(potionTexture);	// 포션

	sf::RectangleShape gameoverBackground(sf::Vector2f(500, 700));
	gameoverBackground.setFillColor(sf::Color::White);

	sf::Font font;
	font.loadFromFile("font/EF_Diary.ttf");	// 폰트
	sf::Text scoreText;
	scoreText.setFont(font);
	scoreText.setCharacterSize(30);
	scoreText.setFillColor(sf::Color::Black);
	
	sf::Text gameoverText;
	gameoverText.setFont(font);
	gameoverText.setString("Game Over");
	gameoverText.setCharacterSize(80);
	gameoverText.setFillColor(sf::Color::Red);

	sf::Text hpText;
	hpText.setFont(font);
	hpText.setCharacterSize(30);
	hpText.setFillColor(sf::Color::Black);

	//sound
	sf::SoundBuffer buffer;
	buffer.loadFromFile("audios/MP_Blop.wav");
	sf::Sound sound;
	sound.setBuffer(buffer);

	// 발판 초기화
	sf::Vector2u platformPosition[7];
	std::uniform_int_distribution<unsigned> platX(0, 500 - platformTexture.getSize().x);
	std::uniform_int_distribution<unsigned> platY(100, 700);
	std::default_random_engine platE(time(0));
	for (size_t i = 0; i < 7; ++i) {
		platformPosition[i].x = platX(platE);
		platformPosition[i].y = platY(platE);
	}

	// 장애물 초기화
	sf::Vector2u obstaclePosition[2];
	std::uniform_int_distribution<unsigned> obX(0, 500 - obstacleTexture.getSize().x);
	std::uniform_int_distribution<unsigned> obY(100, 700);
	std::default_random_engine obE(time(0));
	for (size_t i = 0; i < 2; i++) {
		obstaclePosition[i].x = obX(obE);
		obstaclePosition[i].y = obY(obE);
	}

	// 포션 초기화
	sf::Vector2u potionPosition;
	std::uniform_int_distribution<unsigned> potionX(0, 500 - potionTexture.getSize().x);
	std::uniform_int_distribution<unsigned> potionY(100, 700);
	std::default_random_engine potionE(time(0));
	potionPosition.x = potionX(potionE);
	potionPosition.y = potionY(potionE);

	// 플레이어 위치 & 속도 줄이기
	int playerX = 250;
	int playerY = 151;
	float dy = 0;
	int height = 150;
	int score = 0;
	int hp = 3;

	// 플레이어 바운딩 박스
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

		// 플레이어가 바운더리를 나가면 창 돌아오기
		if (playerX > 500)
			playerX = 0;
		if (playerX < -40)
			playerX = window.getSize().x - playerTexture.getSize().x;

		if (playerY == height && dy < (-1.62))
		{
			score += 1;
		}

		if (player.getGlobalBounds().intersects(plat.getGlobalBounds()) && (dy < 10)) {
			sound.play();
			dy -= 10;
		}

		if (player.getGlobalBounds().intersects(obstacle.getGlobalBounds())) { //intersects 교집합
			//플레이어와 장애물의 충돌 시 움직임이 느려짐 (끈끈이 이미지)
			hp -= 1;
			dy -= 10;
		}

		scoreText.setString("SCORE : " + std::to_string(score));
		scoreText.setPosition(20, 0);
		hpText.setString("HP : " + std::to_string(hp));
		hpText.setPosition(20, 60);
		

		// 플레이어 점프 매커니즘
		dy += 0.2;
		playerY += dy;

		if (playerY < height) {
			for (size_t i = 0; i < 7; ++i) {
				playerY = height;
				platformPosition[i].y -= dy;
				if (platformPosition[i].y > 700)	// 새로운 플랫폼을 맨 위에 두기
				{
					platformPosition[i].y = 0;
					platformPosition[i].x = platX(platE);
				}
			}
			for (size_t i = 0; i < 2; ++i) {
				playerY = height;
				obstaclePosition[i].y -= dy;
				if (obstaclePosition[i].y > 700)	// 새로운 장애물을 맨 위에 두기
				{
					obstaclePosition[i].y = 0;
					obstaclePosition[i].x = obX(obE);
				}
			}
			playerY = height;
			potionPosition.y -= dy;
			if (potionPosition.y > 700)	// 새로운 포션을 맨 위에 두기
			{
				potionPosition.y = 0;
				potionPosition.x = potionX(potionE);
			}
		}
		
		player.setPosition(playerX, playerY);

		window.draw(background);
		window.draw(player);

		for (size_t i = 0; i < 7; ++i) {
			plat.setPosition(platformPosition[i].x, platformPosition[i].y);
			window.draw(plat);
		}

		for (size_t i = 0; i < 2; ++i) {
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
		if (hp == 0)
		{
			gameoverText.setPosition(30, 200);
			scoreText.setPosition(150, 400);
			goto gameover;
		}
		window.draw(scoreText);
		window.draw(hpText);
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
		window.draw(hpText);
		window.display();
	}

	return 0;
}