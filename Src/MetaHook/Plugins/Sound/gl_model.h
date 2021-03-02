
#ifndef GL_MODEL_H
#define GL_MODEL_H

typedef enum
{
	mod_brush,
	mod_sprite,
	mod_alias,
	mod_studio
} modtype_t;

typedef struct model_s
{
	char		name[MAX_QPATH];
	qboolean	needload;
	modtype_t	type;
	int			numframes;
	int			synctype;
	int			flags;
	vec3_t		mins, maxs;
	//...
} model_t;

#endif