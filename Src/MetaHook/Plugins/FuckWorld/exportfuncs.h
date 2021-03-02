extern cl_enginefunc_t gEngfuncs;

int Initialize(struct cl_enginefuncs_s *pEnginefuncs, int iVersion);
void HUD_Init(void);
int HUD_Redraw(float time, int intermission);
void V_CalcRefdef(struct ref_params_s *pparams);
void HUD_DrawTransparentTriangles(void);
void HUD_Shutdown(void);
void HUD_Frame(double time);
int HUD_GetStudioModelInterface(int version, struct r_studio_interface_s **ppinterface, struct engine_studio_api_s *pstudio);