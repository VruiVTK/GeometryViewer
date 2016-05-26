#ifndef GVCONTEXTSTATE_H
#define GVCONTEXTSTATE_H

#include <vvContextState.h>

#include <vtkNew.h>

class vtkActor;
class vtkExternalLight;
class vtkLight;

class gvContextState : public vvContextState
{
public:
  gvContextState();

  // These aren't const-correct bc VTK is not const-correct.
  vtkActor& actor() const { return *m_actor.Get(); }
  vtkExternalLight& headlight() const { return *m_headlight.Get(); }
  vtkLight& flashlight() const { return *m_flashlight.Get(); }

private:
  vtkNew<vtkActor> m_actor;
  vtkNew<vtkExternalLight> m_headlight;
  vtkNew<vtkLight> m_flashlight;
};

#endif // GVCONTEXTSTATE_H
