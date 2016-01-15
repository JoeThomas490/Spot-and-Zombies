//Joe Thomas, Sam Young, Matthew Stephenson
//---------------------------------
//include libraries
//include standard libraries
#include <iostream >         //for output and input: cin >> and cout <<
#include <iomanip>           //for formatted output in 'cout'
#include <conio.h>           //for getch()
#include <string>            //for string
#include <fstream>
#include <vector>			 //for vectors
using namespace std;

//include our own libraries
#include "RandomUtils.h"     //for Seed, Random
#include "ConsoleUtils.h"    //for Clrscr, Gotoxy, etc.
#include "TimeUtils.h"		 //for time and date to string

//---------------------------------
//define constants
//---------------------------------
//define global constants
//defining the size of the grid
const int SIZEY(12);         //vertical dimension
const int SIZEX(20);         //horizontal dimension
//defining symbols used for display of the grid and content
const char SPOT('@');        //spot
const char TUNNEL(' ');      //open space
const char WALL('#');        //border
const char ZOMBIE('Z');		 //zombie
const char PILL('*');		//pills
const int MAXZOMBIES(4);	//maximum number of zombies
const int MAXPILLS(8);		//maximum number of pills
const int MAXLIVES(3);		//maximum number of lives
const int MAXPROTECT(10);	//maximum number of moves before protection wears off
const int MAXWALLS(20);		//maximum number of walls


const char HOLE('0');		 //hole
const int MAXHOLES(12);		//maximum number of holes
//defining the command letters to move the blob on the maze
const int  UP(72);           //up arrow
const int  DOWN(80);         //down arrow
const int  RIGHT(77);        //right arrow
const int  LEFT(75);         //left arrow
//defining the command letter to move the zombie on the maze
const char ZUP('W');		 //W key
const char ZDOWN('S');		 //S key
const char ZLEFT('A');		 //A Key
const char ZRIGHT('D');		 //D key
//defining the other command letters
const char QUIT('Q');        //end the game
const char RESET('R');		 //reset the board positions
const char FREEZE('F');		 //freeze the zombies in place
const char EXTERMINATE('X');  //exterminate all the
const char EATALL('E');		 //eat all pills

const char INFO('I');		 //info key on title screen
const char PLAY('P');		 //play button on title screen
const char RETURN(13);		//enter key pressed

const string scoreFiletype = ".scr";	//create a file ext for score data
const int MaxLives(3);					//maximum number of lives
//data structure to store data for a grid item
struct Item {
	const char symbol;	     //symbol on grid
	int x, y;			     //coordinates
};

//---------------------------------------------------------------------------
//----- run game
//---------------------------------------------------------------------------

int main()
{
	//function declarations (prototypes)
	void initialiseGame(char grid[][SIZEX], Item& spot, vector<Item>& holes, vector<Item>& zombies, vector<Item>& pills, vector<Item>& walls);
	bool wantToQuit(int k);
	bool wantToReset(int k);
	bool isArrowKey(int k);
	bool isZombieKey(int k);
	bool isFreezeOn(bool& freezeOn);
	bool isExterminateOn(bool& exterminateOn, vector<Item>& zombies);
	bool isEatPillsOn(bool& eatPills, vector<Item>& pills, int& pillsLeft, int& lives);
	int  getKeyPress();
	void updateGame(char g[][SIZEX], Item& sp, int k, string& mess, vector<Item>& holes, int& lives, vector<Item>& zombies, bool freezeOn, vector<Item>& pills, int& pillsLeft, bool& protectOn, vector<Item> walls);
	void renderGame(char g[][SIZEX], string mess, int lives, int pillsLeft, bool protectOn);
	void endProgram();
	void outOfLives(char g[][SIZEX], Item& sp, int key, string& mess, vector<Item>& holes, int& lives, vector<Item>& zombies, bool freezeOn, vector<Item>& pills, int& pillsleft, bool& protectOn, vector<Item> walls);
	void setHolesCoordinates(vector<Item>& holes, vector<Item> walls);
	void setZombieCoordinates(vector <Item>& zombies);
	void setPillsCoordinates(vector <Item>& pills, vector<Item> walls);

	void loadTitleScreen(int lives);					//call the title subroutines

	//local variable declarations 
	char grid[SIZEY][SIZEX];							//grid for display
	Item spot = { SPOT };								//Spot's symbol and position (0, 0)
	Item zombie = { ZOMBIE };							//Zombie symbol and position (0,0)
	Item hole = { HOLE };								//Create spot symbol
	Item pill = { PILL };
	Item wall = { WALL };
	string message("LET'S START...      ");				//current message to player
	int lives(MAXLIVES);								//Amount of lives the player has
	int pillsLeft(MAXPILLS);							//Amount of pills left
	int protectLeft(MAXPROTECT);						//Amount of magic protection left
	vector <Item> holes(MAXHOLES, hole);				//make vector to store hole data
	vector <Item> zombies(MAXZOMBIES, zombie);			//make vector to store zombie data
	vector <Item> pills(MAXPILLS, pill);
	vector <Item> walls(MAXWALLS, wall);
	bool freezeOn = false;								//true or false whether you want zombies to move or not
	bool exterminateOn = false;							//true or false whether you want zombies alive or not					
	bool eatPills = false;								//true or false whether you want pills eaten or not
	bool protectOn = false;							//true or false whether you have eaten a pill and have magic protection

	//title screen...
	loadTitleScreen(lives);			//call the title screen subroutine
	Clrscr();						//clear screen of any previous content

	//action...
	initialiseGame(grid, spot, holes, zombies, pills, walls);          //initialise grid (incl. walls and spot)
	int key(' ');														//create key to store keyboard events
	do {
		
		if (lives >= 0)
		{
			renderGame(grid, message, lives, pillsLeft, protectOn);			//render game state on screen
		}
		if (protectOn)
		{
			--protectLeft;													//if magic protection is on take one off protect left every move
			if (protectLeft <= 0)
			{
				protectOn = false;
				protectLeft = MAXPROTECT;
			}
		}
		key = getKeyPress();							//read in next keyboard eventx
		message = "                    ";				//reset message
		if (lives == 0)
		{
			outOfLives(grid, spot, key, message, holes, lives, zombies, freezeOn, pills, pillsLeft, protectOn, walls);
		}
		else if (isArrowKey(key) && lives != 0)
		{
			updateGame(grid, spot, key, message, holes, lives, zombies, freezeOn, pills, pillsLeft, protectOn, walls);
		}
		else if (wantToReset(key) && lives != 0)						//call reset cheat key
		{
			spot.x = SIZEX / 2;
			spot.y = SIZEY / 2;
			lives = 3;
			pillsLeft = MAXPILLS;
			setHolesCoordinates(holes,walls);
			setZombieCoordinates(zombies);
			setPillsCoordinates(pills, walls);
			//add random colours
			updateGame(grid, spot, key, message, holes, lives, zombies, freezeOn, pills, pillsLeft, protectOn, walls);
		}
		else if (toupper(key) == FREEZE)
		{
			isFreezeOn(freezeOn);
		}
		else if (toupper(key) == EXTERMINATE)
		{
			isExterminateOn(exterminateOn, zombies);
			renderGame(grid, message, lives, pillsLeft, protectOn);			//render game state on screen

		}
		else if (toupper(key) == EATALL)
		{
			isEatPillsOn(eatPills, pills, pillsLeft, lives);
			renderGame(grid, message, lives, pillsLeft, protectOn);			//render game state on screen

		}
		else
		{
			message = "INVALID KEY!        ";			//set 'Invalid key' message
		}
	} while (!wantToQuit(key));			//while user does not want to quit
	endProgram();										//display final message
	return 0;
} //end main

void updateGame(char grid[][SIZEX], Item& spot, int key, string& message, vector<Item>& holes, int& lives, vector<Item>& zombies, bool freezeOn, vector<Item>& pills, int& pillsLeft, bool& protectOn, vector<Item> walls)
{ //updateGame state
	void updateSpotCoordinates(const char g[][SIZEX], Item& spot, int key, string& mess, int& lives, vector<Item>& pills, int& pillsLeft, bool& protectOn);
	void updateZombieCoordinates(const char gr[][SIZEX], vector<Item>& zombies, Item spot, bool freezeOn, int& lives, bool protectOn);
	void updateGrid(char g[][SIZEX], Item spot, vector<Item> holes, vector<Item> zombies, vector<Item> pills, vector<Item> walls);
	void winGame(char grid[][SIZEX], Item& spot, int key, string& message, vector<Item>& holes, int& lives, vector<Item>& zombies, bool freezeOn, vector<Item>& pills, int& pillsLeft, bool& protectOn, vector<Item> walls);


	updateZombieCoordinates(grid, zombies, spot, freezeOn, lives, protectOn);
	updateSpotCoordinates(grid, spot, key, message, lives, pills, pillsLeft, protectOn);				//update spot coordinates
	updateGrid(grid, spot, holes, zombies, pills, walls);								//update grid information

	int winCount(0);

	for (int i(0); i < MAXZOMBIES; ++i)	//for each zombie
	{//----------------WIN CONDITION TEST-----------------------------------------------------
		if ((zombies.at(i).y == 0) && (zombies.at(i).x == 0))	//if the zombie's position is 0,0
		{
			++winCount;											//increment the counter

			if (winCount == 4)									//if all 4 zombies are at 0,0 (dead)
			{
				winGame(grid, spot, key, message, holes, lives, zombies, freezeOn, pills, pillsLeft, protectOn, walls);
			}
		}
	}//---------------END OF WIN CONDITION TEST----------------------------------------------
}

//---------------------------------------------------------------------------
//----- initialise game state
//---------------------------------------------------------------------------

void initialiseGame(char grid[][SIZEX], Item& spot, vector<Item>& holes, vector<Item>& zombies, vector<Item>& pills, vector<Item>& walls)
{ //initialise grid and place spot in middle
	void setGrid(char[][SIZEX]);
	void setSpotInitialCoordinates(Item& spot);
	void setHolesCoordinates(vector<Item>& holes, vector<Item> walls);
	void setZombieCoordinates(vector<Item>& zombies);
	void setPillsCoordinates(vector<Item>& pills, vector<Item> walls);
	void setWallCoordinates(vector<Item>& walls);
	void placeHoles(char gr[][SIZEX], vector<Item> holes);
	void placeSpot(char gr[][SIZEX], Item spot);
	void placeZombies(char gr[][SIZEX], vector<Item> zombies);
	void placePills(char gr[][SIZEX], vector<Item> pills);
	void placeWalls(char gr[][SIZEX], vector<Item> walls);


	Seed();										//seed reandom number generator
	setWallCoordinates(walls);
	setSpotInitialCoordinates(spot);			//initialise spot
	setZombieCoordinates(zombies);				//set the zombie coordinates
	setHolesCoordinates(holes, walls);					//set the holes coordinates
	setPillsCoordinates(pills, walls);					//set the pills coordinates
	
	

	setGrid(grid);								//reset empty grid
	placeHoles(grid, holes);
	placePills(grid, pills);
	placeSpot(grid, spot);				//set spot in grid
	placeZombies(grid, zombies);			//set zombies in grid
	placeWalls(grid, walls);

} //end of initialiseGame

void setSpotInitialCoordinates(Item& spot)
{ //set spot coordinates inside the grid at random at beginning of game
	spot.y = (SIZEY / 2);      //vertical coordinate in range [1..(SIZEY / 2)]
	spot.x = (SIZEX / 2);	   //horizontal coordinate in range [1..(SIZEX /2)]
} //end of setSpotInitialoordinates

void setHolesCoordinates(vector <Item> & holes, vector<Item> walls)
{//creates hole coordinates and pushes them onto the grid
	for (int i(0); i < MAXHOLES; ++i) 
	{
		holes.at(i).x = Random(SIZEX - 2);
		holes.at(i).y = Random(SIZEY - 2);
		for (int j(0); j < MAXWALLS; ++j)
		{
			while (((holes.at(i).x == walls.at(j).x) && (holes.at(i).y == walls.at(j).y))		//holes dont spawn on inner walls
				|| ((holes.at(i).x == SIZEX / 2) && (holes.at(i).y == SIZEY / 2))
				|| ((holes.at(i).x == 1) && (holes.at(i).y == 1))
				|| ((holes.at(i).x == SIZEX - 2) && (holes.at(i).y == 1))
				|| ((holes.at(i).x == 1) && (holes.at(i).y == SIZEY - 2))
				|| ((holes.at(i).x == SIZEX - 2) && (holes.at(i).y == SIZEY - 2)))				//holes dont spawn on spot
			{
				holes.at(i).x = Random(SIZEX - 2);
				holes.at(i).y = Random(SIZEY - 2);
			}
		}

	}
}

void setZombieCoordinates(vector<Item>& zombies)
{//sets the zombies at each corner of the grid for start of game
	zombies.at(0).x = 1;		//TOP LEFT
	zombies.at(0).y = 1;

	zombies.at(1).x = SIZEX - 2;	//TOP RIGHT
	zombies.at(1).y = 1;

	zombies.at(2).x = 1;
	zombies.at(2).y = SIZEY - 2;	//BOTTOM LEFT

	zombies.at(3).x = SIZEX - 2;	//BOTTOM RIGHT
	zombies.at(3).y = SIZEY - 2;

}

void setPillsCoordinates(vector <Item> & pills, vector<Item> walls)
{
	for (int i(0); i < MAXPILLS; ++i) {
		pills.at(i).x = Random(SIZEX - 2);
		pills.at(i).y = Random(SIZEY - 2);
		for (int j(0); j < MAXWALLS; ++j)
		{
			while (((pills.at(i).x == walls.at(j).x) && (pills.at(i).y == walls.at(j).y))		//holes dont spawn on inner walls
				|| ((pills.at(i).x == SIZEX / 2) && (pills.at(i).y == SIZEY / 2))
				|| ((pills.at(i).x == 1) && (pills.at(i).y == 1))
				|| ((pills.at(i).x == SIZEX - 2) && (pills.at(i).y == 1))
				|| ((pills.at(i).x == 1) && (pills.at(i).y == SIZEY - 2))
				|| ((pills.at(i).x == SIZEX - 2) && (pills.at(i).y == SIZEY - 2)))				//holes dont spawn on spot
			{
				pills.at(i).x = Random(SIZEX - 2);
				pills.at(i).y = Random(SIZEY - 2);
			}
		}
	}

}

void setWallCoordinates(vector<Item>& walls)
{
	for (int i(0); i < 3; ++i)
	{
		walls.at(i).x = 4;
		walls.at(i).y = 4 + i;
	}
	int counter(0);
	for (int i(3); i < 6; ++i)
	{
		walls.at(i).x = 14;
		walls.at(i).y = 4 + counter;
		++counter;
	}
	counter = 1;
	for (int i(10); i < 15; ++i)
	{
		walls.at(i).x = 6 + counter;
		walls.at(i).y = 9;
		++counter;
	}
	counter = 1;
	for (int i(15); i < 20; ++i)
	{
		walls.at(i).x = 6 + counter;
		walls.at(i).y = 4;
		++counter;
	}
}

void setGrid(char grid[][SIZEX])
{ //reset the empty grid configuration
	for (int row(0); row < SIZEY; ++row) //for each column
	{
		for (int col(0); col < SIZEX; ++col) //for each col
		{
			if ((row == 0) || (row == SIZEY - 1))     //top and bottom walls
				grid[row][col] = WALL;                //draw a wall symbol
			else
				if ((col == 0) || (col == SIZEX - 1)) //left and right walls
					grid[row][col] = WALL;            //draw a wall symbol
				else
					grid[row][col] = TUNNEL;          //draw a space
		} //end of row-loop
	} //end of col-loop
} //end of setGrid

void placeHoles(char gr[][SIZEX], vector<Item> holes)
{
	for (int i(0); i < MAXHOLES; ++i)
	{
		gr[holes.at(i).y][holes.at(i).x] = holes.at(i).symbol;
	}
}

void placeSpot(char gr[][SIZEX], Item spot)
{ //place spot at its new position in grid
	gr[spot.y][spot.x] = spot.symbol;
} //end of placeSpot

void placeZombies(char gr[][SIZEX], vector<Item> zombies)
{//place zombies in the four corners of the grid
	for (int i(0); i < MAXZOMBIES; ++i)
	{
		gr[zombies.at(i).y][zombies.at(i).x] = zombies.at(i).symbol;
	}
}//end of placeZombies

void placePills(char gr[][SIZEX], vector<Item> pills)
{
	for (int i(0); i < MAXPILLS; ++i)
	{
		gr[pills.at(i).y][pills.at(i).x] = pills.at(i).symbol;
	}

}

void placeWalls(char gr[][SIZEX], vector<Item> walls)
{
	for (int i(0); i < MAXWALLS; ++i)
	{
		gr[walls.at(i).y][walls.at(i).x] = walls.at(i).symbol;
	}
}

//---------------------------------------------------------------------------
//----- update grid state
//---------------------------------------------------------------------------

void updateGrid(char grid[][SIZEX], Item spot, vector<Item> holes, vector<Item> zombies, vector<Item> pills, vector<Item> walls)
{ //update grid configuration after each move
	void setGrid(char[][SIZEX]);
	void placeSpot(char g[][SIZEX], Item spot);
	void placeHoles(char g[][SIZEX], vector<Item> holes);
	void placeZombies(char g[][SIZEX], vector<Item> zombies);
	void placePills(char g[][SIZEX], vector<Item> pills);
	void placeWalls(char g[][SIZEX], vector<Item> walls);


	setGrid(grid);	         //reset empty grid
	placeSpot(grid, spot);	 //set spot and zombie in grid
	placeWalls(grid, walls);
	placeHoles(grid, holes);		 //place holes update
	placePills(grid, pills);
	placeZombies(grid, zombies);

} //end of updateGrid

//---------------------------------------------------------------------------
//----- move the spot
//---------------------------------------------------------------------------
void updateSpotCoordinates(const char g[][SIZEX], Item& sp, int key, string& mess, int& lives, vector<Item>& pills, int& pillsLeft, bool& protectOn)
{ //move spot in required direction
	void setKeyDirection(int k, int& dx, int& dy);

	//calculate direction of movement required by key - if any
	int dx(0), dy(0);
	setKeyDirection(key, dx, dy); 	//find direction indicated by key
	//check new target position in grid 
	//and update spot coordinates if move is possible
	const int targetY(sp.y + dy);
	const int targetX(sp.x + dx);
	switch (g[targetY][targetX])
	{		//...depending on what's on the target position in grid...
	case TUNNEL:      //can move
		sp.y += dy;   //go in that Y direction
		sp.x += dx;   //go in that X direction
		break;
	case WALL:        //hit a wall and stay there
		cout << '\a'; //sound the alarm
		mess = "CANNOT GO THERE!";

		break;
	case HOLE:
		cout << '\a';		//sound the alarm
		mess = "YOU DIED";	//display message
		if (lives >= 0)	//end game if lives < 0
			--lives;
		sp.y += dy;   //go in that Y direction
		sp.x += dx;   //go in that X direction
		break;
	case PILL:
		mess = "+1 life"; // 
		++lives;
		--pillsLeft;
		protectOn = true; //turn magic protection on
		sp.y += dy;   //go in that Y direction
		sp.x += dx;   //go in that X direction
		for (int i(0); i < MAXPILLS; ++i)
		{
			if ((targetX == pills.at(i).x) && (targetY == pills.at(i).y))
			{
				pills.at(i).x = 0;
				pills.at(i).y = 0;
			}
		}
		break;
	}
} //end of updateSpotCoordinates

//---------------------------------------------------------------------------
//----- move the zombies
//---------------------------------------------------------------------------
void updateZombieCoordinates(const char gr[][SIZEX], vector<Item>& zombies, Item spot, bool freezeOn, int& lives, bool protectOn)
{
	int reverseDir(1);		//if protection is on this is -1, otherwise it equals 1;
	if (!freezeOn)
	{
		if (protectOn)
		{
			reverseDir = -1;
		}
		else
		{
			reverseDir = 1;
		}
		for (int i(0); i < MAXZOMBIES; ++i)
		{
			int zx(0), zy(0);
			if (zombies.at(i).x != 0 && zombies.at(i).y != 0)
			{
				if (zombies.at(i).x < spot.x)
				{
					zx = 1 * reverseDir;
				}
				else if (zombies.at(i).x > spot.x)
				{
					zx = -1 * reverseDir;
				}
				if (zombies.at(i).y < spot.y)
				{
					zy = 1 * reverseDir;
				}
				else if (zombies.at(i).y > spot.y)
				{
					zy = -1 * reverseDir;
				}
				const int targetx(zombies.at(i).x + zx); //target x coordinate of zombie
				const int targety(zombies.at(i).y + zy); //target y coordinate of zombie
				switch (gr[targety][targetx])
				{	//...depending on what's on the target position in grid...
				case TUNNEL:      //can move
					zombies.at(i).y += zy;   //go in that Y direction
					zombies.at(i).x += zx;  //go in that X direction
					break;
				case HOLE:
					zombies.at(i).y = 0;
					zombies.at(i).x = 0;
					break;
				case ZOMBIE:
					switch (i)
					{
					case 0:
						zombies.at(i).y = 1;	//TOP LEFT
						zombies.at(i).x = 1;
						break;
					case 1:
						zombies.at(i).y = 1;	//TOP RIGHT
						zombies.at(i).x = SIZEX - 2;
						break;
					case 2:
						zombies.at(i).y = SIZEY - 2;	//BOTTOM LEFT
						zombies.at(i).x = 1;
						break;
					case 3:
						zombies.at(i).y = SIZEY - 2;	//BOTTOM RIGHT
						zombies.at(i).x = SIZEX - 2;
						break;
					}
					break;
				case WALL:
					if (protectOn)
					{
						if (gr[zombies.at(i).y][targetx] == WALL)
						{
							zombies.at(i).y -= zy;
						}
						else if (gr[targety][zombies.at(i).x] == WALL)
						{
							zombies.at(i).x -= zx;
						}
					}
					else
					{
						if (gr[zombies.at(i).y][targetx] == WALL)
						{
							zombies.at(i).y += zy;
						}
						else if (gr[targety][zombies.at(i).x] == WALL)
						{
							zombies.at(i).x += zx;
						}
					}
					break;
				case SPOT:
					lives--;
					switch (i)
					{
					case 0:
						zombies.at(i).y = 1;	//TOP LEFT
						zombies.at(i).x = 1;
						break;
					case 1:
						zombies.at(i).y = 1;	//TOP RIGHT
						zombies.at(i).x = SIZEX - 2;
						break;
					case 2:
						zombies.at(i).y = SIZEY - 2;	//BOTTOM LEFT
						zombies.at(i).x = 1;
						break;
					case 3:
						zombies.at(i).y = SIZEY - 2;	//BOTTOM RIGHT
						zombies.at(i).x = SIZEX - 2;
						break;
					}
				case PILL:
				{
					zombies.at(i).y += zy;   //go in that Y direction
					zombies.at(i).x += zx;  //go in that X direction
					break;
				}
				}

			}
			else
			{
				zx = 0;
				zy = 0;
			}


		}

	}
}

//---------------------------------------------------------------------------
//----- process key
//---------------------------------------------------------------------------
void setKeyDirection(int key, int& dx, int& dy)
{ //
	switch (key)    //...depending on the selected key...
	{
	case LEFT:      //when LEFT arrow pressed...
		dx = -1;    //decrease the X coordinate
		dy = 0;
		break;
	case RIGHT:     //when RIGHT arrow pressed...
		dx = +1;    //increase the X coordinate
		dy = 0;
		break;
	case UP:		//UP pressed
		dx = 0;
		dy = -1;
		break;
	case DOWN:		//DOWN Pressed
		dx = 0;
		dy = +1;
		break;
	}
} //end of setKeyDirection

int getKeyPress()
{ //get key or command selected by user
	int keyPressed;
	keyPressed = getch();      //read in the selected arrow key or command letter
	while (keyPressed == 224)     //ignore symbol following cursor key
		keyPressed = getch();
	return(keyPressed);
} //end of getKeyPress

bool isArrowKey(int key)
{ //check if the key pressed is an arrow key (also accept 'K', 'M', 'H' and 'P')
	return ((key == LEFT) || (key == RIGHT) || (key == UP) || (key == DOWN));
} //end of isArrowKey

bool wantToQuit(int key)
{ //check if the key pressed is 'Q'
	key = toupper(key);
	return (key == QUIT);
} //end of wantToQuit

bool wantToReset(int key)
{ //check if the key pressed is 'R'
	key = toupper(key);
	return (key == RESET);
} //end of wantToQuit

bool isFreezeOn(bool& freezeOn)
{// if freeze key is on return true otherwise return false
	if (freezeOn == true)
	{
		freezeOn = false;
		Gotoxy(40, 14);
		SelectBackColour(clBlack);
		SelectTextColour(clWhite);
		cout << "                              ";
	}
	else
	{
		freezeOn = true;
		Gotoxy(40, 14);
		SelectBackColour(clWhite);
		SelectTextColour(clBlue);
		cout << "ZOMBIE FREEZE IS ACTIVATED";
	}
	return freezeOn;
}
bool isExterminateOn(bool& exterminateOn, vector<Item>& zombies)
{// if exterminate key is on return true otherwise return false
	if (exterminateOn == true)
	{
		exterminateOn = false;
		Gotoxy(40, 15);
		SelectBackColour(clBlack);
		SelectTextColour(clWhite);
		cout << "                                       ";
		for (int i(0); i < MAXZOMBIES; ++i)
		{
			switch (i)
			{
			case 0:
				zombies.at(i).y = 1;	//TOP LEFT
				zombies.at(i).x = 1;
				break;
			case 1:
				zombies.at(i).y = 1;	//TOP RIGHT
				zombies.at(i).x = SIZEX - 2;
				break;
			case 2:
				zombies.at(i).y = SIZEY - 2;	//BOTTOM LEFT
				zombies.at(i).x = 1;
				break;
			case 3:
				zombies.at(i).y = SIZEY - 2;	//BOTTOM RIGHT
				zombies.at(i).x = SIZEX - 2;
				break;
			}
		}

	}
	else
	{
		exterminateOn = true;
		Gotoxy(40, 15);
		SelectBackColour(clGreen);
		SelectTextColour(clBlack);
		cout << "ZOMBIE EXTERMINATE IS ACTIVATED";
		for (int i(0); i < MAXZOMBIES; ++i)
		{
			zombies.at(i).y = 0;
			zombies.at(i).x = 0;
		}
	}
	return exterminateOn;
}

bool isEatPillsOn(bool& eatPills, vector<Item>& pills, int& pillsLeft, int& lives)
{

	if (eatPills == true)
	{
		eatPills = false;
		Gotoxy(40, 16);
		SelectBackColour(clBlack);
		SelectTextColour(clWhite);
		cout << "                              ";
	}
	else
	{
		eatPills = true;
		Gotoxy(40, 16);
		SelectBackColour(clYellow);
		SelectTextColour(clBlue);
		cout << "EAT ALL PILLS ACTIVATED";

		for (int i(0); i < MAXPILLS; ++i)
		{
			if (pills.at(i).x == 0 && pills.at(i).y == 0)
			{

			}
			else
			{
				pills.at(i).x = 0;
				pills.at(i).y = 0;
				lives++;
			}
		}
		pillsLeft = 0;
	}
	return eatPills;
}


//---------------------------------------------------------------------------
//----- display info on screen
//---------------------------------------------------------------------------
void clearMessage()
{ //clear message area on screen
	SelectBackColour(clBlack);
	SelectTextColour(clWhite);
	Gotoxy(40, 8);
	string str(20, ' ');
	cout << str;  //display blank message

} //end of setMessage

void renderGame(char gd[][SIZEX], string mess, int lives, int pillsLeft, bool protectOn)
{ //display game title, messages, maze, spot and apples on screen
	void paintGrid(char g[][SIZEX], bool protectOn);
	void showTitle();
	void showOptions(int lives, int pillsLeft);
	void showMessage(string);

	Gotoxy(0, 0);
	//display grid contents
	paintGrid(gd, protectOn);
	//display game title
	showTitle();
	//display menu options available
	showOptions(lives, pillsLeft);
	//display message if any
	showMessage(mess);
} //end of paintGame

void paintGrid(char g[][SIZEX], bool protectOn)
{ //display grid content on screen
	SelectBackColour(clBlack);
	SelectTextColour(clWhite);
	Gotoxy(0, 2);
	for (int row(0); row < SIZEY; ++row)      //for each row (vertically)
	{
		for (int col(0); col < SIZEX; ++col)  //for each column (horizontally)
		{
			if (g[0][0] == ZOMBIE || g[0][0] == PILL)
				g[0][0] = WALL;
			if (g[row][col] == ZOMBIE)
				SelectTextColour(clGreen);
			/*if (g[row][col] == SPOT && protectOn)
			{
				SelectBackColour(clBlue);
				SelectTextColour(clWhite);
			}		*/		
			if ((g[row][col] == SPOT) || (g[row][col] == WALL))
			{
				SelectBackColour(clBlack);
				SelectTextColour(clWhite);
			}
			if (g[row][col] == HOLE)
			{
				SelectBackColour(clBlack);
				SelectTextColour(clRed);
			}
			if (g[row][col] == PILL)
			{
				SelectBackColour(clBlack);
				SelectTextColour(clYellow);
			}
			cout << g[row][col];              //output cell content

		} //end of col-loop
		cout << endl;
	} //end of row-loop
} //end of paintGrid

void showTitle()
{ //display game title
	SelectTextColour(clYellow);
	Gotoxy(0, 0);
	cout << "_____ZOMBIE GAME____\n" << endl;
	SelectBackColour(clWhite);
	SelectTextColour(clRed);
	Gotoxy(40, 0);
	cout << "Matt Stephenson, Sam Young, Joe Thomas :";
	SelectTextColour(clYellow);
	SelectBackColour(clBlack);
	Gotoxy(0, 1);
	cout << GetTime() << "  " << GetDate();//names and temporary date&time 
} //end of showTitle


void showOptions(int lives, int pillsLeft)
{ //show game options
	SelectBackColour(clRed);
	SelectTextColour(clYellow);
	Gotoxy(40, 2);
	cout << "TO MOVE USE KEYBOARD ARROWS        ";
	Gotoxy(40, 3);
	cout << "TO QUIT ENTER 'Q'                  ";
	Gotoxy(40, 4);
	cout << "TO RESET ENTER 'R'                 ";
	Gotoxy(40, 5);
	cout << "TO FREEZE ZOMBIES ENTER 'F'        ";
	Gotoxy(40, 6);
	cout << "TO EXTERMINATE ZOMBIES ENTER 'X'   ";
	Gotoxy(40, 7);
	cout << "TO EAT ALL PILLS ENTER 'E'	";
	Gotoxy(40, 10);
	cout << "LIVES LEFT : " << lives << "                     ";
	Gotoxy(40, 11);
	cout << "PILLS LEFT : " << pillsLeft << "                    ";
} //end of showOptions

void showMessage(string m)
{ //print auxiliary messages if any
	SelectBackColour(clBlack);
	SelectTextColour(clWhite);
	Gotoxy(40, 8);
	cout << m;	//display current message
} //end of showMessage

void outOfLives(char grid[][SIZEX], Item& spot, int key, string& message, vector<Item>& holes, int& lives, vector<Item>& zombies, bool freezeOn, vector<Item>& pills, int& pillsLeft, bool& protectOn, vector<Item> walls)
{//either end or reset the program due to player being out of lives
	void endProgram();
	void setZombieCoordinates(vector <Item>& zombies);
	void displayScoreScreen(int lives);
	void updateGame(char grid[][SIZEX], Item& spot, int key, string& message, vector<Item>& holes, int& lives, vector<Item>& zombies, bool freezeOn, vector<Item>& pills, int& pillsLeft, bool& protectOn, vector<Item> walls);

	int  getKeyPress();
	//int key(' ');
	SelectBackColour(clBlack);
	SelectTextColour(clYellow);
	Gotoxy(40, 12);
	cout << "OUT OF LIVES.";
	Gotoxy(40, 13);
	cout << "'R' to Reset. 'Q' to Quit";
	key = getKeyPress();
	if (wantToReset(key))
	{
		spot.x = SIZEX / 2;
		spot.y = SIZEY / 2;
		lives = 3;
		pillsLeft = MAXPILLS;
		setZombieCoordinates(zombies);
		Gotoxy(40, 12);
		cout << "                                      ";
		Gotoxy(40, 13);
		cout << "                                      ";
		updateGame(grid, spot, key, message, holes, lives, zombies, freezeOn, pills, pillsLeft, protectOn, walls);
	}
	else if (wantToQuit(key))
	{
		endProgram();
	}
	else if (key == RETURN)
	{
		displayScoreScreen(lives);
	}
}

void winGame(char grid[][SIZEX], Item& spot, int key, string& message, vector<Item>& holes, int& lives, vector<Item>& zombies, bool freezeOn, vector<Item>& pills, int& pillsLeft, bool& protectOn, vector<Item> walls)
{//either end or reset the program due to player being out of lives
	void endProgram();
	void setZombieCoordinates(vector <Item>& zombies);
	int  getKeyPress();
	void displayScoreScreen(int lives);
	void writeNameScoreToFile(int lives);
	void updateGame(char grid[][SIZEX], Item& spot, int key, string& message, vector<Item>& holes, int& lives, vector<Item>& zombies, bool freezeOn, vector<Item>& pills, int& pillsLeft, bool& protectOn, vector<Item> walls);

	//int key(' ');
	SelectBackColour(clBlack);
	SelectTextColour(clYellow);
	Gotoxy(40, 12);
	cout << "YOU WIN!";

	Gotoxy(40, 16);
	writeNameScoreToFile(lives);

	Gotoxy(40, 13);
	cout << "'R' to Reset. 'Q' to Quit.";
	Gotoxy(40, 14);
	cout << "<ENTER> to Display Scores.";
	key = getKeyPress();
	if (wantToReset(key))
	{
		spot.x = SIZEX / 2;
		spot.y = SIZEY / 2;
		lives = 3;
		pillsLeft = MAXPILLS;
		setZombieCoordinates(zombies);
		Gotoxy(40, 12);
		cout << "                                      ";
		Gotoxy(40, 13);
		cout << "                                      ";
		updateGame(grid, spot, key, message, holes, lives, zombies, freezeOn, pills, pillsLeft, protectOn, walls);
	}
	else if (wantToQuit(key))
	{
		endProgram();
	}
	else if (key == RETURN)
	{
		displayScoreScreen(lives);
	}
}

void endProgram()
{ //end program with appropriate message
	SelectBackColour(clBlack);
	SelectTextColour(clYellow);
	Gotoxy(40, 8);
	cout << "PLAYER QUITS!          ";
	//hold output screen until a keyboard key is hit
	Gotoxy(40, 9);
	system("pause");
} //end of endProgram

//---------------------------------------------------------------------------
//----- Title Screen Etc...
//---------------------------------------------------------------------------

void loadTitleScreen(int lives)
{//start of load title screen
	//VARIABLES
	int key(' ');										//create key to store keyboard events
	bool exitToGame(false);								//state management for exiting the subroutine
	int maxLength(0);									//max chars for drawing design
	//PROTOTYPES
	void displayInfoScreen(int lives);							//declare prototypes
	void writeNameScoreToFile(int lives);

	Clrscr();											//clear the screen before drawing
	maxLength = screenHeight();							//get the length of the screen


	//display a design and info-----------------------------------------------------------------
	SelectBackColour(clBlack);
	SelectTextColour(clRed);
	cout << "################################################################################" <<	//80chars
			"################################################################################";		//80chars

	//---text line with date below--------------------------------------------------------------
	cout << "### ";														//output left border																						
	SelectTextColour(clWhite);											//set text to white
	cout << GetTime() << " " << GetDate();								//print date and time
	SelectTextColour(clRed);											//reset text to red
	cout << "                                                      ###";//print screen space and r border
	//---end of text line with date-------------------------------------------------------------

	cout << "###                                                                          ###";
	for (int i(0); i < ((maxLength / 3) - 2); ++i)													//loop
	cout << "###                                                                          ###";
	cout << "###                           ZOMBIE GAME - FOP 1Z                           ###" <<	//{
			"###                         TEST TIME INSERTED HERE                          ###" <<	//
			"###                                                                          ###" <<	//
			"###      PRESS:                                         Joe Thomas           ###" <<	//7 lines of content
			"###      I for Info                                 Matthew Stephenson       ###" <<	//
			"###      P to Play                                      Joe Moran            ###" <<	//
			"###      Q to quit                                      Sam Young            ###";		//}
	for (int j(0); j < ((maxLength / 3) - 2); ++j)													//loop
	cout << "###                                                                          ###";
	cout << "################################################################################" <<	//80chars
			"###############################################################################";		//79chars

	//---------------------------------------------------------------------------end of design--

	Gotoxy(25, 18);
	writeNameScoreToFile(lives);						//write to file


	while (toupper(key) != PLAY)
	{
		key = getKeyPress();								//read in next keyboard event
		switch (toupper(key))								//convert key to upperCase
		{
		case INFO:											//when I is pressed...
			displayInfoScreen(lives);							//call info screen sub
			break;
		case QUIT:											//when Q is pressed...
			endProgram();									//Quit program
			break;
		}
	}
	if (key == PLAY)
		Clrscr();

}//end load title screen

void displayInfoScreen(int lives)
{//start display info screen
	int key(' ');
	bool exitToTitle(false);
	int maxLength(0);

	void loadTitleScreen(int lives);

	Clrscr();

	maxLength = screenHeight();

	//display a design and info-----------------------------------------------------------------

	cout << "################################################################################" <<	//80chars
		"################################################################################";		//80chars

	//---text line with date below--------------------------------------------------------------
	cout << "### ";														//output left border																						
	SelectTextColour(clWhite);											//set text to white
	cout << GetTime() << " " << GetDate();								//print date and time
	SelectTextColour(clRed);											//reset text to red
	cout << "                                                      ###";//print screen space and r border
	//---end of text line with date-------------------------------------------------------------

	cout << "###                                                                          ###";
	for (int i(0); i < ((maxLength / 3) - 2); ++i)													//loop for 7 lines
		cout << "###                                                                          ###";
	cout << "###                             GAME INFO SCREEN                             ###" <<	//{
		"###                         TEST INFO INSERTED HERE                          ###" <<	//
		"###                                                                          ###" <<	//
		"###          Avoid the zombies and collect pills to gain protection          ###" <<	//7 lines of content
		"###       Move using the arrow keys and lure zombies into holes to win       ###" <<	//
		"###                                                                          ###" <<	//
		"###                  Press <ENTER> to exit to main menu...                   ###";		//}
	for (int j(0); j < ((maxLength / 3) - 2); ++j)														//loop for 7 lines
		cout << "###                                                                          ###";
	cout << "################################################################################" <<	//80chars
		"###############################################################################";		//79chars

	//---------------------------------------------------------------------------end of design--


	while (toupper(key) != RETURN)
	{
		key = getKeyPress();								//read in next keyboard event
		switch (toupper(key))
		{
		case RETURN:
			loadTitleScreen(lives);
			break;
		}
	}


}//end display info screen

void displayScoreScreen(int lives)
{
	int key(' ');
	bool exitToTitle(false);
	int maxLength(0);
	int getPlayerScore(string filename);
	int calculatePlayerScore(int lives);
	void loadTitleScreen(int lives);

	Clrscr();

	maxLength = screenHeight();
	SelectTextColour(clRed);								//reset text to red

	//display a design and info-----------------------------------------------------------------

cout << "################################################################################" <<	//80chars
		"################################################################################";		//80chars

	//---text line with date below--------------------------------------------------------------
	cout << "### ";														//output left border																						
	SelectTextColour(clWhite);											//set text to white
	cout << GetTime() << " " << GetDate();								//print date and time
	SelectTextColour(clRed);											//reset text to red
	cout << "                                                      ###";//print screen space and r border
	//---end of text line with date-------------------------------------------------------------

	cout << "###                                                                          ###";
	for (int i(0); i < ((maxLength / 3) - 2); ++i)												
cout << "###                                                                          ###";
cout << "###                                TOP SCORES                                ###" <<	
		"###                                                                          ###" <<	
		"###                                                                          ###" <<	
		"###                                                                          ###" <<	
		"###                                                                          ###" <<	
		"###                                                                          ###" <<	
		"###                  Press <ENTER> to exit to main menu...                   ###";		
	for (int j(0); j < ((maxLength / 3) - 2); ++j)												
cout << "###                                                                          ###";
cout << "################################################################################" <<	//80chars
		"###############################################################################";		//79chars

	//---------------------------------------------------------------------------end of design--

	Gotoxy(40, 14);
	cout << calculatePlayerScore(lives);

	while (toupper(key) != RETURN)
	{
		key = getKeyPress();								//read in next keyboard event
		switch (toupper(key))
		{
		case RETURN:
			loadTitleScreen(lives);
			break;
		}
	}
}

//---Start of file writing/reading functions-----------------------------------------------------------

void writeNameScoreToFile(int lives)
{
	int getPlayerScore(string filename);
	int calculatePlayerScore(int lives);
	string getPlayerName();

	ofstream toFile;												//file to write to
	string filename = getPlayerName() + scoreFiletype;
	toFile.open(filename, ios::out);								//create/write to an inputName.scr file

	if (toFile.fail())												//if file fails to open
		cout << "ERROR! File failed to open\n";
	else
		if (getPlayerScore(filename) < calculatePlayerScore(lives))	//if new score is bigger than previous
		{
		toFile << calculatePlayerScore(lives);					//write score into file body(lives in main version?)
		toFile << "\n" << GetDate() << " " << GetTime();		//write date and time into the file on a seperate line
		}


	toFile.close();													//close the file
	cout << "Done! Select a menu item.";
}

string getPlayerName()
{
	string inputName;												//player name string
	//int delChars(19);												//max characters to delete in name (wipes blank chars)

	//cout << "writing name to file..\n"; 
	Gotoxy(25, 19);
	cout << "Input Name... (20 chars max)\n";
	Gotoxy(25, 20);
	cin >> inputName;												//input a name string
	Gotoxy(25, 21);

	return inputName;												//return the name
}

int getPlayerScore(string filename)
{
	string gePlayerName();

	ifstream inputFile;							//file to read from
	string data;								//score data to collect
	int topScore(0);							//score to read from file

	inputFile.open((filename), ios::in);		//read from an inputName.scr file

	if (inputFile.fail())						//if file fails to open
		cout << "ERROR! File failed to open\n";
	else
	{
		if (!inputFile.eof())					//if not the end of the file contents (has a score)
			inputFile >> topScore;				//read the score from file
		else
			topScore = -1;						//new players score is -1
	}
	inputFile.close();							//close file

	return topScore;
}

//-----End of file writing/reading functions-----------------------------------------------------------

//-----Start of score calculation----------------------------------------------------------------------
int calculatePlayerScore(int lives)				// score = lives left x 20 on completion of the game
{
	int scoreValue(0);

	scoreValue = (20 * lives);
	return scoreValue;
}
//----End of score calculation------------------------------------------------------------------------

