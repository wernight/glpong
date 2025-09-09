/**
 * Copyright (c) 2003 Werner BEROUX. All Rights Reserved.
 *
 * The copyright to the contents herein is the property of Werner BEROUX.
 * The contents may be used and/or copied only with the written permission of
 * Werner BEROUX, or in accordance with the terms and conditions stipulated in
 * the agreement/contract under which the contents have been supplied.
 */

#include "SceneManager.h"

#include <memory>

SceneManager::SceneManager(void) {}

SceneManager::~SceneManager(void) {}

void SceneManager::AddObject(std::shared_ptr<IObject> object) { objects_.push_back(object); }

void SceneManager::RemoveObject(std::shared_ptr<IObject> object) {
  for (auto it = objects_.begin(); it != objects_.end(); ++it)
    if (*it == object) {
      objects_.erase(it);
      break;
    }
}

void SceneManager::Update(float dt) {
  for (auto& object : objects_) object->Update(dt);
}

void SceneManager::Render(const glm::mat4& model, const glm::mat4& view,
                          const glm::mat4& projection) const {
  for (const auto& object : objects_) object->Render(model, view, projection);
}

bool SceneManager::ProcessEvent(const SDL_Event& event) {
  for (auto& object : objects_)
    if (object->ProcessEvent(event)) return true;
  return false;
}
