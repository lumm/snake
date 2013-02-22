#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sstream>

#define G_WIDTH 40
#define G_HEIGHT 22
#define CELL_SIZE 6

typedef struct
{
	int pos_x;
	int pos_y;
} Cell;

typedef enum
{
	UP,
	DOWN,
	LEFT,
	RIGHT
} Direction;

void drawCell(const Cell &cell, SDL_Surface *surface, SDL_Surface *target)
{
	SDL_Rect position;
	position.x = cell.pos_x * CELL_SIZE;
	position.y = cell.pos_y * CELL_SIZE;
	SDL_BlitSurface(surface, NULL, target, &position);
}

bool snakeEatTail(std::vector<Cell> snake)
{
	Cell head = snake.back();
	std::vector<Cell>::iterator itSnake;
	for(itSnake=snake.begin(); itSnake != (snake.end()-1); itSnake++)
	{
		Cell c = *itSnake;
		if(c.pos_x == head.pos_x && c.pos_y == head.pos_y)
		{
			return true;
		}
	}
	return false;
}

Cell getFood(std::vector<Cell> snake)
{
	int rx = rand() % G_WIDTH;
	int ry = rand() % G_HEIGHT;

	bool onSnake = false;
	std::vector<Cell>::iterator itSnake;
	for(itSnake=snake.begin(); itSnake != snake.end(); itSnake++)
	{
		Cell c = *itSnake;
		if(c.pos_x == rx && c.pos_y == ry)
		{
			onSnake = true;
			break;
		}
	}

	if(onSnake)
		return getFood(snake);
	Cell food;
	food.pos_x = rx;
	food.pos_y = ry;
	return food;
}

int main()
{
	srand(time(0));
	TTF_Init();
	std::vector<Cell> snake;
	SDL_Surface *cell_surface = IMG_Load("cell.png");
	SDL_Surface *food_surface = IMG_Load("food.png");
	TTF_Font *font = TTF_OpenFont("arial.ttf", 12);
	SDL_Color fontColor = {255,255,255};
	SDL_Surface *score_surface;

	SDL_Surface *window = SDL_SetVideoMode(G_WIDTH * CELL_SIZE,
	                                      (G_HEIGHT * CELL_SIZE) + 25,32,
				              SDL_HWSURFACE | SDL_DOUBLEBUF);


	Uint32 bgColor = SDL_MapRGB(window->format, 0, 0, 0);
	SDL_Event event;

	// init snake
	Cell c;
	c.pos_x = 10; c.pos_y = 10;
	for(int i=0; i<3; i++){
		snake.push_back(c);
		c.pos_x += 1;
	}

	Direction direction = RIGHT;
	bool running = true;

	int lastFrame = SDL_GetTicks();
	int currentTime = lastFrame;

	int slow = 60;
	bool gameOver = false;
	int score = 0;

	score_surface = TTF_RenderText_Solid(font, "Score: ", fontColor);


	Cell food = getFood(snake);

	while(running)
	{
		int wait = 20 + (slow - (score/100));

		currentTime = SDL_GetTicks();

		if(currentTime - lastFrame > wait)
		{
			SDL_PollEvent(&event);
			switch(event.type)
			{
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym)
				{
				case SDLK_DOWN:
					if(direction != UP)
						direction = DOWN;
					break;
				case SDLK_UP:
					if(direction != DOWN)
						direction = UP;
					break;
				case SDLK_LEFT:
					if(direction != RIGHT)
						direction = LEFT;
					break;
				case SDLK_RIGHT:
					if(direction != LEFT)
						direction = RIGHT;
					break;
				default:
					break;
				}

			default:
				break;
			}
			// TICK
			if(!gameOver)
			{
				Cell newCell = snake.back();
				switch(direction)
				{
				case RIGHT:
					newCell.pos_x += 1;
					if(newCell.pos_x > G_WIDTH) newCell.pos_x = 0;
					break;
				case LEFT:
					newCell.pos_x -= 1;
					if(newCell.pos_x < 0) newCell.pos_x = G_WIDTH;
					break;
				case DOWN:
					newCell.pos_y += 1;
					if(newCell.pos_y > G_HEIGHT) newCell.pos_y = 0;
					break;
				case UP:
					newCell.pos_y -= 1;
					if(newCell.pos_y < 0) newCell.pos_y = G_HEIGHT;
					break;
				default:
					break;
				}
				snake.push_back(newCell);
				snake.erase(snake.begin());

				// eat tail ?
				if(snakeEatTail(snake))
				{
					gameOver = true;
				}

				// eat food ?
				if(snake.back().pos_x == food.pos_x && snake.back().pos_y == food.pos_y)
				{
					snake.push_back(food);
					score += 85;
					food = getFood(snake);
				}

			}
			// DRAW
			SDL_FillRect(window, 0, bgColor);

			// snake
			std::vector<Cell>::iterator itSnake;
			for(itSnake=snake.begin(); itSnake != snake.end(); itSnake++)
			{
				drawCell(*itSnake, cell_surface, window);
			}

			// food
			drawCell(food, food_surface, window);

			// score
			std::stringstream ss;
			ss << "Score: " << score;
			SDL_FreeSurface(score_surface);
			score_surface = TTF_RenderText_Blended(font, ss.str().c_str(), fontColor);
			SDL_Rect score_pos;
			score_pos.x = 12;
			score_pos.y = G_HEIGHT * CELL_SIZE + 6;
			SDL_BlitSurface(score_surface, NULL, window, &score_pos);

			SDL_Flip(window);
			lastFrame = currentTime;


		}
		else
		{
			SDL_Delay(wait - (currentTime - lastFrame));
		}
	}

	TTF_CloseFont(font);
	TTF_Quit();
	SDL_Quit();

	return 0;
}
