#include <iostream>
#include <string>
#include <cmath>
#include "Field.h"
#include "Object.h"
#include "Vec.h"
using namespace std;
typedef long long ll;

// Preset strings
string debug_prefix = "[DEBUG]\t";
string info_prefix = "[INFO]\t";

// Settings
float safeZoneC = 1.0;
float mediumZoneC = 0.0;
float dangerZoneC = 0.0;

float basicSafeZoneC = 0.5;
float basicMediumZoneC = 0.3;
float basicDangerZoneC = 0.2;

// Chances to generate food in cell for zones ( 0 -> 1 )
float safeFoodChance = 0.2;
float mediumFoodChance = 0.1;
float dangerFoodChance = 0.2;

// Chance to generate entity in cell;
float entityGenerateChance = 0.06;

const int fieldWidth = 15;
const int fieldHeight = 15;

FieldCell fieldZones[fieldHeight + 1][fieldWidth + 1];	// Basic field with zones
ObjectType fieldFood[fieldHeight + 1][fieldWidth + 1];	// Objects
Entity fieldEntity[fieldHeight + 1][fieldWidth + 1];	// Entities

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

// Check all settings
void checkSettings() {
	if (safeZoneC + mediumZoneC + dangerZoneC != 1.0) {
		safeZoneC = basicSafeZoneC;
		mediumZoneC = basicMediumZoneC;
		dangerZoneC = basicDangerZoneC;
		cout << "All zones coefficients set to default." << endl;
	}
}

// Field layers generators

void generateZoneFieldLayer() {
	int mediumZoneStart = fieldWidth * safeZoneC + 1;
	int dangerZoneStart = fieldWidth * (safeZoneC + mediumZoneC);
	for (int i = 1; i < fieldHeight + 1; i++) {
		for (int j = 1; j < fieldWidth + 1; j++) {
			if (j < mediumZoneStart)
				fieldZones[i][j] = { CellType::SAFE, j, i };
			else if (j >= dangerZoneStart)
				fieldZones[i][j] = { CellType::DANGER, j, i };
			else
				fieldZones[i][j] = { CellType::MEDIUM, j, i };
		}
	}
}

void generateFoodFieldLayer() {
	for (int i = 1; i < fieldHeight + 1; i++) {
		for (int j = 1; j < fieldWidth + 1; j++) {
			auto x = fieldZones[i][j];
			auto type = x.type;
			if (type == CellType::SAFE) {
				int c = safeFoodChance * 100;
				int rnd = random(1, 100);
				if (rnd <= c)
					fieldFood[i][j] = ObjectType::FOOD;
				else
					fieldFood[i][j] = ObjectType::NONE;
			}
			else if (type == CellType::MEDIUM) {
				int c = mediumFoodChance * 100;
				int rnd = random(1, 100);
				if (rnd <= c)
					fieldFood[i][j] = ObjectType::FOOD;
				else
					fieldFood[i][j] = ObjectType::NONE;
			}
			else {
				int c = dangerFoodChance * 100;
				int rnd = random(1, 100);
				if (rnd <= c)
					fieldFood[i][j] = ObjectType::FOOD;
				else
					fieldFood[i][j] = ObjectType::NONE;
			}
		}
	}
}

int entityC = 0;	// Entity counter
void generateEntityFieldLayer() {
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
}

void init() {	// Initialization
	checkSettings();
	generateZoneFieldLayer();
	generateFoodFieldLayer();
	generateEntityFieldLayer();
}

void printFields() {	// Print food field and entity layer on one layer
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
		cout << "\n";
	}
	//cout << info_prefix << "Entities: " << entityC;
}

Vec findNearestPoint(Vec pos, ObjectType obj, int rad) {
	Vec pos1 = { pos.x - rad, pos.y - rad };
	Vec pos2 = { pos.x + rad, pos.y + rad };
	if (pos1.x < 1)
		pos1.x = 1;
	if (pos1.y < 1)
		pos1.y = 1;
	if (pos2.x > fieldWidth)
		pos2.x = fieldWidth + 1;
	if (pos2.y > fieldHeight)
		pos2.y = fieldHeight + 1;
	float min = rad*rad;
	Vec minV = pos;
	for (int i = pos1.y; i < pos2.y; i++) {
		for (int j = pos1.x; j < pos2.x; j++) {
			int x = pos.x, y = pos.y;
			if (fieldFood[i][j] == ObjectType::FOOD) {
				float length;
				if (i == y)
					length = abs(j - x);
				else if (j == x)
					length = abs(i - y);
				else
					length = sqrt(abs(j - x) + abs(i - y));
				if (length < min) {
					min = length;
					minV = { j, i };
				}
			}
		}
	}
	return minV;
}

// Main
int main() {
	init();
	printFields();
	ll generations = 100;		// Generations count
	int x = -1, y = -1;
	for (int i = 1; i < fieldHeight + 1; i++) {
		for (int j = 1; j < fieldWidth + 1; j++) {
			if (fieldEntity[i][j].type == ObjectType::CELL) {
				x = j;
				y = i;
				break;
			}
		}
		if (x != -1 && y != -1)
			break;
	}
	Vec cords = findNearestPoint({x, y}, ObjectType::FOOD, 20);
	cout << "\n" << x << " " << y << "\n";
	cout << cords.x << " " << cords.y;
}