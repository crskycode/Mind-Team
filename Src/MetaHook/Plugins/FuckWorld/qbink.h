#ifndef QBINK_H
#define QBINK_H

#include "bink.h"

extern void PTR4* (RADEXPLINK *qBinkLogoAddress)(void);
extern void (RADEXPLINK *qBinkSetError)(const char PTR4* err);
extern char PTR4* (RADEXPLINK *qBinkGetError)(void);
extern HBINK (RADEXPLINK *qBinkOpen)(const char PTR4* name,U32 flags);
extern void (RADEXPLINK *qBinkGetFrameBuffersInfo)( HBINK bink, BINKFRAMEBUFFERS * fbset );
extern void (RADEXPLINK *qBinkRegisterFrameBuffers)( HBINK bink, BINKFRAMEBUFFERS * fbset );
extern S32 (RADEXPLINK *qBinkDoFrame)(HBINK bnk);
extern void (RADEXPLINK *qBinkNextFrame)(HBINK bnk);
extern S32 (RADEXPLINK *qBinkWait)(HBINK bnk);
extern void (RADEXPLINK *qBinkClose)(HBINK bnk);
extern S32 (RADEXPLINK *qBinkPause)(HBINK bnk,S32 pause);
extern S32 (RADEXPLINK *qBinkCopyToBuffer)(HBINK bnk,void* dest,S32 destpitch,U32 destheight,U32 destx,U32 desty,U32 flags);
extern S32 (RADEXPLINK *qBinkCopyToBufferRect)(HBINK bnk,void* dest,S32 destpitch,U32 destheight,U32 destx,U32 desty,U32 srcx, U32 srcy, U32 srcw, U32 srch, U32 flags);
extern S32 (RADEXPLINK *qBinkGetRects)(HBINK bnk,U32 flags);
extern void (RADEXPLINK *qBinkGoto)(HBINK bnk,U32 frame,S32 flags);  // use 1 for the first frame
extern U32 (RADEXPLINK *qBinkGetKeyFrame)(HBINK bnk,U32 frame,S32 flags);
extern S32 (RADEXPLINK *qBinkSetVideoOnOff)(HBINK bnk,S32 onoff);
extern S32 (RADEXPLINK *qBinkSetSoundOnOff)(HBINK bnk,S32 onoff);
extern void (RADEXPLINK *qBinkFreeGlocalMemory)( void );
extern void (RADEXPLINK *qBinkSetVolume)(HBINK bnk, U32 trackid, S32 volume);
extern void (RADEXPLINK *qBinkSetPan)(HBINK bnk,U32 trackid, S32 pan);
extern void (RADEXPLINK *qBinkSetMixBins)(HBINK bnk,U32 trackid, U32 PTR4* mix_bins, U32 total);
extern void (RADEXPLINK *qBinkSetMixBinVolumes)(HBINK bnk,U32 trackid, U32 PTR4* vol_mix_bins, S32 PTR4* volumes, U32 total);
extern void (RADEXPLINK *qBinkService)(HBINK bink);
extern S32 (RADEXPLINK *qBinkShouldSkip)(HBINK bink);
extern void (RADEXPLINK *qBinkGetPalette)( void * out_pal );
extern S32 (RADEXPLINK *qBinkControlBackgroundIO)(HBINK bink,U32 control);
extern HBINKTRACK (RADEXPLINK *qBinkOpenTrack)(HBINK bnk,U32 trackindex);
extern void (RADEXPLINK *qBinkCloseTrack)(HBINKTRACK bnkt);
extern U32 (RADEXPLINK *qBinkGetTrackData)(HBINKTRACK bnkt,void PTR4* dest);
extern U32 (RADEXPLINK *qBinkGetTrackType)(HBINK bnk,U32 trackindex);
extern U32 (RADEXPLINK *qBinkGetTrackMaxSize)(HBINK bnk,U32 trackindex);
extern U32 (RADEXPLINK *qBinkGetTrackID)(HBINK bnk,U32 trackindex);
extern void (RADEXPLINK *qBinkGetSummary)(HBINK bnk,BINKSUMMARY PTR4* sum);
extern void (RADEXPLINK *qBinkGetRealtime)(HBINK bink,BINKREALTIME PTR4* run,U32 frames);
extern void (RADEXPLINK *qBinkSetSoundTrack)(U32 total_tracks, U32 PTR4* tracks);
extern void (RADEXPLINK *qBinkSetIO)(BINKIOOPEN io);
extern void (RADEXPLINK *qBinkSetFrameRate)(U32 forcerate,U32 forceratediv);
extern void (RADEXPLINK *qBinkSetSimulate)(U32 sim);
extern void (RADEXPLINK *qBinkSetIOSize)(U32 iosize);
extern S32  (RADEXPLINK *qBinkSetSoundSystem)(BINKSNDSYSOPEN open, U32 param);
extern void (RADEXPLINK *qBinkControlCPUFeatures)( S32 use, S32 dont_use );
extern BINKSNDOPEN (RADEXPLINK *qBinkOpenDirectSound)(U32 param); // don't call directly
extern BINKSNDOPEN (RADEXPLINK *qBinkOpenWaveOut)(U32 param); // don't call directly
extern BINKSNDOPEN (RADEXPLINK *qBinkOpenMiles)(U32 param); // don't call directly
extern S32 (RADEXPLINK *qBinkDX8SurfaceType)(void* lpD3Ds);
extern S32 (RADEXPLINK *qBinkDX9SurfaceType)(void* lpD3Ds);
extern HBINKBUFFER (RADEXPLINK *qBinkBufferOpen)( void* /*HWND*/ wnd, U32 width, U32 height, U32 bufferflags);
extern S32 (RADEXPLINK *qBinkBufferSetHWND)( HBINKBUFFER buf, void* /*HWND*/ newwnd);
extern S32 (RADEXPLINK *qBinkDDSurfaceType)(void PTR4* lpDDS);
extern S32 (RADEXPLINK *qBinkIsSoftwareCursor)(void PTR4* lpDDSP, void* /*HCURSOR*/ cur);
extern S32 (RADEXPLINK *qBinkCheckCursor)(void* /*HWND*/ wnd,S32 x,S32 y,S32 w,S32 h);
extern S32 (RADEXPLINK *qBinkBufferSetDirectDraw)(void PTR4* lpDirectDraw, void PTR4* lpPrimary);
extern void (RADEXPLINK *qBinkBufferClose)( HBINKBUFFER buf);
extern S32 (RADEXPLINK *qBinkBufferLock)( HBINKBUFFER buf);
extern S32 (RADEXPLINK *qBinkBufferUnlock)( HBINKBUFFER buf);
extern void (RADEXPLINK *qBinkBufferSetResolution)( S32 w, S32 h, S32 bits);
extern void (RADEXPLINK *qBinkBufferCheckWinPos)( HBINKBUFFER buf, S32 PTR4* NewWindowX, S32 PTR4* NewWindowY);
extern S32 (RADEXPLINK *qBinkBufferSetOffset)( HBINKBUFFER buf, S32 destx, S32 desty);
extern void (RADEXPLINK *qBinkBufferBlit)( HBINKBUFFER buf, BINKRECT PTR4* rects, U32 numrects );
extern S32 (RADEXPLINK *qBinkBufferSetScale)( HBINKBUFFER buf, U32 w, U32 h);
extern char PTR4* (RADEXPLINK *qBinkBufferGetDescription)( HBINKBUFFER buf);
extern char PTR4* (RADEXPLINK *qBinkBufferGetError)();
extern void (RADEXPLINK *qBinkRestoreCursor)(S32 checkcount);
extern S32 (RADEXPLINK *qBinkBufferClear)(HBINKBUFFER buf, U32 RGB);
extern void (RADEXPLINK *qBinkSetMemory)(BINKMEMALLOC a,BINKMEMFREE f);

void QBink_Init(void);
void QBink_Shutdown(void);

#endif