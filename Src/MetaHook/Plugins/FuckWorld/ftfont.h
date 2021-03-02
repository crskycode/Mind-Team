void FTF_Init(void);
void FTF_Shutdown(void);
int FTF_CreateFont(const char *filename, int tall, bool bold, bool italic, bool antialias);
bool FTF_GetCharInfo(int font, int code, int *wide, int *tall, int *horiBearingX, int *horiBearingY, int *horiAdvance, GLuint *texture, float coords[]);
int FTF_GetFontTall(int font);
void FTF_DrawString(int x, int y, int font, wchar_t *string);
void FTF_GetStringSize(int font, wchar_t *string, int *wide, int *tall);