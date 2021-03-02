#ifndef STUDIO_MODEL_H
#define STUDIO_MODEL_H

#include "mathlib.h"
#include <studio.h>

class StudioModel
{
public:
	StudioModel();
	~StudioModel();

	void Init(int width, int height);
	void Load(const char *modelname);
	void Free(void);
	void Draw(void);
	void AdvanceFrame(float dt);
	void ExtractBbox(float *mins, float *maxs);
	int SetSequence(int iSequence);
	int GetSequence(void);
	void GetSequenceInfo(float *pflFrameRate, float *pflGroundSpeed);
	float SetController(int iController, float flValue);
	float SetMouth(float flValue);
	float SetBlending(int iBlender, float flValue);
	int SetBodygroup(int iGroup, int iValue);
	int SetSkin(int iValue);
	void SetPerspective(float fovy, float aspect, float zNear, float zFar);
	void SetOrigin(float x, float y, float z);
	void SetAngles(float x, float y, float z);
	void SetScale(float s);
	int GetColorBuffer(void);

private:
	bool m_initailize;
	int m_width;
	int m_height;
	int m_iFrameBuffer;
	int m_iColorBuffer;
	int m_iRenderBuffer;
	int m_iPrevFrameBuffer;
	float m_fovy;
	float m_aspect;
	float m_zNear;
	float m_zFar;
	vec3_t m_origin;
	vec3_t m_angles;
	float m_scale;
	int m_sequence;
	float m_frame;
	int m_bodynum;
	int m_skinnum;
	byte m_controller[4];
	byte m_blending[2];
	byte m_mouth;

	studiohdr_t *m_pstudiohdr;
	mstudiomodel_t *m_pmodel;

	studiohdr_t *m_ptexturehdr;
	studioseqhdr_t *m_panimhdr[32];

	vec4_t m_adj;

	studiohdr_t *LoadModel(const char *modelname);
	studioseqhdr_t *LoadDemandSequences(const char *modelname);

	void CalcBoneAdj(void);
	void CalcBoneQuaternion(int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *q);
	void CalcBonePosition(int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *pos);
	void CalcRotations(vec3_t *pos, vec4_t *q, mstudioseqdesc_t *pseqdesc, mstudioanim_t *panim, float f);
	mstudioanim_t *GetAnim(mstudioseqdesc_t *pseqdesc);
	void SlerpBones(vec4_t q1[], vec3_t pos1[], vec4_t q2[], vec3_t pos2[], float s);
	void SetUpBones(void);

	void DrawPoints(void);

	void Lighting(float *lv, int bone, int flags, vec3_t normal);
	void Chrome(int *chrome, int bone, vec3_t normal);

	void SetupLighting(void);

	void SetupModel(int bodypart);

	void UploadTexture(mstudiotexture_t *ptexture, byte *data, byte *pal);

	//
	vec3_t m_vright;
	float m_lambert;

	vec3_t m_xformverts[MAXSTUDIOVERTS];
	vec3_t m_lightvalues[MAXSTUDIOVERTS];
	vec3_t *m_pxformverts;
	vec3_t *m_pvlightvalues;

	vec3_t m_lightvec;
	vec3_t m_blightvec[MAXSTUDIOBONES];
	int m_ambientlight;
	float m_shadelight;
	vec3_t m_lightcolor;

	int m_smodels_total;

	float m_bonetransform[MAXSTUDIOBONES][3][4];

	int m_chrome[MAXSTUDIOVERTS][2];
	int m_chromeage[MAXSTUDIOBONES];
	vec3_t m_chromeup[MAXSTUDIOBONES];
	vec3_t m_chromeright[MAXSTUDIOBONES];
};

#endif