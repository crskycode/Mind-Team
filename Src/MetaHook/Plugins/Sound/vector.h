
#ifndef VECTOR_H
#define VECTOR_H

#define VectorCopy(a, b) \
	{ \
		(b)[0] = (a)[0]; \
		(b)[1] = (a)[1]; \
		(b)[2] = (a)[2]; \
	}

#define VectorCopy_HL2AL(a, b) \
	{ \
		(b)[0] = -(a)[1]; \
		(b)[1] =  (a)[2]; \
		(b)[2] = -(a)[0]; \
	}

#endif