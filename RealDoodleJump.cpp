#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <random>
#include <ctime>

using namespace sf;
using namespace std;

int main() {
	RenderWindow window(VideoMode(500, 700), "Jump Game", Style::Close);	// �������� �Ұ���
	window.setFramerateLimit(60);
	
	// �̹��� ���� ����
	Texture backTexture;
	Texture playerTexture;
	Texture platTexture;
	Texture obstacleTexture;
	Texture healTexture;

	// �̹��� ���� �ҷ�����
	backTexture.loadFromFile("images/background.png");
	playerTexture.loadFromFile("images/rabbit2.png");
	platTexture.loadFromFile("images/step4.png");
	obstacleTexture.loadFromFile("images/rock.png");
	healTexture.loadFromFile("images/hp_potion.png");

	// ���� �ֱ�
	Sprite background(backTexture);
	Sprite player(playerTexture);
	Sprite plat(platTexture);
	Sprite obstacle(obstacleTexture);
	Sprite heal(healTexture);

	// ���ӿ��� ��� �ֱ�
	RectangleShape gameoverBackground(Vector2f(500, 700));
	gameoverBackground.setFillColor(Color::White);

	// ��Ʈ
	Font font;
	font.loadFromFile("font/EF_Diary.ttf");

	// �ؽ�Ʈ
	Text scoreText;	// ���� �ؽ�Ʈ
	scoreText.setFont(font);
	scoreText.setCharacterSize(30);
	scoreText.setFillColor(Color::Black);

	Text gameoverText;	// ���� ���� �ؽ�Ʈ
	gameoverText.setFont(font);
	gameoverText.setString("Game Over");
	gameoverText.setCharacterSize(80);
	gameoverText.setFillColor(Color::Red);

	Text hpText;	// ü�� �ؽ�Ʈ
	hpText.setFont(font);
	hpText.setCharacterSize(30);
	hpText.setFillColor(Color::Black);

	// ����
	SoundBuffer buffer;
	buffer.loadFromFile("audios/MP_Blop.wav");	// ���� �ҷ�����
	Sound sound;
	sound.setBuffer(buffer);	// ���忡 ���� �ֱ�

	// ���� �ʱ�ȭ
	Vector2u platPosition[10];
	uniform_int_distribution<unsigned> px(0, 500 - platTexture.getSize().x);
	uniform_int_distribution<unsigned> py(100, 700);
	default_random_engine pe(time(0));

	for (size_t i = 0; i < 10; ++i) {
		platPosition[i].x = px(pe);
		platPosition[i].y = py(pe);
	}

	// ��ֹ� �ʱ�ȭ
	Vector2u obstaclePosition[2];
	uniform_int_distribution<unsigned> obx(0, 500 - obstacleTexture.getSize().x);
	uniform_int_distribution<unsigned> oby(100, 700);
	default_random_engine obe(time(0));

	for (size_t i = 0; i < 2; ++i) {
		obstaclePosition[i].x = obx(obe);
		obstaclePosition[i].y = oby(obe);
	}

	// �� ���� �ʱ�ȭ
	Vector2u healPosition;
	uniform_int_distribution<unsigned> hx(0, 500 - healTexture.getSize().x);
	uniform_int_distribution<unsigned> hy(100, 700);
	default_random_engine he(time(0));

	healPosition.x = hx(he);
	healPosition.y = hy(he);

	// �÷��̾��� ��ġ�� �߷�
	int playerX = 250;
	int playerY = 151;
	float dy = 0;	// ����
	int height = 150;
	int score = 0;	// ����
	int hp = 3;	// ü��

	// �÷��̾� �ٿ�� �ڽ�. ����� �޶����� �ٲپ�� ��
	const int PLAYER_LEFT_BOUNDING_BOX = 20;
	const int PLAYER_RIGHT_BOUNDING_BOX = 60;
	const int PLAYER_BOTTOM_BOUNDING_BOX = 70;

	while (window.isOpen()) {
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed)
				window.close();
		}

		if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left))
			playerX -= 4;
		if (Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right))
			playerX += 4;

		// �÷��̾ �ٿ������ ������ â ���ƿ���
		if (playerX > 500)
			playerX = 0;
		if (playerX < -40)	// -40 == playerTexture x ������ / 2
			playerX = window.getSize().x - playerTexture.getSize().x;

		// ����
		// dy -1.60001�� �÷��̾ ���ǿ� ���� �� �̻� �ö��� �ʴ� �ӵ�
		// �� ��Ȳ���� ������ �ö��� ����
		if (playerY == height && dy < (-1.62)) {
			score += 1;
			scoreText.setString("SCORE : " + to_string(score));
			scoreText.setPosition(20, 0);
			hpText.setString("HP : " + to_string(hp));
			hpText.setPosition(20, 60);
		}

		// �÷��̾� ���� ��Ŀ����
		dy += 0.2;
		playerY += dy;

		if (playerY < height) {
			for (size_t i = 0; i < 10; ++i) {
				playerY = height;
				platPosition[i].y -= dy;	// ���� ��ȯ
				if (platPosition[i].y > 700)	// �� ������ ���� �α�
				{
					platPosition[i].y = 0;
					platPosition[i].x = px(pe);
				}
			}

			for (size_t i = 0; i < 2; ++i) {
				playerY = height; 
				obstaclePosition[i].y -= dy;	// ���� ��ȯ
				if (obstaclePosition[i].y > 700)	// �� ��ֹ��� ���� �α�
				{
					obstaclePosition[i].y = 0;
					obstaclePosition[i].x = obx(obe);
				}
			}

			playerY = height;
			healPosition.y -= dy;	// ���� ��ȯ
			if (healPosition.y > 700)	// �� ������ ���� �α�
			{
				healPosition.y = 0;
				healPosition.x = hx(he);
			}
		}

		// ���� ����
		for (size_t i = 0; i < 10; ++i) {
			if ((playerX + PLAYER_RIGHT_BOUNDING_BOX > platPosition[i].x) && (playerX + PLAYER_LEFT_BOUNDING_BOX < platPosition[i].x + platTexture.getSize().x)	// �÷��̾��� ���� ������ ������ �ǵ帱 �� ����
				&& (playerY + PLAYER_BOTTOM_BOUNDING_BOX > platPosition[i].y) && (playerY + PLAYER_BOTTOM_BOUNDING_BOX < platPosition[i].y + platTexture.getSize().y)	// �÷��̾��� ���� ������ ������ �ǵ帱 �� ����
				&& (dy > 0))	// �÷��̾ ������ ��
			{
				sound.play();
				dy -= 17;
			}
		}
		for (size_t i = 0; i < 2; ++i) {
			if ((playerX + PLAYER_RIGHT_BOUNDING_BOX > obstaclePosition[i].x) && (playerX + PLAYER_LEFT_BOUNDING_BOX < obstaclePosition[i].x + obstacleTexture.getSize().x)	// �÷��̾��� ���� ������ ��ֹ��� �ǵ帱 �� ����
				&& (playerY + PLAYER_BOTTOM_BOUNDING_BOX > obstaclePosition[i].y) && (playerY + PLAYER_BOTTOM_BOUNDING_BOX < obstaclePosition[i].y + obstacleTexture.getSize().y)	// �÷��̾��� ���� ������ ��ֹ��� �ǵ帱 �� ����
				&& (dy > 0))	// �÷��̾ ������ ��
			{
				sound.play();
				hp -= 1;
				dy -= 17;
			}
		}
		if ((playerX + PLAYER_RIGHT_BOUNDING_BOX > healPosition.x) && (playerX + PLAYER_LEFT_BOUNDING_BOX < healPosition.x + healTexture.getSize().x)	// �÷��̾��� ���� ������ ������ �ǵ帱 �� ����
			&& (playerY + PLAYER_BOTTOM_BOUNDING_BOX > healPosition.y) && (playerY + PLAYER_BOTTOM_BOUNDING_BOX < healPosition.y + healTexture.getSize().y)	// �÷��̾��� ���� ������ ������ �ǵ帱 �� ����
			&& (dy > 0))	// �÷��̾ ������ ��
		{
			sound.play();
			hp += 1;
			dy -= 16;
		}
		player.setPosition(playerX, playerY);

		window.draw(background);
		window.draw(player);

		// ���� �׸���
		for (size_t i = 0; i < 10; ++i) {
			plat.setPosition(platPosition[i].x, platPosition[i].y);
			window.draw(plat);
		}

		// ��ֹ� �׸���
		for (size_t i = 0; i < 2; ++i) {
			obstacle.setPosition(obstaclePosition[i].x, obstaclePosition[i].y);
			window.draw(obstacle);
		}

		// ���� �׸���
		heal.setPosition(healPosition.x, healPosition.y);
		window.draw(heal);

		// game over
		if (playerY > 700 || hp == 0)
		{
			gameoverText.setPosition(30, 200);
			scoreText.setPosition(150, 400);
			hpText.setPosition(150, 450);
			goto gameover;
		}
		window.draw(scoreText);
		window.draw(hpText);
		window.display();
	}

	// Game Over
gameover:
	while (window.isOpen()) {
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed)
				window.close();
		}
		window.draw(gameoverBackground);
		window.draw(gameoverText);
		window.draw(scoreText);
		window.display();
	}
	return 0;
}