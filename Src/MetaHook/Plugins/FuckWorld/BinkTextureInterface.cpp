
#include <metahook.h>
#include "bink/bink.h"
#include "qgl.h"

#include "BinkTextureInterface.h"


static bool g_bink_initialized = false;

class BinkTextureObject : public IBinkTextureObject {
public:
	BinkTextureObject();
	~BinkTextureObject();
	bool open(const char *fileName) override;
	void close() override;
	void updateFrame() override;
	void updateTexture() override;
	void getTexture(unsigned int *texture, float *coords) override;
	bool hasAlphaChannel() override;
	void setPause(bool state) override;
	void setFrame(unsigned int frame) override;
	void setLoop(bool state) override;
private:
	HBINK m_bikHeader;				// bink file header
	bool m_hasAlphaChannel;			// video has an alpha channel ?
	U32 m_bikWidth;					// width of video
	U32 m_bikHeight;				// height of video
	U32 m_bikPitch;					// byte size of one row of pixels
	U32 m_bikCopyFlags;				// use in BinkCopyToBuffer
	GLuint m_glPixelBuffers[2];		// opengl pixel buffer object
	GLsizei m_glPixelBufferSize;	// byte size of pixel buffer
	GLuint m_glTexture;				// opengl texture object
	GLsizei m_glTextureWidth;		// width of created opengl texture
	GLsizei m_glTextureHeight;		// height of created opengl texture
	GLenum m_glTextureFormat;		// use in glTexImage/glTexSubImage e.g GL_RGB/GL_RGBA
	GLfloat m_glTextureCoords[4];	// { left, right, top, bottom }
	bool m_loop;					// loop video
};

IBinkTextureObject *createBinkTextureObject() {
	return new BinkTextureObject();
}

void deleteBinkTextureObject(IBinkTextureObject *object) {
	if (object) {
		delete object;
	}
}

static GLsizei suggestTexSize(GLsizei size) {
	GLsizei texSize = 1;
	while (texSize < size) {
		texSize <<= 1;
	}
	return texSize;
}

BinkTextureObject::BinkTextureObject() {
	m_bikHeader = nullptr;
	m_hasAlphaChannel = false;
	m_bikWidth = 0;
	m_bikHeight = 0;
	m_bikPitch = 0;
	m_bikCopyFlags = 0;
	m_glPixelBuffers[0] = m_glPixelBuffers[1] = 0;
	m_glPixelBufferSize = 0;
	m_glTexture = 0;
	m_glTextureWidth = 0;
	m_glTextureHeight = 0;
	m_glTextureFormat = 0;
	m_glTextureCoords[0] = m_glTextureCoords[1] =
		m_glTextureCoords[2] = m_glTextureCoords[3] = 0;
	m_loop = false;
}

BinkTextureObject::~BinkTextureObject() {
	close();
}

bool BinkTextureObject::open(const char *fileName) {
	if (!g_bink_initialized) {
		g_bink_initialized = true;
		BinkSoundUseWaveOut();
	}
	if (m_bikHeader) {
		// you need to close current video
		return false;
	}
	m_bikHeader = BinkOpen(fileName, BINKALPHA | BINKSNDTRACK);
	if (m_bikHeader == nullptr) {
		return false;
	}
	BinkPause(m_bikHeader, true);
	m_hasAlphaChannel = (m_bikHeader->BinkType & BINKALPHA) ? true : false;
	m_bikWidth = m_bikHeader->Width;
	m_bikHeight = m_bikHeader->Height;
	m_bikPitch = m_bikWidth * (m_hasAlphaChannel ? 4 : 3);
	m_bikCopyFlags = BINKCOPYALL | (m_hasAlphaChannel ? BINKSURFACE32RA : BINKSURFACE24R);
	// create pixel buffer
	m_glPixelBufferSize = m_bikPitch * m_bikHeight;
	qglGenBuffersARB(2, m_glPixelBuffers);
	qglBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_glPixelBuffers[0]);
	qglBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_glPixelBufferSize, nullptr, GL_STREAM_DRAW_ARB);
	qglBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_glPixelBuffers[1]);
	qglBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_glPixelBufferSize, nullptr, GL_STREAM_DRAW_ARB);
	qglBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
	// create texture
	m_glTextureFormat = (m_hasAlphaChannel ? GL_RGBA : GL_RGB);
	m_glTextureWidth = suggestTexSize(m_bikWidth);
	m_glTextureHeight = suggestTexSize(m_bikHeight);
	qglGenTextures(1, &m_glTexture);
	qglBindTexture(GL_TEXTURE_2D, m_glTexture);
	qglTexImage2D(GL_TEXTURE_2D, 0, m_glTextureFormat, m_glTextureWidth, m_glTextureHeight, 0, m_glTextureFormat, GL_UNSIGNED_BYTE, nullptr);
	qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	qglBindTexture(GL_TEXTURE_2D, 0);
	// calculate texture coords
	m_glTextureCoords[0] = 0.0f;
	m_glTextureCoords[1] = (GLfloat)m_bikWidth / (GLfloat)m_glTextureWidth;
	m_glTextureCoords[2] = 0.0f;
	m_glTextureCoords[3] = (GLfloat)m_bikHeight / (GLfloat)m_glTextureHeight;
	// player control
	m_loop = false;
	return true;
}

void BinkTextureObject::close() {
	if (m_bikHeader) {
		BinkClose(m_bikHeader);
		m_bikHeader = nullptr;
	}
	m_hasAlphaChannel = false;
	m_bikWidth = 0;
	m_bikHeight = 0;
	m_bikPitch = 0;
	m_bikCopyFlags = 0;
	for (int i = 0; i < 2; ++i) {
		if (m_glPixelBuffers[i]) {
			qglDeleteBuffersARB(1, &m_glPixelBuffers[i]);
			m_glPixelBuffers[i] = 0;
		}
	}
	m_glPixelBufferSize = 0;
	if (m_glTexture) {
		qglDeleteTextures(1, &m_glTexture);
		m_glTexture = 0;
	}
	m_glTextureWidth = 0;
	m_glTextureHeight = 0;
	m_glTextureFormat = 0;
	m_loop = false;
}

void BinkTextureObject::updateFrame() {
	if (m_bikHeader == nullptr) {
		return;
	}
	BinkDoFrame(m_bikHeader);
	if (BinkWait(m_bikHeader)) {
		return;
	}
	while (BinkShouldSkip(m_bikHeader)) {
		BinkNextFrame(m_bikHeader);
		BinkDoFrame(m_bikHeader);
	}
	if (m_bikHeader->FrameNum >= m_bikHeader->Frames) {
		if (m_loop) {
			BinkGoto(m_bikHeader, 0, 0);
		}
		return;
	}
	BinkNextFrame(m_bikHeader);
}

void BinkTextureObject::updateTexture() {
	static int index = 0;
	int nextIndex = 0;
	// swap pixel buffer
	index = (index + 1) % 2;
	nextIndex = (index + 1) % 2;
	// copy pixel from front buffer to texture
	qglBindTexture(GL_TEXTURE_2D, m_glTexture);
	qglBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_glPixelBuffers[index]);
	qglTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_bikWidth, m_bikHeight, m_glTextureFormat, GL_UNSIGNED_BYTE, nullptr);
	// copy pixel from bink to back buffer
	qglBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_glPixelBuffers[nextIndex]);
	qglBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_glPixelBufferSize, nullptr, GL_STREAM_DRAW_ARB);
	void *ptr = qglMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_READ_WRITE_ARB);
	if (ptr) {
		BinkCopyToBuffer(m_bikHeader, ptr, m_bikPitch, m_bikHeight, 0, 0, m_bikCopyFlags);
		qglUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB);
	}
	qglBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
}

void BinkTextureObject::getTexture(unsigned int *texture, float *coords) {
	if (texture) {
		*texture = m_glTexture;
	}
	if (coords) {
		for (int i = 0; i < 4; ++i) {
			coords[i] = m_glTextureCoords[i];
		}
	}
}

bool BinkTextureObject::hasAlphaChannel() {
	return m_hasAlphaChannel;
}

void BinkTextureObject::setPause(bool state) {
	if (m_bikHeader) {
		BinkPause(m_bikHeader, state ? 1 : 0);
	}
}

void BinkTextureObject::setFrame(unsigned int frame) {
	if (m_bikHeader) {
		BinkGoto(m_bikHeader, frame, 0);
	}
}

void BinkTextureObject::setLoop(bool state) {
	m_loop = state;
}
