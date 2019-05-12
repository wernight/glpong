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

#include "StdAfx.h"
#include "Paddle.h"
#include "Board.h"

#define PADDLE_BEVEL			4.0f
#define	PADDLE_SPEED			150.0f
#define	PADDLE_ILLUMINATE		0.5f
#define	PADDLE_ILLUMINATE_FADE	0.3f

// Constructor
CPaddle::CPaddle(bool bLeftPaddle) :
	m_fSpeed(0.0f),
	m_fY(0.0f),
	m_fIlluminate(0.0f)
{
	m_bLeftPaddle = bLeftPaddle;
}

/** Initialize the object.
 * Once OpenGL ready the initialize function of each object is called.
 * In this function object should initialize their OpenGL related data
 * and prepare to render.
 *
 * @return True if initialize successful and ready to update/render.
 */
bool CPaddle::Initialize()
{
	m_nGLPaddle = glGenLists(1);
	glNewList(m_nGLPaddle, GL_COMPILE);
	if (m_bLeftPaddle)
	{
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, -1.0f);
		glVertex3f(CBoard::GetLeft(), GetHeight()/2.0f, -PADDLE_BEVEL);
		glVertex3f(CBoard::GetLeft(), -GetHeight()/2.0f, -PADDLE_BEVEL);
		glVertex3f(CBoard::GetLeft()-GetWidth(), -GetHeight()/2.0f, -PADDLE_BEVEL);
		glVertex3f(CBoard::GetLeft()-GetWidth(), GetHeight()/2.0f, -PADDLE_BEVEL);

		glNormal3f(-1.0f, 0.0f, 0.0f);
		glVertex3f(CBoard::GetLeft()-GetWidth(), GetHeight()/2.0f, -PADDLE_BEVEL);
		glVertex3f(CBoard::GetLeft()-GetWidth(), -GetHeight()/2.0f, -PADDLE_BEVEL);
		glVertex3f(CBoard::GetLeft()-GetWidth(), -GetHeight()/2.0f, 0);
		glVertex3f(CBoard::GetLeft()-GetWidth(), GetHeight()/2.0f, 0);

		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(CBoard::GetLeft(), -GetHeight()/2.0f, -PADDLE_BEVEL);
		glVertex3f(CBoard::GetLeft(), -GetHeight()/2.0f, 0);
		glVertex3f(CBoard::GetLeft()-GetWidth(), -GetHeight()/2.0f, 0);
		glVertex3f(CBoard::GetLeft()-GetWidth(), -GetHeight()/2.0f, -PADDLE_BEVEL);
		glEnd();
	}
	else
	{
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, -1.0f);
		glVertex3f(CBoard::GetRight()+GetWidth(), GetHeight()/2.0f, -PADDLE_BEVEL);
		glVertex3f(CBoard::GetRight()+GetWidth(), -GetHeight()/2.0f, -PADDLE_BEVEL);
		glVertex3f(CBoard::GetRight(), -GetHeight()/2.0f, -PADDLE_BEVEL);
		glVertex3f(CBoard::GetRight(), GetHeight()/2.0f, -PADDLE_BEVEL);

		glNormal3f(1.0f, 0.0f, 0.0f);
		glVertex3f(CBoard::GetRight()+GetWidth(), GetHeight()/2.0f, 0);
		glVertex3f(CBoard::GetRight()+GetWidth(), -GetHeight()/2.0f, 0);
		glVertex3f(CBoard::GetRight()+GetWidth(), -GetHeight()/2.0f, -PADDLE_BEVEL);
		glVertex3f(CBoard::GetRight()+GetWidth(), GetHeight()/2.0f, -PADDLE_BEVEL);

		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(CBoard::GetRight()+GetWidth(), -GetHeight()/2.0f, -PADDLE_BEVEL);
		glVertex3f(CBoard::GetRight()+GetWidth(), -GetHeight()/2.0f, 0);
		glVertex3f(CBoard::GetRight(), -GetHeight()/2.0f, 0);
		glVertex3f(CBoard::GetRight(), -GetHeight()/2.0f, -PADDLE_BEVEL);
		glEnd();
	}
	glEndList();
	return true;
}

/** Update the object.
 * @param fTime		Time elapsed between two updates.
 */
void CPaddle::Update(float fTime)
{
	// Update paddle position.
	m_fY += m_fSpeed * fTime;
	if (m_fY > CBoard::GetTop() - GetHeight()/2.0f)
	{
		m_fY = CBoard::GetTop() - GetHeight()/2.0f;;
		m_fSpeed = 0.0f;
	}
	if (m_fY < CBoard::GetBottom() + GetHeight()/2.0f)
	{
		m_fY = CBoard::GetBottom() + GetHeight()/2.0f;
		m_fSpeed = 0.0f;
	}

	// Fade hightlight.
	if (m_fIlluminate > 0.0f)
		m_fIlluminate -= PADDLE_ILLUMINATE_FADE*fTime;
	else
		m_fIlluminate = 0.0f;
}

/** Render the object.
 */
void CPaddle::Render() const
{
	// Paddle.
	glPushMatrix();
	glTranslatef(0, m_fY, 0);
	glColor3f(m_fIlluminate, 1.0f, m_fIlluminate);
	glCallList(m_nGLPaddle);
	glPopMatrix();
}

/** Process event.
 * The object receive an event to process.
 * If he has processed this event and it should not be processed by
 * any other object, then it return true.
 *
 * @param nEvent	Type of event (mouse click, keyboard, ...).
 * @param wParam	A value depending of the event type.
 * @param lParam	A value depending of the event type.
 * @return True if the message has been processed.
 */
bool CPaddle::ProcessEvent(EEvent nEvent, unsigned long wParam, unsigned long lParam)
{
	switch (nEvent)
	{
	case eventKeyDown:
		switch (wParam)
		{
		case SDLK_LSHIFT:
			if (m_bLeftPaddle)
			{
				MoveUp();
				return true;
			}
			break;
		case SDLK_LCTRL:
			if (m_bLeftPaddle)
			{
				MoveDown();
				return true;
			}
			break;
		case SDLK_UP:
			if (!m_bLeftPaddle)
			{
				MoveUp();
				return true;
			}
			break;
		case SDLK_DOWN:
			if (!m_bLeftPaddle)
			{
				MoveDown();
				return true;
			}
			break;
		}
		break;

	case eventKeyUp:
		switch (lParam)
		{
		case SDLK_LSHIFT:
		case SDLK_LCTRL:
			if (m_bLeftPaddle)
			{
				Stop();
				return true;
			}
			break;
		case SDLK_UP:
		case SDLK_DOWN:
			if (!m_bLeftPaddle)
			{
				Stop();
				return true;
			}
			break;
		}
		break;
	}

	return false;
}

// Move paddle upward.
void CPaddle::MoveUp()
{
	m_fSpeed = PADDLE_SPEED;
}

// Move paddle downward.
void CPaddle::MoveDown()
{
	m_fSpeed = -PADDLE_SPEED;
}

// Stop moving paddle.
void CPaddle::Stop()
{
	m_fSpeed = 0.0f;
}

// Illuminate paddle.
void CPaddle::Illuminate()
{
	m_fIlluminate = PADDLE_ILLUMINATE;
}

