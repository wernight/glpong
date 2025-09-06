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
#include "Ball.h"
#include "GLPong.h"

#define BALL_SPEED      110.0f
#define BALL_SPEED_INCREASE  5.0f
#define BALL_RADIUS      2.0f
#define BALL_MAX_ANGLE    M_PI/3.0f  // y = a*x
#define BALL_MIN_ANGLE    M_PI/7.0f  // y = a*x
#define PART_SIZE      1.7f

// Constructor
CBall::CBall()
{
}

CBall::~CBall()
{
}

// Create the ball.
bool CBall::Create(CBoard *pBoard, CPaddle *pLeftPaddle, CPaddle *pRightPaddle, GLuint texture)
{
  int i;

  m_pBoard = pBoard;
  m_pLeftPaddle = pLeftPaddle;
  m_pRightPaddle = pRightPaddle;
  m_texture = texture;

  // Create a new ball.
  m_vBallPosition.y = 0.0f;
  NewBall(m_rand.RandomInt()%2 == 0);

  // Init particles.
  for (i=0; i<sizeof(m_particles)/sizeof(m_particles[0]); ++i)  // i Through All The Particles
  {
    m_particles[i].life = 1.0f;
    m_particles[i].fade = (float) m_rand.RandomRange(3.0f, 28.0f);  // Random Fade Value
    m_particles[i].x = m_vBallSpeed.x;
    m_particles[i].y = m_vBallSpeed.y;
    m_particles[i].z = -BALL_RADIUS;
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
bool CBall::Initialize()
{
  GLUquadricObj  *pQuadObj;

  // Initiliaz random number generator.
  m_rand.Randomize();

  // Ball
  m_nList = glGenLists(1);
  glNewList(m_nList, GL_COMPILE);
    glColor3f(0.0f, 1.0f, 0.0f);
    pQuadObj = gluNewQuadric();
    gluQuadricNormals(pQuadObj, GLU_SMOOTH);
    gluSphere(pQuadObj, BALL_RADIUS, 7, 5);
    gluDeleteQuadric(pQuadObj);
  glEndList();
  return true;
}

/** Update the object.
 * @param fTime    Time elapsed between two updates.
 */
void CBall::Update(float fTime)
{
  CVector2D  vNewBallPos(m_vBallPosition + m_vBallSpeed*fTime);
  double    dAngle,
        dSpeed;
  int i;

  // Bounce top/bottom
  if (vNewBallPos.y+BALL_RADIUS > CBoard::GetTop())
  {
    m_vBallSpeed.y = -m_vBallSpeed.y;
    vNewBallPos.y = 2.0f*(CBoard::GetTop()-BALL_RADIUS) - vNewBallPos.y;
  }
  else if (vNewBallPos.y-BALL_RADIUS < CBoard::GetBottom())
  {
    m_vBallSpeed.y = -m_vBallSpeed.y;
    vNewBallPos.y = 2.0f*(CBoard::GetBottom()+BALL_RADIUS) - vNewBallPos.y;
  }

  // Left paddle collision detection.
  if (vNewBallPos.x+BALL_RADIUS > CBoard::GetLeft()-CPaddle::GetWidth())
  {
    // y = a*x + b
    float  a = m_vBallSpeed.y / m_vBallSpeed.x,
        b = m_vBallPosition.y - a*m_vBallPosition.x,
        y;

    // Bounce on paddle?
    if (m_vBallPosition.x+BALL_RADIUS <= CBoard::GetLeft()-CPaddle::GetWidth() &&
      (y = a*(CBoard::GetLeft()-CPaddle::GetWidth()-BALL_RADIUS) + b)-BALL_RADIUS <= m_pLeftPaddle->GetPosition()+CPaddle::GetHeight()*0.5f &&
      y+BALL_RADIUS >= m_pLeftPaddle->GetPosition()-CPaddle::GetHeight()*0.5f)
    {
      // Illuminate.
      m_pLeftPaddle->Illuminate();
      // Bounce.
      vNewBallPos.x = 2.0f*(CBoard::GetLeft()-CPaddle::GetWidth()-BALL_RADIUS) - vNewBallPos.x;
      // Bouce angle.
      dAngle = (m_pLeftPaddle->GetPosition() - vNewBallPos.y) * M_PI/4.0f / (CPaddle::GetHeight()/2.0f) + M_PI;

      // Increase speed
      dSpeed = m_vBallSpeed.Norm() + BALL_SPEED_INCREASE;
      m_vBallSpeed.x = float(cos(dAngle)*dSpeed);
      m_vBallSpeed.y = float(sin(dAngle)*dSpeed);
    }
/*    // Bounce on corners?
    else if (HitPoint(m_vBallPosition, vNewBallPos, m_vBallSpeed, CVector2D(CBoard::GetLeft()-CPaddle::GetWidth(), m_pLeftPaddle->GetPosition()+CPaddle::GetHeight()*0.5f)) ||
         HitPoint(m_vBallPosition, vNewBallPos, m_vBallSpeed, CVector2D(CBoard::GetLeft()-CPaddle::GetWidth(), m_pLeftPaddle->GetPosition()-CPaddle::GetHeight()*0.5f)))
    { }*/
    // Score?
    else if (vNewBallPos.x+BALL_RADIUS > CBoard::GetLeft())
    {
      m_pBoard->Score(true);
      NewBall(true);
      vNewBallPos = m_vBallPosition;
    }
  }
  // Right paddle collision detection.
  else if (vNewBallPos.x-BALL_RADIUS < CBoard::GetRight()+CPaddle::GetWidth())
  {
    // y = a*x + b
    float  a = m_vBallSpeed.y / m_vBallSpeed.x,
        b = m_vBallPosition.y - a*m_vBallPosition.x,
        y;

    // Bounce on paddle?
    if (m_vBallPosition.x-BALL_RADIUS >= CBoard::GetRight()+CPaddle::GetWidth() &&
      (y = a*(CBoard::GetRight()+CPaddle::GetWidth()+BALL_RADIUS) + b)-BALL_RADIUS <= m_pRightPaddle->GetPosition()+CPaddle::GetHeight()*0.5f &&
      y+BALL_RADIUS >= m_pRightPaddle->GetPosition()-CPaddle::GetHeight()*0.5f)
    {
      // Illuminate.
      m_pRightPaddle->Illuminate();
      // Bounce.
      vNewBallPos.x = 2.0f*(CBoard::GetRight()+CPaddle::GetWidth()+BALL_RADIUS) - vNewBallPos.x;
      // Bouce angle.
      dAngle = (vNewBallPos.y - m_pRightPaddle->GetPosition()) * M_PI/4.0f / (CPaddle::GetHeight()/2.0f);

      // Increase speed
      dSpeed = m_vBallSpeed.Norm() + BALL_SPEED_INCREASE;
      m_vBallSpeed.x = float(cos(dAngle)*dSpeed);
      m_vBallSpeed.y = float(sin(dAngle)*dSpeed);
    }
/*    // Bounce on corners?
    else if (HitPoint(m_vBallPosition, vNewBallPos, m_vBallSpeed, CVector2D(CBoard::GetRight()+CPaddle::GetWidth(), m_pRightPaddle->GetPosition()+CPaddle::GetHeight()*0.5f)) ||
         HitPoint(m_vBallPosition, vNewBallPos, m_vBallSpeed, CVector2D(CBoard::GetRight()+CPaddle::GetWidth(), m_pRightPaddle->GetPosition()-CPaddle::GetHeight()*0.5f)))
    { }*/
    // Score?
    else if (vNewBallPos.x-BALL_RADIUS < CBoard::GetRight())
    {
      m_pBoard->Score(false);
      NewBall(false);
      vNewBallPos = m_vBallPosition;
    }
  }

  // Ball's position.
  m_vBallPosition = vNewBallPos;

  // Particles.
  for (i=0; i<sizeof(m_particles)/sizeof(m_particles[0]); ++i)// i Through All The Particles
  {
    // Update
    m_particles[i].life -= m_particles[i].fade*fTime;  // Reduce Particles Life By 'Fade'

    // Regenerate
    if (m_particles[i].life < 0.0f)        // If Particle Is Burned Out
    {
      m_particles[i].life = 1.0f;
      m_particles[i].fade = (float) m_rand.RandomRange(3.0f, 28.0f);  // Random Fade Value
      m_particles[i].x = m_vBallPosition.x + (float)m_rand.RandomRange(-BALL_RADIUS*0.5f, BALL_RADIUS*0.5f);
      m_particles[i].y = m_vBallPosition.y + (float)m_rand.RandomRange(-BALL_RADIUS*0.5f, BALL_RADIUS*0.5f);
      m_particles[i].z = -BALL_RADIUS + (float)m_rand.RandomRange(-BALL_RADIUS*0.5f, BALL_RADIUS*0.5f);
    }
  }
}

/** Render the object.
 */
void CBall::Render() const
{
  float       fModelView[16];
  CVector3D    vCenter,
          vRight,
          vUp,
          vVertex;
  float      ps;
  int  i;

  // Ball
  glPushMatrix();
  glTranslatef(m_vBallPosition.x, m_vBallPosition.y, -BALL_RADIUS);
  glCallList(m_nList);
  glPopMatrix();

  // Billboarding
  glGetFloatv(GL_MODELVIEW_MATRIX, fModelView);

  vRight.x = fModelView[0];
  vRight.y = fModelView[4];
  vRight.z = fModelView[8];

  vUp.x = fModelView[1];
  vUp.y = fModelView[5];
  vUp.z = fModelView[9];

  // Particules
  glColor3f(0.0f, 1.0f, 0.0f);
  if (m_texture != 0)
    glBindTexture(GL_TEXTURE_2D, m_texture);
  glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glBegin(GL_QUADS);                  // Build Quad From A Triangle Strip
      for (i=0; i<sizeof(m_particles)/sizeof(m_particles[0]); ++i)          // i Through All The Particles
      {
        ps = m_particles[i].life*PART_SIZE;
        vCenter.x = m_particles[i].x;
        vCenter.y = m_particles[i].y;
        vCenter.z = m_particles[i].z;

        // Draw The Particle Using Our RGB Values, Fade The Particle Based On It's Life
        glTexCoord2d(0,1); glVertex3fv(vCenter - (vRight + vUp) * ps); // Top Left
        glTexCoord2d(1,1); glVertex3fv(vCenter + (vRight - vUp) * ps); // Top Right
        glTexCoord2d(1,0); glVertex3fv(vCenter + (vRight + vUp) * ps); // Bottom Right
        glTexCoord2d(0,0); glVertex3fv(vCenter - (vRight - vUp) * ps); // Bottom Left
      }
    glEnd();                      // Done Building Triangle Strip
  glPopAttrib();
}

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
bool CBall::ProcessEvent(EEvent nEvent, unsigned long wParam, unsigned long lParam)
{
  return false;
}

// Create a new ball aimed toward left or right player.
void CBall::NewBall(bool bGoToLeft)
{
  float fAngle;

  // Selects a random angle.
  do
    fAngle = (float)m_rand.RandomRange(-BALL_MAX_ANGLE, +BALL_MAX_ANGLE);
  while (fabs(fAngle) < BALL_MIN_ANGLE);

  if (bGoToLeft)
  {
    fAngle += float(M_PI);
    m_vBallPosition.x = CBoard::GetLeft()-CPaddle::GetWidth();
  }
  else
    m_vBallPosition.x = CBoard::GetRight()+CPaddle::GetWidth();
  m_vBallSpeed.x = (float)cos(fAngle)*BALL_SPEED;
  m_vBallSpeed.y = (float)sin(fAngle)*BALL_SPEED;
}

/** Detects if the ball collides with a point A.
 * @param vPos    Current position of the ball.
 * @param vSpeed  Speed of the ball that will be updated only if the ball collides.
 * @param ptA    Position of the point A.
 * @return If colliside returns true and update speed vector, else returns false.
 */
inline bool CBall::HitPoint(CVector2D &vOldPos, CVector2D &vNewPos, CVector2D &vSpeed, CVector2D &ptA)
{
  // Position where distance from line y=a*x+b to the point A is the shortest.
  float  a = vSpeed.y / vSpeed.x,
      b = vOldPos.y - a*vOldPos.x,
      t0 = -a*a*(ptA.x*ptA.x - BALL_RADIUS*BALL_RADIUS) - 2.0f*a*(b-ptA.y)*ptA.x - b*b + 2.0f*b*ptA.y - ptA.y*ptA.y + BALL_RADIUS*BALL_RADIUS;
  // Check if hit.
  if (t0 < 0)
    return false;
  // Find hit position.
  float  t1 = sqrt(t0),
      t2 = -a*(b-ptA.y) + ptA.x,
      t3 = a*a + 1,
      x1 = (+t1 + t2)/t3,
      x2 = (-t1 + t2)/t3,
      x;
  // Hit position is one of the two x coordiantes.
  if (fabs(vOldPos.x - x1) < fabs(vOldPos.x - x2))
    x = x1;
  else
    x = x2;
  // Is hit position is between vOldPos and vNewPos?
  if (!(vOldPos.x <= x && x <= vNewPos.x ||
      vNewPos.x <= x && x <= vOldPos.x))
    return false;

  // The shortest vector from A to the line defined by y=a*x+b.
  CVector2D vect(x - ptA.x, a*x+b - ptA.y);

  // Update speed vector.
  vSpeed = vect * ((vSpeed.Norm()+BALL_SPEED_INCREASE) / vect.Norm());
  // Update position.
  //! TODO
  vNewPos = vOldPos; // Temporary position version that gives bad results on slow machines.
/*
R is the new direction vector
I is the old direction vector before the collision
N is the Normal at the collision point

The new vector R is calculated as follows:

R= 2*(-I dot N)*N + I

The restriction is that the I and N vectors have to be unit vectors. The velocity vector as used in our examples represents speed and direction. Therefore it can not be plugged into the equation in the place of I, without any transformation. The speed has to be extracted. The speed for such a velocity vector is extracted finding the magnitude of the vector. Once the magnitude is found, the vector can be transformed to a unit vector and plugged into the equation giving the reflection vector R. R shows us now the direction, of the reflected ray, but in order to be used as a velocity vector it must also incorporate the speed. Therefore it gets, multiplied with the magnitude of the original ray, thus resulting in the correct velocity vector.
*/
  return true;
}

