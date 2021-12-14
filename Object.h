enum class ObjectType {
	FOOD,
	CELL,
	NONE
};

class Entity {
public:
	ObjectType type;
	int speed;
	int health;
	int food;
	int x;
	int y;

	int getX() {
		return x;
	}
	int getY() {
		return y;
	}
	int getSpeed() {
		return speed;
	}
	int getHealth() {
		return health;
	}
	int getFood() {
		return food;
	}
	ObjectType getType() {
		return type;
	}

	void setX(int n) {
		x = n;
	}
	void setY(int n) {
		y = n;
	}
	void setSpeed(int n) {
		speed = n;
	}
	void setHealth(int n) {
		health = n;
	}
	void setFood(int n) {
		food = n;
	}
};