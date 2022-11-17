#include "graphics/graphics_engine.h"

MY_LIB_NAMESPACE_BEGIN

CGraphicsEngine* CGraphicsEngine::s_instance = nullptr;

CGraphicsEngine::CGraphicsEngine()
{

}

CGraphicsEngine::~CGraphicsEngine()
{

}

void CGraphicsEngine::CreateInstance()
{
	if (!s_instance)
	{
		s_instance = new CGraphicsEngine();
	}
}

CGraphicsEngine& CGraphicsEngine::GetInstance()
{
	if (!s_instance)
	{
		throw;
	}
	return *s_instance;
}

void CGraphicsEngine::Delete()
{
	if (s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

MY_LIB_NAMESPACE_END