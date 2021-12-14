enum class ObjectType {
	FOOD,
	CELL,
	NONE
};

class Entity {
public:
	ObjectType type;
	Vec pos;
	float speed;
	float health;
	float bEnergy;	// Energy you can spend on generation
	float energy;	// Energy spent on generation
	float extra;
	int moves;
};