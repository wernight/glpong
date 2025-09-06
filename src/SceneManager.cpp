/**
 * Copyright (c) 2003 Werner BEROUX. All Rights Reserved.
 *
 * The copyright to the contents herein is the property of Werner BEROUX.
 * The contents may be used and/or copied only with the written permission of
 * Werner BEROUX, or in accordance with the terms and conditions stipulated in
 * the agreement/contract under which the contents have been supplied.
 */

#include "StdAfx.h"
#include <memory>
#include "SceneManager.h"

SceneManager::SceneManager(void)
{
}

SceneManager::~SceneManager(void)
{
}

void SceneManager::AddObject(std::shared_ptr<IObject> object)
{
  objects_.push_back(object);
}

void SceneManager::RemoveObject(std::shared_ptr<IObject> object)
{
  for (auto it = objects_.begin(); it != objects_.end(); ++it)
    if (*it == object) {
      objects_.erase(it);
      break;
    }
}

void SceneManager::Update(float fTime)
{
  for (auto& object : objects_)
    object->Update(fTime);
}

/** Asks objects to render.
 */
void SceneManager::Render() const
{
  for (const auto& object : objects_)
    object->Render();
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
bool SceneManager::ProcessEvent(IObject::EEvent nEvent, DWORD wParam, DWORD lParam)
{
  for (auto& object : objects_)
    if (object->ProcessEvent(nEvent, wParam, lParam))
      return true;
  return false;
}

