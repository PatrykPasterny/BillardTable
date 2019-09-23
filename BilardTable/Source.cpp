#include <SFML\Graphics.hpp>
#include <random>
#include <iostream>
#include <windows.h>

float vectorLength(sf::Vector2f x)
{
	float length = pow(x.x, 2) + pow(x.y, 2);
	length = sqrt(length);
	return length;
}

float cosBetweenVectorsRad(sf::Vector2f x, sf::Vector2f y)
{
	sf::Vector2f z = x - y;
	float cosinus = (pow(vectorLength(x), 2) + pow(vectorLength(y), 2) - pow(vectorLength(z), 2)) / ((vectorLength(x)) * (vectorLength(y)));
	return cosinus;
}

class Ball
{
public:
	float dt;
	static float radius;

	Ball(sf::Vector2f pos)
	{
		this->setPosition(pos);
		billardBall.setRadius(radius);
		billardBall.setFillColor(sf::Color::Red);
		billardBall.setPointCount(60);
	}
	sf::CircleShape getBillardBall() const
	{
		return billardBall;
	}
	void setVelocity(sf::Vector2f v)
	{
		vel = v;
	}
	sf::Vector2f& getVelocity()
	{
		return vel;
	}
	sf::Vector2f getPosition()
	{
		return billardBall.getPosition() + sf::Vector2f({ Ball::radius,Ball::radius });
	} const
		void setPosition(sf::Vector2f position)
	{
		billardBall.setPosition(position - sf::Vector2f(Ball::radius, Ball::radius));
	}
	void Update(float deltat)
	{
		dt = deltat;
		this->setPosition(this->getPosition() + (vel * dt));
	}
	bool ifAllowedPosition(Ball& comparedBall)
	{
		if (abs(this->getPosition().x - comparedBall.getPosition().x) <= 2 * Ball::radius &&
			abs(this->getPosition().y - comparedBall.getPosition().y) <= 2 * Ball::radius)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	void handleBallCollision(Ball& comparedBall)
	{
		sf::Vector2f unchangedPositionC(comparedBall.getPosition());
		sf::Vector2f unchangedPosition(this->getPosition());
		sf::Vector2f changedPositionC(comparedBall.getPosition() - vel * dt);
		sf::Vector2f changedPosition(this->getPosition() - vel * dt);

		float dtToHit;
		float a = pow(vectorLength(this->getVelocity() - comparedBall.getVelocity()), 2);
		float b = -vectorLength(unchangedPosition - unchangedPositionC) * vectorLength(this->getVelocity() - comparedBall.getVelocity()) *
			cosBetweenVectorsRad((unchangedPosition - unchangedPositionC), (this->getVelocity() - comparedBall.getVelocity()));
		float c = pow(vectorLength(unchangedPosition - unchangedPositionC), 2) - pow(2 * Ball::radius, 2);

		float delta = pow(b, 2) - (4 * a*c);

		if (!(a == 0.0f || delta < 0))
		{
			float deltaT = this->dt;
			dtToHit = (-b + sqrt(delta)) / (2 * a);
			this->Update(-dtToHit);
			comparedBall.Update(-dtToHit);

			sf::Vector2f hitPositionC(comparedBall.getPosition());
			sf::Vector2f hitPosition(this->getPosition());

			sf::Vector2f vectorBetweenBallCentres(hitPosition - hitPositionC);
			float lengthBetweenBallCentres = vectorLength(vectorBetweenBallCentres);
			vectorBetweenBallCentres.x /= lengthBetweenBallCentres;
			vectorBetweenBallCentres.y /= lengthBetweenBallCentres;

			float usingCos = cosBetweenVectorsRad((this->getVelocity() - comparedBall.getVelocity()), vectorBetweenBallCentres);
			usingCos /= 2.0f;
			sf::Vector2f newVelocity;
			newVelocity.x = this->getVelocity().x - vectorBetweenBallCentres.x * (vectorLength(this->getVelocity() - comparedBall.getVelocity()) *
				vectorLength(vectorBetweenBallCentres) * usingCos);
			newVelocity.y = this->getVelocity().y - vectorBetweenBallCentres.y * (vectorLength(this->getVelocity() - comparedBall.getVelocity()) *
				vectorLength(vectorBetweenBallCentres) * usingCos);

			sf::Vector2f newVelocityC;
			newVelocityC.x = comparedBall.getVelocity().x + vectorBetweenBallCentres.x * (vectorLength(this->getVelocity() - comparedBall.getVelocity()) *
				vectorLength(vectorBetweenBallCentres) * usingCos);
			newVelocityC.y = comparedBall.getVelocity().y + vectorBetweenBallCentres.y * (vectorLength(this->getVelocity() - comparedBall.getVelocity()) *
				vectorLength(vectorBetweenBallCentres) * usingCos);

			this->setVelocity(newVelocity);
			comparedBall.setVelocity(newVelocityC);

			this->Update(dtToHit);
			comparedBall.Update(dtToHit);
			this->dt = deltaT;
			comparedBall.dt = deltaT;
		}

	}
private:
	sf::CircleShape billardBall;
	sf::Vector2f vel = { 150.0f,0.0f };
};

class Table
{
private:
	enum class BoundIndex
	{
		TopBound,
		BottomBound,
		LeftBound,
		RightBound,
		Count
	};
public:
	Table(float leftPoint, float topPoint, float width, float height, int amount)
		:
		ballsAmount(amount)
	{
		visibleTable.setFillColor(sf::Color::Green);
		for (int i = 0; i < int(BoundIndex::Count); i++)
		{
			nBounds[i].setFillColor(sf::Color::Color(150, 75, 0));
		}
		invisibleTable.left = leftPoint;
		invisibleTable.top = topPoint;
		invisibleTable.width = width;
		invisibleTable.height = height;
		visibleTable.setPosition({ leftPoint,topPoint });
		visibleTable.setSize({ width,height });
		nBounds[int(BoundIndex::LeftBound)].setPosition({ leftPoint - width / 20, topPoint });
		nBounds[int(BoundIndex::RightBound)].setPosition({ leftPoint + width, topPoint });
		nBounds[int(BoundIndex::TopBound)].setPosition({ leftPoint - width / 20, topPoint - width / 20 });
		nBounds[int(BoundIndex::BottomBound)].setPosition({ leftPoint - width / 20, topPoint + height });
		nBounds[int(BoundIndex::LeftBound)].setSize({ width / 20, height });
		nBounds[int(BoundIndex::RightBound)].setSize({ width / 20, height });
		nBounds[int(BoundIndex::TopBound)].setSize({ width + width / 10 , width / 20 });
		nBounds[int(BoundIndex::BottomBound)].setSize({ width + width / 10, width / 20 });
		this->initializeBalls();
	}
	void Draw(sf::RenderTarget& rt) const
	{
		for (int i = 0; i < int(BoundIndex::Count); i++)
		{
			rt.draw(nBounds[i]);
		}

		rt.draw(visibleTable);

		for (unsigned int i = 0; i < nBalls.size(); i++)
		{
			rt.draw((nBalls[i].getBillardBall()));
		}
	}
	void initializeBalls()
	{
		std::random_device rdx;
		std::random_device rdy;
		std::mt19937 genx(rdx());
		std::mt19937 geny(rdy());
		std::uniform_real_distribution<float> positionX(visibleTable.getPosition().x + Ball::radius, visibleTable.getPosition().x + visibleTable.getSize().x - Ball::radius);
		std::uniform_real_distribution<float> positionY(visibleTable.getPosition().y + Ball::radius, visibleTable.getPosition().y + visibleTable.getSize().y - Ball::radius);

		float posX;
		float posY;
		for (int i = 0; i < ballsAmount; i++)
		{
			posX = positionX(genx);
			posY = positionY(geny);
			for (unsigned int j = 0; j < nBalls.size();)
			{
				if (!nBalls[j].ifAllowedPosition(Ball(sf::Vector2f({ posX,posY }))))
				{
					posX = positionX(genx);
					posY = positionY(geny);
					j = 0;
				}
				else
				{
					j++;
				}
			}
			nBalls.push_back(Ball(sf::Vector2f({ posX, posY })));
			nBalls[i].setVelocity(sf::Vector2f({ 25.0f,1.0f * i }));
		}
	}
	void bandCollision()
	{
		for (unsigned int i = 0; i < nBalls.size(); i++)
		{
			if (visibleTable.getPosition().y >= nBalls[i].getPosition().y - Ball::radius)
			{
				handleBallAndBoundColission(nBalls[i], visibleTable.getPosition().y, true);
			}
			else if (visibleTable.getPosition().y + visibleTable.getSize().y <= nBalls[i].getPosition().y + Ball::radius)
			{
				handleBallAndBoundColission(nBalls[i], visibleTable.getPosition().y + visibleTable.getSize().y, true);
			}
			else if (visibleTable.getPosition().x >= nBalls[i].getPosition().x - Ball::radius)
			{
				handleBallAndBoundColission(nBalls[i], visibleTable.getPosition().x, false);
			}
			else if (visibleTable.getPosition().x + visibleTable.getSize().x <= nBalls[i].getPosition().x + Ball::radius)
			{
				handleBallAndBoundColission(nBalls[i], visibleTable.getPosition().x + visibleTable.getSize().x, false);

			}
		}
	}
	void handleBallAndBoundColission(Ball& ball, float endOfTable, bool isUpOrDown)
	{
		float ballDt = ball.dt;
		sf::Vector2f unchangedBallPosition = ball.getPosition();
		sf::Vector2f changedBallPosition = ball.getPosition();
		sf::Vector2f nearBoundPosition;

		float dtToHitBound;
		if (isUpOrDown)
		{
			dtToHitBound = abs(endOfTable - ball.getPosition().y) - Ball::radius;
			dtToHitBound /= abs(ball.getVelocity().y);
			ball.Update(dtToHitBound);
			ball.getVelocity().y *= -1.0f;
			ball.Update(ballDt - dtToHitBound);
		}
		else
		{
			dtToHitBound = abs(endOfTable - ball.getPosition().x) - Ball::radius;
			dtToHitBound /= abs(ball.getVelocity().x);
			ball.Update(dtToHitBound);
			ball.getVelocity().x *= -1.0f;
			ball.Update(ballDt - dtToHitBound);
		}
	}
	void ballCollision()
	{
		for (unsigned i = 0; i < nBalls.size(); i++)
		{
			for (unsigned j = i + 1; j < nBalls.size(); j++)
			{
				if (vectorLength(nBalls[i].getPosition() - nBalls[j].getPosition()) < 2 * Ball::radius)
				{
					nBalls[i].handleBallCollision(nBalls[j]);
				}
			}
		}
	}
	std::vector<Ball>& getAllBalls()
	{
		return nBalls;
	}

private:
	int ballsAmount;
	sf::RectangleShape visibleTable;
	sf::Rect<float> invisibleTable;
	sf::RectangleShape nBounds[int(BoundIndex::Count)];
	std::vector<Ball> nBalls;
};
float Ball::radius = 20.0f;

int main()
{
	std::cout << "Symulator stolu bilardowego przygotowany jako projekt z kursu Metody Obliczeniowe Fizyki i Techniki." << std::endl;

	//float rad;
	//std::cout << "Podaj promien kul na stole: ";
	//std::cin >> rad;
	//Ball::radius = rad;

	// Create the main window
	sf::RenderWindow window(sf::VideoMode(1000, 800), "SFML window");
	Table billardTable(200.0f, 100.0f, 600.0f, 450.0f, 50);

	// Start the game loop
	while (window.isOpen())
	{
		// Process events
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Close window: exit
			if (event.type == sf::Event::Closed)
				window.close();
		}
		//handle input
		for (unsigned int i = 0; i < billardTable.getAllBalls().size(); i++)
		{
			billardTable.getAllBalls()[i].Update(1.0f / 60.0f);
		}

		// Clear screen
		window.clear();
		// Draw a table

		billardTable.bandCollision();
		billardTable.ballCollision();
		billardTable.Draw(window);

		//// Update the window
		window.display();
	}
	return EXIT_SUCCESS;
}
