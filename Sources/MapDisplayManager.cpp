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

//TODO : CRITICAL : Rewrite the 'changeCurrentSelection' function

#include "MapDisplayManager.h"

MapDisplayManager::MapDisplayManager(int mat[][6]){
	currentSelection.draw = true;
	currentSelection.show = false;

	lockedSelection.draw = false;
	lockedSelection.show = false;

	/*if (Bfile_CreateMainMemory("filename.ext") != 0){
		PrintMini(0,0,"ERROR : Could't create the file", MINI_OR);
		return;
	}

	int handle = Bfile_OpenMainMemory("filename.ext");

	if (handle < 0){
		PrintMini(0,20,"ERROR : Could't open the file", MINI_OR);
		return;
	}*/

	//char buffer[];

	/*if (Bfile_ReadFile(handle, buffer, 30*sizeof(char), -1) < 0){
		PrintMini(0,40,"ERROR : Could't read the file", MINI_OR);
		return;
	}*/

	/*for (size_t i = 0; i < 5; ++i){
		for (size_t j = 0; j < 6; ++j){
			map[i][j] = buffer[j+i*j];
		}
	}*/

	//Bfile_CloseFile(handle);

	for (size_t i = 0; i < 5; ++i){
		for (size_t j = 0; j < 6; ++j){
			map[i][j] = mat[i][j];

			if (map[i][j] == 1)
				setSelection(i, j, CURRENT_SELECTION);
		}
	}

	setSelection(0, 0, LOCKED_SELECTION);
}

void MapDisplayManager::hexagon(size_t line, size_t column, PlayerType type){
	int cx[6] = {0,4,12,16,12,4};
    int cy[6] = {4,0,0,4,8,8};

    for (size_t i = 0; i < 6; ++i){
    	if (line%2 == 0){
    		cx[i] += 22*column;
    		cy[i] += 13*line;
    	} else {
    		cx[i] += 22*column + 11;
    		cy[i] += 13*line;
    	}
    }

	if (type == PLAYER_1){
		ML_polygon(cx, cy, 6, ML_BLACK);
	} else if (type == PLAYER_2){
		ML_polygon(cx, cy, 6, ML_BLACK);

		for (size_t i = 0; i < 4; ++i){
			ML_horizontal_line(i + cy[1] + 1, (3 + cx[0]) - i, cx[2] + i, ML_BLACK);
			ML_horizontal_line(cy[4] - 1 - i, (3 + cx[0]) - i, cx[2] + i, ML_BLACK);
		}
	} else {
		ML_polygon(cx, cy, 6, ML_BLACK);

		for (size_t i = 0; i < 4; ++i){
			ML_horizontal_line(cy[4] - 1 - i, (3 + cx[0]) - i, cx[2] + i, ML_BLACK);
		}
	}
}

void MapDisplayManager::drawLine(int line, int column){
	if (line > 5 || column > 6)
		return;

	if (line - 1 >= 0){
		if (line%2 == 0){
			if (map[line - 1][column] != 0)
				ML_line(22*column + 12, 13*line, 22*column + 15, 13*line - 6, ML_BLACK);
			if (column - 1 >= 0){
				if (map[line - 1][column - 1] != 0)
					ML_line(22*column + 4, 13*line, 22*(column - 1) + 23, 13*line - 6, ML_BLACK);
			}
		} else {
			if (map[line - 1][column + 1] != 0)
				ML_line(22*column + 23, 13*line, 22*(column + 1) + 4, 13*line - 6, ML_BLACK);
			if (map[line - 1][column] != 0)
				ML_line(22*column + 15, 13*line, 22*column + 12, 13*line - 6, ML_BLACK);
		}
	}

	if (column - 1 >= 0){
		if (map[line][column - 1] != 0){
			if (line%2 == 0)
				ML_line(22*column, 13*line + 4, 22*(column - 1) + 16, 13*line + 4, ML_BLACK);
			else
				ML_line(22*column + 11, 13*line + 4, 22*(column - 1) + 26, 13*line + 4, ML_BLACK);
		}
	}
}

void MapDisplayManager::setSelection(size_t line, size_t column, SelectionType selection){
	if (selection == CURRENT_SELECTION){
		if (map[line][column] == 0)
			return;

		currentSelection.line = line;
		currentSelection.column = column;
		currentSelection.selectionType = (PlayerType)(map[line][column] - 1);
	} else {
		lockedSelection.line = line;
		lockedSelection.column = column;
		lockedSelection.selectionType = currentSelection.selectionType;
	}
}

void MapDisplayManager::printCurrentSelection(){
	if (currentSelection.draw){
		if (currentSelection.show)
			map[currentSelection.line][currentSelection.column] = (int)(currentSelection.selectionType) + 1;
		else
			map[currentSelection.line][currentSelection.column] = 0;
		
		currentSelection.show = !currentSelection.show;
	}
}

void MapDisplayManager::printLockedSelection(){
	if (lockedSelection.draw){
		if (lockedSelection.show)
			map[lockedSelection.line][lockedSelection.column] = (int)(lockedSelection.selectionType) + 1;
		else 
			map[lockedSelection.line][lockedSelection.column] = 0;

		lockedSelection.show = !lockedSelection.show;
	}
}

void MapDisplayManager::changeCurrentSelection(Direction dir){
	map[currentSelection.line][currentSelection.column] = (int)(currentSelection.selectionType) + 1;
	currentSelection.show = false;

	switch(dir){
		case TOP:
			if (currentSelection.line > 0){
				if (lockedSelection.draw){
					if (lockedSelection.line - (currentSelection.line - 1) > 1)
						return;
				}

				if (currentSelection.line%2 == 0){
					if (currentSelection.column - 1 >= 0){
						if (lockedSelection.draw){
							if (currentSelection.column < lockedSelection.column)
								return;
						}

						if (map[currentSelection.line - 1][currentSelection.column - 1] != 0){
							setSelection(currentSelection.line - 1, currentSelection.column - 1, CURRENT_SELECTION);
						} else if (map[currentSelection.line - 1][currentSelection.column] != 0){
							if (lockedSelection.draw){
								if (currentSelection.column > lockedSelection.column)
									return;
							}

							setSelection(currentSelection.line - 1, currentSelection.column, CURRENT_SELECTION);
						}
					} else if (map[currentSelection.line - 1][currentSelection.column] != 0){
						if (lockedSelection.draw){
							if (currentSelection.column > lockedSelection.column)
								return;
						}

						setSelection(currentSelection.line - 1, currentSelection.column, CURRENT_SELECTION);
					}
				} else {
					if (map[currentSelection.line - 1][currentSelection.column] != 0){
						setSelection(currentSelection.line - 1, currentSelection.column, CURRENT_SELECTION);
					} else if (currentSelection.column + 1 < 6){
						if (lockedSelection.draw){
							if (currentSelection.column > lockedSelection.column)
								return;
						}

						if (map[currentSelection.line - 1][currentSelection.column + 1] != 0)
							setSelection(currentSelection.line - 1, currentSelection.column + 1, CURRENT_SELECTION);
					}
				}
			}
		break;

		case LEFT:
			if (currentSelection.column > 0){
				if (lockedSelection.draw){
					if (lockedSelection.line != currentSelection.line){
						if (lockedSelection.line%2 == 0){
							if (lockedSelection.column != currentSelection.column){
								return;
							}
						} else {
							if (lockedSelection.column == currentSelection.column){
								return;
							}
						}
					} else if (lockedSelection.column - (currentSelection.column - 1) > 1){
						return;
					}
				}

				setSelection(currentSelection.line, currentSelection.column - 1, CURRENT_SELECTION);
			}
		break;

		case RIGHT:
			if (currentSelection.column < 5){
				if (lockedSelection.draw){
					if (lockedSelection.line != currentSelection.line){
						if (lockedSelection.line%2 == 0){
							if (lockedSelection.column == currentSelection.column){
								return;
							}
						} else {
							if (lockedSelection.column != currentSelection.column){
								return;
							}
						}
					} else if ((currentSelection.column + 1) - lockedSelection.column > 1){
						return;
					}
				}

				setSelection(currentSelection.line, currentSelection.column + 1, CURRENT_SELECTION);
			}
		break;

		case BOTTOM:
			if (currentSelection.line < 4){
				if (lockedSelection.draw){
					if ((currentSelection.line + 1) - lockedSelection.line > 1)
						return;
				}

				if (currentSelection.line%2 == 0){
					if (currentSelection.column - 1 >= 0){
						if (lockedSelection.draw){
							if (currentSelection.column < lockedSelection.column)
								return;
						}

						if (map[currentSelection.line + 1][currentSelection.column - 1] != 0){
							setSelection(currentSelection.line + 1, currentSelection.column - 1, CURRENT_SELECTION);
						} else if (map[currentSelection.line + 1][currentSelection.column] != 0){
							if (lockedSelection.draw){
								if (currentSelection.column > lockedSelection.column)
									return;
							}

							setSelection(currentSelection.line + 1, currentSelection.column, CURRENT_SELECTION);
						}
					} else if (map[currentSelection.line + 1][currentSelection.column] != 0){
						if (lockedSelection.draw){
							if (currentSelection.column > lockedSelection.column)
								return;
						}

						setSelection(currentSelection.line + 1, currentSelection.column, CURRENT_SELECTION);
					}
				} else {
					if (map[currentSelection.line + 1][currentSelection.column] != 0){
						setSelection(currentSelection.line + 1, currentSelection.column, CURRENT_SELECTION);
					} else if (currentSelection.column + 1 < 6){
						if (lockedSelection.draw){
							if (currentSelection.column > lockedSelection.column)
								return;
						}

						if (map[currentSelection.line + 1][currentSelection.column + 1] != 0)
							setSelection(currentSelection.line + 1, currentSelection.column + 1, CURRENT_SELECTION);
					}
				}
			}
		break;

		default:
		break;
	}
}

void MapDisplayManager::setLockedToCurrent(){
	setSelection(currentSelection.line, currentSelection.column, LOCKED_SELECTION);
	lockedSelection.draw = true;
}

void MapDisplayManager::resetLockedSelection(){
	map[lockedSelection.line][lockedSelection.column] = (int)(lockedSelection.selectionType) + 1; 
	lockedSelection.draw = false; 
	lockedSelection.show = false;
}

void MapDisplayManager::printDigit(size_t line, size_t column, size_t digit, PlayerType type){
	if (digit < 10){
		size_t cx;
		
		if (line%2 == 0)
			cx = 22*column + 7;
		else
			cx = 22*column + 18;

		size_t cy = 13*line + 2;

		if (type == PLAYER_1){
			switch (digit){
				case 1:
					ML_line(cx + 1, cy, cx + 1, cy + 5, ML_BLACK);
				break;

				case 2:
					ML_line(cx, cy, cx + 2, cy, ML_BLACK);
					ML_line(cx + 2, cy, cx + 2, cy + 2, ML_BLACK);
					ML_line(cx + 2, cy + 2, cx, cy + 2, ML_BLACK);
					ML_line(cx, cy + 2, cx, cy + 4, ML_BLACK);
					ML_line(cx, cy + 4, cx + 3, cy + 4, ML_BLACK);
				break;

				case 3:
					ML_line(cx, cy, cx + 2, cy, ML_BLACK);
					ML_line(cx + 2, cy, cx + 2, cy + 5, ML_BLACK);
					ML_line(cx + 2, cy + 2, cx - 1, cy + 2, ML_BLACK);
					ML_line(cx + 2, cy + 4, cx - 1, cy + 4, ML_BLACK);
				break;

				case 4:
					ML_line(cx, cy, cx, cy + 3, ML_BLACK);
					ML_line(cx, cy + 3, cx + 2, cy + 3, ML_BLACK);
					ML_line(cx + 2, cy, cx + 2, cy + 5, ML_BLACK);
				break;

				case 5:
					ML_line(cx + 2, cy, cx, cy, ML_BLACK);
					ML_line(cx, cy, cx, cy + 2, ML_BLACK);
					ML_line(cx, cy + 2, cx + 2, cy + 2, ML_BLACK);
					ML_line(cx + 2, cy + 2, cx + 2, cy + 4, ML_BLACK);
					ML_line(cx + 2, cy + 4, cx - 1, cy + 4, ML_BLACK);
				break;

				case 6:
					ML_line(cx + 2, cy, cx, cy, ML_BLACK);
					ML_line(cx, cy, cx, cy + 4, ML_BLACK);
					ML_line(cx, cy + 4, cx + 2, cy + 4, ML_BLACK);
					ML_line(cx + 2, cy + 4, cx + 2, cy + 2, ML_BLACK);
					ML_line(cx + 2, cy + 2, cx, cy + 2, ML_BLACK);
				break;

				case 7:
					ML_line(cx, cy, cx + 2, cy, ML_BLACK);
					ML_line(cx + 2, cy, cx + 2, cy + 5, ML_BLACK);
					ML_pixel(cx, cy + 1, ML_BLACK);
				break;

				case 8:
					ML_line(cx, cy, cx + 2, cy, ML_BLACK);
					ML_line(cx + 2, cy, cx + 2, cy + 4, ML_BLACK);
					ML_line(cx + 2, cy + 4, cx, cy + 4, ML_BLACK);
					ML_line(cx, cy + 4, cx, cy, ML_BLACK);
					ML_pixel(cx + 1, cy + 2, ML_BLACK);
				break;

				case 9:
					ML_line(cx, cy, cx + 2, cy, ML_BLACK);
					ML_line(cx + 2, cy, cx + 2, cy + 4, ML_BLACK);
					ML_line(cx + 2, cy + 4, cx, cy + 4, ML_BLACK);
					ML_line(cx + 2, cy + 2, cx, cy + 2, ML_BLACK);
					ML_line(cx, cy + 2, cx, cy, ML_BLACK);
					ML_pixel(cx, cy + 4, ML_BLACK);
				break;

				default:
					return;
				break;
			}
		} else if (type == PLAYER_2){
			switch (digit){
				case 1:
					ML_line(cx + 1, cy, cx + 1, cy + 5, ML_WHITE);
				break;

				case 2:
					ML_line(cx, cy, cx + 2, cy, ML_WHITE);
					ML_line(cx + 2, cy, cx + 2, cy + 2, ML_WHITE);
					ML_line(cx + 2, cy + 2, cx, cy + 2, ML_WHITE);
					ML_line(cx, cy + 2, cx, cy + 4, ML_WHITE);
					ML_line(cx, cy + 4, cx + 3, cy + 4, ML_WHITE);
				break;

				case 3:
					ML_line(cx, cy, cx + 2, cy, ML_WHITE);
					ML_line(cx + 2, cy, cx + 2, cy + 5, ML_WHITE);
					ML_line(cx + 2, cy + 2, cx - 1, cy + 2, ML_WHITE);
					ML_line(cx + 2, cy + 4, cx - 1, cy + 4, ML_WHITE);
				break;

				case 4:
					ML_line(cx, cy, cx, cy + 3, ML_WHITE);
					ML_line(cx, cy + 3, cx + 2, cy + 3, ML_WHITE);
					ML_line(cx + 2, cy, cx + 2, cy + 5, ML_WHITE);
				break;

				case 5:
					ML_line(cx + 2, cy, cx, cy, ML_WHITE);
					ML_line(cx, cy, cx, cy + 2, ML_WHITE);
					ML_line(cx, cy + 2, cx + 2, cy + 2, ML_WHITE);
					ML_line(cx + 2, cy + 2, cx + 2, cy + 4, ML_WHITE);
					ML_line(cx + 2, cy + 4, cx - 1, cy + 4, ML_WHITE);
				break;

				case 6:
					ML_line(cx + 2, cy, cx, cy, ML_WHITE);
					ML_line(cx, cy, cx, cy + 4, ML_WHITE);
					ML_line(cx, cy + 4, cx + 2, cy + 4, ML_WHITE);
					ML_line(cx + 2, cy + 4, cx + 2, cy + 2, ML_WHITE);
					ML_line(cx + 2, cy + 2, cx, cy + 2, ML_WHITE);
				break;

				case 7:
					ML_line(cx, cy, cx + 2, cy, ML_WHITE);
					ML_line(cx + 2, cy, cx + 2, cy + 5, ML_WHITE);
					ML_pixel(cx, cy + 1, ML_WHITE);
				break;

				case 8:
					ML_line(cx, cy, cx + 2, cy, ML_WHITE);
					ML_line(cx + 2, cy, cx + 2, cy + 4, ML_WHITE);
					ML_line(cx + 2, cy + 4, cx, cy + 4, ML_WHITE);
					ML_line(cx, cy + 4, cx, cy, ML_WHITE);
					ML_pixel(cx + 1, cy + 2, ML_WHITE);
				break;

				case 9:
					ML_line(cx, cy, cx + 2, cy, ML_WHITE);
					ML_line(cx + 2, cy, cx + 2, cy + 4, ML_WHITE);
					ML_line(cx + 2, cy + 4, cx, cy + 4, ML_WHITE);
					ML_line(cx + 2, cy + 2, cx, cy + 2, ML_WHITE);
					ML_line(cx, cy + 2, cx, cy, ML_WHITE);
					ML_pixel(cx, cy + 4, ML_WHITE);
				break;

				default:
					return;
				break;
			}
		}
	}
}

void MapDisplayManager::loadMap(PointsManager points){
	for (size_t i = 0; i < 5; ++i){
		for (size_t j = 0; j < 6; ++j){
			if (map[i][j] != 0){
				drawLine(i, j);
				hexagon(i, j, (PlayerType)(map[i][j] - 1));
			}
		}
	}

	for (size_t i = 0; i < 5; ++i){
		for (size_t j = 0; j < 6; ++j){
			if (points.numberAt(i, j) != 0){
				if (map[i][j] == 1)
					printDigit(i, j, points.numberAt(i, j), PLAYER_1);
				else if (map[i][j] == 2)
					printDigit(i, j, points.numberAt(i, j), PLAYER_2);
			}
		}
	}
}

void MapDisplayManager::changeOwner(size_t line, size_t column, PlayerType owner){
	if (line < 5 && column < 6){
		map[line][column] = (int)(owner) + 1;
		currentSelection.selectionType = owner;
	}
}

size_t MapDisplayManager::getReinforcementPoints(PlayerType player){
	size_t tmp = 0;

	for (size_t i = 0; i < 5; ++i){
		for (size_t j = 0; j < 6; ++j){
			if ((PlayerType)(map[i][j] - 1) == player)
				++tmp;
			else if (i == currentSelection.line && j == currentSelection.column && currentSelection.selectionType == player)
				++tmp;
		}
	}

	return tmp;
}