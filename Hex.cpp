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

//TODO : IMPORTANT : Upgrade the random map generation by adding a pathfinding algo to detect isolated cells

#include "Sources/MapDisplayManager.h"

extern "C" {
    #include "Sources/getTicks.h"
    #include <stdlib.h>
    #include <math.h>
    #include <string.h>
};

enum Neighbour {UP_R = 0, R, BOTTOM_R, BOTTOM_L, L, UP_L};

MapDisplayManager manager;
PointsManager points;

PlayerType turn;
bool reinforcement;

int timeRefCurrent;
int timeRefLocked;

size_t rPointsPlayer1;
size_t rPointsPlayer2;

unsigned char* toArray(int number){
    int n = log10(number) + 1;
    int i;

    unsigned char *numberArray = (unsigned char*)(calloc(n, sizeof(unsigned char)));

    for (i = 0; i < n; ++i, number /= 10){
        numberArray[i] = number % 10;
    }

    if (n > 1){
        unsigned char *tmp = new unsigned char;
        
        for (i = 0; i < n; ++i)
            tmp[i] = numberArray[i];

        for (i = 0; i < n; ++i)
            numberArray[i] = tmp[n - i - 1];

        delete tmp;
    }

    return numberArray;
}

void printReinforcementPoints(){
    unsigned char *asciiNumber = new unsigned char;

    if (turn == PLAYER_1){
        if (rPointsPlayer1 != 0) 
            asciiNumber = toArray(rPointsPlayer1);
        else
            asciiNumber[0] = 0;
        
        for (size_t i = 0; i < (log10(rPointsPlayer1) + 1); ++i){
            asciiNumber[i] += 48;
        }

        asciiNumber[(int)(log10(rPointsPlayer1) + 1)] = '\0';
    } else {
        if (rPointsPlayer2 != 0)
            asciiNumber = toArray(rPointsPlayer2);
        else
            asciiNumber[0] = 0;

        for (size_t i = 0; i < (log10(rPointsPlayer2) + 1); ++i){
            asciiNumber[i] += 48;
        }

        asciiNumber[(int)(log10(rPointsPlayer2) + 1)] = '\0';
    }

    PrintMini(0, 15, asciiNumber, MINI_OR);

    delete asciiNumber;
}

void printTurn(){
    if (turn == PLAYER_1)
        ML_rectangle(0, 40, 6, 46, 1, ML_BLACK, ML_WHITE);
    else
        ML_rectangle(0, 40, 6, 46, 1, ML_BLACK, ML_BLACK);
}

void updateDisplay(){
    int timeAt = RTC_getTicks();

    if (timeAt - timeRefCurrent > 15){
        manager.printCurrentSelection();
        timeRefCurrent = timeAt;
    }

    if (timeAt - timeRefLocked > 75){
        manager.printLockedSelection();
        timeRefLocked = timeAt;
    }

    ML_clear_vram();
        if (reinforcement)
            printReinforcementPoints();
        printTurn();
        manager.loadMap(points);
    ML_display_vram();
}

int randomInt(unsigned int rangeMin, unsigned int rangeMax){
    return rand() % (rangeMax + 1 - rangeMin) + rangeMin;
}

bool attackWin(int source, int target){
    int offset = source - target;

    if (offset > 1){
        return true;
    } else if (offset == 1){
        return randomInt(0, 100) >= 25;
    } else if (offset == 0){
        return randomInt(0, 100) >= 50;
    } else if (offset == -1){
        return randomInt(0, 100) >= 75;
    } else {
        return false;
    }
}

void applyDamageFrom(PlayerType attacker){
    int targetLine = manager.getCurrentLine();
    int targetColumn = manager.getCurrentColumn();

    int sourceLine = manager.getLockedLine();
    int sourceColumn = manager.getLockedColumn();

    int offset = points.numberAt(sourceLine, sourceColumn) - points.numberAt(targetLine, targetColumn);

    if (points.numberAt(targetLine, targetColumn) == 0){
        points.setNumberAt(targetLine, targetColumn, points.numberAt(sourceLine, sourceColumn) - 1);
        manager.changeOwner(targetLine, targetColumn, attacker);
    } else if (attackWin(points.numberAt(sourceLine, sourceColumn), points.numberAt(targetLine, targetColumn))){
        if (offset > 1)
            points.setNumberAt(targetLine, targetColumn, points.numberAt(sourceLine, sourceColumn) - points.numberAt(targetLine, targetColumn));
        else
            points.setNumberAt(targetLine, targetColumn, 1);

        manager.changeOwner(targetLine, targetColumn, attacker);
    } else {
        if (offset < -1)
            points.setNumberAt(targetLine, targetColumn, points.numberAt(targetLine, targetColumn) - points.numberAt(sourceLine, sourceColumn));
        else
            points.setNumberAt(targetLine, targetColumn, 1);
    }

    points.setNumberAt(sourceLine, sourceColumn, 1);
}

void neighbourArray(int i, int j, int map[][6], int array[]){
	if (i%2 == 0){
		if (i-1 >= 0){
			array[UP_R] = (map[i-1][j] != 0);
			if (j-1 >= 0)
				array[UP_L] = (map[i-1][j-1] != 0);
		}

		if (i+1 < 5){
			array[BOTTOM_R] = (map[i+1][j] != 0);
			if (j-1 >= 0)
				array[BOTTOM_L] = (map[i+1][j-1] != 0);
		}

		if (j+1 < 6)
			array[R] = (map[i][j+1] != 0);
	} else {
		if (i-1 >= 0){
			array[UP_L] = (map[i-1][j] != 0);
			if (j+1 < 5)
				array[UP_R] = (map[i-1][j+1] != 0);
		}

		if (i+1 < 5){
			array[BOTTOM_L] = (map[i+1][j] != 0);
			if (j+1 < 5)
				array[BOTTOM_R] = (map[i+1][j+1] != 0);
		}

		if (j+1 < 5)
			array[R] = (map[i][j+1] != 0);
	}

	if (j-1 >= 0)
		array[L] = (map[i][j-1] != 0);
}

void randomMap(int map[][6]){
	for (size_t i = 0; i < 5; ++i){
		for (size_t j = 0; j < 6; ++j){
			unsigned int num = randomInt(0, 2);

			if (i%2 != 0 && j == 5)
				num = 0;

			map[i][j] = (num != 0) * 3;
		}
	}

	map[0][0] = 1;
	map[0][1] = 3;
	map[4][4] = 3;
    map[4][5] = 2;

	int array[6];

	for (int i = 0; i < 5; ++i){
		for (int j = 0; j < 6; ++j){
			if (!(i == 0 && j == 0) && !(i == 4 && j == 5) && map[i][j] != 0){
				for (unsigned int k = 0; k < 6; ++k)
					array[k] = 0;
				neighbourArray(i, j, map, array);

				unsigned int count = 0;

				for (unsigned int k = 0; k < 6; ++k){
					if (array[k] != 0)
						count++;
				}

				if (count < 2){
					for (unsigned int k = 0; k < 6; ++k){
						if (array[k] == 0){
							switch (k) {
								case UP_R:
									if (i - 1 >= 0){
										if (i%2 == 0){
											if (j < 5){
												map[i-1][j] = 3;
												++count;
											}
										} else {
											map[i-1][j+1] = 3;
											++count;
										}
									}
								break;

								case R:
									if (j + 1 < 6 - (i%2 != 0)){
										map[i][j+1] = 3;
										++count;
									}
								break;

								case BOTTOM_R:
									if (i + 1 < 5){
										if (i%2 == 0){
											if (j < 5){
												map[i+1][j] = 3;
												++count;
											}
										} else {
											if (j + 1 < 5){
												map[i+1][j+1] = 3;
												++count;
											}
										}
									}
								break;

								case BOTTOM_L:
									if (i + 1 < 5){
										if (i%2 == 0){
											if (j - 1 >= 0){
												map[i+1][j-1] = 3;
												++count;
											}
										} else {
											map[i+1][j] = 3;
											++count;
										}
									}
								break;

								case L:
									if (j - 1 >= 0){
										map[i][j-1] = 3;
										++count;
									}
								break;

								case UP_L:
									if (i - 1 >= 0){
										if (i%2 == 0){
											if (j - 1 >= 0){
												map[i-1][j-1] = 3;
												++count;
											}
										} else {
											map[i-1][j] = 3;
											++count;
										}
									}
								break;

								default:
								break;
							}
						}

						if (count == 2)
							break;
					}
				}
			}
		}
	}
}

int main(){
    srand(RTC_getTicks());

    int map[5][6];

    bool selectionMade = false;

    while (!IsKeyDown(KEY_CTRL_EXE)){
    	ML_clear_vram();
    		if (!selectionMade){
    			PrintMini(0, 0, "[0] = Random map", MINI_OR);
    			PrintMini(0, 10, "[1] = Pre-generated map", MINI_OR);
    		} else {
    			PrintMini(0, 0, "Press [EXE] to start", MINI_OR);
    		}
    	ML_display_vram();

    	unsigned int key = 0;

        GetKey(&key);

        if (!selectionMade){
			if (key == KEY_CHAR_0){
	        	randomMap(map);
	    	} else if (key == KEY_CHAR_1){
	    		map[0][0] = 3;
			    map[0][1] = 3;
			    map[0][2] = 3;
			    map[0][3] = 3;
			    map[0][4] = 1;
			    map[0][5] = 3;
			    //
			    map[1][0] = 0;
			    map[1][1] = 3;
			    map[1][2] = 3;
			    map[1][3] = 3;
			    map[1][4] = 0;
			    map[1][5] = 0;
			    //
			    map[2][0] = 0;
			    map[2][1] = 3;
			    map[2][2] = 3;
			    map[2][3] = 3;
			    map[2][4] = 3;
			    map[2][5] = 0;
			    //
			    map[3][0] = 0;
			    map[3][1] = 3;
			    map[3][2] = 3;
			    map[3][3] = 3;
			    map[3][4] = 0;
			    map[3][5] = 0;
			    //
			    map[4][0] = 3;
			    map[4][1] = 2;
			    map[4][2] = 3;
			    map[4][3] = 3;
			    map[4][4] = 3;
			    map[4][5] = 3;
	    	}

	    	selectionMade = true;
	    }
    }

    manager = MapDisplayManager(map);
    points = PointsManager(map);
    
    turn = PLAYER_1;
    reinforcement = false;
    
    timeRefCurrent = RTC_getTicks();
    timeRefLocked = RTC_getTicks();

    rPointsPlayer1 = 0;
    rPointsPlayer2 = 0;
    
    SetTimer(1, 25, updateDisplay);

    bool locked = false;

    while (!IsKeyDown(KEY_CTRL_EXIT)){

        unsigned int key = 0;

        GetKey(&key);

        switch (key){
            case KEY_CTRL_UP:
                manager.changeCurrentSelection(TOP);
            break;

            case KEY_CTRL_DOWN:
                manager.changeCurrentSelection(BOTTOM);
            break;

            case KEY_CTRL_RIGHT:
                manager.changeCurrentSelection(RIGHT);
            break;

            case KEY_CTRL_LEFT:
                manager.changeCurrentSelection(LEFT);
            break;

            case KEY_CTRL_EXE:
                if (turn == PLAYER_1){
                    if (!reinforcement){
                        if (!locked){
                            if (manager.playerAtCurrentSelection() == PLAYER_1 && points.numberAt(manager.getCurrentLine(), manager.getCurrentColumn()) > 1){
                                manager.setLockedToCurrent();
                                locked = true;
                            }
                        } else {
                        	if (manager.getCurrentLine() == manager.getLockedLine() && manager.getCurrentColumn() == manager.getLockedColumn()){
                        		manager.resetLockedSelection();
                            	locked = false;
                            }
							
							if (manager.playerAtCurrentSelection() != PLAYER_1 && (manager.getCurrentLine() != manager.getLockedLine() || manager.getCurrentColumn() != manager.getLockedColumn())){
								applyDamageFrom(PLAYER_1);
								manager.resetLockedSelection();
                            	locked = false;
							}
                        }
                    } else {
                        if (manager.playerAtCurrentSelection() == PLAYER_1 && rPointsPlayer1 != 0 && points.numberAt(manager.getCurrentLine(), manager.getCurrentColumn()) < 8){
                            points.setNumberAt(manager.getCurrentLine(), manager.getCurrentColumn(), points.numberAt(manager.getCurrentLine(), manager.getCurrentColumn()) + 1);
                            --rPointsPlayer1;
                        }
                    }
                } else {
                    if (!reinforcement){
                        if (!locked){
                            if (manager.playerAtCurrentSelection() == PLAYER_2 && points.numberAt(manager.getCurrentLine(), manager.getCurrentColumn()) > 1){
                                manager.setLockedToCurrent();
                                locked = true;
                            }
                        } else {
                        	if (manager.getCurrentLine() == manager.getLockedLine() && manager.getCurrentColumn() == manager.getLockedColumn()){
                        		manager.resetLockedSelection();
                            	locked = false;
                            }
							
							if (manager.playerAtCurrentSelection() != PLAYER_2 && (manager.getCurrentLine() != manager.getLockedLine() || manager.getCurrentColumn() != manager.getLockedColumn())){
								applyDamageFrom(PLAYER_2);
								manager.resetLockedSelection();
                            	locked = false;
							}
                        }
                    } else {
                        if (manager.playerAtCurrentSelection() == PLAYER_2 && rPointsPlayer2 != 0 && points.numberAt(manager.getCurrentLine(), manager.getCurrentColumn()) < 8){
                            points.setNumberAt(manager.getCurrentLine(), manager.getCurrentColumn(), points.numberAt(manager.getCurrentLine(), manager.getCurrentColumn()) + 1);
                            --rPointsPlayer2;
                        }
                    }
                }
            break;

            case KEY_CTRL_OPTN:
                if (!locked){
                	if (turn == PLAYER_1){
	                    if (reinforcement)
	                        turn = PLAYER_2;
	                    else
	                        rPointsPlayer1 = manager.getReinforcementPoints(PLAYER_1);

	                    reinforcement = !reinforcement;
	                } else {
	                    if (reinforcement)
	                        turn = PLAYER_1;
	                    else
	                        rPointsPlayer2 = manager.getReinforcementPoints(PLAYER_2);

	                    reinforcement = !reinforcement;
	                }
	            }
            break;

            default:
            break;
        }
    }

    KillTimer(1);

    return 1;
}

extern "C" {
    int AddIn_main(int isAppli, unsigned short OptionNum){
        return main();
    }

    #pragma section _BR_Size
    unsigned long BR_Size;
    #pragma section

    #pragma section _TOP

    int InitializeSystem(int isAppli, unsigned short OptionNum){
        return INIT_ADDIN_APPLICATION(isAppli, OptionNum);
    }

    #pragma section
};