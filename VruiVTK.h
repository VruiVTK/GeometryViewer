#ifndef _VRUIVTK_H
#define _VRUIVTK_H

// OpenGL/Motif includes
#include <GL/gl.h>
#include <GL/GLObject.h>
#include <GLMotif/Slider.h>
#include <GLMotif/DropdownBox.h>

// Vrui includes
#include <Vrui/Application.h>

// VTK includes
#include <vtkSmartPointer.h>

/* Forward Declarations */
namespace GLMotif
{
  class Popup;
  class PopupMenu;
}

class vtkActor;
class ExternalVTKWidget;
class vtkLight;

class BaseLocator;

class VruiVTK:public Vrui::Application,public GLObject
{
/* Embedded classes: */
private:
  struct DataItem : public GLObject::DataItem
  {
  /* Elements */
  public:
    /* VTK components */
    vtkSmartPointer<ExternalVTKWidget> externalVTKWidget;
    vtkSmartPointer<vtkActor> actor;
    vtkSmartPointer<vtkLight> flashlight;

    /* Constructor and destructor*/
    DataItem(void);
    virtual ~DataItem(void);
  };

  /* Elements: */
  GLMotif::PopupMenu* mainMenu; // The program's main menu

  /* Private methods: */
  GLMotif::PopupMenu* createMainMenu(void); // Creates the program's main menu

  /* Name of file to load */
  char* FileName;

  /* Opacity value */
  double Opacity;

  /* Representation Type */
  int RepresentationType;

  virtual void toolCreationCallback(
    Vrui::ToolManager::ToolCreationCallbackData* cbData);
  virtual void toolDestructionCallback(
    Vrui::ToolManager::ToolDestructionCallbackData* cbData);

  BaseLocator* baseLocator;

public:
  /* Constructors and destructors: */
  VruiVTK(int& argc,char**& argv);
  virtual ~VruiVTK(void);

  /* Methods to set/get the filename to read */
  void setFileName(const char* name);
  const char* getFileName(void);

  /* Methods to manage render context */
  virtual void initContext(GLContextData& contextData) const;
  virtual void display(GLContextData& contextData) const;
  virtual void frame(void);

  /* Callback methods */
  void centerDisplayCallback(Misc::CallbackData* cbData);
  void opacitySliderCallback(GLMotif::Slider::ValueChangedCallbackData* cbData);
  void reprDropdownBoxCallback(GLMotif::DropdownBox::ValueChangedCallbackData* cbData);

  /* Methods to create widgets */
  GLMotif::Popup* createRenderOptionsMenu(void);
};

#endif //_VRUIVTK_H
