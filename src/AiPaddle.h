/**
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * Copyright (c) 2003 Werner BEROUX
 * Mail: werner@beroux.com
 * Web : www.beroux.com
 */

#ifndef INC_AI_PADDLE_H_
#define INC_AI_PADDLE_H_

#ifdef _WIN32
#pragma once
#endif

#include "Paddle.h"
#include "Ball.h"

class CAiPaddle : public CPaddle
{
public:
// Constructor
  CAiPaddle(bool bLeftPaddle);

  // Create the AI.
  bool Create(CBall *pBall);

// Overrides
/*  // Initialize the object.
  bool Initialize();*/

  // Update the object.
  void Update(float fTime);

  // Process event.
  bool ProcessEvent(EEvent nEvent, unsigned long wParam, unsigned long lParam);

// Implementation
private:
  CBall  *m_pBall;
};

#endif

