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

#ifndef METASTUDIORENDERER_H
#define METASTUDIORENDERER_H

#include "metastudio.h"

typedef struct
{
	int		program;

	int		position;
	int		normal;
	int		texcoord;
	int		numbones;
	int		bones;
	int		weights;

	int		bonetransform;
	int		diffusemap;

} metastudioshader_t;

class CMetaStudioRenderer
{
public:
	CMetaStudioRenderer();
	~CMetaStudioRenderer();

	void Init(void);
	void GetKeyFrame( vec3_t *pos, vec4_t *quat, metastudioanim_t *panim, int index );
	void CalcRotations( vec3_t *pos, vec4_t *quat, metastudioanim_t *panim, int frame );
	int EstimateFrame( metastudioseqdesc_t *pseqdesc );
	void SetupBones( void );
	void SetupSkin( void );
	void SetupModel( int bodypart );
	void SetupLighting( void );
	void DrawPoints( void );
	void RenderModel( void );
	void SetUpTransform( void );
	bool CheckBBox(void);
	int DrawModel( int flags );
	void SetupPlayerBones( void );
	void SetupAttachmentBones( int parentBone );
	void SavePlayerBone( int bone );
	int DrawPlayer( int flags, entity_state_t *player );
	void DrawHulls(void);
	void LoadModel(void);

	double	m_clTime;
	double	m_clOldTime;
	double	m_clFrameTime;

	int m_nFrameCount;

	cl_entity_t			*m_pCurrentEntity;
	model_t				*m_pRenderModel;
	metastudiohdr_t		*m_pStudioHeader;

	mat4_t	m_rotatematrix;	// rotate for entity

	mat4_t	m_bonematrix[ META_STUDIO_BONES ];
	mat4_t	m_skinmatrix[ META_STUDIO_BONES ];

	mat4_t	m_savematrix[ META_STUDIO_BONES ];	// Save the player model skeleton for attachment

	metastudiomodel_t *m_pModel;

	metastudioshader_t m_shader;
};

#endif