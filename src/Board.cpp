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
#include "Board.h"
#include "GLPong.h"	// For the firework.

#define BORDER_BEVEL		4.0f
#define BORDER_WIDTH		4.0f

#define ILLUMINATE_DURATION 0.5f

#define DIGIT_HEIGHT		20.0f
#define DIGIT_WIDTH			12.0f
#define DIGIT_BORDER		2.0f
#define DIGIT_INNER_SPACING	0.6f
#define DIGIT_SPACING		3.0f

// Constructor
CBoard::CBoard() :
	m_nLeftScore(0),
	m_nRightScore(0),
	m_fIlluminateLeftBorder(0.0f),
	m_fIlluminateRightBorder(0.0f)
{
}


/** Initialize the object.
 * Once OpenGL ready the initialize function of each object is called.
 * In this function object should initialize their OpenGL related data
 * and prepare to render.
 *
 * @return True if initialize successful and ready to update/render.
 */
bool CBoard::Initialize()
{
	return true;
}

/** Update the object.
 * @param fTime		Time elapsed between two updates.
 */
void CBoard::Update(float fTime)
{
	// Illumination.
	if (m_fIlluminateLeftBorder > 0.0f)
		m_fIlluminateLeftBorder -= fTime;
	else
		m_fIlluminateLeftBorder = 0.0f;

	if (m_fIlluminateRightBorder > 0.0f)
		m_fIlluminateRightBorder -= fTime;
	else
		m_fIlluminateRightBorder = 0.0f;
}

/** Render the object.
 */
void CBoard::Render() const
{
	// Ground
	glBegin(GL_QUADS);
	// Background
	glColor3f(0.0f, 0.15f, 0.0f);
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(GetLeft(), GetTop(), 0);
	glVertex3f(GetLeft(), GetBottom(), 0);
	glVertex3f(GetRight(), GetBottom(), 0);
	glVertex3f(GetRight(), GetTop(), 0);

	glColor3f(0.0f, 0.4f, 0.0f);
	// Border top
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(GetLeft()+BORDER_WIDTH, GetTop()+BORDER_WIDTH, -BORDER_BEVEL);
	glVertex3f(GetLeft(), GetTop(), -BORDER_BEVEL);
	glVertex3f(GetRight(), GetTop(), -BORDER_BEVEL);
	glVertex3f(GetRight()-BORDER_WIDTH, GetTop()+BORDER_WIDTH, -BORDER_BEVEL);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(GetLeft(), GetTop(), -BORDER_BEVEL);
	glVertex3f(GetLeft(), GetTop(), 0);
	glVertex3f(GetRight(), GetTop(), 0);
	glVertex3f(GetRight(), GetTop(), -BORDER_BEVEL);

	// Border left
	glColor3f(0.0f+m_fIlluminateLeftBorder, 0.4f+m_fIlluminateLeftBorder, 0.0f+m_fIlluminateLeftBorder);
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(GetLeft()+BORDER_WIDTH, GetTop()+BORDER_WIDTH, -BORDER_BEVEL);
	glVertex3f(GetLeft()+BORDER_WIDTH, GetBottom()-BORDER_WIDTH, -BORDER_BEVEL);
	glVertex3f(GetLeft(), GetBottom(), -BORDER_BEVEL);
	glVertex3f(GetLeft(), GetTop(), -BORDER_BEVEL);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(GetLeft(), GetTop(), -BORDER_BEVEL);
	glVertex3f(GetLeft(), GetBottom(), -BORDER_BEVEL);
	glVertex3f(GetLeft(), GetBottom(), 0);
	glVertex3f(GetLeft(), GetTop(), 0);
	glColor3f(0.0f, 0.4f, 0.0f);

	// Border right
	glColor3f(0.0f+m_fIlluminateRightBorder, 0.4f+m_fIlluminateRightBorder, 0.0f+m_fIlluminateRightBorder);
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(GetRight(), GetTop(), -BORDER_BEVEL);
	glVertex3f(GetRight(), GetBottom(), -BORDER_BEVEL);
	glVertex3f(GetRight()-BORDER_WIDTH, GetBottom()-BORDER_WIDTH, -BORDER_BEVEL);
	glVertex3f(GetRight()-BORDER_WIDTH, GetTop()+BORDER_WIDTH, -BORDER_BEVEL);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(GetRight(), GetTop(), 0);
	glVertex3f(GetRight(), GetBottom(), 0);
	glVertex3f(GetRight(), GetBottom(), -BORDER_BEVEL);
	glVertex3f(GetRight(), GetTop(), -BORDER_BEVEL);
	glColor3f(0.0f, 0.4f, 0.0f);

	// Border bottom
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(GetLeft(), GetBottom(), -BORDER_BEVEL);
	glVertex3f(GetLeft()+BORDER_WIDTH, GetBottom()-BORDER_WIDTH, -BORDER_BEVEL);
	glVertex3f(GetRight()-BORDER_WIDTH, GetBottom()-BORDER_WIDTH, -BORDER_BEVEL);
	glVertex3f(GetRight(), GetBottom(), -BORDER_BEVEL);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(GetLeft()+BORDER_WIDTH, GetBottom()-BORDER_WIDTH, -BORDER_BEVEL);
	glVertex3f(GetLeft()+BORDER_WIDTH, GetBottom()-BORDER_WIDTH, 0);
	glVertex3f(GetRight()-BORDER_WIDTH, GetBottom()-BORDER_WIDTH, 0);
	glVertex3f(GetRight()-BORDER_WIDTH, GetBottom()-BORDER_WIDTH, -BORDER_BEVEL);
	glEnd();

	// Draw score
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glPushMatrix();
	glTranslatef(30.0f+DIGIT_WIDTH, GetTop()+20.0f, 0.0f);
	DrawDigitNumber(m_nLeftScore);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-30.0f, GetTop()+20.0f, 0.0f);
	DrawDigitNumber(m_nRightScore);
	glPopMatrix();

	glPopAttrib();
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
bool CBoard::ProcessEvent(EEvent nEvent, unsigned long wParam, unsigned long lParam)
{
	return false;
}

// Add points to a player's score.
void CBoard::Score(bool bLeftPlayer)
{
	int *pnScore;

	// Left or right player?
	if (bLeftPlayer)
	{
		pnScore = &m_nLeftScore;
		m_fIlluminateLeftBorder = ILLUMINATE_DURATION;
	}
	else
	{
		pnScore = &m_nRightScore;
		m_fIlluminateRightBorder = ILLUMINATE_DURATION;
	}

	// Update player's score.
	if (*pnScore < 30)
		*pnScore += 15;
	else
		*pnScore += 10;

	// The player won?
	if ((m_nLeftScore > 40 || m_nRightScore > 40) &&
		abs(m_nLeftScore-m_nRightScore) > 10)
	{
		// Affiche le feu d'artifice
		DrawFirework();
		m_nLeftScore = m_nRightScore = 0;
	}
}

void CBoard::DrawDigitNumber(int nNumber)
{
	bool	a,b,c,d,e,f,g;
	/*                                                   f
	__        __   __        __   __   __   __   __   a   e  _
	|  |    |  __|  __| |__| |__  |__     | |__| |__|    g   |_|
	|__|    | |__   __|    |  __| |__|    | |__|  __|  b   d | |
	c
	*/
	do
	{
		switch (nNumber%10)
		{
		case 0:
			a = true;	b = true;	c = true;	d = true;	e = true;	f = true;	g = false;
			break;
		case 1:
			a = false;	b = false;	c = false;	d = true;	e = true;	f = false;	g = false;
			break;
		case 2:
			a = false;	b = true;	c = true;	d = false;	e = true;	f = true;	g = true;
			break;
		case 3:
			a = false;	b = false;	c = true;	d = true;	e = true;	f = true;	g = true;
			break;
		case 4:
			a = true;	b = false;	c = false;	d = true;	e = true;	f = false;	g = true;
			break;
		case 5:
			a = true;	b = false;	c = true;	d = true;	e = false;	f = true;	g = true;
			break;
		case 6:
			a = true;	b = true;	c = true;	d = true;	e = false;	f = true;	g = true;
			break;
		case 7:
			a = false;	b = false;	c = false;	d = true;	e = true;	f = true;	g = false;
			break;
		case 8:
			a = true;	b = true;	c = true;	d = true;	e = true;	f = true;	g = true;
			break;
		case 9:
			a = true;	b = false;	c = true;	d = true;	e = true;	f = true;	g = true;
			break;
		}

		// Draw digital number
		glBegin(GL_QUADS);
		if (a)
		{
			glVertex2f(0.0f, DIGIT_HEIGHT-DIGIT_INNER_SPACING);
			glVertex2f(0.0f, DIGIT_HEIGHT/2.0f+DIGIT_INNER_SPACING);
			glVertex2f(-DIGIT_BORDER, DIGIT_HEIGHT/2.0f+DIGIT_BORDER/2.0f+DIGIT_INNER_SPACING);
			glVertex2f(-DIGIT_BORDER, DIGIT_HEIGHT-DIGIT_BORDER-DIGIT_INNER_SPACING);
		}
		if (b)
		{
			glVertex2f(0.0f, DIGIT_HEIGHT/2.0f-DIGIT_INNER_SPACING);
			glVertex2f(0.0f, DIGIT_INNER_SPACING);
			glVertex2f(-DIGIT_BORDER, DIGIT_BORDER+DIGIT_INNER_SPACING);
			glVertex2f(-DIGIT_BORDER, DIGIT_HEIGHT/2.0f-DIGIT_BORDER/2.0f-DIGIT_INNER_SPACING);
		}
		if (c)
		{
			glVertex2f(-DIGIT_INNER_SPACING, 0.0f);
			glVertex2f(-DIGIT_WIDTH+DIGIT_INNER_SPACING, 0.0f);
			glVertex2f(-DIGIT_WIDTH+DIGIT_INNER_SPACING+DIGIT_BORDER, DIGIT_BORDER);
			glVertex2f(-DIGIT_INNER_SPACING-DIGIT_BORDER, DIGIT_BORDER);
		}
		if (d)
		{
			glVertex2f(-DIGIT_WIDTH, DIGIT_HEIGHT/2.0f-DIGIT_INNER_SPACING);
			glVertex2f(-DIGIT_WIDTH+DIGIT_BORDER, DIGIT_HEIGHT/2.0f-DIGIT_BORDER/2.0f-DIGIT_INNER_SPACING);
			glVertex2f(-DIGIT_WIDTH+DIGIT_BORDER, DIGIT_BORDER+DIGIT_INNER_SPACING);
			glVertex2f(-DIGIT_WIDTH, DIGIT_INNER_SPACING);
		}
		if (e)
		{
			glVertex2f(-DIGIT_WIDTH, DIGIT_HEIGHT-DIGIT_INNER_SPACING);
			glVertex2f(-DIGIT_WIDTH+DIGIT_BORDER, DIGIT_HEIGHT-DIGIT_BORDER-DIGIT_INNER_SPACING);
			glVertex2f(-DIGIT_WIDTH+DIGIT_BORDER, DIGIT_HEIGHT/2.0f+DIGIT_BORDER/2.0f+DIGIT_INNER_SPACING);
			glVertex2f(-DIGIT_WIDTH, DIGIT_HEIGHT/2.0f+DIGIT_INNER_SPACING);
		}
		if (f)
		{
			glVertex2f(-DIGIT_INNER_SPACING, DIGIT_HEIGHT);
			glVertex2f(-DIGIT_INNER_SPACING-DIGIT_BORDER, DIGIT_HEIGHT-DIGIT_BORDER);
			glVertex2f(-DIGIT_WIDTH+DIGIT_INNER_SPACING+DIGIT_BORDER, DIGIT_HEIGHT-DIGIT_BORDER);
			glVertex2f(-DIGIT_WIDTH+DIGIT_INNER_SPACING, DIGIT_HEIGHT);
		}
		if (g)
		{
			// top
			glVertex2f(-DIGIT_INNER_SPACING, DIGIT_HEIGHT/2.0f);
			glVertex2f(-DIGIT_WIDTH+DIGIT_INNER_SPACING, DIGIT_HEIGHT/2.0f);
			glVertex2f(-DIGIT_WIDTH+DIGIT_BORDER, DIGIT_HEIGHT/2.0f+DIGIT_BORDER/2.0f);
			glVertex2f(-DIGIT_BORDER, DIGIT_HEIGHT/2.0f+DIGIT_BORDER/2.0f);
			// bottom
			glVertex2f(-DIGIT_INNER_SPACING, DIGIT_HEIGHT/2.0f);
			glVertex2f(-DIGIT_BORDER, DIGIT_HEIGHT/2.0f-DIGIT_BORDER/2.0f);
			glVertex2f(-DIGIT_WIDTH+DIGIT_BORDER, DIGIT_HEIGHT/2.0f-DIGIT_BORDER/2.0f);
			glVertex2f(-DIGIT_WIDTH+DIGIT_INNER_SPACING, DIGIT_HEIGHT/2.0f);
		}
		glEnd();
		glTranslatef(DIGIT_WIDTH+DIGIT_SPACING, 0.0f, 0.0f);
	} while ((nNumber/=10) != 0);
}

