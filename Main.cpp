#include <iostream>
#include <string>
#include <cmath>
#include "Field.h"
#include "Object.h"
#include "Vec.h"
#include "DefaultSettings.h"
using namespace std;
typedef long long ll;

// Prefixes for output
string debug_prefix = "[DEBUG]\t";
string info_prefix = "[INFO]\t";
string warn_prefix = "[WARNING]\t";
string error_prefix = "[ERROR]\t";

// Settings	(all values are 0 -> 1)
float safeZoneC = 0.5;
float mediumZoneC = 0.3;
float dangerZoneC = 0.2;

ll generations = 100;		// Generations count

// Chances to generate food in cell for zones 
float safeFoodChance = 0.2;
float mediumFoodChance = 0.3;
float dangerFoodChance = 0.35;

// Chance to generate entity in cell;
float entityGenerateChance = 0.05;

// Field size (more than 10)
const int fieldWidth = 50;
const int fieldHeight = 50;

FieldCell fieldZones[fieldHeight + 1][fieldWidth + 1];	// Basic field with zones
ObjectType fieldFood[fieldHeight + 1][fieldWidth + 1];	// Objects
Entity fieldEntity[fieldHeight + 1][fieldWidth + 1];	// Entities

// Random function
int random(int min, int max)
{
	static bool flag;
	if (!flag)
	{
		srand(time(NULL));
		flag = true;
	}
	return min + rand() % (max - min);
}

// Check all settings and change values
void checkSettings() {
	if (safeZoneC + mediumZoneC + dangerZoneC != 1.0) {
		safeZoneC = defaultSafeZoneC;
		mediumZoneC = defaultMediumZoneC;
		dangerZoneC = defaultDangerZoneC;
		cout << warn_prefix << "All zones coefficients set to default." << endl;
	}
	if (generations < 1) {
		generations = defaultGenerations;
		cout << warn_prefix << "Generations value set to default." << endl;
	}
}

// Field layers generators

// Creating zones layer
ll safeCount = 0;
ll mediumCount = 0;
ll dangerCount = 0;
int generateZoneFieldLayer() {
	int mediumZoneStart = fieldWidth * safeZoneC + 1;
	int dangerZoneStart = fieldWidth * (safeZoneC + mediumZoneC);
	for (int i = 1; i < fieldHeight + 1; i++) {
		for (int j = 1; j < fieldWidth + 1; j++) {
			if (j < mediumZoneStart) {
				fieldZones[i][j] = { CellType::SAFE, j, i };
				safeCount++;
			}
			else if (j >= dangerZoneStart) {
				fieldZones[i][j] = { CellType::DANGER, j, i };
				mediumCount++;
			}
			else {
				fieldZones[i][j] = { CellType::MEDIUM, j, i };
				dangerCount++;
			}
		}
	}
	if (dangerCount == 0 && mediumCount == 0 && safeCount == 0) {
		cout << error_prefix << "Can't create zones!";
		return -1;
	}
	return 1;
}

int foodCount = 0;	// Count of food
// Creating food layer
int generateFoodFieldLayer() {
	for (int i = 1; i < fieldHeight + 1; i++) {
		for (int j = 1; j < fieldWidth + 1; j++) {
			auto x = fieldZones[i][j];
			auto type = x.type;
			if (type == CellType::SAFE) {
				int c = safeFoodChance * 100;
				int rnd = random(1, 100);
				if (rnd <= c) {
					fieldFood[i][j] = ObjectType::FOOD;
					foodCount++;
				}
				else
					fieldFood[i][j] = ObjectType::NONE;
			}
			else if (type == CellType::MEDIUM) {
				int c = mediumFoodChance * 100;
				int rnd = random(1, 100);
				if (rnd <= c) {
					fieldFood[i][j] = ObjectType::FOOD;
					foodCount++;
				}
				else
					fieldFood[i][j] = ObjectType::NONE;
			}
			else {
				int c = dangerFoodChance * 100;
				int rnd = random(1, 100);
				if (rnd <= c) {
					fieldFood[i][j] = ObjectType::FOOD;
					foodCount++;
				}
				else
					fieldFood[i][j] = ObjectType::NONE;
			}
		}
	}
	if (foodCount == 0) {
		cout << error_prefix << "Can't create food";
		return -1;
	}
	return 1;
}

int entityC = 0;	// Entity counter
// Crating entity layer
int generateEntityFieldLayer() {
	for (int i = 1; i < fieldHeight + 1; i++) {
		for (int j = 1; j < fieldWidth + 1; j++) {
			if (fieldZones[i][j].type != CellType::SAFE) {
				fieldEntity[i][j] = { ObjectType::NONE, {j, i}, 0.0, 0.0, 0.0, 0.0, 0.0, 0};
				continue;
			}
			if (fieldFood[i][j] == ObjectType::FOOD) {
				fieldEntity[i][j] = { ObjectType::NONE, {j, i}, 0.0, 0.0, 0.0, 0.0, 0.0, 0 };
				continue;
			}		
			int c = entityGenerateChance * 100;
			int rnd = random(1, 100);
			if (rnd <= c) {
				fieldEntity[i][j] = { ObjectType::CELL, {j, i}, 1.0, 1.0, 10.0, 1.0, 0.0, 0 };
				entityC++;
			}
			else
				fieldEntity[i][j] = { ObjectType::NONE, {j, i}, 0.0, 0.0, 0.0, 0.0, 0.0, 0 };
		}
	}
	if (entityC == 0) {
		cout << error_prefix << "Can't create entitites";
		return -1;
	}
	return 1;
}

// Print food field and entity layer on one layer with zone borders
void printFields() {
	for (int i = 1; i < fieldHeight + 1; i++) {
		for (int j = 1; j < fieldWidth + 1; j++) {
			if (fieldZones[i][j].type != fieldZones[i][j - 1].type)
				cout << "|";
			auto x = fieldEntity[i][j];
			auto type = x.type;
			if (fieldFood[i][j] == ObjectType::FOOD)
				cout << "*";
			else {
				if (type == ObjectType::NONE)
					cout << " ";
				else
					cout << "@";
			}
		}
		cout << " " << i << "\n";
	}
	cout << info_prefix << "Entities: " << entityC << "\tFood: " << foodCount << 
		"\nSafe zone: " << safeCount << "\t Medium zone: " << mediumCount << 
		"  Danger zone: " << dangerCount << endl;
}

// Checking coordinates and returning edited value
Vec checkCords(Vec pos) {
	if (pos.x < 1)
		pos.x = 1;
	if (pos.y < 1)
		pos.y = 1;
	if (pos.x > fieldWidth)
		pos.x = fieldWidth + 1;
	if (pos.y > fieldHeight)
		pos.y = fieldHeight + 1;
	return pos;
}

// Find nearest food from coordinates
Vec findNearestFood(Vec pos, int rad) {
	Vec pos1 = checkCords({ pos.x - rad, pos.y - rad });
	Vec pos2 = checkCords({ pos.x + rad, pos.y + rad });
	float min = rad*rad;
	Vec minV = {-1, -1};
	for (int i = pos1.y; i < pos2.y; i++) {
		for (int j = pos1.x; j < pos2.x; j++) {
			int x = pos.x, y = pos.y;
			if (fieldFood[i][j] == ObjectType::FOOD) {
				float length;
				int a = abs(j - x);
				int b = abs(i - y);
				length = sqrt(a*a + b*b);
				if (length < min) {
					min = length;
					minV = { j, i };
				}
			}
		}
	}
	return minV;
}

// Initialization function
int init() {
	checkSettings();
	if (generateZoneFieldLayer() == -1)
		return -1;
	if (generateFoodFieldLayer() == -1)
		return -1;
	if (generateEntityFieldLayer() == -1)
		return -1;
	return 1;
}

// Main
int main() {
	if (init() == -1) {		// If input values contain errors
		cout << "\nProgramm stopped!";
		return 0;
	}
	printFields();
}