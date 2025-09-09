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

#pragma once

#include <SDL2/SDL_events.h>
#include <glm/glm.hpp>

class IObject {
 public:


  // Constructors
  virtual ~IObject() {};

  // Operations
  /** Update the object.
   * @param fTime    Time elapsed between two updates.
   */
  virtual void Update(float fTime) = 0;

  /** Render the object.
   */
  virtual void Render(const glm::mat4& view, const glm::mat4& model,
                      const glm::mat4& projection) const = 0;

  /** Process event.
   * The object receive an event to process.
   * If he has processed this event and it should not be processed by
   * any other object, then it return true.
   *
   * @param event  The SDL event to process.
   * @return True if the message has been processed.
   */
  virtual bool ProcessEvent(const SDL_Event& event) = 0;
};
