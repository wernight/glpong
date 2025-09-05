/**
 * Copyright (c) 2003 Werner BEROUX. All Rights Reserved.
 *
 * The copyright to the contents herein is the property of Werner BEROUX.
 * The contents may be used and/or copied only with the written permission of
 * Werner BEROUX, or in accordance with the terms and conditions stipulated in
 * the agreement/contract under which the contents have been supplied.
 */

#include "StdAfx.h"
#include "SceneManager.h"

CSceneManager::CSceneManager(void)
{
}

CSceneManager::~CSceneManager(void)
{
}

/** Add object to the list.
 * @param pObject  Pointer to the new object to add.
 * @return True if successful.
 */
bool CSceneManager::AddObject(IObject *pObject)
{
  try
  {
    m_pvObjects.push_back(pObject);
  }
  catch (...)
  {
    return false;
  }
  return true;
}

/** Initialize the object.
 * Once OpenGL ready the initialize function of each object is called.
 * In this function object should initialize their OpenGL related data
 * and prepare to render.
 *
 * @return True if initialize successful and ready to update/render.
 */
bool CSceneManager::Initialize()
{
  std::vector<IObject*>::iterator  iter;

  for (iter=m_pvObjects.begin(); iter!=m_pvObjects.end(); ++iter)
    if (!(*iter)->Initialize())
      return false;
  return true;
}

/** Asks objects to update their content.
 * @param fTime    Time elapsed between two updates.
 */
void CSceneManager::Update(float fTime)
{
  std::vector<IObject*>::iterator  iter;

  for (iter=m_pvObjects.begin(); iter!=m_pvObjects.end(); ++iter)
    (*iter)->Update(fTime);
}

/** Asks objects to render.
 */
void CSceneManager::Render() const
{
  std::vector<IObject*>::const_iterator  iter;

  for (iter=m_pvObjects.begin(); iter!=m_pvObjects.end(); ++iter)
    (*iter)->Render();
}

/** Asks objects to process an event.
 * The object receive an event to process.
 * If he has processed this event and it should not be processed by
 * any other object, then it return true.
 *
 * @param nEvent  Type of event (mouse click, keyboard, ...).
 * @param wParam  A value depending of the event type.
 * @param lParam  A value depending of the event type.
 * @return True if the message has been processed.
 */
bool CSceneManager::ProcessEvent(IObject::EEvent nEvent, DWORD wParam, DWORD lParam)
{
  std::vector<IObject*>::iterator  iter;

  for (iter=m_pvObjects.begin(); iter!=m_pvObjects.end(); ++iter)
    if ((*iter)->ProcessEvent(nEvent, wParam, lParam))
      return true;
  return false;
}

