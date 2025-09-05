/**
 * Copyright (c) 2003 Werner BEROUX. All Rights Reserved.
 *
 * The copyright to the contents herein is the property of Werner BEROUX.
 * The contents may be used and/or copied only with the written permission of
 * Werner BEROUX, or in accordance with the terms and conditions stipulated in
 * the agreement/contract under which the contents have been supplied.
 *
 * Render a scene and send events to the scene objects.
 *
 * \brief Create an OpenGL window and start the game.
 * \author Werner BEROUX
 * \date 2004.03.06
 * \warning The objects order in important.
 */

#ifndef INC_SCENE_MANAGER_H_
#define INC_SCENE_MANAGER_H_

#ifdef _WIN32
#pragma once
#endif

#include "IObject.h"
#include <vector>

class CSceneManager : public IObject
{
// Constructors
public:
  CSceneManager(void);
  virtual ~CSceneManager(void);

// Attributes

// Operations
  // Add object to the list.
  virtual bool  AddObject(IObject *pObject);

  // Initialize the object.
  virtual bool  Initialize();

  // Asks objects to update their content.
  virtual void  Update(float fTime);

  // Asks objects to render.
  virtual void  Render() const;

  // Asks objects to process an event.
  virtual bool  ProcessEvent(IObject::EEvent nEvent, DWORD wParam, DWORD lParam);

// Implementation
private:
  std::vector<IObject*> m_pvObjects;
};

#endif

