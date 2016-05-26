#include "gvContextState.h"

#include <GL/glew.h>

#include <vtkActor.h>
#include <vtkExternalLight.h>
#include <vtkExternalOpenGLRenderer.h>
#include <vtkLight.h>

gvContextState::gvContextState()
{
  this->renderer().AddActor(m_actor.Get());

  // This external light models the VRUI-default headlight at GL_LIGHT0:
  m_headlight->SetLightIndex(GL_LIGHT0);
  m_headlight->SetIntensity(1.);
  m_headlight->SetDiffuseColor(1., 1., 1.);
  this->renderer().AddExternalLight(m_headlight.Get());
}
