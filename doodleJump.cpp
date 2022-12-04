#include <SFML/Graphics.hpp>
#include <ctime>
#include <vector>
#include <SFML/Audio.hpp>
using namespace sf;
using namespace std;

#define WIDTH 400	// 가로
#define HEIGHT 600	// 세로
#define BAR_COUNT 8	// 밟는 바 개수
#define OB_COUNT 2	// 장애물 개수
static const float GRAVITY = 0.2f;	// 중력

// 플레이어 클래스
class Player
{
private:
	int x, y;
	int imgWidth, imgHeight;
	float dy;
	Sprite* imgJump;
	Sprite* imgReady;
	Texture jump;
	Texture ready;
	bool jumpFlag;

	const Sprite& GetImg() {
		if (jumpFlag)
			return *imgJump;
		else
			return *imgReady;
	}

public:
	Player() : dy(0), jumpFlag(true) {
		x = static_cast<int>(WIDTH / 2);
		y = static_cast<int>(HEIGHT / 2);

		jump.loadFromFile("images/character.png");
		ready.loadFromFile("images/character.png");

		imgJump = new Sprite(jump);
		imgReady = new Sprite(ready);

		imgWidth = static_cast<int>(imgReady->getTexture()->getSize().x);
		imgHeight = static_cast<int>(imgReady->getTexture()->getSize().y);
	}
	~Player() {
		delete(imgJump);
		delete(imgReady);
	}

	void setPosition() {
		imgReady->setPosition(x, y);
		imgJump->setPosition(x, y);
	}
	void Move() {
		if (Keyboard::isKeyPressed(Keyboard::Right))	// 오른쪽 이동
			x += 4;
		if (Keyboard::isKeyPressed(Keyboard::Left))	// 왼쪽 이동
			x -= 4;
		if (x >= WIDTH)	// 벽 건너감
			x = 0;
		if (x < -40)	// 벽 건너감
			x = WIDTH - jump.getSize().x;

		jumpFlag = true;
		dy += GRAVITY;
		y += static_cast<int>(dy);

		if (y > HEIGHT - imgHeight) {
			jumpFlag = false;
			dy -= 15;
		}
	}

	void Draw(RenderWindow& window) {
		window.draw(GetImg());
	}

	float GetDy() const {
		return dy;
	}
	int GetY() const {
		return y;
	}
	int GetX() const {
		return x;
	}
	int GetWidth() const {
		return imgWidth;
	}
	int GetHeight() const {
		return imgHeight;
	}
	void setY(int _y) {
		y = _y;
	}
	void Jump() {
		jumpFlag = false;
		dy -= 15;
	}
};

// 점프 바 클래스
class Bar {
private:
	struct Pos {
		int x, y;
	};
	vector<Pos> vBar;
	Sprite* imgBar;
	Texture bar;
	int imgWidth;

public:
	Bar() {
		srand(static_cast<unsigned int>(time(nullptr)));

		bar.loadFromFile("images/step3.png");
		imgBar = new Sprite(bar);

		imgWidth = imgBar->getTexture()->getSize().x;

		for (int i = 0; i < BAR_COUNT; ++i) {
			Pos p;
			p.x = rand() % WIDTH - imgWidth / 2;
			p.y = rand() % HEIGHT;
			vBar.push_back(p);
		}
		vBar[0].y = HEIGHT - 200;
	}
	~Bar() {
		delete(imgBar);
	}

	void Draw(RenderWindow& window) {
		for (int i = 0; i < BAR_COUNT; ++i) {
			imgBar->setPosition(vBar[i].x, vBar[i].y);
			window.draw(*imgBar);
		}
	}

	bool CheckCollision(Player* pPlayer) {
		// null 체크
		if (pPlayer == nullptr) {
			return false;
		}

		for (int i = 0; i < BAR_COUNT; ++i) {
			if (pPlayer->GetDy() > 0
				&& pPlayer->GetX() + pPlayer->GetWidth() > vBar[i].x
				&& pPlayer->GetX() < vBar[i].x + imgWidth
				&& pPlayer->GetY() + pPlayer->GetHeight() > vBar[i].y
				&& pPlayer->GetY() + pPlayer->GetHeight() < vBar[i].y + 10) {
				pPlayer->Jump();
				return true;
			}
		}
		return false;
	}

	void MoveAndReset(Player* pPlayer) {
		static const int limit = HEIGHT / 3;
		if (pPlayer->GetY() < limit) {
			for (int i = 0; i < BAR_COUNT; ++i) {
				pPlayer->setY(limit);
				vBar[i].y -= static_cast<int>(pPlayer->GetDy());
				if (vBar[i].y > HEIGHT + 10) {
					vBar[i].y = rand() % HEIGHT / 3 + 100;
					vBar[i].x = rand() % WIDTH;
				}
			}
		}
	}
};

// 장애물 클래스
class Obstacle {
private:
	struct Pos {
		int x, y;
	};
	vector<Pos> vObstacle;
	Sprite* imgOb;
	Texture obstacle;
	int imgWidth;

public:
	Obstacle() {
		srand(static_cast<unsigned int>(time(nullptr)));

		obstacle.loadFromFile("images/rock.png");
		imgOb = new Sprite(obstacle);

		imgWidth = imgOb->getTexture()->getSize().x;

		for (int i = 0; i < OB_COUNT; ++i) {
			Pos p;
			p.x = rand() % WIDTH - imgWidth / 2;
			p.y = rand() % HEIGHT;
			vObstacle.push_back(p);
		}
		vObstacle[0].y = HEIGHT - 200;
	}
	~Obstacle() {
		delete(imgOb);
	}

	void Draw(RenderWindow& window) {
		for (int i = 0; i < OB_COUNT; ++i) {
			imgOb->setPosition(vObstacle[i].x, vObstacle[i].y);
			window.draw(*imgOb);
		}
	}

	bool CheckCollision(Player* pPlayer) {
		// null 체크
		if (pPlayer == nullptr) {
			return false;
		}

		for (int i = 0; i < OB_COUNT; ++i) {
			if (pPlayer->GetDy() > 0
				&& pPlayer->GetX() + pPlayer->GetWidth() > vObstacle[i].x
				&& pPlayer->GetX() < vObstacle[i].x + imgWidth
				&& pPlayer->GetY() + pPlayer->GetHeight() > vObstacle[i].y
				&& pPlayer->GetY() + pPlayer->GetHeight() < vObstacle[i].y + 10) {
				pPlayer->Jump();
				return true;
			}
		}
		return false;
	}

	void MoveAndReset(Player* pPlayer) {
		static const int limit = HEIGHT / 3;
		if (pPlayer->GetY() < limit) {
			for (int i = 0; i < OB_COUNT; ++i) {
				pPlayer->setY(limit);
				vObstacle[i].y -= static_cast<int>(pPlayer->GetDy());
				if (vObstacle[i].y > HEIGHT + 10) {
					vObstacle[i].y = rand() % HEIGHT / 3 + 100;
					vObstacle[i].x = rand() % WIDTH;
				}
			}
		}
	}
};

int main(void) {
	RenderWindow window(VideoMode(WIDTH, HEIGHT), "Jump Game");
	window.setFramerateLimit(60);

	// 폰트
	Font font;
	font.loadFromFile("font/EF_Diary.ttf");	// 폰트
	
	// 점수 텍스트
	Text scoreText;
	scoreText.setFont(font);
	scoreText.setCharacterSize(30);
	scoreText.setFillColor(Color::Black);

	// 게임오버 텍스트
	Text gameoverText;
	gameoverText.setFont(font);
	gameoverText.setString("Game Over");
	gameoverText.setCharacterSize(80);
	gameoverText.setFillColor(Color::Red);

	// 체력 텍스트
	Text hpText;
	hpText.setFont(font);
	hpText.setCharacterSize(30);
	hpText.setFillColor(Color::Black);

	//sound
	SoundBuffer buffer;
	buffer.loadFromFile("audios/MP_Blop.wav");
	Sound sound;
	sound.setBuffer(buffer);

	// 세팅
	Player* pPlayer = new Player();
	Bar* pBar = new Bar();
	Obstacle* pOb = new Obstacle();

	while (window.isOpen()) {
		Event e;
		if (window.pollEvent(e)) {
			if (e.type == Event::Closed) {
				window.close();
			}
		}

		// 로직
		pPlayer->Move();
		pBar->MoveAndReset(pPlayer);
		pOb->MoveAndReset(pPlayer);
		pBar->CheckCollision(pPlayer);
		pOb->CheckCollision(pPlayer);
		pPlayer->setPosition();

		// 그리기
		window.clear(Color::White);
		pPlayer->Draw(window);
		pBar->Draw(window);
		pOb->Draw(window);
		window.display();
	}

	delete(pBar);
	delete(pPlayer);
	delete(pOb);
	
	return 0;
}