/**
 * Copyright (c) 2003 Werner BEROUX. All Rights Reserved.
 *
 * The copyright to the contents herein is the property of Werner BEROUX.
 * The contents may be used and/or copied only with the written permission of
 * Werner BEROUX, or in accordance with the terms and conditions stipulated in
 * the agreement/contract under which the contents have been supplied.
 *
 * Scene object that can be rendered.
 *
 * @brief Create an OpenGL window and start the game.
 * @author Werner BEROUX
 * @date 2004.03.17
 */

/**
 * Copyright (c) 2003 Werner BEROUX
 * Mail: werner@beroux.com
 * Web : www.beroux.com
 */

#ifndef INC_INTERFACE_OBJECT_H_
#define INC_INTERFACE_OBJECT_H_

#ifdef _WIN32
#pragma once
#endif

class IObject
{
public:
  enum EEvent
  {
    eventMouseMove,    // xPos = LOWORD(lParam); yPos = HIWORD(lParam);
    eventMouseClick,  // xPos = LOWORD(lParam); yPos = HIWORD(lParam);
    eventChar,      // nVirtKey = (int) wParam; lKeyData = wParam;
    eventKeyDown,    // nVirtKey = (int) wParam; lKeyData = wParam;
    eventKeyUp      // nVirtKey = (int) wParam; lKeyData = wParam;
  };

// Constructors
  virtual ~IObject() {};

  /** Initialize the object.
   * Once OpenGL ready the initialize function of each object is called.
   * In this function object should initialize their OpenGL related data
   * and prepare to render.
   *
   * @return True if initialize successful and ready to update/render.
   */
  virtual bool Initialize() = 0;

// Operations
  /** Update the object.
   * @param fTime    Time elapsed between two updates.
   */
  virtual void Update(float fTime) = 0;

  /** Render the object.
   */
  virtual void Render() const = 0;

  /** Process event.
   * The object receive an event to process.
   * If he has processed this event and it should not be processed by
   * any other object, then it return true.
   *
   * @param nEvent  Type of event (mouse click, keyboard, ...).
   * @param wParam  A value depending of the event type.
   * @param lParam  A value depending of the event type.
   * @return True if the message has been processed.
   */
  virtual bool ProcessEvent(EEvent nEvent, unsigned long wParam, unsigned long lParam) = 0;
};

#endif

