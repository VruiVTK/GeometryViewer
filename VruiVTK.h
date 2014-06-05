#ifndef _VRUIVTK_H
#define _VRUIVTK_H

// OpenGL/Motif includes
#include <GL/gl.h>
#include <GL/GLObject.h>
#include <GLMotif/ToggleButton.h>

// Vrui includes
#include <Vrui/Application.h>

// VTK includes
#include <vtkSmartPointer.h>

namespace GLMotif
{
  class Popup;
  class PopupMenu;
}

class vtkExternalOpenGLRenderWindow;
class vtkExternalOpenGLRenderer;

class VruiVTK:public Vrui::Application,public GLObject
{
/* Embedded classes: */
private:
  /* Elements: */
  GLMotif::PopupMenu* mainMenu; // The program's main menu

  /* Private methods: */
  GLMotif::PopupMenu* createMainMenu(void); // Creates the program's main menu

  /* Name of file to load */
  char* FileName;

  /* Constructors and destructors: */
public:
  VruiVTK(int& argc,char**& argv);
  virtual ~VruiVTK(void);

  /* Methods: */
  virtual void initContext(GLContextData& contextData) const;
  virtual void display(GLContextData& contextData) const;
  void centerDisplayCallback(Misc::CallbackData* cbData);

  vtkSmartPointer<vtkExternalOpenGLRenderWindow> renWin;
  vtkSmartPointer<vtkExternalOpenGLRenderer> ren;
};

#endif //_VRUIVTK_H
