
#include <metahook.h>

#include <unordered_map>
#include <list>

class IResource
{
public:
	// return the data pointer if the resource is successfully load.
	//  if the resource is loading or bad load, return NULL.
	virtual void *GetData(void) = 0;
};

enum LoadingMode
{
	NOW,		// start load now, wait for loading.
	LATER,		// start load in background thread, return immediately.
	NEED,		// start load when you needed it ( when you call GetData ).
};

class IResourceManager
{
public:
	// load the file into memory
	virtual IResource *LoadResource(const char *filename, LoadingMode mode) = 0;
};


enum ResourceState
{
	INITIAL,
	LOADING,		// in queue
	INVALID,
	AVAILABLE,
};

//===================================================================
// A Simple Mutex
//===================================================================
class LoadingLock
{
public:
	LoadingLock()
	{
		InitializeCriticalSection(&m_cs);
	}

	~LoadingLock()
	{
		DeleteCriticalSection(&m_cs);
	}

	void Lock(void)
	{
		EnterCriticalSection(&m_cs);
	}

	void UnLock(void)
	{
		LeaveCriticalSection(&m_cs);
	}

private:
	CRITICAL_SECTION m_cs;
};

class CResourceManager;

class CResource : public IResource
{
public:
	void Init(CResourceManager *manager, const char *filename, LoadingMode mode);
	void Free(void);

	void StartLoading(void);
	void FinishLoading(bool fail);

	virtual void LoadData(void);
	virtual void FreeData(void);

	// access data of the resource.
	virtual void *GetData(void);

private:
	char *m_filename;
	void *m_data;
	CResourceManager *m_manager;
	LoadingMode m_mode;
	ResourceState m_state;	// need lock
	LoadingLock m_lock;
};

void CResource::Init(CResourceManager *manager, const char *filename, LoadingMode mode)
{
	m_manager = m_manager;
	m_mode = mode;
	m_state = INITIAL;
	m_filename = NULL;
	m_data = NULL;

	// duplicate string
	m_filename = new char[strlen(filename) + 1];
	strcpy(m_filename, filename);
}

void CResource::Free(void)
{
	if (m_state == AVAILABLE)
		FreeData();

	if (m_filename)
		delete[] m_filename;
}

void CResource::StartLoading(void)
{
	m_lock.Lock();
	m_state = LOADING;
	m_lock.UnLock();

	LoadData();
}

void CResource::FinishLoading(bool fail)
{
	// call from LoadData()

	m_lock.Lock();

	if (fail)
		m_state = INVALID;
	else
		m_state = AVAILABLE;

	m_lock.UnLock();
}

void CResource::LoadData(void)
{
}

void CResource::FreeData(void)
{
}

void *CResource::GetData(void)
{
	void	*data;

	m_lock.Lock();

	if (m_state == INITIAL)
	{
		// trying to get an uninitialized resource ..

		if (m_mode == NEED)
		{
			// this resource can start load now ..
			m_state = LOADING;
//			m_manager->AddToQueue(this);
		}
		else
		{
			// this resource does not request to load,
			//  so, it's a invalid resource.
			m_state = INVALID;
		}

		data = NULL;
	}
	else if (m_state == LOADING || m_state == INVALID)
	{
		// the resource in the loading queue (waiting for load),
		//  or load fail.
		data = NULL;
	}
	else
	{
		// the resource is available, return the data.
		data = m_data;
	}

	m_lock.UnLock();

	return data;
}

class CResourceBinary : public CResource
{
public:
};

class CResourceImage : public CResource
{
public:
};

class CResourceModel : public CResource
{
public:
};

class CResourceSound : public CResource
{
public:
};


enum ResourceType
{
	NONE,		// bad type
	BINARY,		// binary file
	IMAGE,		// image file
	MODEL,		// model file
	SOUND,		// sound file
};

typedef std::list<CResource*> ResourceList_t;

class CResourceManager : public IResourceManager
{
	friend CResource;	// for AddToQueue()

public:
	void Initialize(void);
	void Shutdown(void);

	IResource *LoadResource(const char *filename, LoadingMode mode);

private:
	// get type by file name.
	ResourceType FindResourceType(const char *filename);

	// add a resource to the loading queue, the resource will be load in the background thread.
	void AddToQueue(CResource *resource);

private:
	// background thread.
	static DWORD WINAPI ProcessQueue(LPVOID lpParam);

private:
	// hold all the resources
	ResourceList_t m_resourceList;

	LoadingLock m_lock;				// lock for 'm_loadingQueue'
	ResourceList_t m_loadingQueue;
	HANDLE m_events[2];
	HANDLE m_thread;
};

void CResourceManager::Initialize(void)
{
	// create events ( for the thread active or quit )
	m_events[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_events[1] = CreateEvent(NULL, FALSE, FALSE, NULL);

	// create queue processing thread
	m_thread = CreateThread(NULL, 0, ProcessQueue, this, 0, NULL);
}

void CResourceManager::Shutdown(void)
{
	// send quit signal
	SetEvent(m_events[1]);
	// if the thread is dormant, active it to receive quit signal.
	SetEvent(m_events[0]);

	// waiting for the thread exit
	WaitForSingleObject(m_thread, INFINITE);

	// free handles
	CloseHandle(m_events[0]);
	CloseHandle(m_events[1]);
	CloseHandle(m_thread);

	// clear the loading queue
	m_loadingQueue.clear();

	// free all the resources in list
	for (ResourceList_t::iterator it = m_resourceList.begin(); it != m_resourceList.end(); it++)
	{
		(*it)->Free();
		delete (*it);
	}
	m_resourceList.clear();
}

IResource *CResourceManager::LoadResource(const char *filename, LoadingMode mode)
{
	ResourceType type;
	CResource *resource;

	// we must know the type of resource file
	type = FindResourceType(filename);

	if (type == NONE)
	{
		// unknown type
		//  this resource never load
		resource = new CResource();
		resource->Init(this, filename, NOW);
		return resource;
	}

	// choose loader
	switch (type)
	{
	case BINARY:
		resource = new CResourceBinary();
		break;
	case IMAGE:
		resource = new CResourceImage();
		break;
	case MODEL:
		resource = new CResourceModel();
		break;
	case SOUND:
		resource = new CResourceSound();
		break;
	}

	// initialize the resource, ready for load
	resource->Init(this, filename, mode);

	// save the resource
	m_resourceList.push_back(resource);

	if (mode == NOW)
	{
		// load the resource now ..
		resource->StartLoading();
		return resource;
	}
	else if (mode == LATER)
	{
		// add to loading queue, return immediately
		AddToQueue(resource);
		return resource;
	}

	return resource;
}

ResourceType CResourceManager::FindResourceType(const char *filename)
{
	int			i, len;
	const char	*p;

	// invalid pointer
	if (filename == NULL)
		return NONE;
	// null name
	if (filename[0] == '\0')
		return NONE;

	len = strlen(filename);
	// last char
	p = filename + (len - 1);

	while (1)
	{
		if (*p == '\0' || *p == '/' || *p == '\\')
			return NONE;	// bad file name ( a bad char founded before "." )
		// found
		if (*p == '.')
			break;
		p--;
	}

	p++;	// skip "."

	if (stricmp(p, "dds") == 0)
		return IMAGE;
	else if (stricmp(p, "mdl") == 0)
		return MODEL;
	else if (stricmp(p, "wav") == 0)
		return SOUND;

	// a unknow type, use binary loader
	return BINARY;
}

void CResourceManager::AddToQueue(CResource *resource)
{
	m_lock.Lock();
	m_loadingQueue.push_back(resource);
	m_lock.UnLock();

	// active the thread
	SetEvent(m_events[0]);
}

DWORD WINAPI CResourceManager::ProcessQueue(LPVOID lpParam)
{
	CResourceManager *manager;

	manager = (CResourceManager *)lpParam;

	while (1)
	{
		// dormant, wait for active signal
		WaitForSingleObject(manager->m_events[0], INFINITE);

		while (1)
		{
			CResource *resource;

			// quit signal is received, exit the thread
			if (WaitForSingleObject(manager->m_events[1], 0) == WAIT_OBJECT_0)
				return 0;

			manager->m_lock.Lock();

			if (manager->m_loadingQueue.empty())
			{
				resource = NULL;
			}
			else
			{
				// get a resource and remove it in the queue
				resource = manager->m_loadingQueue.front();
				manager->m_loadingQueue.pop_front();
			}

			manager->m_lock.UnLock();

			// not any resources in the queue, dormant
			if (resource == NULL)
				break;

			resource->StartLoading();
		}
	}

	return 0;
}