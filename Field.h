#include "Vec.h"

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
