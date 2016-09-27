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

#include "Sources/getTicks.h"

int SysCallCode[3] = {0xD201422B,0x60F20000,0x80010070}; // déclaration en global des adresses
int (*SysCall)(int R4, int R5, int R6, int R7, int FNo) = (void*)&SysCallCode; // et des registres
// à ne placer dans votre code qu'une seule fois

int RTC_getTicks(void){
     return (*SysCall)(0, 0, 0, 0, 0x3B); // on déclare la fonction voulue selon son numéro (ici 0x3B)
}

// on peut répéter l'opération avec d'autres syscalls (ceci est un exemple sûrement non fonctionnel) :
int SYS_call(void){
     return (*SysCall)(0, 0, 0, 0, 0x42);
}