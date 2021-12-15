#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include "Field.h"
#include "Object.h"
#include "Vec.h"
#include "DefaultSettings.h"
#include "Skill.h"
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

float foodSaturation = 2.0;		// Energy that food can give

// Chance to generate entity in cell;
float entityGenerateChance = 0.05;
CellType entityGenerationCell = CellType::SAFE;

// Field size (more than 10)
const int fieldWidth = 50;
const int fieldHeight = 50;

FieldCell fieldZones[fieldHeight + 1][fieldWidth + 1];	// Basic field with zones
ObjectType fieldFood[fieldHeight + 1][fieldWidth + 1];	// Objects
Entity fieldEntity[fieldHeight + 1][fieldWidth + 1];	// Entities
vector <Entity> allEntitiesList;

// Skills values on entity creation
// What each skill does you can see in 'Skills.h'
float eSpeed = 1.0;
float eArmor = 1.0;
float eEnergy = 10;
int eVision = 7;
float eExtra = 1.0;

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

// Returns true with chance
bool makeChance(float chance) {
	int chanceRnd = chance * 100;
	int rnd = random(1, 100);
	if (rnd <= chanceRnd)
		return true;
	return false;
}

int foodCount = 0;	// Count of food
// Creating food layer
int generateFoodFieldLayer() {
	for (int i = 1; i < fieldHeight + 1; i++) {
		for (int j = 1; j < fieldWidth + 1; j++) {
			auto x = fieldZones[i][j];
			auto type = x.type;
			if (type == CellType::SAFE) {
				if (makeChance(safeFoodChance)) {
					fieldFood[i][j] = ObjectType::FOOD;
					foodCount++;
				}
				else
					fieldFood[i][j] = ObjectType::NONE;
			}
			else if (type == CellType::MEDIUM) {
				if (makeChance(mediumFoodChance)) {
					fieldFood[i][j] = ObjectType::FOOD;
					foodCount++;
				}
				else
					fieldFood[i][j] = ObjectType::NONE;
			}
			else {
				if (makeChance(dangerFoodChance)) {
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

Skill basicEntitySkill = { eSpeed, eArmor, eEnergy, 5, 1.0, 0.0 };
Skill basicNoneTypeSkill = { 0.0, 0.0, 0.0, 0, 0.0, 0.0 };
int entityC = 0;	// Entity counter
// Crating entity layer
int generateEntityFieldLayer() {
	for (int i = 1; i < fieldHeight + 1; i++) {
		for (int j = 1; j < fieldWidth + 1; j++) {
			if (fieldZones[i][j].type != entityGenerationCell) {
				fieldEntity[i][j] = { ObjectType::NONE, {j, i}, basicNoneTypeSkill, 0 };
				continue;
			}
			if (fieldFood[i][j] == ObjectType::FOOD) {
				fieldEntity[i][j] = { ObjectType::NONE, {j, i}, basicNoneTypeSkill, 0 };
				continue;
			}
			if (makeChance(entityGenerateChance)) {
				fieldEntity[i][j] = { ObjectType::CELL, {j, i}, basicEntitySkill, 0 };
				allEntitiesList.push_back(fieldEntity[i][j]);
				entityC++;
			}
			else
				fieldEntity[i][j] = { ObjectType::NONE, {j, i}, basicNoneTypeSkill, 0 };
		}
	}
	if (entityC == 0) {
		cout << error_prefix << "Can't create entitites";
		return -1;
	}
	return 1;
}

// Print food field and entity layer on one layer with zone borders
void printFields(bool isBordered) {
	for (int i = 1; i < fieldHeight + 1; i++) {
		if(isBordered)
			for (int i = 0; i < (fieldWidth + 1)*2; i++)
				cout << '-';
		cout << '\n';
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
			if (isBordered)
				cout << "|";
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

float generateEnergyCons(Entity* entity, float length) {
	return length * entity->skill.extra / entity->skill.speed;
}

Vec findNearestFood(Vec pos, int rad, Entity* entity) {
	Vec pos1 = checkCords({ pos.x - rad, pos.y - rad });
	Vec pos2 = checkCords({ pos.x + rad, pos.y + rad });
	float min = rad * rad;
	Vec minV = { -1, -1 };
	for (int i = pos1.y; i < pos2.y; i++) {
		for (int j = pos1.x; j < pos2.x; j++) {
			int x = pos.x, y = pos.y;
			if (fieldFood[i][j] == ObjectType::FOOD) {
				int a = abs(j - x);
				int b = abs(i - y);
				float length;
				length = sqrt(a * a + b * b);
				if (length < min) {
					min = length;
					minV = { j, i };
				}
			}
		}
	}
	entity->skill.usedEnergy += generateEnergyCons(entity, min);
	return minV;
}

void onEatFood(Vec pos, Entity* entity) {
	fieldFood[pos.y][pos.x] = ObjectType::NONE;
	entity->skill.usedEnergy -= foodSaturation;
}

void onEntityMove(Vec pos, Vec to, Entity* entity) {
	auto a = fieldEntity[pos.y][pos.x];
	fieldEntity[pos.y][pos.x] = fieldEntity[to.y][to.x];
	fieldEntity[to.y][to.x] = a;
	if (fieldFood[to.y][to.x] == ObjectType::FOOD)
		onEatFood(to, entity);
	entity->moves++;
	entity->pos.x = to.x;
	entity->pos.y = to.y;
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
	printFields(true);
}