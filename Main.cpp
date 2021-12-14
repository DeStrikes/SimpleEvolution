#include <iostream>
#include <string>
#include "Field.h"
#include "Object.h"
using namespace std;

// Preset strings
string debug_prefix = "[DEBUG]\t";
string info_prefix = "[INFO]\t";

// Settings
float safeZoneC = 0.5;
float mediumZoneC = 0.3;
float dangerZoneC = 0.2;

float basicSafeZoneC = 0.5;
float basicMediumZoneC = 0.3;
float basicDangerZoneC = 0.2;

// Chances to generate food in cell for zones ( 0 -> 1 )
float safeFoodChance = 0.05;
float mediumFoodChance = 0.1;
float dangerFoodChance = 0.2;

// Chance to generate entity in cell;
float entityGenerateChance = 0.01;

const int fieldWidth = 100;
const int fieldHeight = 100;

FieldCell fieldZones[fieldHeight + 1][fieldWidth + 1];	// Basic field with zones
ObjectType fieldFood[fieldHeight + 1][fieldWidth + 1];	// Objects
Entity fieldEntity[fieldHeight + 1][fieldWidth + 1];	// Entities

int randInt(int low, int max) {
	return rand() % max + low;
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
			auto type = x.getType();
			if (type == CellType::SAFE) {
				int c = safeFoodChance * 100;
				int rnd = randInt(1, 100);
				if (rnd <= c)
					fieldFood[i][j] = ObjectType::FOOD;
				else
					fieldFood[i][j] = ObjectType::NONE;
			}
			else if (type == CellType::MEDIUM) {
				int c = mediumFoodChance * 100;
				int rnd = randInt(1, 100);
				if (rnd <= c)
					fieldFood[i][j] = ObjectType::FOOD;
				else
					fieldFood[i][j] = ObjectType::NONE;
			}
			else {
				int c = dangerFoodChance * 100;
				int rnd = randInt(1, 100);
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
			if (fieldZones[i][j].getType() != CellType::SAFE) {
				fieldEntity[i][j] = { ObjectType::NONE, 0, 0, 0, j, i };
				continue;
			}
			if (fieldFood[i][j] == ObjectType::FOOD) {
				fieldEntity[i][j] = { ObjectType::NONE, 0, 0, 0, j, i };
				continue;
			}		
			int c = entityGenerateChance * 100;
			int rnd = randInt(1, 100);
			if (rnd <= c) {
				fieldEntity[i][j] = { ObjectType::CELL, 1, 1, 1, j, i };
				entityC++;
			}
			else
				fieldEntity[i][j] = { ObjectType::NONE, 0, 0, 0, j, i };
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
			if (fieldZones[i][j].getType() != fieldZones[i][j - 1].getType())
				cout << "|";
			auto x = fieldEntity[i][j];
			auto type = x.getType();
			if (fieldFood[i][j] == ObjectType::FOOD)
				cout << "F";
			else {
				if (type == ObjectType::NONE)
					cout << " ";
				else
					cout << "@";
			}
		}
		cout << "\n";
	}
	cout << info_prefix << "Entities: " << entityC;
}

// Main
int main() {
	init();
	printFields();
}