/*
	"Hex" is a porting of the "Influence" game for mobile to Casio calculators
    Copyright (C) 2016 Nozalia

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MAP_DISPLAY_MANAGER_H
#define MAP_DISPLAY_MANAGER_H

extern "C" {
    #include "fxlib.h"
    #include "Sources/MonochromeLib.h"
};

#include "Sources/PointsManager.h"

enum PlayerType {PLAYER_1 = 0, PLAYER_2 = 1, DEFAULT = 2};
enum Direction {TOP = 0, LEFT = 1, RIGHT = 2, BOTTOM = 3, NONE = 4};
enum SelectionType {CURRENT_SELECTION = 0, LOCKED_SELECTION = 1};

class MapDisplayManager {

public:
	MapDisplayManager(){}
	MapDisplayManager(int mat[][6]);
	~MapDisplayManager(){}

	void loadMap(PointsManager points);

	void changeOwner(size_t line, size_t column, PlayerType owner);
	size_t getReinforcementPoints(PlayerType player);

	PlayerType playerAtCurrentSelection(){return currentSelection.selectionType;}
	
	void changeCurrentSelection(Direction dir);
	
	int getCurrentLine(){return currentSelection.line;}
	int getCurrentColumn(){return currentSelection.column;}

	int getLockedLine(){return lockedSelection.line;}
	int getLockedColumn(){return lockedSelection.column;}
	
	void setLockedToCurrent();
	void resetLockedSelection();
	
	void printCurrentSelection();
	void printLockedSelection();
private:
	void hexagon(size_t line, size_t column, PlayerType type);
	void drawLine(int line, int column);

	void setSelection(size_t line, size_t column, SelectionType selection);

	void printDigit(size_t line, size_t column, size_t digit, PlayerType type);

	int map[5][6];

	struct Selection {
		bool draw;
		bool show;
		int line;
		int column;
		PlayerType selectionType;
	};

	Selection currentSelection;
	Selection lockedSelection;
};

#endif