#pragma once

#include <array>
#include <utility>
#include <iostream>
#include <random>

#include "Console.h"

void clearScreen();
void printTextInclolor(std::string, TColor);

enum FieldGraphics
{
	UNKNOWN,     // 0 
	EMPTY,       // 1 
	SHIP_DEAD,   // 2 
	SHIP_ALIVE,  // 3 
	NEXT_TO_SHIP // 4
};

void printFieldPointFriendly(int fieldPoint)
{
	if (fieldPoint == UNKNOWN)
	{
		printColoredText("~", DARKGRAY);
	}
	else if (fieldPoint == EMPTY)
	{
		printColoredText("*", LIGHTBLUE);
	}
	else if (fieldPoint == SHIP_DEAD)
	{
		printColoredText("X", RED);
	}
	else if (fieldPoint == SHIP_ALIVE)
	{
		printColoredText("S", LIGHTGREEN);
	}
	else if (fieldPoint == NEXT_TO_SHIP)
	{
		printColoredText("~", RED);
	}
}

void printFieldPointOpponent(int fieldPoint)
{
	if (fieldPoint == UNKNOWN)
	{
		printColoredText("~", DARKGRAY);
	}
	else if (fieldPoint == EMPTY)
	{
		printColoredText("*", LIGHTBLUE);
	}
	else if (fieldPoint == SHIP_DEAD)
	{
		printColoredText("X", RED);
	}
	else if (fieldPoint == SHIP_ALIVE)
	{
		printColoredText("~", DARKGRAY);
	}
	else if (fieldPoint == NEXT_TO_SHIP)
	{
		printColoredText("~", RED);
	}
}


void printPressToContinueScreen(std::string printText = "", bool clearBefore = true)
{
	int x;
	if (clearBefore)
	{
		clearScreen();
	}

	std::cout << printText << "Enter any number to continue: \n";
	std::cin >> x;
	clearScreen();
}

class Field
{
private:
	// Indexes 0 and 11 are offsets
	std::array<std::array<int, 12>, 12> field{};

	int shipsLeft = 10;

	bool isValidShipPlacement(int x, int y, int size, bool horiz)
	{
		if ((1 > x || x > 10) || (1 > y || y > 10))
		{
			return false;
		}

		if (horiz)
		{
			if (x + size - 1 > 10)
			{
				return false;
			}

			// Check overlapping
			for (int i = x; i < x + size; i++)
			{
				if (field[i][y] == NEXT_TO_SHIP || field[i][y] == SHIP_ALIVE)
				{
					return false;
				}
			}
		}
		else
		{
			if (y + size - 1 > 10)
			{
				return false;
			}

			// Check overlapping
			for (int i = y; i < y + size; i++)
			{
				if (field[x][i] == NEXT_TO_SHIP || field[x][i] == SHIP_ALIVE)
				{
					return false;
				}
			}
		}
		return true;
	}

	std::pair<int, int> inputPlaceShipCoordinates(int& shipSizeIndex, bool& horiz, const std::array<int, 10>& shipSizes)
	{
		if (horiz)
		{
			printColoredText("Placing a ");
			printColoredText(std::to_string(shipSizes[shipSizeIndex]) + "-deck", LIGHTBLUE);
			printColoredText(" ship");
			printColoredText(" horizontally.\n", LIGHTGREEN);
			std::cout << "Enter coordinates for the leftmost point of the ship," << std::endl;
		}
		else
		{
			printColoredText("Placing a ");
			printColoredText(std::to_string(shipSizes[shipSizeIndex]) + "-deck", LIGHTBLUE);
			printColoredText(" ship");
			printColoredText(" vertically.\n", LIGHTGREEN);
			std::cout << "Enter coordinates for the upmost point of the ship," << std::endl;
		}
		std::cout << "or enter '-1' to rotate ship by 90 degrees" << std::endl
			<< "or enter '-2' to reset all ships and place them again" << std::endl;

		int x;
		int y;

		std::cin >> x;
		if (x == -1)
		{
			horiz = !horiz;
			return { -1, -1 };
		}
		if (x == -2)
		{
			field.fill({ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 });
			horiz = true;
			shipSizeIndex = 0;
			return { -2, -2 };
		}
		std::cin >> y;

		return { x, y };
	}

	void placeShip(int x, int y, int size, bool horiz) // Gets valid parameters
	{
		if (horiz)
		{
			for (int i = x; i < x + size; i++)
			{
				field[i][y] = SHIP_ALIVE;

				// Mark all nearby points on grid
				for (int k = i - 1; k <= i + 1; k++)
				{
					for (int j = y - 1; j <= y + 1; j++)
					{
						if (field[k][j] != SHIP_ALIVE)
						{
							field[k][j] = NEXT_TO_SHIP;
						}
					}
				}
			}
		}
		else
		{
			for (int i = y; i < y + size; i++)
			{
				field[x][i] = SHIP_ALIVE;

				// Mark all nearby points on grid
				for (int k = x - 1; k <= x + 1; k++)
				{
					for (int j = i - 1; j <= i + 1; j++)
					{
						if (field[k][j] != SHIP_ALIVE)
						{
							field[k][j] = NEXT_TO_SHIP;
						}
					}
				}
			}
		}
	}

	void clearFieldFrom_NEXT_TO()
	{
		for (auto& x : field)
		{
			for (auto& y : x)
			{
				if (y == NEXT_TO_SHIP)
				{
					y = UNKNOWN;
				}
			}
		}
	}

	bool isValidShot(int x, int y)
	{
		if ((1 > x || x > 10) || (1 > y || y > 10))
		{
			return false;
		}

		if (field[x][y] != UNKNOWN && field[x][y] != SHIP_ALIVE)
		{
			return false;
		}

		return true;
	}

	std::pair<int, int> getShootCoordsFromPlayer()
	{
		int x, y;
		do
		{
			std::cout << "Enter coordinates to shoot at:\n";
			std::cin >> x >> y;
		} while (!isValidShot(x, y));

		return { x, y };
	}

	bool makeShot(int x, int y) // Gets valid coordinates
	{

		if (field[x][y] == UNKNOWN)
		{
			field[x][y] = EMPTY;
		}
		else if (field[x][y] == SHIP_ALIVE)
		{
			shipsLeft--;

			// Kill the ship
			for (int i = x; field[i][y] == SHIP_ALIVE; i++)
			{
				field[i][y] = SHIP_DEAD;

				// Mark nearby points as EMPTY
				for (int j = i - 1; j <= i + 1; j++)
				{
					for (int k = y - 1; k <= y + 1; k++)
					{
						if (field[j][k] != SHIP_DEAD && field[j][k] != SHIP_ALIVE)
						{
							field[j][k] = EMPTY;
						}
					}
				}
			}

			for (int i = x - 1; field[i][y] == SHIP_ALIVE; i--)
			{
				field[i][y] = SHIP_DEAD;

				// Mark nearby points as EMPTY
				for (int j = i - 1; j <= i + 1; j++)
				{
					for (int k = y - 1; k <= y + 1; k++)
					{
						if (field[j][k] != SHIP_DEAD && field[j][k] != SHIP_ALIVE)
						{
							field[j][k] = EMPTY;
						}
					}
				}
			}

			for (int i = y + 1; field[x][i] == SHIP_ALIVE; i++)
			{
				field[x][i] = SHIP_DEAD;

				// Mark nearby points as EMPTY
				for (int j = x - 1; j <= x + 1; j++)
				{
					for (int k = i - 1; k <= i + 1; k++)
					{
						if (field[j][k] != SHIP_DEAD && field[j][k] != SHIP_ALIVE)
						{
							field[j][k] = EMPTY;
						}
					}
				}
			}

			for (int i = y - 1; field[x][i] == SHIP_ALIVE; i--)
			{
				field[x][i] = SHIP_DEAD;

				// Mark nearby points as EMPTY
				for (int j = x - 1; j <= x + 1; j++)
				{
					for (int k = i - 1; k <= i + 1; k++)
					{
						if (field[j][k] != SHIP_DEAD && field[j][k] != SHIP_ALIVE)
						{
							field[j][k] = EMPTY;
						}
					}
				}
			}
		}

		if (shipsLeft == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

public:
	Field() {}

	typedef void (*printGraphics)(int);

	void printField(printGraphics printGraphicsFunc)
	{
		// Print grid numbers
		textColor(DARKGRAY);
		for (int i = 1; i < 11; i++)
		{
			std::cout << i << " ";
		}
		std::cout << std::endl;
		textColor(LIGHTGRAY);

		for (int i = 1; i < 11; i++)
		{
			for (int j = 1; j < 11; j++)
			{
				printGraphicsFunc(field[j][i]);
				std::cout << ' ';
			}
			textColor(DARKGRAY);
			std::cout << i << std::endl;
			textColor(LIGHTGRAY);
		}
	}

	void placeShipsProcess()
	{
		std::array <int, 10> shipSizes = { 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 };
		bool horiz = true;
		int shipSizeIndex = 0;
		std::pair <int, int> coords;

		while (shipSizeIndex != shipSizes.size())
		{
			clearScreen();
			this->printField(printFieldPointFriendly);
			coords = inputPlaceShipCoordinates(shipSizeIndex, horiz, shipSizes);

			if (isValidShipPlacement(coords.first, coords.second, shipSizes[shipSizeIndex], horiz))
			{
				placeShip(coords.first, coords.second, shipSizes[shipSizeIndex], horiz);
				shipSizeIndex++;
			}
		}

		std::cout << "All ships placed!\n";
		clearFieldFrom_NEXT_TO();
	}

	void placeShipsComputer()
	{
		std::array <int, 10> shipSizes = { 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 };
		
		// For each ship, generate a pair of random numbers (coordinates on the field) and look for a valid ship placement starting from there
		std::random_device rd;
		std::mt19937 gen(rd()); // seed
		std::uniform_int_distribution<> distrX(1, 10);
		std::uniform_int_distribution<> distrY(1, 10);

		bool horiz = true;
		int shipSizeIndex = 0;
		std::pair <int, int> coords;
		int iterateDirection = 1;
		
		while (shipSizeIndex != shipSizes.size())
		{
			coords = std::make_pair(distrX(gen), distrY(gen));

			horiz = distrX(gen) % 2;
			if (distrY(gen) % 2 == 0)
			{
				iterateDirection *= -1;
			}

			bool shipPlaced = false;

			for (int i = coords.first; 0 < i && i <= 10; i += iterateDirection)
			{
				if (shipPlaced) break;

				for (int j = coords.second; 0 < j && j <= 10; j += iterateDirection)
				{
					if (isValidShipPlacement(coords.first, coords.second, shipSizes[shipSizeIndex], horiz))
					{
						placeShip(coords.first, coords.second, shipSizes[shipSizeIndex], horiz);
						shipSizeIndex++;
						shipPlaced = true;
						break;
					}
				}
			}
		}

		std::cout << "Computer placed all ships!\n";
		clearFieldFrom_NEXT_TO();
	}

	bool shootAtFieldProcess_Player()
	{
		std::pair <int, int> coords;
		
		coords = getShootCoordsFromPlayer();

		return makeShot(coords.first, coords.second);
	}

	bool shootAtFieldProcess_Computer()
	{
		std::pair <int, int> coords;

		std::random_device rd;
		std::mt19937 gen(rd()); // seed
		std::uniform_int_distribution<> distrX(1, 10);
		std::uniform_int_distribution<> distrY(1, 10);

		bool shotMade = false;
		int iterateDirection = 1;

		while (!shotMade)
		{
			if (distrY(gen) % 2 == 0)
			{
				iterateDirection *= -1;
			}

			coords = std::make_pair(distrX(gen), distrY(gen));

			for (int i = coords.first; i <= 10; i+= iterateDirection)
			{
				if (shotMade) break;

				for (int j = coords.second; j <= 10; j += iterateDirection)
				{
					if (isValidShot(coords.first, coords.second))
					{
						makeShot(coords.first, coords.second);
						shotMade = true;
						break;
					}
				}
			}
		}
		if (shipsLeft == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	const std::array<int, 12>& operator [] (int n) const
	{
		return field[n];
	}

	std::array<int, 12>& operator [] (int n)
	{
		return field[n];
	}
};