// STD includes
#include <iostream>
#include <string>

// OpenGL/Motif includes
#include <GL/GLContextData.h>
#include <GL/gl.h>
#include <GLMotif/CascadeButton.h>
#include <GLMotif/Menu.h>
#include <GLMotif/Popup.h>
#include <GLMotif/PopupMenu.h>
#include <GLMotif/StyleSheet.h>
#include <GLMotif/SubMenu.h>
#include <GLMotif/WidgetManager.h>

// VRUI includes
#include <Vrui/Application.h>
#include <Vrui/Tool.h>
#include <Vrui/ToolManager.h>
#include <Vrui/Vrui.h>

// VTK includes
#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkExternalOpenGLRenderer.h>
#include "vtkExternalOpenGLRenderWindow.h"
#include <vtkLight.h>
#include <vtkNew.h>
#include <vtkOBJReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

#include <vtkImplicitPlaneWidget2.h>
#include <vtkImplicitPlaneRepresentation.h>
#include <vtkPlane.h>
#include <vtkClipPolyData.h>
#include <vtkRenderWindowInteractor.h>

// VruiVTK includes
#include "VruiVTK.h"

//----------------------------------------------------------------------------
VruiVTK::vtkIPWCallback* VruiVTK::vtkIPWCallback::New()
{
  return new VruiVTK::vtkIPWCallback();
}

//----------------------------------------------------------------------------
VruiVTK::vtkIPWCallback::vtkIPWCallback()
  :plane(0),
  actor(0)
{
}

//----------------------------------------------------------------------------
void VruiVTK::vtkIPWCallback::Execute(vtkObject* caller,
  unsigned long vtkNotUsed(eventId), void* vtkNotUsed(callData))
{
  vtkImplicitPlaneWidget2 *planeWidget =
    reinterpret_cast<vtkImplicitPlaneWidget2*>(caller);
  vtkImplicitPlaneRepresentation* rep =
    reinterpret_cast<vtkImplicitPlaneRepresentation*>(
      planeWidget->GetRepresentation());
  rep->GetPlane(this->plane);
}

//----------------------------------------------------------------------------
VruiVTK::DataItem::DataItem(void)
{
  /* Initialize VTK renderwindow and renderer */
  this->renWin =
    vtkSmartPointer<vtkExternalOpenGLRenderWindow>::New();
  this->ren = vtkSmartPointer<vtkExternalOpenGLRenderer>::New();
  this->renWin->AddRenderer(this->ren.GetPointer());
  this->actor = vtkSmartPointer<vtkActor>::New();
  this->ren->AddActor(this->actor);
  this->flashlight = vtkSmartPointer<vtkLight>::New();
  this->flashlight->SwitchOff();
  this->flashlight->SetLightTypeToHeadlight();
  this->flashlight->SetColor(0.0, 1.0, 1.0);
  this->ren->AddLight(this->flashlight);

  this->iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  this->iren->SetRenderWindow(this->renWin);
//  this->renWin->SetAlphaBitPlanes(1);
//  this->ren->SetUseDepthPeeling(1);
//  this->ren->SetMaximumNumberOfPeels(50);
//  this->ren->SetOcclusionRatio(0.1);
}

//----------------------------------------------------------------------------
VruiVTK::DataItem::~DataItem(void)
{
}

//----------------------------------------------------------------------------
VruiVTK::VruiVTK(int& argc,char**& argv)
  :Vrui::Application(argc,argv),
  FileName(0),
  mainMenu(0),
  Opacity(1.0),
  RepresentationType(2)
{
  /* Create the user interface: */
  mainMenu=createMainMenu();
  Vrui::setMainMenu(mainMenu);

  /* Initialize Vrui navigation transformation: */
  centerDisplayCallback(0);
}

//----------------------------------------------------------------------------
VruiVTK::~VruiVTK(void)
{
  /* Delete the user interface: */
  delete mainMenu;
}

//----------------------------------------------------------------------------
void VruiVTK::setFileName(const char* name)
{
  if(this->FileName && name && (!strcmp(this->FileName, name)))
    {
    return;
    }
  if(this->FileName && name)
    {
    delete [] this->FileName;
    }
  this->FileName = new char[strlen(name) + 1];
  strcpy(this->FileName, name);
}

//----------------------------------------------------------------------------
const char* VruiVTK::getFileName(void)
{
  return this->FileName;
}

//----------------------------------------------------------------------------
GLMotif::PopupMenu* VruiVTK::createMainMenu(void)
{
  /* Create a top-level shell for the main menu: */
  GLMotif::PopupMenu* mainMenuPopup =
    new GLMotif::PopupMenu("MainMenuPopup",Vrui::getWidgetManager());
  mainMenuPopup->setTitle("Interaction");

  /* Create the actual menu inside the top-level shell: */
  GLMotif::Menu* mainMenu = new GLMotif::Menu("MainMenu",mainMenuPopup,false);

  /* Create a cascade button to show the "Rendering Options" submenu */
  GLMotif::CascadeButton* renderOptionsCascade = new GLMotif::CascadeButton(
    "RenderOptionsCascade", mainMenu, "Rendering Options");
  renderOptionsCascade->setPopup(createRenderOptionsMenu());

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
GLMotif::Popup* VruiVTK::createRenderOptionsMenu(void)
{
  /* Get Vrui stylesheet to keep it consistent across GLMotif */
  const GLMotif::StyleSheet* style =
    Vrui::getWidgetManager()->getStyleSheet();

  /* Create the submenu's top-level shell */
  GLMotif::Popup* renderOptionsMenuPopup =
    new GLMotif::Popup("RenderOptionsMenuPopup", Vrui::getWidgetManager());

  /* Create the array of render options inside the top-level shell */
  GLMotif::SubMenu* renderOptionsMenu =
    new GLMotif::SubMenu("RenderOptionsMenu", renderOptionsMenuPopup, false);

  /* Create the opacity sub-menu */
  GLMotif::Popup* opacityPopup =
    new GLMotif::Popup("OpacityPopup", Vrui::getWidgetManager());
  GLMotif::SubMenu* opacityMenu =
    new GLMotif::SubMenu("OpacityMenu", opacityPopup, false);
  opacityMenu->manageChild();

  /* Create a cascade button to show the Opacity menu */
  GLMotif::CascadeButton* opacityCascade = new GLMotif::CascadeButton(
    "OpacityCascade", renderOptionsMenu, "Opacity");
  opacityCascade->setPopup(opacityPopup);

  /* Create a slider that pops up from the opacity sub-menu */
  GLMotif::Slider* opacitySlider = new GLMotif::Slider("OpacitySlider",
    opacityMenu, GLMotif::Slider::HORIZONTAL, style->fontHeight*10.0f);
  opacitySlider->setValue(1.0);
  opacitySlider->setValueRange(0.0, 1.0, 0.1);
  opacitySlider->getValueChangedCallbacks().add(this, &VruiVTK::opacitySliderCallback);

  /* Create the representation sub-menu */
  GLMotif::Popup* reprPopup =
    new GLMotif::Popup("ReprPopup", Vrui::getWidgetManager());
  GLMotif::SubMenu* reprMenu =
    new GLMotif::SubMenu("ReprMenu", reprPopup, false);
  reprMenu->manageChild();

  /* Create a cascade button to show the Opacity menu */
  GLMotif::CascadeButton* reprCascade = new GLMotif::CascadeButton(
    "ReprCascade", renderOptionsMenu, "Representation");
  reprCascade->setPopup(reprPopup);

  /* Create a DropdownBox that pops up from the repr sub-menu */
  std::vector<std::string> items;
  items.push_back(std::string("Points"));
  items.push_back(std::string("Wireframe"));
  items.push_back(std::string("Surface"));
  GLMotif::DropdownBox* reprBox =
    new GLMotif::DropdownBox("Representation", reprMenu, items);
  reprBox->setSelectedItem(2);
  reprBox->getValueChangedCallbacks().add(this, &VruiVTK::reprDropdownBoxCallback);

  /* Calculate the submenu's proper layout */
  renderOptionsMenu->manageChild();

  /* Return the created top-level shell */
  return renderOptionsMenuPopup;
}

//----------------------------------------------------------------------------
void VruiVTK::frame(void)
{
}

//----------------------------------------------------------------------------
void VruiVTK::initContext(GLContextData& contextData) const
{
  /* Create a new context data item */
  DataItem* dataItem = new DataItem();
  contextData.addDataItem(this, dataItem);

  vtkNew<vtkPlane> plane;
  vtkNew<vtkClipPolyData> clipper;
  clipper->SetClipFunction(plane.GetPointer());
  clipper->InsideOutOn();

  vtkNew<vtkPolyDataMapper> mapper;
  dataItem->actor->SetMapper(mapper.GetPointer());
  if(this->FileName)
    {
    vtkNew<vtkOBJReader> reader;
    reader->SetFileName(this->FileName);
    clipper->SetInputConnection(reader->GetOutputPort());
    }
  else
    {
    vtkNew<vtkCubeSource> cube;
    clipper->SetInputConnection(cube->GetOutputPort());
    }
  mapper->SetInputConnection(clipper->GetOutputPort());

  vtkNew<vtkProperty> backFaces;
  backFaces->SetDiffuseColor(0.8, 0.8, 0.4);
  dataItem->actor->SetBackfaceProperty(backFaces.GetPointer());

  /* Clip polydata widget */
  vtkNew<VruiVTK::vtkIPWCallback> ipwCallback;
  ipwCallback->plane = plane.GetPointer();
  ipwCallback->actor = dataItem->actor;

  vtkNew<vtkImplicitPlaneRepresentation> rep;
  rep->SetPlaceFactor(1.25);
  rep->PlaceWidget(dataItem->actor->GetBounds());
  rep->SetNormal(plane->GetNormal());

  vtkNew<vtkImplicitPlaneWidget2> planeWidget;
  planeWidget->SetInteractor(dataItem->iren);
  planeWidget->SetRepresentation(rep.GetPointer());
  planeWidget->AddObserver(vtkCommand::InteractionEvent,
    ipwCallback.GetPointer());
}

//----------------------------------------------------------------------------
void VruiVTK::display(GLContextData& contextData) const
{
  /* Save OpenGL state: */
  glPushAttrib(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_ENABLE_BIT|
    GL_LIGHTING_BIT|GL_POLYGON_BIT);
  /* Get context data item */
  DataItem* dataItem = contextData.retrieveDataItem<DataItem>(this);

  Vrui::ToolManager* toolManager = Vrui::getToolManager();
  std::vector<Vrui::Tool*>::const_iterator toolListIter =
    toolManager->beginTools();
  for(; toolListIter != toolManager->endTools(); ++toolListIter)
    {
    if(std::string((*toolListIter)->getName()).compare("Flashlight") == 0)
      {
      dataItem->flashlight->SwitchOn();
      break;
      }
    }
  if(toolListIter == toolManager->endTools())
    {
    dataItem->flashlight->SwitchOff();
    }

  /* Set actor opacity */
  dataItem->actor->GetProperty()->SetOpacity(this->Opacity);
  dataItem->actor->GetProperty()->SetRepresentation(this->RepresentationType);
  /* Render the scene */
  dataItem->iren->Initialize();
  dataItem->renWin->Render();
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
    Vrui::Scalar(8)*Vrui::getInchFactor()/Vrui::Scalar(16.0));
  Vrui::setNavigationTransformation(nav);
}

//----------------------------------------------------------------------------
void VruiVTK::opacitySliderCallback(
  GLMotif::Slider::ValueChangedCallbackData* cbData)
{
  this->Opacity = static_cast<double>(cbData->value);
}

//----------------------------------------------------------------------------
void VruiVTK::reprDropdownBoxCallback(
  GLMotif::DropdownBox::ValueChangedCallbackData* cbData)
{
  this->RepresentationType = cbData->newSelectedItem;
}
