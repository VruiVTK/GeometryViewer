#ifndef _GEOMETRYVIEWER_H
#define _GEOMETRYVIEWER_H

// OpenGL/Motif includes
// Must come before any gl.h include
#include <GL/glew.h>

#include <GL/gl.h>

#include <vvApplication.h>

// Vrui includes
#include <GL/GLObject.h>
#include <GLMotif/PopupWindow.h>
#include <GLMotif/Slider.h>
#include <GLMotif/TextField.h>
#include <GLMotif/ToggleButton.h>
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
class BaseLocator;
class ClippingPlane;
class ExternalVTKWidget;
class Lighting;
class RGBAColor;
class vtkExternalLight;
class vtkLight;

class GeometryViewer : public vvApplication
{
/* Embedded classes: */
  typedef std::vector<BaseLocator*> BaseLocatorList;
private:
  /* Elements: */
  GLMotif::PopupMenu* mainMenu; // The program's main menu
  GLMotif::PopupMenu* createMainMenu(void);
  GLMotif::Popup* createRepresentationMenu(void);
  GLMotif::Popup* createAnalysisToolsMenu(void);
  GLMotif::PopupWindow* lightingDialog;
  GLMotif::PopupWindow* renderingDialog;
  GLMotif::PopupWindow* createRenderingDialog(void);
  GLMotif::TextField* opacityValue;

  /* Name of file to load */
  char* FileName;

  /* Opacity value */
  double Opacity;

  /* Representation Type */
  int RepresentationType;

  /* bounds */
  double* DataBounds;

  /* First Frame */
  bool FirstFrame;

  /* Data Center */
  Vrui::Point Center;

  /* Data Radius  */
  Vrui::Scalar Radius;

  BaseLocatorList baseLocators;

  /* Analysis Tools */
  int analysisTool;

  /* Clipping Plane */
  ClippingPlane * ClippingPlanes;
  int NumberOfClippingPlanes;

  /* Flashlight position and direction */
  int * FlashlightSwitch;
  double * FlashlightPosition;
  double * FlashlightDirection;

  RGBAColor * ambientColor;
  RGBAColor * diffuseColor;
  RGBAColor * specularColor;
  float intensity;

  /* Constructors and destructors: */
public:
  using Superclass = vvApplication;

  GeometryViewer(int& argc,char**& argv);
  virtual ~GeometryViewer(void);

  /* Methods to set/get the filename to read */
  void setFileName(const char* name);
  const char* getFileName(void);

  /* Clipping Planes */
  ClippingPlane * getClippingPlanes(void);
  int getNumberOfClippingPlanes(void);

  /* Get Flashlight position and direction */
  int * getFlashlightSwitch(void);
  double * getFlashlightPosition(void);
  double * getFlashlightDirection(void);

  /* Methods to manage render context */
  void initialize() override;
  void initContext(GLContextData& contextData) const override;
  void display(GLContextData& contextData) const override;
  void frame() override;


  /* Callback methods */
  void centerDisplayCallback(Misc::CallbackData* cbData);
  void opacitySliderCallback(GLMotif::Slider::ValueChangedCallbackData* cbData);
  void changeRepresentationCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData);
  void showLightingDialogCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData);
  void showRenderingDialogCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData);
  void changeAnalysisToolsCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData);

  void setAmbientColor(float r, float g, float b);
  void setDiffuseColor(float r, float g, float b);
  void setSpecularColor(float r, float g, float b);
  void setIntensity(float intensity);


  virtual void toolCreationCallback(Vrui::ToolManager::ToolCreationCallbackData* cbData);
  virtual void toolDestructionCallback(Vrui::ToolManager::ToolDestructionCallbackData* cbData);

protected:
  vvContextState* createContextState() const override;
};

#endif //_GEOMETRYVIEWER_H
