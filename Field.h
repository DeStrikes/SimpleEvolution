enum class CellType {
	SAFE,
	MEDIUM,
	DANGER
};

class FieldCell {
public:
	CellType type;
	int x;
	int y;

	int getX() {
		return x;
	}

	int getY() {
		return y;
	}

	CellType getType() {
		return type;
	}
};