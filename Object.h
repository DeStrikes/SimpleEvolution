#pragma once
#include "Skill.h"

enum class ObjectType {
	FOOD,
	CELL,
	NONE
};

class Entity {
public:
	ObjectType type;
	Vec pos;	
	Skill skill;
	int moves;
};