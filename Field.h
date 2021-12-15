#include "Vec.h"

// Class for field cells

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
};