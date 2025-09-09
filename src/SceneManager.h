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

#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "IObject.h"

class SceneManager : public IObject {
  // Constructors
 public:
  SceneManager();
  virtual ~SceneManager();

  // Attributes

  // Operations
  // Add object to the list.
  void AddObject(std::shared_ptr<IObject> object);
  void RemoveObject(std::shared_ptr<IObject> object);

  // Asks objects to update their content.
  virtual void Update(float dt) override;

  // Asks objects to render.
  virtual void Render(const glm::mat4& view, const glm::mat4& model,
                      const glm::mat4& projection) const override;

  // Asks objects to process an event.
  virtual bool ProcessEvent(const SDL_Event& event) override;

  // Implementation
 private:
  std::vector<std::shared_ptr<IObject>> objects_;
};
