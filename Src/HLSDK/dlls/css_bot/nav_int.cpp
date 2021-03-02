
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "source.h"
#include "nav_mesh.h"


void NAV_Install(void)
{
	if (TheNavMesh)
	{
		delete TheNavMesh;
	}

	TheNavMesh = new CNavMesh();
}

void NAV_Uninstall(void)
{
	if (TheNavMesh)
	{
		delete TheNavMesh;
	}
}

void NAV_Load(void)
{
	if (TheNavMesh)
	{
		if (TheNavMesh->Load() != NAV_OK)
		{
			Msg( "ERROR: Navigation Mesh load failed.\n" );
		}
		Msg( "Navigation map loaded.\n" );
	}
}

void NAV_Update(void)
{
	if (TheNavMesh)
	{
		TheNavMesh->Update();
	}
}