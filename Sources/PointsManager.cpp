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

#include "PointsManager.h"

PointsManager::PointsManager(int mat[][6]){
	for (size_t i = 0; i < 5; ++i){
		for (size_t j = 0; j < 6; ++j){
			if (mat[i][j] == 3 || mat[i][j] == 0)
				setNumberAt(i, j, 0);
			else if (mat[i][j] == 1)
				setNumberAt(i, j, 2);
			else
				setNumberAt(i, j, 3);
		}
	}
}

PointsManager::~PointsManager(){

}

int PointsManager::numberAt(size_t i, size_t j){
	if (i < 5 && j < 6)
		return map[i][j];
}

void PointsManager::setNumberAt(size_t i, size_t j, size_t number){
	if (number < 10 && i < 5 && j < 6)
		map[i][j] = number;
}