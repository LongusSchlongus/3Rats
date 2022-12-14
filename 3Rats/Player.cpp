#include "Player.h"

Player::Player(SDL_Renderer* renderTarget, std::string filePath, int x, int y, int framesX, int framesY)
{
	SDL_Surface* surface = IMG_Load(filePath.c_str());
	if (surface == NULL)
		std::cout << "Error" << std::endl;
	else
	{
		texture = SDL_CreateTextureFromSurface(renderTarget, surface);
		if (texture == NULL)
			std::cout << "Error" << std::endl;
	}

	SDL_FreeSurface(surface);

	SDL_QueryTexture(texture, NULL, NULL, &cropRect.w, &cropRect.h);

	positionRect.x = x;
	positionRect.y = y;

	textureWidth = cropRect.w;

	cropRect.w /= framesX;
	cropRect.h /= framesY;

	frameWidth = positionRect.w = cropRect.w;
	frameHeight = positionRect.h = cropRect.h;

	originX = frameWidth / 2;
	originY = frameHeight / 2;

	radius = frameWidth / 2;

	isActive = false;

	static int playerNumber = 0;
	playerNumber++;

	this->playerNumber = playerNumber;

	if (playerNumber == 1)
	{
		keys[0] = SDL_SCANCODE_W;
		keys[1] = SDL_SCANCODE_S;
		keys[2] = SDL_SCANCODE_A;
		keys[3] = SDL_SCANCODE_D;
	}
	else if (playerNumber == 2)
	{
		keys[0] = SDL_SCANCODE_UP;
		keys[1] = SDL_SCANCODE_DOWN;
		keys[2] = SDL_SCANCODE_LEFT;
		keys[3] = SDL_SCANCODE_RIGHT;
	}
	else if (playerNumber == 3)
	{
		keys[0] = SDL_SCANCODE_UP;
		keys[1] = SDL_SCANCODE_DOWN;
		keys[2] = SDL_SCANCODE_LEFT;
		keys[3] = SDL_SCANCODE_RIGHT;
	}
	moveSpeed = 200.0f;

	search = false;
	bananPicked = false;
	found = false;
	wait = false;

	searchCounter = rand() % 100;
	searchCounter /= 100.0f;
}

Player::~Player()
{
	SDL_DestroyTexture(texture);
}

void Player::Update(float delta, const Uint8* keyState, int mode, Player& p, Item& i, int& banan)
{
	isActive = true;

	int ratX = this->GetOriginX();
	int ratY = this->GetOriginY();

	int frontRatX = p.GetOriginX();
	int frontRatY = p.GetOriginY();

	switch (p.GetDirection()) {
	case 0:
		ratY -= 55;
		break;
	case 1:
		ratY += 55;
		break;
	case 2:
		ratX -= 55;
		break;
	case 3:
		ratX += 55;
		break;
	default:
		break;
	}

	float dist1 = sqrt(pow(abs(p.GetOriginX() - ratX), 2) + pow(abs(p.GetOriginY() - ratY), 2));
	float dist2 = sqrt(pow(abs(p.GetOriginX() - ratX), 2) + pow(abs(p.GetOriginY() - ratY), 2));

	if (playerNumber == 1)//--------------------Player control
	{
		if (keyState[keys[0]])
		{
			positionRect.y -= moveSpeed * delta;
			cropRect.y = frameHeight * 3;
			direction = 0;
		}
		else if (keyState[keys[1]])
		{
			positionRect.y += moveSpeed * delta;
			cropRect.y = 0;
			direction = 1;
		}
		else if (keyState[keys[2]])
		{
			positionRect.x -= moveSpeed * delta;
			cropRect.y = frameHeight;
			direction = 2;
		}
		else if (keyState[keys[3]])
		{
			positionRect.x += moveSpeed * delta;
			cropRect.y = frameHeight * 2;
			direction = 3;
		}
		else
			isActive = false;
	} 
	else
	{
		switch (mode)
		{
		case 1://--------------------Ai control
			if (!wait)
			{
				if (ratY > goalY)
				{
					positionRect.y -= moveSpeed * delta;
					cropRect.y = frameHeight * 3;
					direction = 0;
				}
				else if (ratY < goalY)
				{
					positionRect.y += moveSpeed * delta;
					cropRect.y = 0;
					direction = 1;
				}
				else if (ratX > goalX)
				{
					positionRect.x -= moveSpeed * delta;
					cropRect.y = frameHeight;
					direction = 2;
				}
				else if (ratX < goalX)
				{
					positionRect.x += moveSpeed * delta;
					cropRect.y = frameHeight * 2;
					direction = 3;
				}
				else if (ratX == goalX && ratY == goalY)
				{
					std::cout << "found!";
					i.SetExistence(false);
					bananPicked = true;
					found = true;
					banan++;
				}
			}
			break;

		case 0://--------------------Follow control (autopilot)

			if (!wait)
			{
				if (ratY > frontRatY)
				{
					positionRect.y -= moveSpeed * delta;
					cropRect.y = frameHeight * 3;
					direction = 0;
				}
				else if (ratY < frontRatY)
				{
					positionRect.y += moveSpeed * delta;
					cropRect.y = 0;
					direction = 1;
				}
				else if (ratX > frontRatX)
				{
					positionRect.x -= moveSpeed * delta;
					cropRect.y = frameHeight;
					direction = 2;
				}
				else if (ratX < frontRatX)
				{
					positionRect.x += moveSpeed * delta;
					cropRect.y = frameHeight * 2;
					direction = 3;
				}
				else
				{
					direction = p.GetDirection();
				}

				if (direction == 0) cropRect.y = frameHeight * 3;
				if (direction == 1) cropRect.y = 0;
				if (direction == 2) cropRect.y = frameHeight;
				if (direction == 3) cropRect.y = frameHeight * 2;
			}
			break;
		}
	}

	if (bananPicked)
	{

		if (direction == 0)
		{
			i.SetX(GetOriginX() - 24);
			i.SetY(GetOriginY() - 32 - 14);
		}
		else if (direction == 1)
		{
			i.SetX(GetOriginX() - 24);
			i.SetY(GetOriginY() - 32 + 14);
		}
		else if (direction == 2)
		{
			i.SetX(GetOriginX() - 24 - 14);
			i.SetY(GetOriginY() - 32);
		}
		else if (direction == 3)
		{
			i.SetX(GetOriginX() - 24 + 14);
			i.SetY(GetOriginY() - 32);
		}
		
	}

	if (isActive)
	{
		frameCounter += delta;
		searchCounter += delta;
		if (wait) waitCounter += delta;


		if (frameCounter >= 0.25f)
		{
			frameCounter = 0;
			cropRect.x += frameWidth;
			if (cropRect.x >= textureWidth)
				cropRect.x = 0;
		}

		if (searchCounter >= 2.0f)
		{
			searchCounter = 0;
			wait = true;
		}

		if (waitCounter >= 0.5f)
		{
			waitCounter = 0;
			wait = false;
		}

	}
	else
	{
		frameCounter = 0;
		cropRect.x = frameWidth;
	}

	if (i.GetExistence())
	{
		SetNewGoal(i.GetOriginX(), i.GetOriginY());
	}
	else if(found)
	{
		found = false;
		SetNewGoal();
	}
	else
	{ }

}

void Player::Draw(SDL_Renderer* renderTarget)
{
	SDL_RenderCopy(renderTarget, texture, &cropRect, &positionRect);
}

void Player::SetNewGoal()
{
	goalX = rand() % 600;
	goalY = rand() % 400;
}

void Player::SetNewGoal(int x, int y)
{
	goalX = x;
	goalY = y;
}

int Player::GetOriginX() { return positionRect.x + originX; }
int Player::GetOriginY() { return positionRect.y + originY; }
int Player::GetRadius() { return radius; }
int Player::GetDirection() { return direction; }
