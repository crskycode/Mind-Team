
#include <metahook.h>
#include <r_efx.h>
#include <parsemsg.h>
#include <list>
#include "qgl.h"

struct Line
{
	vec3_t start;
	vec3_t end;
	int r, g, b;
	float life;
	float time;
};

typedef std::list<Line *> TLineList;

TLineList TheLineList;


extern "C"
{
	__declspec(dllexport) void DebugLine(float *start, float *end, int r, int g, int b, float life);
}

static int s_beam = 0;

void DebugLine(float *start, float *end, int r, int g, int b, float life)
{
	/*if ( s_beam == 0 )
	{
		gEngfuncs.CL_LoadModel( "sprites/smoke.spr", &s_beam );
	}
	if ( s_beam != -1 )
	{
		gEngfuncs.pEfxAPI->R_BeamPoints( start, end, s_beam, life, 1, 0, 1.0f, 0, 0, 0, r/255.0, g/255.0, b/255.0 );
	}*/

	Line *l = new Line;
	l->start[0] = start[0];
	l->start[1] = start[1];
	l->start[2] = start[2];
	l->end[0] = end[0];
	l->end[1] = end[1];
	l->end[2] = end[2];
	l->r = r;
	l->g = g;
	l->b = b;
	l->life = life;
	l->time = gEngfuncs.GetClientTime();

	TheLineList.push_back( l );
}

void DebugLine_Redraw(void)
{
	if ( TheLineList.empty() )
		return;

	TLineList::iterator it = TheLineList.begin();

//	qglDisable(GL_DEPTH_TEST);
	qglDisable(GL_TEXTURE_2D);
	qglEnable(GL_BLEND);
	qglLineWidth(1);
	qglBegin(GL_LINES);

	while ( it != TheLineList.end() )
	{
		Line *l = *it;

		float left = gEngfuncs.GetClientTime() - l->time;

		if (left > l->life)
		{
			delete *it;
			it = TheLineList.erase(it);
		}
		else
		{
			/*int blend = (l->life - left) / l->life * 255.0;
			if (blend < 0)
				blend = 0;
			if (blend > 255)
				blend = 255;*/
			//do draw
			qglColor4ub(l->r, l->g, l->b, 255);
			qglVertex3fv(l->start);
			qglVertex3fv(l->end);

			it++;
		}
	}

	qglEnd();
	
	qglEnable(GL_TEXTURE_2D);
//	qglEnable(GL_BLEND);
//	qglEnable(GL_DEPTH_TEST);
}