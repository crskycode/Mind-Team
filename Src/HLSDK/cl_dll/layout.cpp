#include "cl_dll.h"

class CLayout
{
public:
	int Alloc(void);

	void SetInTime(int layout, float time);
	void SetHoldTime(int layout, float time);
	void SetOutTime(int layout, float time);
	void Redraw(int layout);
};