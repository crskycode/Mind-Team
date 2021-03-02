
#ifndef __BINKTEXTUREINTERFACE_H__
#define __BINKTEXTUREINTERFACE_H__

class IBinkTextureObject {
public:
	virtual ~IBinkTextureObject() {}
	virtual bool open(const char *fileName) = 0;							// open bink file
	virtual void close() = 0;												// close bink file
	virtual void updateFrame() = 0;											// update player
	virtual void updateTexture() = 0;										// update opengl texture of current frame
	virtual void getTexture(unsigned int *texture, float *coords) = 0;		// get opengl texture object
	virtual bool hasAlphaChannel() = 0;										// the video has an alpha channel ?
	virtual void setPause(bool state) = 0;									// set video pause
	virtual void setFrame(unsigned int frame) = 0;							// set video current frame
	virtual void setLoop(bool state) = 0;									// set video loop
};

// factory
IBinkTextureObject *createBinkTextureObject();					// create bink texture object
void deleteBinkTextureObject(IBinkTextureObject *object);		// delete bink texture object

#endif
