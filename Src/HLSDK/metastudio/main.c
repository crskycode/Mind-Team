/****
 * 
 * MetaStudio is an improved MDL file format, that supports most of the features of 
 * the original MDL format. It implements skinning animation and keyframe animation.
 * Just storing little skeleton data can show very smooth animation (using quaternion 
 * for rotation interpolation).
 * This project is developed and shared by Crsky, and all explanatory rights are owned by Crsky.
 * 
 * Welcome anyone to improve this project or make suggestions to improve this project, 
 * if you can contact me, I will be very grateful to you.
 * 
 * Email : crskycode@foxmail.com
 * Date : 2017-8-29
 * 
****/

#include <stdio.h>
#include <io.h>		// _access
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "mathlib.h"
#include "scriplib.h"

#include "metastudio.h"

// conversion from 'double' to 'float'
#pragma warning( disable:4244 )
#pragma warning( disable:4305 )


#define strcpyn( a, b ) \
	{ \
		strncpy( (a), (b), sizeof( (a) ) ); \
		(a)[ sizeof( (a) ) - 1 ] = 0; \
	}


// max models in file
#define META_STUDIO_MODELS 32
// max vertices in model
#define META_STUDIO_VERTICES 40000
// max meshes in model
#define META_STUDIO_MESHES 32
// max bodyparts in file
#define META_STUDIO_BODYPARTS 32
// max triangles in mesh
#define META_STUDIO_TRIANGLES 20000
// max bones in file
#define META_STUDIO_BONES 256
// max keyframes in animation
#define META_STUDIO_KEYFRAMES 1024
// max animations in file
#define META_STUDIO_ANIMATIONS 2048
// max events in sequence
#define META_STUDIO_EVENTS 8
// max sequences in file
#define META_STUDIO_SEQUENCES 2048
// max blends(animations) in sequence
#define META_STUDIO_BLENDS 16
// max textures in file
#define META_STUDIO_TEXTURES 128
// max attachments in file
#define META_STUDIO_ATTACHMENTS 32


typedef struct
{
	char			name[64];
	int				flags;
	int				width;
	int				height;
	int				index;

} _mstudiotexture_t;

typedef struct
{
	char			name[32];
	char			path[64];
	unsigned short	index;
	int				flags;

} s_texture_t;

typedef struct
{
	vec3_t			pos;
	vec3_t			norm;
	vec2_t			uv;
	int				numbones;
	int				bone[4];
	float			weight[4];

} s_vertex_t;

typedef struct
{
	unsigned short	indices[3];

} s_triangle_t;

typedef struct
{
	char			name[32];
	int				numtris;
	s_triangle_t	*tris;
	int				texref[4];

} s_mesh_t;

typedef struct
{
	char			name[32];
	int				parent;

} s_node_t;

typedef struct
{
	vec3_t			pos;
	vec4_t			quat;

	mat4_t			local;	// transform vector from joint-space to parent joint-space
	mat4_t			global;	// transform vector from joint-space to global-space
	mat4_t			offset;	// inverse global

} s_bone_t;

typedef struct
{
	char			name[32];

	int				numbones;
	s_node_t		*node;
	s_bone_t		*skeleton;

	int				numverts;
	s_vertex_t		*vertex;

	int				nummesh;
	s_mesh_t		*mesh;

} s_model_t;

typedef struct
{
	char			name[32];

	int				nummodels;
	s_model_t		*model[ META_STUDIO_MODELS ];

	int				base;

} s_bodypart_t;

typedef struct
{
	char			name[32];

	int				flags;

	int				numbones;
	s_node_t		*node;

	int				numkeyframes;
	int				*time;				// time[numkeyframes]
	vec3_t			*pos[ META_STUDIO_BONES ];	// pos[bone][numkeyframes]
	vec4_t			*quat[ META_STUDIO_BONES ];	// quat[bone][numkeyframes]

	int				duration;	// The value from the last TIME in SMD file

} s_animation_t;

typedef struct
{
	int				event;
	int				frame;
	char			options[64];

} s_event_t;

typedef struct
{
	char			name[32];

	int				flags;

	float			fps;

	int				duration;

	vec3_t			bmin;
	vec3_t			bmax;

	int				numblends;
	s_animation_t	*anim[ META_STUDIO_BLENDS ];

	int				animindex;

	int				activity;
	int				actweight;

	int				numevents;
	s_event_t		event[ META_STUDIO_EVENTS ];

} s_sequence_t;

typedef struct
{
	char			name[32];
	int				parent;
	vec3_t			bmin;
	vec3_t			bmax;

} s_bonetable_t;

typedef struct
{
	char			bonename[32];
	int				bone;
	vec3_t			org;

} s_attachment_t;

typedef struct
{
	char			name[32];
	int				bone;
	int				group;
	vec3_t			bmin;
	vec3_t			bmax;

} s_hitbox_t;

typedef struct
{
	char			source[32];
	char			replace[32];

} s_replacetexture_t;

// structure
vec3_t				bbmin, bbmax;

// model
int					nummodels;
s_model_t			model[ META_STUDIO_MODELS ];
// bodypart
int					numbodyparts;
s_bodypart_t		bodypart[ META_STUDIO_BODYPARTS ];
// texture
int					numtextures;
s_texture_t			texture[ META_STUDIO_TEXTURES ];
// animation
int					numani;
s_animation_t		animation[ META_STUDIO_ANIMATIONS ];
// sequence
int					numseq;
s_sequence_t		sequence[ META_STUDIO_SEQUENCES ];
// bone
int					numbones;
s_bonetable_t		bonetable[ META_STUDIO_BONES ];
// attachment
int					numattachments;
s_attachment_t		attachment[ META_STUDIO_ATTACHMENTS ];
// hitbox
int					numhitboxes;
s_hitbox_t			hitbox[ META_STUDIO_BONES ];
// replace texture
int					numreplacetexture;
s_replacetexture_t	replacetexture[ 32 ];

// adjust vertex and skeleton position
mat4_t				adjustmatrix;
// rotate models (just animated skeleton)
mat4_t				rotatemodels;

// input file
char	filename[ 1024 ];
FILE	*input;
char	line[ 1024 ];
int		linecount;

// output file
char	outname[ 1024 ];
int		cdset;
char	cdpartial[ 256 ];
char	cddir[ 1024 ];


void *kalloc( int num, int size )
{
	return calloc( num, size );
}


/*
=================
Cmd_Modelname
=================
*/
void Cmd_Modelname( void )
{
	GetToken( false );
	strcpyn( outname, token );
}

void ClearModel( void )
{
	MatrixIdentity( adjustmatrix );

	// rotate by Z axis
	vec3_t axis = { 0.0, 0.0, 1.0 };

	// setup the matrix (90 degrees)
	MatrixSetupRotate( 1.570796, axis, rotatemodels );

	//
	// clear all the arrays
	//

	nummodels = 0;
	memset( model, 0, sizeof( model ) );

	numbodyparts = 0;
	memset( bodypart, 0, sizeof( bodypart ) );

	numtextures = 0;
	memset( texture, 0, sizeof( texture ) );

	numani = 0;
	memset( animation, 0, sizeof( animation ) );

	numseq = 0;
	memset( sequence, 0, sizeof( sequence ) );

	numbones = 0;
	memset( bonetable, 0, sizeof( bonetable ) );

	numattachments = 0;
	memset( attachment, 0, sizeof( attachment ) );

	numhitboxes = 0;
	memset( hitbox, 0, sizeof( hitbox ) );

	numreplacetexture = 0;
	memset( replacetexture, 0, sizeof( replacetexture ) );
}

int lookup_texture( char *texturename )
{
	int		i;

	for ( i = 0; i < numtextures; i++ )
	{
		if ( !stricmp( texture[i].name, texturename ) )
		{
			return i;
		}
	}

	if ( i >= META_STUDIO_TEXTURES )
	{
		Error( "too many textures" );
	}

	strcpyn( texture[i].name, texturename );
	numtextures++;

	return i;
}

s_mesh_t *lookup_mesh( s_model_t *pmodel, char *texturename )
{
	int		i, j;

/*
	j = lookup_texture( texturename );

	for ( i = 0; i < pmodel->nummesh; i++ )
	{
		if ( pmodel->mesh[i].skinref == j )
		{
			return &pmodel->mesh[i];
		}
	}
*/

	for ( i = 0; i < pmodel->nummesh; i++ )
	{
		if ( !stricmp( pmodel->mesh[i].name, texturename ) )
		{
			return &pmodel->mesh[i];
		}
	}

	if ( i >= META_STUDIO_MESHES )
	{
		Error( "too many meshes in model: '%s'", pmodel->name );
	}

	strcpyn( pmodel->mesh[i].name, texturename );
	pmodel->mesh[i].tris = kalloc( META_STUDIO_TRIANGLES, sizeof( s_triangle_t ) );
	pmodel->nummesh++;

	return &pmodel->mesh[i];
}

int lookup_vertex( s_model_t *pmodel, s_vertex_t *pv )
{
	int		i, j;

	for ( i = 0; i < pmodel->numverts; i++ )
	{
		if ( VectorCompare( pmodel->vertex[i].pos, pv->pos ) && 
			 VectorCompare( pmodel->vertex[i].norm, pv->norm ) && 
			 VectorCompare2( pmodel->vertex[i].uv, pv->uv ) )
		{
			if ( pmodel->vertex[i].numbones == pv->numbones )
			{
				for ( j = 0; j < pv->numbones; j++ )
				{
					if ( pmodel->vertex[i].bone[j] != pv->bone[j] || 
						 pmodel->vertex[i].weight[j] != pv->weight[j] )
					{
						break;
					}
				}

				if ( j == pv->numbones )
				{
					return i;
				}
			}
		}
	}

	if ( i >= META_STUDIO_VERTICES )
	{
		Error( "too many vertices in model: '%s'", pmodel->name );
	}

	memcpy( &pmodel->vertex[i], pv, sizeof( s_vertex_t ) );
	pmodel->numverts++;

	return i;
}

void _CompareNodes( s_node_t *pnodes1, int count1, s_node_t *pnodes2, int count2 )
{
	int		i;

	if ( pnodes1 == pnodes2 )
	{
		return;
	}

	if ( count1 != count2 )
	{
		Error( "nodes are not all the same" );
	}

	for ( i = 0; i < count1; i++ )
	{
		if ( ( strcmp( pnodes1[i].name, pnodes2[i].name ) != 0 ) || 
			pnodes1[i].parent != pnodes2[i].parent )
		{
			Error( "nodes are not all the same" );
		}
	}
}

void CompareNodes( s_node_t *pnodes, int count )
{
	int		i;

	for ( i = 0; i < nummodels; i++ )
	{
		_CompareNodes( model[i].node, model[i].numbones, pnodes, count );
	}

	for ( i = 0; i < numani; i++ )
	{
		_CompareNodes( animation[i].node, animation[i].numbones, pnodes, count );
	}
}

/*
=================
Grab_Nodes
=================
*/
int Grab_Nodes( s_node_t *pnodes )
{
	int		index;
	char	name[ 64 ];
	int		parent;
	int		count;

	count = 0;

	while ( fgets( line, sizeof( line ), input ) )
	{
		linecount++;

		if ( !strncmp( line, "end", 3 ) )
		{
			return count;
		}
		else if ( sscanf( line, "%d \"%[^\"]\" %d", &index, name, &parent ) == 3 )
		{
			if ( count >= META_STUDIO_BONES )
			{
				Error( "too many bones" );
			}

			strcpyn( pnodes[index].name, name );
			pnodes[index].parent = parent;
			count++;
		}
		else
		{
			Error( "%s : error at line %d", filename, linecount );
		}
	}

	Error( "%s : unexpected EOF at line %d", filename, linecount );

	return 0;
}

/*
void adjust_vertex( vec3_t pv )
{
	VectorAdd( pv, adjustmatrix[3], pv );
}
*/

/*
=================
Grab_Skeleton
=================
*/
void Grab_Skeleton( s_node_t *pnodes, s_bone_t *pbones )
{
	int		count, index;
	vec3_t	pos;
	vec4_t	rot;

	while ( fgets( line, sizeof( line ), input ) )
	{
		linecount++;

		if ( !strncmp( line, "end", 3 ) )
		{
			return;
		}
		else if ( !strncmp( line, "time", 4 ) )
		{
			continue;
		}
		else if ( ( count = sscanf( line, "%d %f %f %f %f %f %f %f", 
								&index, 
								&pos[0], 
								&pos[1], 
								&pos[2], 
								&rot[0], 
								&rot[1], 
								&rot[2], 
								&rot[3] ) ) >= 7 )
		{
			/*if ( pnodes[index].parent == -1 )
			{
				adjust_vertex( pos );
			}*/

			VectorCopy( pos, pbones[index].pos );

			if ( count == 7 )
			{
				GL_AngleQuaternion( rot, pbones[index].quat );
			}
			else if ( count == 8 )
			{
				VectorCopy4( rot, pbones[index].quat );
			}
		}
		else
		{
			Error( "%s : error at line %d", filename, linecount );
		}
	}

	Error( "%s : unexpected EOF at line %d", filename, linecount );
}

/*
=================
Build_Reference
=================
*/
void Build_Reference( s_model_t *pmodel )
{
	int		i;

	for ( i = 0; i < pmodel->numbones; i++ )
	{
		mat4_t	bonematrix;
		int		parent;

		GL_QuaternionMatrix( pmodel->skeleton[i].quat, bonematrix );
		VectorCopy( pmodel->skeleton[i].pos, bonematrix[3] );

		parent = pmodel->node[i].parent;

		if ( parent == -1 )
		{
			MatrixCopy( bonematrix, pmodel->skeleton[i].local );
			MatrixCopy( bonematrix, pmodel->skeleton[i].global );
			GL_MatrixInverse( pmodel->skeleton[i].global, pmodel->skeleton[i].offset );
		}
		else
		{
			MatrixCopy( bonematrix, pmodel->skeleton[i].local );
			GL_ConcatTransforms( pmodel->skeleton[parent].global, bonematrix, pmodel->skeleton[i].global );
			GL_MatrixInverse( pmodel->skeleton[i].global, pmodel->skeleton[i].offset );
		}
	}
}

/*
=================
Grab_Triangles
=================
*/
void Grab_Triangles( s_model_t *pmodel )
{
	int		i, j, k;

	while ( 1 )
	{
		if ( fgets( line, sizeof( line ), input ) )
		{
			char			texturename[ 32 ];
			s_mesh_t		*pmesh;
			s_triangle_t	*pTri;

			linecount++;

			if ( !strncmp( line, "end", 3 ) )
			{
				return;
			}

			strcpyn( texturename, line );

			for ( i = strlen( texturename ) - 1; i >= 0; i-- )
			{
				if ( !isprint( texturename[i] ) )
				{
					texturename[i] = '\0';
				}
				else
				{
					break;
				}
			}

			if ( !texturename[0] )
			{
				Error( "%s : error at line %d", filename, linecount );
			}

			for ( i = 0; i < numreplacetexture; i++ )
			{
				if ( !stricmp( replacetexture[i].source, texturename ) )
				{
					strcpyn( texturename, replacetexture[i].replace );
					break;
				}
			}

			pmesh = lookup_mesh( pmodel, texturename );

			if ( pmesh->numtris >= META_STUDIO_TRIANGLES )
			{
				Error( "too many triangles in model '%s'", pmodel->name );
			}

			pTri = &pmesh->tris[pmesh->numtris];

			for ( i = 0; i < 3; i++ )
			{
				s_vertex_t	v;

				memset( &v, 0, sizeof( s_vertex_t ) );

				if ( fgets( line, sizeof( line ), input ) )
				{
					linecount++;

					if ( ( j = sscanf( line, "%d %f %f %f %f %f %f %f %f %d %d %f %d %f %d %f %d %f", 
								&k, 
								&v.pos[0], &v.pos[1], &v.pos[2], 
								&v.norm[0], &v.norm[1], &v.norm[2], 
								&v.uv[0], &v.uv[1], 
								&v.numbones, 
								&v.bone[0], &v.weight[0], 
								&v.bone[1], &v.weight[1], 
								&v.bone[2], &v.weight[2], 
								&v.bone[3], &v.weight[3] ) ) >= 9 )
					{
						if ( j == 9 )
						{
							v.numbones = 1;
							v.bone[0] = k;
							v.weight[0] = 1.0;
						}

						/*adjust_vertex( v.pos );*/

						v.uv[1] = 1.0f - v.uv[1];

						// engine request reverse order
						pTri->indices[2-i] = lookup_vertex( pmodel, &v );
					}
					else
					{
						Error( "%s : error at line %d", filename, linecount );
					}
				}
			}

			pmesh->numtris++;
		}
		else
		{
			break;
		}
	}

	Error( "%s : unexpected EOF at line %d", filename, linecount );
}

/*
=================
Grab_Studio
=================
*/
void Grab_Studio( s_model_t *pmodel )
{
	char	cmd[ 1024 ];
	int		option;

	sprintf( filename, "%s/%s.smd", cddir, pmodel->name );

	printf( "grabbing \"%s\"\n", filename );

	if ( !( input = fopen( filename, "rb" ) ) )
	{
		Error( "could not open file '%s'", filename );
	}

	linecount = 0;

	while ( fgets( line, sizeof( line ), input ) )
	{
		linecount++;

		if ( sscanf( line, "%s %d", cmd, &option ) )
		{
			if ( !strcmp( cmd, "version" ) )
			{
				if ( option != 1 )
				{
					Error( "%s : bad version", filename );
				}
			}
			else if ( !strcmp( cmd, "nodes" ) )
			{
				pmodel->numbones = Grab_Nodes( pmodel->node );
				CompareNodes( model->node, pmodel->numbones );
			}
			else if ( !strcmp( cmd, "skeleton" ) )
			{
				Grab_Skeleton( pmodel->node, pmodel->skeleton );
				Build_Reference( pmodel );
			}
			else if ( !strcmp( cmd, "triangles" ) )
			{
				Grab_Triangles( pmodel );
			}
			else
			{
				Error( "%s : unknown studio command at line %d", filename, linecount );
			}
		}
	}

	fclose( input );
}

/*
=================
Option_Studio
=================
*/
void Option_Studio( void )
{
	if ( nummodels >= META_STUDIO_MODELS )
	{
		Error( "too many models" );
	}

	if ( !GetToken( false ) )
	{
		return;
	}

	bodypart[numbodyparts].model[bodypart[numbodyparts].nummodels] = &model[nummodels];

	strcpyn( model[nummodels].name, token );

	model[nummodels].node = kalloc( META_STUDIO_BONES, sizeof( s_node_t ) );
	model[nummodels].skeleton = kalloc( META_STUDIO_BONES, sizeof( s_bone_t ) );
	model[nummodels].vertex = kalloc( META_STUDIO_VERTICES, sizeof( s_vertex_t ) );
	model[nummodels].mesh = kalloc( META_STUDIO_MESHES, sizeof( s_mesh_t ) );

	Grab_Studio( &model[nummodels] );

	bodypart[numbodyparts].nummodels++;
	nummodels++;
}

/*
=================
Option_Blank
=================
*/
void Option_Blank( void )
{
	if ( nummodels >= META_STUDIO_MODELS )
	{
		Error( "too many models" );
	}

	bodypart[numbodyparts].model[bodypart[numbodyparts].nummodels] = &model[nummodels];

	strcpyn( model[nummodels].name, "blank" );

	bodypart[numbodyparts].nummodels++;
	nummodels++;
}

/*
=================
Cmd_Bodygroup
=================
*/
void Cmd_Bodygroup( void )
{
	if ( numbodyparts >= META_STUDIO_BODYPARTS )
	{
		Error( "too many bodyparts" );
	}

	if ( !GetToken( false ) )
	{
		return;
	}

	strcpyn( bodypart[numbodyparts].name, token );

	if ( numbodyparts == 0 )
	{
		bodypart[numbodyparts].base = 1;
	}
	else
	{
		bodypart[numbodyparts].base = bodypart[numbodyparts-1].base * bodypart[numbodyparts-1].nummodels;
	}

	do 
	{
		GetToken( true );

		if ( endofscript )
		{
			return;
		}
		else if ( token[0] == '{' )
		{
		}
		else if ( token[0] == '}' )
		{
			break;
		}
		else if ( !strcmp( token, "studio" ) )
		{
			Option_Studio();
		}
		else if ( !strcmp( token, "blank" ) )
		{
			Option_Blank();
		}

	} while ( 1 );

	numbodyparts++;
}

/*
=================
Cmd_Body
=================
*/
void Cmd_Body( void )
{
	if ( numbodyparts >= META_STUDIO_BODYPARTS )
	{
		Error( "too many bodyparts" );
	}

	if ( !GetToken( false ) )
	{
		return;
	}

	strcpyn( bodypart[numbodyparts].name, token );

	if ( numbodyparts == 0 )
	{
		bodypart[numbodyparts].base = 1;
	}
	else
	{
		bodypart[numbodyparts].base = bodypart[numbodyparts - 1].base * bodypart[numbodyparts - 1].nummodels;
	}

	Option_Studio();

	numbodyparts++;
}

/*
=================
Grab_Animation
=================
*/
void Grab_Animation( s_animation_t *panim )
{
	int		iBone, iFrame, iCount;
	char	cmd[ 1024 ];
	int		option;
	vec3_t	pos;
	vec4_t	rot;

	for ( iBone = 0; iBone < panim->numbones; iBone++ )
	{
		panim->time = kalloc( META_STUDIO_KEYFRAMES, sizeof( int ) );
		panim->pos[iBone] = kalloc( META_STUDIO_KEYFRAMES, sizeof( vec3_t ) );
		panim->quat[iBone] = kalloc( META_STUDIO_KEYFRAMES, sizeof( vec4_t ) );
	}

	while ( fgets( line, sizeof( line ), input ) )
	{
		linecount++;

		if ( !strncmp( line, "end", 3 ) )
		{
			return;
		}
		else if ( ( iCount = sscanf( line, "%d %f %f %f %f %f %f %f", 
								&iBone, 
								&pos[0], &pos[1], &pos[2], 
								&rot[0], &rot[1], &rot[2], &rot[3] ) ) >= 7 )
		{
			if ( panim->node[iBone].parent == -1 )
			{
				vec4_t	quat;
				mat4_t	mat1, mat2;

				if ( iCount == 7 )
				{
					GL_AngleQuaternion( rot, quat );
				}
				else if ( iCount == 8 )
				{
					VectorCopy4( rot, quat );
				}

				/*adjust_vertex( pos );*/

				// out = adjustmatrix * rotatemodels * root

				GL_QuaternionMatrix( quat, mat1 );
				VectorCopy( pos, mat1[3] );

				GL_ConcatTransforms( rotatemodels, mat1, mat2 );
				GL_ConcatTransforms( adjustmatrix, mat2, mat1 );

				GL_MatrixQuaternion( mat1, rot );
				VectorCopy( mat1[3], pos );
			}

			VectorCopy( pos, panim->pos[iBone][iFrame] );

			if ( iCount == 7 )
			{
				GL_AngleQuaternion( rot, panim->quat[iBone][iFrame] );
			}
			else if ( iCount == 8 )
			{
				VectorCopy4( rot, panim->quat[iBone][iFrame] );
			}
		}
		else if ( sscanf( line, "%s %d", cmd, &option ) == 2 )
		{
			if ( !strcmp( cmd, "time" ) )
			{
				if ( panim->numkeyframes >= META_STUDIO_KEYFRAMES )
				{
					Error( "too many keyframe in animation '%s'", panim->name );
				}

				iFrame = panim->numkeyframes;
				panim->time[iFrame] = option;
				panim->duration = option;
				panim->numkeyframes++;
			}
			else
			{
				Error( "%s : error at line %d", filename, linecount );
			}
		}
		else
		{
			Error( "%s : error at line %d", filename, linecount );
		}
	}

	Error( "%s : unexpected EOF at line %d", filename, linecount );
}

/*
=================
Option_Animation
=================
*/
void Option_Animation( char *name, s_animation_t *panim )
{
	char	cmd[ 1024 ];
	int		option;

	sprintf( filename, "%s/%s.smd", cddir, name );

	printf( "grabbing \"%s\"\n", filename );

	if ( !( input = fopen( filename, "rb" ) ) )
	{
		Error("could not open file '%s'", filename );
	}

	strcpyn( panim->name, name );

	linecount = 0;

	while ( fgets( line, sizeof( line ), input ) )
	{
		linecount++;

		if ( sscanf( line, "%s %d", cmd, &option ) )
		{
			if ( !strcmp( cmd, "version" ) )
			{
				if ( option != 1 )
				{
					Error( "%s : bad version", filename );
				}
			}
			else if ( !strcmp( cmd, "nodes" ) )
			{
				panim->numbones = Grab_Nodes( panim->node );
				CompareNodes( panim->node, panim->numbones );
			}
			else if ( !strcmp( cmd, "skeleton" ) )
			{
				Grab_Animation( panim );
			}
			else
			{
				Error( "%s : unknown studio command at line %d", filename, linecount );
			}
		}
	}

	fclose( input );
}

/*
=================
Option_Event
=================
*/
void Option_Event( s_sequence_t *psequence )
{
	if ( psequence->numevents >= META_STUDIO_EVENTS )
	{
		Error( "too many events in sequence %s", psequence->name );
	}

	GetToken( false );
	psequence->event[psequence->numevents].event = atoi( token );

	GetToken( false );
	psequence->event[psequence->numevents].frame = atoi( token );

	GetToken( false );
	strcpyn( psequence->event[psequence->numevents].options, token );

	psequence->numevents++;
}

/*
=================
CheckMetaStudio
=================
*/
void CheckMetaStudio( void )
{
	if ( numtextures == 0 )
		Error( "Need a texture" );

	if ( nummodels == 0 )
		Error( "Need a model" );

	if ( numseq == 0 )
		Error( "Need a sequence" );
}

/*
=================
Build_BoneTable
=================
*/
void Build_BoneTable( void )
{
	int			i;
	s_model_t	*pmodel;

	if ( !nummodels )
		return;

	pmodel = &model[ 0 ];

	for ( i = 0; i < pmodel->numbones; i++ )
	{
		strcpyn( bonetable[numbones].name, pmodel->node[i].name );
		bonetable[numbones].parent = pmodel->node[i].parent;
		numbones++;
	}
}

/*
=================
Build_Hitbox
=================
*/
void Build_Hitbox( void )
{
	int		i, j, k;

	printf( "building bounding box for bones\n" );

	if ( numhitboxes == 0 )
	{
		for ( i = 0; i < numbones; i++ )
		{
			VectorClear( bonetable[i].bmin );
			VectorClear( bonetable[i].bmax );
		}

		for ( i = 0; i < nummodels; i++ )
		{
			s_vertex_t	*pv;
			vec3_t		p;

			for ( j = 0; j < model[i].numverts; j++ )
			{
				pv = &model[i].vertex[j];

				for ( k = 0; k < pv->numbones; k++ )
				{
					// transform the position to joint space
					GL_VectorTransform( pv->pos, model[i].skeleton[ pv->bone[k] ].offset, p );

					if ( p[0] < bonetable[ pv->bone[k] ].bmin[0] ) bonetable[ pv->bone[k] ].bmin[0] = p[0];
					if ( p[1] < bonetable[ pv->bone[k] ].bmin[1] ) bonetable[ pv->bone[k] ].bmin[1] = p[1];
					if ( p[2] < bonetable[ pv->bone[k] ].bmin[2] ) bonetable[ pv->bone[k] ].bmin[2] = p[2];
					if ( p[0] > bonetable[ pv->bone[k] ].bmax[0] ) bonetable[ pv->bone[k] ].bmax[0] = p[0];
					if ( p[1] > bonetable[ pv->bone[k] ].bmax[1] ) bonetable[ pv->bone[k] ].bmax[1] = p[1];
					if ( p[2] > bonetable[ pv->bone[k] ].bmax[2] ) bonetable[ pv->bone[k] ].bmax[2] = p[2];
				}
			}
		}

		for ( i = 0; i < numbones; i++ )
		{
			// filter out the box too small
			if ( bonetable[i].bmin[0] < bonetable[i].bmax[0] - 1.0f && 
				 bonetable[i].bmin[1] < bonetable[i].bmax[1] - 1.0f && 
				 bonetable[i].bmin[2] < bonetable[i].bmax[2] - 1.0f )
			{
				strcpyn( hitbox[numhitboxes].name, bonetable[i].name );
				hitbox[numhitboxes].bone = i;
				hitbox[numhitboxes].group = 0;	//test
				VectorCopy( bonetable[i].bmin, hitbox[numhitboxes].bmin );
				VectorCopy( bonetable[i].bmax, hitbox[numhitboxes].bmax );
				numhitboxes++;
			}
		}
	}
	else
	{
		for ( i = 0; i < numhitboxes; i++ )
		{
			for ( j = 0; j < numbones; j++ )
			{
				if ( !strcmp( bonetable[j].name, hitbox[i].name ) )
				{
					hitbox[i].bone = j;
					break;
				}
			}

			if ( j >= numbones )
			{
				Error( "cannot find bone %s for hitbox\n", hitbox[i].name );
			}
		}
	}
}

/*
=================
Build_SequenceBox
=================
*/
void Build_SequenceBox( void )
{
	int		i, j, k;
	int		n, m, w;

	printf( "building bounding box for sequences\n" );

	for ( i = 0; i < numseq; i++ )
	{
		vec3_t			bmin, bmax;

		for ( j = 0; j < 3; j++ )
		{
			bmin[j] = 9999.0f;
			bmax[j] = -9999.0f;
		}

		for ( j = 0; j < sequence[i].numblends; j++ )
		{
			s_animation_t *panim;

			panim = sequence[i].anim[j];

			for ( k = 0; k < panim->numkeyframes; k++ )
			{
				static mat4_t	bonematrix[ META_STUDIO_BONES ];	// transform position joint-space to global-space
				static mat4_t	skinmatrix[ META_STUDIO_BONES ];	// transform position bind-pose to current-pose in global-space

				// build bonetransform
				for ( n = 0; n < numbones; n++ )
				{
					mat4_t	mat;
					int		parent;

					GL_QuaternionMatrix( panim->quat[n][k], mat);
					VectorCopy( panim->pos[n][k], mat[3] );

					parent = bonetable[n].parent;

					if ( parent == -1 )
					{
						MatrixCopy(mat, bonematrix[n] );
					}
					else
					{
						GL_ConcatTransforms( bonematrix[parent], mat, bonematrix[n] );
					}
				}

				// transform vertex
				for ( n = 0; n < nummodels; n++ )
				{
					s_model_t	*pmodel;
					s_vertex_t	*pv;

					pmodel = &model[n];

					// build skin matrix for this model
					for ( m = 0; m < numbones; m++ )
					{
						GL_ConcatTransforms( bonematrix[m], pmodel->skeleton[m].offset, skinmatrix[m] );
					}

					for ( m = 0; m < pmodel->numverts; m++ )
					{
						vec3_t		pos;
						vec3_t		tmp;

						pv = &pmodel->vertex[m];

						VectorClear( pos );

						// apply weights
						for ( w = 0; w < pv->numbones; w++ )
						{
							GL_VectorTransform( pv->pos, skinmatrix[ pv->bone[w] ], tmp );
							VectorMA( pos, pv->weight[w], tmp, pos );
						}

						if ( pos[0] < bmin[0] ) bmin[0] = pos[0];
						if ( pos[1] < bmin[1] ) bmin[1] = pos[1];
						if ( pos[2] < bmin[2] ) bmin[2] = pos[2];
						if ( pos[0] > bmax[0] ) bmax[0] = pos[0];
						if ( pos[1] > bmax[1] ) bmax[1] = pos[1];
						if ( pos[2] > bmax[2] ) bmax[2] = pos[2];
					} // vert
				} // model
			} //frame
		} // anim

		VectorCopy( bmin, sequence[i].bmin );
		VectorCopy( bmax, sequence[i].bmax );
	} // seq
}

/*
=================
Build_BBox
=================
*/
void Build_BBox( void )
{
	int		i, j;
	vec3_t	min, max;

	for ( i = 0; i < 3; i++ )
	{
		min[i] = 9999.0f;
		max[i] = -9999.0f;
	}

	for ( i = 0; i < numseq; i++ )
	{
		for ( j = 0; j < 3; j++ )
		{
			if ( sequence[i].bmin[j] < min[j] )
				min[j] = sequence[i].bmin[j];

			if ( sequence[i].bmax[j] > max[j] )
				max[j] = sequence[i].bmax[j];
		}
	}

	VectorCopy( min, bbmin );
	VectorCopy( max, bbmax );
}

/*
=================
Build_Attachment
=================
*/
void Build_Attachment( void )
{
	int		i, j;

	printf( "building attachments\n" );

	for ( i = 0; i < numattachments; i++ )
	{
		for ( j = 0; j< numbones; j++ )
		{
			if ( !strcmp( bonetable[j].name, attachment[i].bonename ) )
			{
				attachment[i].bone = j;
				break;
			}
		}

		if ( j >= numbones )
		{
			Error( "cannot find bone %s for attachment\n", attachment[i].bonename );
		}
	}
}

/*
=================
Cmd_Sequence
=================
*/
void Cmd_Sequence( void )
{
	static char	smdfilename[ META_STUDIO_BLENDS ][ 64 ];
	int			depth;
	int			i;
	int			numblends;

	if ( numseq >= META_STUDIO_SEQUENCES )
	{
		Error( "too many sequences" );
	}

	if ( !GetToken( false ) )
	{
		return;
	}

	strcpyn( sequence[numseq].name, token );

	sequence[numseq].fps = 1000.0f;

	depth = 0;
	numblends = 0;

	while ( 1 )
	{
		if ( depth > 0 )
		{
			if ( !GetToken( true ) )
			{
				break;
			}
		}
		else
		{
			if ( !TokenAvailable() )
			{
				break;
			}
			else
			{
				GetToken( false );
			}
		}
		
		if ( endofscript )
		{
			if ( depth != 0 )
			{
				Error( "missing }" );
			}

			return;
		}

		if ( !strcmp( token, "{" ) )
		{
			depth++;
		}
		else if ( !strcmp( token, "}" ) )
		{
			depth--;
		}
		else if ( !strcmp( token, "fps" ) )
		{
			GetToken( false );
			sequence[numseq].fps = atof( token );
		}
		else if ( !strcmp( token, "loop" ) )
		{
			sequence[numseq].flags |= META_STUDIO_LOOPING;
		}
		else
		{
			strcpyn( smdfilename[numblends], token );
			numblends++;
		}
		if ( depth < 0 )
		{
			Error( "missing {" );
		}
	}

	if ( numblends == 0 )
	{
		Error( "no animations found" );
	}

	for ( i = 0; i < numblends; i++ )
	{
		if ( numani >= META_STUDIO_ANIMATIONS )
		{
			Error( "too many animations" );
		}

		animation[numani].node = kalloc( META_STUDIO_BONES, sizeof( s_node_t ) );
		sequence[numseq].anim[i] = &animation[numani];
		Option_Animation( smdfilename[i], &animation[numani] );
		numani++;
	}

	if ( numblends > 1 )
	{
		sequence[numseq].duration = 1;
	}
	else
	{
		sequence[numseq].duration = sequence[numseq].anim[0]->duration;
	}

	sequence[numseq].numblends = numblends;
	numseq++;
}

/*
=================
Cmd_Hitbox
=================
*/
void Cmd_Hitbox( void )
{
	GetToken( false );
	hitbox[numhitboxes].group = atoi( token );

	GetToken( false );
	strcpyn( hitbox[numhitboxes].name, token );

	GetToken( false );
	hitbox[numhitboxes].bmin[0] = atof( token );
	GetToken( false );
	hitbox[numhitboxes].bmin[1] = atof( token );
	GetToken( false );
	hitbox[numhitboxes].bmin[2] = atof( token );
	GetToken( false );
	hitbox[numhitboxes].bmax[0] = atof( token );
	GetToken( false );
	hitbox[numhitboxes].bmax[1] = atof( token );
	GetToken( false );
	hitbox[numhitboxes].bmax[2] = atof( token );

	numhitboxes++;
}

/*
=================
Cmd_Attachment
=================
*/
void Cmd_Attachment( void )
{
	GetToken( false );
	strcpyn( attachment[numattachments].bonename, token );

	GetToken( false );
	attachment[numattachments].org[0] = atof( token );
	GetToken( false );
	attachment[numattachments].org[1] = atof( token );
	GetToken( false );
	attachment[numattachments].org[2] = atof( token );

	numattachments++;
}

/*
=================
Cmd_ReplaceTexture
=================
*/
void Cmd_ReplaceTexture( void )
{
	GetToken( false );
	strcpyn( replacetexture[numreplacetexture].source, token );
	GetToken( false );
	strcpyn( replacetexture[numreplacetexture].replace, token );

	numreplacetexture++;
}

/*
=================
Cmd_TexDef
=================
*/
void Cmd_TexDef( void )
{
	int		i;
	int		j;
	char	texturename[32];
	char	path[64];

	// $texdef "ID" "NAME" "PATH"

	GetToken( false );
	j = atoi( token );

	GetToken( false );
	strcpyn( texturename, token );
	GetToken( false );
	strcpyn( path, token );

	i = lookup_texture( texturename );

	strcpyn( texture[i].path, path );
	texture[i].index = j;
}

/*
=================
Cmd_TexRef
=================
*/
void Cmd_TexRef( void )
{
	int		i, j;
	char	modelname[32];
	char	meshname[32];
	char	texturename[32];
	int		refnum;

	// $texref "REF" "MODEL" "MESH" "TEXTURE"

	GetToken( false );
	refnum = atoi( token );
	GetToken( false );
	strcpyn( modelname, token );
	GetToken( false );
	strcpyn( meshname, token );
	GetToken( false );
	strcpyn( texturename, token );

	if ( refnum < 0 || refnum > 3 )
	{
		Error( "refnum out of range (0~3)" );
		return;
	}

	for ( i = 0; i < nummodels; i++ )
	{
		if ( !stricmp( model[i].name, modelname ) )
			break;
	}

	if ( i >= META_STUDIO_MODELS )
	{
		Error( "cannot find model %s for $texref", modelname );
		return;
	}

	for ( j = 0; j < model[i].nummesh; j++ )
	{
		if ( !stricmp( model[i].mesh[j].name, meshname ) )
			break;
	}

	if ( j >= model[i].nummesh )
	{
		Error( "cannot find mesh %s in model %s for $texref",
			meshname, modelname );
		return;
	}

	model[i].mesh[j].texref[refnum] = lookup_texture( texturename );
}

/*
=================
Cmd_Adjust
=================
*/
void Cmd_Adjust( void )
{
	vec3_t	origin;
	vec3_t	angles;

	GetToken( false );
	origin[0] = atof( token );
	GetToken( false );
	origin[1] = atof( token );
	GetToken( false );
	origin[2] = atof( token );

	GetToken( false );
	angles[0] = atof( token );
	GetToken( false );
	angles[1] = atof( token );
	GetToken( false );
	angles[2] = atof( token );

	GL_AngleMatrix(angles, adjustmatrix);
	VectorCopy(origin, adjustmatrix[3]);
}

/*
=================
ParseScript
=================
*/
void ParseScript( void )
{
	while ( 1 )
	{
		do 
		{
			// look for a line starting with a $ command
			GetToken( true );

			if ( endofscript )
			{
				return;
			}

			if ( token[0] == '$' )
			{
				break;
			}

			while ( TokenAvailable() )
			{
				GetToken( false );
			}

		} while ( 1 );

		if ( !strcmp( token, "$modelname" ) )
		{
			Cmd_Modelname();
		}
		else if ( !strcmp( token, "$cd" ) )
		{
			if ( cdset )
			{
				Error( "Two $cd in one model" );
			}
			cdset = 1;
			GetToken( false );
			strcpyn( cdpartial, token );
			strcpyn( cddir, ExpandPath( token ) );
		}
		else if ( !strcmp( token, "$body" ) )
		{
			Cmd_Body();
		}
		else if ( !strcmp( token, "$bodygroup" ) )
		{
			Cmd_Bodygroup();
		}
		else if ( !strcmp( token, "$sequence" ) )
		{
			Cmd_Sequence();
		}
		else if ( !strcmp( token, "$hbox" ) )
		{
			Cmd_Hitbox();
		}
		else if ( !strcmp( token, "$attachment" ) )
		{
			Cmd_Attachment();
		}
		else if ( !strcmp( token, "$replacetexture" ) )
		{
			Cmd_ReplaceTexture();
		}
		else if ( !strcmp( token, "$texdef" ) )
		{
			Cmd_TexDef();
		}
		else if ( !strcmp( token, "$texref" ) )
		{
			Cmd_TexRef();
		}
		else if ( !strcmp( token, "$adjust" ) )
		{
			Cmd_Adjust();
		}
		else
		{
			Error( "bad command '%s'", token );
		}
	}
}

/*
=================
WriteModel
=================
*/
byte			*pData;
byte			*pStart;
metastudiohdr_t	*phdr;
int				total_frames;

#define ALIGN( a ) a = (byte *)( (int)( (byte *)a + 3 ) & ~3 )

#define SIZE_KB( a ) ( (a) / 1024.0 )

/*
=================
WriteBoneInfo
=================
*/
void WriteBoneInfo( void )
{
	int						i;
	metastudiobone_t		*pbone;
	metastudioattachment_t	*pattachment;
	metastudiohitbox_t		*phitbox;

	pbone = (metastudiobone_t *)pData;
	phdr->numbones = numbones;
	phdr->boneindex = ( pData - pStart );

	for ( i = 0; i < numbones; i++ )
	{
		strcpyn( pbone[i].name, bonetable[i].name );
		pbone[i].parent = bonetable[i].parent;
	}

	pData += numbones * sizeof( metastudiobone_t );
	ALIGN( pData );

	pattachment = (metastudioattachment_t *)pData;
	phdr->numattachments = numattachments;
	phdr->attachmentindex = ( pData - pStart );

	for ( i = 0; i < numattachments; i++ )
	{
		pattachment[i].bone = attachment[i].bone;
		VectorCopy( attachment[i].org, pattachment[i].org );
	}

	pData += numattachments * sizeof( metastudioattachment_t );
	ALIGN( pData );

	phitbox = (metastudiohitbox_t *)pData;
	phdr->numhitboxes = numhitboxes;
	phdr->hitboxindex = ( pData - pStart );

	for ( i = 0; i < numhitboxes; i++ )
	{
		phitbox[i].bone = hitbox[i].bone;
		phitbox[i].group = hitbox[i].group;
		VectorCopy( hitbox[i].bmin, phitbox[i].bmin );
		VectorCopy( hitbox[i].bmax, phitbox[i].bmax );
	}

	pData += numhitboxes * sizeof( metastudiohitbox_t );
	ALIGN( pData );
}

/*
=================
WriteSequenceInfo
=================
*/
void WriteSequenceInfo( void )
{
	int					i, j;
	metastudioseqdesc_t	*pseqdesc;
	metastudioevent_t	*pevent;

	pseqdesc = (metastudioseqdesc_t *)pData;
	phdr->numseq = numseq;
	phdr->seqindex = ( pData - pStart );

	pData += numseq * sizeof( metastudioseqdesc_t );
	ALIGN( pData );

	for ( i = 0; i < numseq; i++ )
	{
		strcpyn( pseqdesc[i].label, sequence[i].name );
		pseqdesc[i].flags = sequence[i].flags;
		pseqdesc[i].fps = sequence[i].fps;
		pseqdesc[i].activity = sequence[i].activity;
		pseqdesc[i].actweight = sequence[i].actweight;
		pseqdesc[i].duration = sequence[i].duration;
		pseqdesc[i].numblends = sequence[i].numblends;
		pseqdesc[i].animindex = sequence[i].animindex;
		VectorCopy( sequence[i].bmin, pseqdesc[i].bmin );
		VectorCopy( sequence[i].bmax, pseqdesc[i].bmax );

		pevent = (metastudioevent_t *)pData;
		pseqdesc->numevents = sequence[i].numevents;
		pseqdesc->eventindex = ( pData - pStart );

		for ( j = 0; j < sequence[i].numevents; j++ )
		{
			pevent[j].frame = sequence[i].event[j].frame;
			pevent[j].event = sequence[i].event[j].event;
			strcpyn( pevent[j].options, sequence[i].event[j].options );
		}

		pData += sequence[i].numevents * sizeof( metastudioevent_t );
		ALIGN( pData );
	}
}

/*
=================
WriteAnimations
=================
*/
void WriteAnimations( void )
{
	int					seq, blend, frame, bone;
	metastudioanim_t	*panim;

	total_frames = 0;

	for ( seq = 0; seq < numseq; seq++ )
	{
		panim = (metastudioanim_t *)pData;
		sequence[seq].animindex = ( pData - pStart );

		pData += sequence[seq].numblends * sizeof( metastudioanim_t );
		ALIGN( pData );

		for ( blend = 0; blend < sequence[seq].numblends; blend++ )
		{
			int						*ptime;
			metastudioanimvalue_t	*pvalue;

			panim[blend].numkeyframes = sequence[seq].anim[blend]->numkeyframes;
			panim[blend].numbones = sequence[seq].anim[blend]->numbones;

			ptime = (int *)pData;
			panim[blend].timeindex = ( pData - pStart );

			for ( frame = 0; frame < panim[blend].numkeyframes; frame++ )
			{
				ptime[frame] = sequence[seq].anim[blend]->time[frame];
			}

			pData += panim[blend].numkeyframes * sizeof( int );
			ALIGN( pData );

			pvalue = (metastudioanimvalue_t *)pData;
			panim[blend].valueindex = ( pData - pStart );

			for ( frame = 0; frame < panim[blend].numkeyframes; frame++ )
			{
				for ( bone = 0; bone < panim[blend].numbones; bone++ )
				{
					VectorCopy( sequence[seq].anim[blend]->pos[bone][frame], pvalue->pos );
					VectorCopy4( sequence[seq].anim[blend]->quat[bone][frame], pvalue->quat );
					pvalue++;
				}

				total_frames++;
			}

			pData += panim[blend].numkeyframes * panim[blend].numbones * sizeof( metastudioanimvalue_t );
			ALIGN( pData );
		}
	}
}

/*
=================
WriteTexturesStandard
=================
*/
void WriteTexturesStandard( void )
{
	_mstudiotexture_t	*ptexture;

	ptexture = (_mstudiotexture_t *)pData;
	phdr->_numtextures = 1;
	phdr->_textureindex = ( pData - pStart );

	pData += sizeof( _mstudiotexture_t );
	ALIGN( pData );

	phdr->_texturedataindex = ( pData - pStart );

	pData += ( 2 * 2 + 256 * 3 );	// pixel + palette
	ALIGN( pData );

	strcpyn( ptexture[0].name, "blank" );
	ptexture[0].width = 2;
	ptexture[0].height = 2;
	ptexture[0].index = phdr->_texturedataindex;
}

/*
=================
WriteTextures
=================
*/
void WriteTextures( void )
{
	int					i;
	metastudiotexture_t	*ptexture;

	ptexture = (metastudiotexture_t *)pData;
	phdr->numtextures = numtextures;
	phdr->textureindex = ( pData - pStart );

	for ( i = 0; i < numtextures; i++ )
	{
		strcpyn( ptexture[i].name, texture[i].name );
		strcpyn( ptexture[i].path, texture[i].path );
		ptexture[i].index = texture[i].index;
		ptexture[i].flags = texture[i].flags;
	}

	pData += numtextures * sizeof( metastudiotexture_t );
	ALIGN( pData );
}

/*
=================
WriteModel
=================
*/
void WriteModel( void )
{
	int						i;
	int						iModel, iBone, iVert, iMesh, iTri;
	metastudiobodyparts_t	*pbodypart;
	metastudiomodel_t		*pmodel;
	byte					*prev;

	pbodypart = (metastudiobodyparts_t *)pData;
	phdr->numbodyparts = numbodyparts;
	phdr->bodypartindex = ( pData - pStart );

	pData += numbodyparts * sizeof( metastudiobodyparts_t );
	ALIGN( pData );

	pmodel = (metastudiomodel_t *)pData;

	pData += nummodels * sizeof( metastudiomodel_t );
	ALIGN( pData );

	iModel = 0;
	for ( i = 0; i < numbodyparts; i++ )
	{
		strcpyn( pbodypart[i].name, bodypart[i].name );
		pbodypart[i].nummodels = bodypart[i].nummodels;
		pbodypart[i].base = bodypart[i].base;
		pbodypart[i].modelindex = (byte *)&pmodel[iModel] - pStart;
		iModel += bodypart[i].nummodels;
	}

	for ( iModel = 0; iModel < nummodels; iModel++ )
	{
		mat4_t				*pmat;
		metastudiovertex_t	*pvertex;
		metastudiomesh_t	*pmesh;
		int					total_tris;

		strcpyn( pmodel[iModel].name, model[iModel].name );

		pmat = (mat4_t *)pData;
		pmodel[iModel].matindex = ( pData - pStart );

		for ( iBone = 0; iBone < model[iModel].numbones; iBone++ )
		{
			MatrixCopy( model[iModel].skeleton[iBone].offset, pmat[iBone] );
		}

		pData += model[iModel].numbones * sizeof( mat4_t );
		ALIGN( pData );

		pvertex = (metastudiovertex_t *)pData;
		pmodel[iModel].numverts = model[iModel].numverts;
		pmodel[iModel].vertindex = ( pData - pStart );

		for ( iVert = 0; iVert < model[iModel].numverts; iVert++ )
		{
			VectorCopy( model[iModel].vertex[iVert].pos, pvertex[iVert].pos );
			VectorCopy( model[iModel].vertex[iVert].norm, pvertex[iVert].norm );
			VectorCopy2( model[iModel].vertex[iVert].uv, pvertex[iVert].uv );
			pvertex[iVert].numbones = model[iModel].vertex[iVert].numbones;
			// copy weights
			for ( i = 0; i < 4; i++ )
			{
				pvertex[iVert].bone[i] = model[iModel].vertex[iVert].bone[i];
				pvertex[iVert].weight[i] = model[iModel].vertex[iVert].weight[i];
			}
		}

		prev = pData;

		pData += model[iModel].numverts * sizeof( metastudiovertex_t );
		ALIGN( pData );

		printf( "vertices     %8.2f KB (%d)\n", SIZE_KB( pData - prev ), model[iModel].numverts );

		pmesh = (metastudiomesh_t *)pData;
		pmodel[iModel].nummesh = model[iModel].nummesh;
		pmodel[iModel].meshindex = ( pData - pStart );

		prev = pData;

		pData += model[iModel].nummesh * sizeof( metastudiomesh_t );
		ALIGN( pData );

		total_tris = 0;

		for ( iMesh = 0; iMesh < model[iModel].nummesh; iMesh++ )
		{
			metastudiotriangle_t *ptris;

			ptris = (metastudiotriangle_t *)pData;
			pmesh[iMesh].numtris = model[iModel].mesh[iMesh].numtris;
			pmesh[iMesh].triindex = ( pData - pStart );
			// copy texture ref
			for (i = 0; i < 3; i++)
			{
				pmesh[iMesh].texref[i] = model[iModel].mesh[iMesh].texref[i];
			}

			for ( iTri = 0; iTri < pmesh[iMesh].numtris; iTri++ )
			{
				for ( i = 0; i < 3; i++ )
				{
					ptris[iTri].indices[i] = model[iModel].mesh[iMesh].tris[iTri].indices[i];
				}
			}

			total_tris += pmesh[iMesh].numtris;

			pData += pmesh[iMesh].numtris * sizeof( metastudiotriangle_t );
			ALIGN( pData );
		}

		printf( "mesh         %8.2f KB (%d tris)\n", SIZE_KB( pData - prev ), total_tris );
	}
}


#define FILEBUFFER ( 8 * 1024 * 1024 )


/*
=================
WriteToFile
=================
*/
void WriteToFile( void )
{
	FILE	*modelouthandle;
	byte	*prev;

	pStart = kalloc( 1, FILEBUFFER );
	pData = pStart;

	printf( "---------------------\n" );
	printf( "writing %s:\n", outname );

	if ( !( modelouthandle = fopen( outname, "wb" ) ) )
	{
		Error( "couldn't create file '%s'", outname );
	}

	phdr = (metastudiohdr_t *)pData;
	phdr->id = IDSTUDIOHEADER;
	phdr->version = STUDIO_VERSION;

	strcpyn( phdr->name, outname );
	phdr->flags = META_STUDIO;

	VectorCopy(bbmin, phdr->min);
	VectorCopy(bbmax, phdr->max);
	VectorCopy(bbmin, phdr->bbmin);
	VectorCopy(bbmax, phdr->bbmax);

	pData += sizeof( metastudiohdr_t );
	ALIGN( pData );

	prev = pData;
	WriteBoneInfo();
	printf( "bones        %8.2f KB (%d)\n", SIZE_KB( pData - prev ), numbones );

	prev = pData;
	WriteAnimations();
	printf( "animations   %8.2f KB (%d anims, %d frames)\n", SIZE_KB( pData - prev ), numani, total_frames );

	prev = pData;
	WriteSequenceInfo();
	printf( "sequences    %8.2f KB (%d)\n", SIZE_KB( pData - prev ), numseq );

	prev = pData;
	WriteModel();
	printf( "models       %8.2f KB (%d)\n", SIZE_KB( pData - prev ), nummodels );

	prev = pData;
	WriteTextures();
	WriteTexturesStandard();	// must be write to tail
	printf( "textures     %8.2f KB (%d)\n", SIZE_KB( pData - prev ), numtextures );

	phdr->length = pData - pStart;

	printf( "total        %8.2f KB\n", SIZE_KB( phdr->length ) );

	if ( fwrite( pStart, phdr->length, 1, modelouthandle ) != 1 )
	{
		Error( "error writing '%s'", outname );
	}

	fclose( modelouthandle );
}

int main(int argc, char **argv)
{
	int		silent;
	char	path[ 260 ];

	//
	// setup cmlib
	//

	myargc = argc;
	myargv = argv;

	if ( CheckParm( "-s" ) )
	{
		silent = 1;
	}
	else
	{
		silent = 0;
	}

	//
	// startup
	//

	printf( "================================\n" );
	printf( "  Meta Studio Compiler 1.0\n" );
	printf( "  By: Crsky\n" );
	printf( "  E-Mail: crskycode@foxmail.com\n" );
	printf( "================================\n" );
	printf( "\n" );

	if ( argc < 2 )
	{
		printf( "usage: metastudio [file.qc]\n" );

		if ( !silent )
		{
			system( "PAUSE" );
		}

		return 1;
	}

	strcpyn( path, argv[1] );

	if ( _access( path, 4 ) == -1 )
	{
		printf( "file does not exist\n" );

		if ( !silent )
		{
			system( "PAUSE" );
		}

		return 1;
	}

	LoadScriptFile( path );
	ClearModel();
	ParseScript();
	CheckMetaStudio();
	Build_BoneTable();
	Build_Hitbox();
	Build_SequenceBox();
	Build_BBox();
	Build_Attachment();
	WriteToFile();

	if ( !silent )
	{
		system( "PAUSE" );
	}

	return 0;
}