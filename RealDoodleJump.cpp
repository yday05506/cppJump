#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <random>
#include <ctime>

using namespace sf;
using namespace std;

int main() {
	RenderWindow window(VideoMode(500, 700), "Jump Game", Style::Close);	// 리사이즈 불가능
	window.setFramerateLimit(60);
	
	// 이미지 변수 선언
	Texture backTexture;
	Texture playerTexture;
	Texture platTexture;
	Texture obstacleTexture;
	Texture healTexture;
	Texture mortarTexture;

	// 이미지 파일 불러오기
	backTexture.loadFromFile("images/background2.png");
	playerTexture.loadFromFile("images/rabbit2.png");
	platTexture.loadFromFile("images/step4.png");
	obstacleTexture.loadFromFile("images/rock.png");
	healTexture.loadFromFile("images/hp_potion.png");
	mortarTexture.loadFromFile("images/mortar.png");

	// 질감 넣기
	Sprite background(backTexture);
	Sprite player(playerTexture);
	Sprite plat(platTexture);
	Sprite obstacle(obstacleTexture);
	Sprite heal(healTexture);
	Sprite mortar(mortarTexture);

	// 게임오버 배경 넣기
	RectangleShape gameoverBackground(Vector2f(500, 700));	// 사이즈 설정
	gameoverBackground.setFillColor(Color::White);	// 배경 하얀색으로 칠하기

	// 폰트
	Font font;
	font.loadFromFile("font/EF_Diary.ttf");	// 폰트 파일 불러오기

	// 텍스트
	Text scoreText;	// 점수 텍스트
	scoreText.setFont(font);	// 폰트 적용
	scoreText.setCharacterSize(30);	// 텍스트 사이즈 설정
	scoreText.setFillColor(Color::White);	// 텍스트 색상 설정

	Text gameoverText;	// 게임 오버 텍스트
	gameoverText.setFont(font);	// 폰트 적용하기
	gameoverText.setString("Game Over");	// 문자열 설정
	gameoverText.setCharacterSize(80);	// 텍스트 사이즈 설정
	gameoverText.setFillColor(Color::Red);	// 텍스트 색상 설정

	Text hpText;	// 체력 텍스트
	hpText.setFont(font);	// 폰트 적용하기
	hpText.setCharacterSize(30);	// 텍스트 사이즈 설정
	hpText.setFillColor(Color::White);	// 텍스트 색상 설정

	// 사운드
	SoundBuffer buffer;
	buffer.loadFromFile("audios/MP_Blop.wav");	// 파일 불러오기
	Sound sound;
	sound.setBuffer(buffer);	// 사운드에 버퍼 넣기

	// 발판 초기화
	Vector2u platPosition[10];
	uniform_int_distribution<unsigned> px(0, 500 - platTexture.getSize().x);
	uniform_int_distribution<unsigned> py(100, 700);
	default_random_engine pe(time(0));	// 랜덤

	for (size_t i = 0; i < 10; ++i) {
		platPosition[i].x = px(pe);
		platPosition[i].y = py(pe);
	}

	// 장애물 초기화
	Vector2u obstaclePosition[2];
	uniform_int_distribution<unsigned> obx(0, 500 - obstacleTexture.getSize().x);
	uniform_int_distribution<unsigned> oby(100, 700);
	default_random_engine obe(time(0));	// 랜덤

	for (size_t i = 0; i < 2; ++i) {
		obstaclePosition[i].x = obx(obe);
		obstaclePosition[i].y = oby(obe);
	}

	// 힐 포션 초기화
	Vector2u healPosition;
	uniform_int_distribution<unsigned> hx(0, 500 - healTexture.getSize().x);
	uniform_int_distribution<unsigned> hy(100, 700);
	default_random_engine he(time(0));	// 랜덤

	healPosition.x = hx(he);
	healPosition.y = hy(he);

	// 절구 초기화
	Vector2u mortarPosition;
	uniform_int_distribution<unsigned> mx(0, 500 - mortarTexture.getSize().x);
	uniform_int_distribution<unsigned> my(100, 700);
	default_random_engine me(time(0));	// 랜덤

	// 플레이어의 위치와 중력
	int playerX = 250;
	int playerY = 151;
	float dy = 0;	// 높이
	int height = 150;
	int score = 0;	// 점수
	int hp = 3;	// 체력

	// 플레이어 바운딩 박스. 사이즈가 달라지면 바꿔야 함
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

		// 플레이어가 바운더리를 나가면 창 돌아오기
		if (playerX > 500)
			playerX = 0;
		if (playerX < -40)	// -40 == playerTexture x 사이즈 / 2
			playerX = window.getSize().x - playerTexture.getSize().x;

		// 점수
		// dy -1.60001은 플레이어가 발판에 서서 더 이상 올라가지 않음
		// 이 상황에선 점수가 올라가지 않음
		if (playerY == height && dy < (-1.62)) {
			score += 1;
			scoreText.setString("SCORE : " + to_string(score));	// 점수 텍스트 설정
			scoreText.setPosition(20, 0);	// 점수 텍스트 위치 설정	
			hpText.setString("HP : " + to_string(hp));	// HP 텍스트 설정
			hpText.setPosition(20, 60);	// HP 텍스트 위치 설정
		}

		// 플레이어 점프 매커니즘
		dy += 0.2;	// 높이 0.2씩
		playerY += dy;	// 플레이어 올라가기

		if (playerY < height) {
			// 발판
			for (size_t i = 0; i < 10; ++i) {
				playerY = height;
				platPosition[i].y -= dy;	// 수직 변환
				if (platPosition[i].y > 700)	// 새 발판을 위에 두기
				{
					platPosition[i].y = 0;
					platPosition[i].x = px(pe);
				}
			}

			// 장애물
			for (size_t i = 0; i < 2; ++i) {
				playerY = height; 
				obstaclePosition[i].y -= dy;	// 수직 변환
				if (obstaclePosition[i].y > 700)	// 새 장애물을 위에 두기
				{
					obstaclePosition[i].y = 0;
					obstaclePosition[i].x = obx(obe);
				}
			}

			// 힐 포션
			playerY = height;
			healPosition.y -= dy;	// 수직 변환
			if (healPosition.y > 700)	// 새 포션을 위에 두기
			{
				healPosition.y = 0;
				healPosition.x = hx(he);
			}

			// 절구
			playerY = height;
			mortarPosition.y -= dy;	// 수직 변환
			if (mortarPosition.y > 700)	// 새 포션을 위에 두기
			{
				mortarPosition.y = 0;
				mortarPosition.x = mx(me);
			}
		}

		// 점프 감지
		// 발판
		for (size_t i = 0; i < 10; ++i) {
			if ((playerX + PLAYER_RIGHT_BOUNDING_BOX > platPosition[i].x) && (playerX + PLAYER_LEFT_BOUNDING_BOX < platPosition[i].x + platTexture.getSize().x)	// 플레이어의 수평 범위가 발판을 건드릴 수 있음
				&& (playerY + PLAYER_BOTTOM_BOUNDING_BOX > platPosition[i].y) && (playerY + PLAYER_BOTTOM_BOUNDING_BOX < platPosition[i].y + platTexture.getSize().y)	// 플레이어의 수직 범위가 발판을 건드릴 수 있음
				&& (dy > 0))	// 플레이어가 떨어질 때
			{
				sound.play();
				dy -= 17;
			}
		}
		// 장애물
		for (size_t i = 0; i < 2; ++i) {
			if ((playerX + PLAYER_RIGHT_BOUNDING_BOX > obstaclePosition[i].x) && (playerX + PLAYER_LEFT_BOUNDING_BOX < obstaclePosition[i].x + obstacleTexture.getSize().x)	// 플레이어의 수평 범위가 장애물을 건드릴 수 있음
				&& (playerY + PLAYER_BOTTOM_BOUNDING_BOX > obstaclePosition[i].y) && (playerY + PLAYER_BOTTOM_BOUNDING_BOX < obstaclePosition[i].y + obstacleTexture.getSize().y)	// 플레이어의 수직 범위가 장애물을 건드릴 수 있음
				&& (dy > 0))	// 플레이어가 떨어질 때
			{
				sound.play();
				hp -= 1;	// 밟으면 체력 -1
				dy -= 17;
			}
		}
		// 힐 포션
		if ((playerX + PLAYER_RIGHT_BOUNDING_BOX > healPosition.x) && (playerX + PLAYER_LEFT_BOUNDING_BOX < healPosition.x + healTexture.getSize().x)	// 플레이어의 수평 범위가 포션을 건드릴 수 있음
			&& (playerY + PLAYER_BOTTOM_BOUNDING_BOX > healPosition.y) && (playerY + PLAYER_BOTTOM_BOUNDING_BOX < healPosition.y + healTexture.getSize().y)	// 플레이어의 수직 범위가 포션을 건드릴 수 있음
			&& (dy > 0))	// 플레이어가 떨어질 때
		{
			sound.play();
			hp += 1;	// 밟으면 체력 +1
			dy -= 17;
		}
		// 절구
		if ((playerX + PLAYER_RIGHT_BOUNDING_BOX > mortarPosition.x) && (playerX + PLAYER_LEFT_BOUNDING_BOX < mortarPosition.x + mortarTexture.getSize().x)	// 플레이어의 수평 범위가 절구를 건드릴 수 있음
			&& (playerY + PLAYER_BOTTOM_BOUNDING_BOX > mortarPosition.y) && (playerY + PLAYER_BOTTOM_BOUNDING_BOX < mortarPosition.y + mortarTexture.getSize().y)	// 플레이어의 수직 범위가 절구를 건드릴 수 있음
			&& (dy > 0))	// 플레이어가 떨어질 때
		{
			sound.play();
			dy -= 35;	// 밟으면 높이 올라감
		}
		player.setPosition(playerX, playerY);

		window.draw(background);
		window.draw(player);

		// 발판 그리기
		for (size_t i = 0; i < 10; ++i) {
			plat.setPosition(platPosition[i].x, platPosition[i].y);
			window.draw(plat);
		}

		// 장애물 그리기
		for (size_t i = 0; i < 2; ++i) {
			obstacle.setPosition(obstaclePosition[i].x, obstaclePosition[i].y);
			window.draw(obstacle);
		}

		// 포션 그리기
		heal.setPosition(healPosition.x, healPosition.y);
		window.draw(heal);

		// 절구 그리기
		mortar.setPosition(mortarPosition.x, mortarPosition.y);
		window.draw(mortar);

		// game over
		if (playerY > 700 || hp == 0)
		{
			gameoverText.setPosition(30, 200);	// 게임오버 텍스트 위치 설정
			scoreText.setPosition(150, 400);	// 점수 텍스트 위치 설정
			scoreText.setFillColor(Color::Black);	// 점수 텍스트 색상 변경
			hpText.setPosition(150, 450);	// hp 텍스트 위치 설정
			goto gameover;	// 게임오버 화면으로 가기 → goto 쓰지 말 것. 로직이 꼬일 수 있음
		}
		window.draw(scoreText);	// 점수 텍스트 그리기
		window.draw(hpText);	// 체력 텍스트 그리기
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
		window.draw(gameoverBackground);	// 게임오버 배경 그리기
		window.draw(gameoverText);	// 게임오버 텍스트 그리기
		window.draw(scoreText);	// 점수 텍스트 그리기
		window.display();
	}
	return 0;
}