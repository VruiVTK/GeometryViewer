// STD includes
#include <iostream>
#include <string>

// OpenGL/Motif includes
#include <GL/gl.h>
#include <GLMotif/CascadeButton.h>
#include <GLMotif/Menu.h>
#include <GLMotif/Popup.h>
#include <GLMotif/PopupMenu.h>
#include <GLMotif/WidgetManager.h>

// VRUI includes
#include <Vrui/Application.h>
#include <Vrui/Vrui.h>

// VTK includes
#include <vtkActor.h>
#include <vtkExternalOpenGLRenderer.h>
#include "vtkExternalOpenGLRenderWindow.h"
#include <vtkLightCollection.h>
#include <vtkLight.h>
#include <vtkNew.h>
#include <vtkOBJReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkSphereSource.h>

// VruiVTK includes
#include "VruiVTK.h"

//----------------------------------------------------------------------------
GLMotif::PopupMenu* VruiVTK::createMainMenu(void)
{
  /* Create a top-level shell for the main menu: */
  GLMotif::PopupMenu* mainMenuPopup =
    new GLMotif::PopupMenu("MainMenuPopup",Vrui::getWidgetManager());
  mainMenuPopup->setTitle("Interaction");

  /* Create the actual menu inside the top-level shell: */
  GLMotif::Menu* mainMenu = new GLMotif::Menu("MainMenu",mainMenuPopup,false);

  /* Create a button to reset the navigation coordinates
   * to the default (showing the entire Earth):
   */
  GLMotif::Button* centerDisplayButton =
    new GLMotif::Button("CenterDisplayButton",mainMenu,"Center Display");
  centerDisplayButton->getSelectCallbacks().add(
    this,&VruiVTK::centerDisplayCallback);

  /* Calculate the main menu's proper layout: */
  mainMenu->manageChild();

  /* Return the created top-level shell: */
  return mainMenuPopup;
}

//----------------------------------------------------------------------------
VruiVTK::VruiVTK(int& argc,char**& argv)
  :Vrui::Application(argc,argv),
  FileName(0),
  mainMenu(0)
{
  if(argc > 1)
    {
    this->FileName = new char[strlen(argv[1]) + 1];
    strcpy(this->FileName, argv[1]);
    }

  /* Create the user interface: */
  mainMenu=createMainMenu();
  Vrui::setMainMenu(mainMenu);

  /* Initialize Vrui navigation transformation: */
  centerDisplayCallback(0);

  /* Initialize VTK renderwindow and renderer */
  this->renWin =
    vtkSmartPointer<vtkExternalOpenGLRenderWindow>::New();
  this->ren = vtkSmartPointer<vtkExternalOpenGLRenderer>::New();
}

//----------------------------------------------------------------------------
VruiVTK::~VruiVTK(void)
{
  /* Delete the user interface: */
  delete mainMenu;
}

//----------------------------------------------------------------------------
void VruiVTK::initContext(GLContextData& contextData) const
{
  this->renWin->AddRenderer(this->ren.GetPointer());
  vtkNew<vtkPolyDataMapper> mapper;
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper.GetPointer());

  vtkNew<vtkOBJReader> reader;
  reader->SetFileName(this->FileName);
  mapper->SetInputConnection(reader->GetOutputPort());

  // For testing
//  vtkNew<vtkSphereSource> sphere;
//  mapper->SetInputConnection(sphere->GetOutputPort());
  this->ren->AddActor(actor.GetPointer());
}

//----------------------------------------------------------------------------
void VruiVTK::display(GLContextData& contextData) const
{
  /* Save OpenGL state: */
  glPushAttrib(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_ENABLE_BIT|
    GL_LIGHTING_BIT|GL_POLYGON_BIT);
  /* Render the scene */
  renWin->Render();
  glPopAttrib();
}

//----------------------------------------------------------------------------
void VruiVTK::centerDisplayCallback(Misc::CallbackData* cbData)
{
  Vrui::NavTransform nav=Vrui::NavTransform::identity;
  nav*=Vrui::NavTransform::translateFromOriginTo(
    Vrui::getDisplayCenter());
  nav*=Vrui::NavTransform::rotate(Vrui::Rotation::rotateFromTo(
      Vrui::Vector(0,0,1),Vrui::getUpDirection()));
  nav*=Vrui::NavTransform::scale(
    Vrui::Scalar(8)*Vrui::getInchFactor()/Vrui::Scalar(2.0));
  Vrui::setNavigationTransformation(nav);
}
