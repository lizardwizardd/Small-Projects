#pragma once

#include "Field.h"

class Game
{
private:
	Field fieldPlayer1;
	Field fieldPlayer2;

public:
	Game() = default;

	void start()
	{
        printColoredText("SEA BATTLE\n\n", LIGHTBLUE);

        int input = -1;
        while (input != 1 && input != 2)
        {
            std::cout << "Enter 1 to play against computer\nEnter 2 to play against other player\n\n";
            std::cin >> input;
        }

        clearScreen();

        if (input == 1)
        {
            placingShipsStage_OnePlayer();
            shootingStage_OnePlayer();
        }
        else
        {
            placingShipsStage_TwoPlayers();
            shootingStage_TwoPlayers();
        }
	}

private:
	void placingShipsStage_TwoPlayers()
	{
		fieldPlayer1.placeShipsProcess();
		printPressToContinueScreen("Player 2 turn to place ships\n");

		fieldPlayer2.placeShipsProcess();
		printPressToContinueScreen("Player 1 turn to shoot\n");
	}

	void placingShipsStage_OnePlayer()
	{
		fieldPlayer2.placeShipsComputer();
		fieldPlayer1.placeShipsProcess();
		printPressToContinueScreen("All ships placed.\n");
	}

	void shootingStage_TwoPlayers()
	{
		while (true)
		{
			// PLAYER 1 TURN
			printColoredText("Player 1 field\n", LIGHTGREEN);
			fieldPlayer1.printField(printFieldPointFriendly);
			printColoredText("\nPlayer 2 field\n", RED);
			fieldPlayer2.printField(printFieldPointOpponent);
			if (fieldPlayer2.shootAtFieldProcess_Player())
			{
				printPressToContinueScreen("Player 1 wins!\n");
				break;
			}
			clearScreen();
			printColoredText("Player 2 field\n", YELLOW);
			fieldPlayer2.printField(printFieldPointOpponent);
			printPressToContinueScreen("Player 2 turn to shoot\n", false);
			

			// PLAYER 2 TURN
			printColoredText("Player 2 field\n", LIGHTGREEN);
			fieldPlayer2.printField(printFieldPointFriendly);
			printColoredText("\nPlayer 1 field\n", RED);
			fieldPlayer1.printField(printFieldPointOpponent);
			if (fieldPlayer1.shootAtFieldProcess_Player())
			{
				printPressToContinueScreen("Player 2 wins!\n");
				break;
			}
			clearScreen();
			printColoredText("Player 1 field\n", YELLOW);
			fieldPlayer1.printField(printFieldPointOpponent); 
			printPressToContinueScreen("Player 1 turn to shoot\n", false);
		}

		fieldPlayer1.printField(printFieldPointFriendly);
		fieldPlayer2.printField(printFieldPointFriendly);
	}

	void shootingStage_OnePlayer()
	{
		while (true)
		{
			// PLAYER 1 TURN
			printColoredText("Player 1 field\n", LIGHTGREEN);
			fieldPlayer1.printField(printFieldPointFriendly);
			printColoredText("Computer's field\n", RED);
			fieldPlayer2.printField(printFieldPointOpponent);
			if (fieldPlayer2.shootAtFieldProcess_Player())
			{
				printPressToContinueScreen("Player 1 wins!\n");
				break;
			}
			clearScreen();
			printColoredText("Computer's field\n", YELLOW);
			fieldPlayer2.printField(printFieldPointOpponent);
			printPressToContinueScreen("Computer makes a shot\n", false);


			// COMPUTER TURN
			if (fieldPlayer1.shootAtFieldProcess_Computer())
			{
				printPressToContinueScreen("Computer wins!\n");
				break;
			}
			printColoredText("Player 1 field\n", YELLOW);
			fieldPlayer1.printField(printFieldPointOpponent);
			printPressToContinueScreen("Player 1 turn to shoot\n", false);
		}

		fieldPlayer1.printField(printFieldPointFriendly);
		fieldPlayer2.printField(printFieldPointFriendly);
	}
};