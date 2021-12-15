#pragma once
class Skill {
public:
	// Skills
	float speed;	// Speed of entity
	float armor;	// Default is 1.0, if armor: 2.0 - entity can go in medium zone, 3.0 - entity can go in danger zone
	float genEnergy;	// Energy entity can use per generation
	int vision;	// Radius of finding food

	// ENERGY FORMULA PER MOVE: length * extra / speed
	float extra;	// Use in calculation
	float usedEnergy;	// Energy that entity already used
};

