// GeometryViewer includes
#include "GeometryViewer.h"

#include "BaseLocator.h"
#include "ClippingPlane.h"
#include "ClippingPlaneLocator.h"
#include "gvApplicationState.h"
#include "gvContextState.h"
#include "Lighting.h"
#include "RGBAColor.h"

#include <iostream>
#include <string>
#include <math.h>

// VTK includes
#include <ExternalVTKWidget.h>
#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkExternalLight.h>
#include <vtkLight.h>
#include <vtkNew.h>
#include <vtkOBJReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

// OpenGL/Motif includes
#include <GL/GLContextData.h>
#include <GLMotif/CascadeButton.h>
#include <GLMotif/Menu.h>
#include <GLMotif/Popup.h>
#include <GLMotif/PopupMenu.h>
#include <GLMotif/RadioBox.h>
#include <GLMotif/ToggleButton.h>
#include <GLMotif/StyleSheet.h>
#include <GLMotif/SubMenu.h>
#include <GLMotif/WidgetManager.h>

// VRUI includes
#include <Vrui/Application.h>
#include <Vrui/Tool.h>
#include <Vrui/ToolManager.h>
#include <Vrui/Vrui.h>
#include <Vrui/VRWindow.h>
#include <Vrui/WindowProperties.h>

//----------------------------------------------------------------------------
GeometryViewer::GeometryViewer(int &argc, char **&argv)
  : Superclass(argc, argv, new gvApplicationState),
    FileName(0),
    intensity(1.0),
    mainMenu(NULL),
    renderingDialog(NULL),
    Opacity(1.0),
    opacityValue(NULL),
    RepresentationType(2),
    FirstFrame(true),
    analysisTool(0),
    ClippingPlanes(NULL),
    NumberOfClippingPlanes(6)
{
  this->DataBounds = new double[6];

  ambientColor = new RGBAColor(0.0f, 0.0f, 0.0f, 0.0f);
  diffuseColor = new RGBAColor(1.0f, 1.0f, 1.0f, 0.0f);
  specularColor = new RGBAColor(0.0f, 0.0f, 0.0f, 0.0f);

  /* Initialize the clipping planes */
  ClippingPlanes = new ClippingPlane[NumberOfClippingPlanes];
  for(int i = 0; i < NumberOfClippingPlanes; ++i)
    {
    ClippingPlanes[i].setAllocated(false);
    ClippingPlanes[i].setActive(false);
    }
}

//----------------------------------------------------------------------------
GeometryViewer::~GeometryViewer()
{
  if(this->DataBounds)
    {
    delete[] this->DataBounds;
    }
}

//----------------------------------------------------------------------------
void GeometryViewer::initialize()
{
  this->Superclass::initialize();

  /* Create the user interface: */
  lightingDialog = new Lighting(this);
  renderingDialog = createRenderingDialog();
  mainMenu=createMainMenu();
  Vrui::setMainMenu(mainMenu);
}

//----------------------------------------------------------------------------
void GeometryViewer::setFileName(const char* name)
{
  if (this->FileName && name && (!strcmp(this->FileName, name)))
    {
    return;
    }
  if (this->FileName)
    {
    delete [] this->FileName;
    }
  this->FileName = new char[strlen(name) + 1];
  strcpy(this->FileName, name);
}

//----------------------------------------------------------------------------
const char* GeometryViewer::getFileName()
{
  return this->FileName;
}

//----------------------------------------------------------------------------
GLMotif::PopupMenu* GeometryViewer::createMainMenu()
{
  GLMotif::PopupMenu *mainMenuPopup =
      new GLMotif::PopupMenu("MainMenuPopup", Vrui::getWidgetManager());
  mainMenuPopup->setTitle("Main Menu");
  GLMotif::Menu *mainMenu = new GLMotif::Menu("MainMenu", mainMenuPopup, false);

  GLMotif::CascadeButton *representationCascade =
      new GLMotif::CascadeButton("RepresentationCascade", mainMenu,
                                 "Representation");
  representationCascade->setPopup(createRepresentationMenu());

  GLMotif::CascadeButton *analysisToolsCascade =
      new GLMotif::CascadeButton("AnalysisToolsCascade", mainMenu,
                                 "Analysis Tools");
  analysisToolsCascade->setPopup(createAnalysisToolsMenu());

  GLMotif::Button *centerDisplayButton =
      new GLMotif::Button("CenterDisplayButton", mainMenu, "Center Display");
  centerDisplayButton->getSelectCallbacks().add(
        this, &GeometryViewer::centerDisplayCallback);

  GLMotif::ToggleButton *showRenderingDialog =
      new GLMotif::ToggleButton("ShowRenderingDialog", mainMenu,
                                "Rendering");
  showRenderingDialog->setToggle(false);
  showRenderingDialog->getValueChangedCallbacks().add(
        this, &GeometryViewer::showRenderingDialogCallback);

  GLMotif::ToggleButton *showLightingDialog =
      new GLMotif::ToggleButton("ShowLightingDialog", mainMenu,
                                "Lighting");
  showLightingDialog->setToggle(false);
  showLightingDialog->getValueChangedCallbacks().add(
        this, &GeometryViewer::showLightingDialogCallback);

  mainMenu->manageChild();
  return mainMenuPopup;
}

//----------------------------------------------------------------------------
GLMotif::Popup* GeometryViewer::createRepresentationMenu()
{
  const GLMotif::StyleSheet *ss = Vrui::getWidgetManager()->getStyleSheet();

  GLMotif::Popup *representationMenuPopup =
      new GLMotif::Popup("representationMenuPopup", Vrui::getWidgetManager());
  GLMotif::SubMenu *representationMenu =
      new GLMotif::SubMenu("representationMenu", representationMenuPopup,
                           false);

  GLMotif::RadioBox *representation_RadioBox =
      new GLMotif::RadioBox("Representation RadioBox", representationMenu,
                            true);

  GLMotif::ToggleButton *showSurface =
      new GLMotif::ToggleButton("ShowSurface", representation_RadioBox,
                                "Surface");
  showSurface->getValueChangedCallbacks().add(
        this, &GeometryViewer::changeRepresentationCallback);
  GLMotif::ToggleButton *showSurfaceWEdges =
      new GLMotif::ToggleButton("ShowSurfaceWEdges", representation_RadioBox,
                                "Surface With Edges");
  showSurfaceWEdges->getValueChangedCallbacks().add(
        this, &GeometryViewer::changeRepresentationCallback);

  GLMotif::ToggleButton *showWireframe =
      new GLMotif::ToggleButton("ShowWireframe", representation_RadioBox,
                                "Wireframe");
  showWireframe->getValueChangedCallbacks().add(
        this, &GeometryViewer::changeRepresentationCallback);
  GLMotif::ToggleButton *showPoints =
      new GLMotif::ToggleButton("ShowPoints", representation_RadioBox,
                                "Points");
  showPoints->getValueChangedCallbacks().add(
        this, &GeometryViewer::changeRepresentationCallback);

  representation_RadioBox->setSelectionMode(GLMotif::RadioBox::ATMOST_ONE);
  representation_RadioBox->setSelectedToggle(showSurface);

  representationMenu->manageChild();
  return representationMenuPopup;
}

//----------------------------------------------------------------------------
GLMotif::Popup* GeometryViewer::createAnalysisToolsMenu()
{
  const GLMotif::StyleSheet *ss = Vrui::getWidgetManager()->getStyleSheet();

  GLMotif::Popup *analysisToolsMenuPopup =
      new GLMotif::Popup("analysisToolsMenuPopup", Vrui::getWidgetManager());
  GLMotif::SubMenu *analysisToolsMenu =
      new GLMotif::SubMenu("representationMenu", analysisToolsMenuPopup, false);

  GLMotif::RadioBox *analysisTools_RadioBox =
      new GLMotif::RadioBox("analysisTools", analysisToolsMenu, true);

  GLMotif::ToggleButton *showClippingPlane =
      new GLMotif::ToggleButton("ClippingPlane", analysisTools_RadioBox,
                                "Clipping Plane");
  showClippingPlane->getValueChangedCallbacks().add(
        this, &GeometryViewer::changeAnalysisToolsCallback);

  analysisTools_RadioBox->setSelectionMode(GLMotif::RadioBox::ALWAYS_ONE);
  analysisTools_RadioBox->setSelectedToggle(showClippingPlane);

  analysisToolsMenu->manageChild();
  return analysisToolsMenuPopup;
}

//----------------------------------------------------------------------------
GLMotif::PopupWindow* GeometryViewer::createRenderingDialog()
{
  const GLMotif::StyleSheet& ss = *Vrui::getWidgetManager()->getStyleSheet();
  GLMotif::PopupWindow *dialogPopup =
      new GLMotif::PopupWindow("RenderingDialogPopup", Vrui::getWidgetManager(),
                               "Rendering Dialog");

  GLMotif::RowColumn *dialog =
      new GLMotif::RowColumn("RenderingDialog", dialogPopup, false);
  dialog->setOrientation(GLMotif::RowColumn::HORIZONTAL);

  /* Create opacity slider */
  GLMotif::Slider *opacitySlider =
      new GLMotif::Slider("OpacitySlider", dialog, GLMotif::Slider::HORIZONTAL,
                          ss.fontHeight * 10.0f);
  opacitySlider->setValue(this->Opacity);
  opacitySlider->setValueRange(0.0, 1.0, 0.1);
  opacitySlider->getValueChangedCallbacks().add(
        this, &GeometryViewer::opacitySliderCallback);
  opacityValue = new GLMotif::TextField("OpacityValue", dialog, 6);
  opacityValue->setFieldWidth(6);
  opacityValue->setPrecision(3);
  opacityValue->setValue(Opacity);

  dialog->manageChild();
  return dialogPopup;
}

//----------------------------------------------------------------------------
void GeometryViewer::frame()
{
  if (this->FirstFrame)
    {
    /* Compute the data center and Radius once */
    this->Center[0] = (this->DataBounds[0] + this->DataBounds[1])/2.0;
    this->Center[1] = (this->DataBounds[2] + this->DataBounds[3])/2.0;
    this->Center[2] = (this->DataBounds[4] + this->DataBounds[5])/2.0;

    this->Radius = sqrt((this->DataBounds[1] - this->DataBounds[0])*
                        (this->DataBounds[1] - this->DataBounds[0]) +
                        (this->DataBounds[3] - this->DataBounds[2])*
                        (this->DataBounds[3] - this->DataBounds[2]) +
                        (this->DataBounds[5] - this->DataBounds[4])*
                        (this->DataBounds[5] - this->DataBounds[4]));
    /* Scale the Radius */
    this->Radius *= 0.75;
    /* Initialize Vrui navigation transformation: */
    this->centerDisplayCallback(0);
    this->FirstFrame = false;
    }

  this->Superclass::frame();
}

//----------------------------------------------------------------------------
void GeometryViewer::initContext(GLContextData& contextData) const
{
  this->Superclass::initContext(contextData);

  // Created by superclass:
  gvContextState *state = contextData.retrieveDataItem<gvContextState>(this);
  assert("Context state initialized by vvApplication." && state);

  vtkNew<vtkPolyDataMapper> mapper;
  state->actor().SetMapper(mapper.GetPointer());

  if(this->FileName)
    {
    vtkNew<vtkOBJReader> reader;
    reader->SetFileName(this->FileName);
    reader->Update();
    reader->GetOutput()->GetBounds(this->DataBounds);
    mapper->SetInputData(reader->GetOutput());
    }
  else
    {
    vtkNew<vtkCubeSource> cube;
    cube->Update();
    cube->GetOutput()->GetBounds(this->DataBounds);
    mapper->SetInputData(cube->GetOutput());
    }
}

//----------------------------------------------------------------------------
void GeometryViewer::display(GLContextData &contextData) const
{
  int maxClipPlanes;
  glGetIntegerv(GL_MAX_CLIP_PLANES, &maxClipPlanes);
  int clipPlaneIdx = 0;
  for (int i = 0;
       i < this->NumberOfClippingPlanes && clipPlaneIdx < maxClipPlanes;
       ++i)
    {
    if (this->ClippingPlanes[i].isActive())
      {
      /* Enable the clipping plane: */
      glEnable(GL_CLIP_PLANE0 + clipPlaneIdx);
      GLdouble clippingPlane[4];
      for (int j = 0; j < 3; ++j)
        {
        clippingPlane[j] = this->ClippingPlanes[i].getPlane().getNormal()[j];
        }
      clippingPlane[3] = -this->ClippingPlanes[i].getPlane().getOffset();
      glClipPlane(GL_CLIP_PLANE0 + clipPlaneIdx, clippingPlane);
      /* Go to the next clipping plane: */
      ++clipPlaneIdx;
      }
    }

  /* Get context data item */
  gvContextState *state = contextData.retrieveDataItem<gvContextState>(this);

  /* Set light properties */
  state->headlight().SetIntensity(this->intensity);
  state->headlight().SetAmbientColor(this->ambientColor->getValues(0),
                                     this->ambientColor->getValues(1),
                                     this->ambientColor->getValues(2));
  state->headlight().SetDiffuseColor(this->diffuseColor->getValues(0),
                                     this->diffuseColor->getValues(1),
                                     this->diffuseColor->getValues(2));
  state->headlight().SetSpecularColor(this->specularColor->getValues(0),
                                      this->specularColor->getValues(1),
                                      this->specularColor->getValues(2));

  /* Set actor opacity */
  state->actor().GetProperty()->SetOpacity(this->Opacity);
  if (this->RepresentationType < 3)
    {
    state->actor().GetProperty()->SetRepresentation(this->RepresentationType);
    state->actor().GetProperty()->EdgeVisibilityOff();
    }
  else if (this->RepresentationType == 3)
    {
    state->actor().GetProperty()->SetRepresentationToSurface();
    state->actor().GetProperty()->EdgeVisibilityOn();
    }

  // Render the scene before removing clip planes:
  this->Superclass::display(contextData);

  clipPlaneIdx = 0;
  for (int i = 0;
       i < this->NumberOfClippingPlanes && clipPlaneIdx < maxClipPlanes;
       ++i)
    {
    if (ClippingPlanes[i].isActive())
      {
      /* Disable the clipping plane: */
      glDisable(GL_CLIP_PLANE0 + clipPlaneIdx);
      /* Go to the next clipping plane: */
      ++clipPlaneIdx;
      }
    }
}

//----------------------------------------------------------------------------
void GeometryViewer::setAmbientColor(float r, float g, float b)
{
  this->ambientColor->setValues(0, r);
  this->ambientColor->setValues(1, g);
  this->ambientColor->setValues(2, b);
}

//----------------------------------------------------------------------------
void GeometryViewer::setDiffuseColor(float r, float g, float b)
{
  this->diffuseColor->setValues(0, r);
  this->diffuseColor->setValues(1, g);
  this->diffuseColor->setValues(2, b);
}

//----------------------------------------------------------------------------
void GeometryViewer::setSpecularColor(float r, float g, float b)
{
  this->specularColor->setValues(0, r);
  this->specularColor->setValues(1, g);
  this->specularColor->setValues(2, b);
}

//----------------------------------------------------------------------------
void GeometryViewer::setIntensity(float intensity)
{
  this->intensity = intensity;
}
//----------------------------------------------------------------------------
void GeometryViewer::centerDisplayCallback(Misc::CallbackData *callBackData)
{
  if (!this->DataBounds)
    {
    std::cerr << "ERROR: Data bounds not set!!" << std::endl;
    return;
    }
  Vrui::setNavigationTransformation(this->Center, this->Radius);
}

//----------------------------------------------------------------------------
void GeometryViewer::opacitySliderCallback(
  GLMotif::Slider::ValueChangedCallbackData *callBackData)
{
  this->Opacity = static_cast<double>(callBackData->value);
  this->opacityValue->setValue(callBackData->value);
}

//----------------------------------------------------------------------------
void GeometryViewer::changeRepresentationCallback(
    GLMotif::ToggleButton::ValueChangedCallbackData *callBackData)
{
  /* Adjust representation state based on which toggle button changed state: */
  if (strcmp(callBackData->toggle->getName(), "ShowSurface") == 0)
    {
    this->RepresentationType = 2;
    }
  else if (strcmp(callBackData->toggle->getName(), "ShowWireframe") == 0)
    {
    this->RepresentationType = 1;
    }
  else if (strcmp(callBackData->toggle->getName(), "ShowPoints") == 0)
    {
    this->RepresentationType = 0;
    }
  else if (strcmp(callBackData->toggle->getName(), "ShowSurfaceWEdges") == 0)
    {
    this->RepresentationType = 3;
    }
}
//----------------------------------------------------------------------------
void GeometryViewer::changeAnalysisToolsCallback(
    GLMotif::ToggleButton::ValueChangedCallbackData *callBackData)
{
  /* Set the new analysis tool: */
  if (strcmp(callBackData->toggle->getName(), "ClippingPlane") == 0)
    {
    this->analysisTool = 0;
    }
}

//----------------------------------------------------------------------------
void GeometryViewer::showRenderingDialogCallback(
    GLMotif::ToggleButton::ValueChangedCallbackData *callBackData)
{
  /* open/close rendering dialog based on which toggle button changed state: */
  if (strcmp(callBackData->toggle->getName(), "ShowRenderingDialog") == 0) {
    if (callBackData->set)
      {
      /* Open the rendering dialog at the same position as the main menu: */
      Vrui::getWidgetManager()->popupPrimaryWidget(
            renderingDialog,
            Vrui::getWidgetManager()->calcWidgetTransformation(mainMenu));
      }
    else
      {
      /* Close the rendering dialog: */
      Vrui::popdownPrimaryWidget(renderingDialog);
      }
    }
}

//----------------------------------------------------------------------------
void GeometryViewer::showLightingDialogCallback(
  GLMotif::ToggleButton::ValueChangedCallbackData *callBackData)
{
  /* open/close lighting dialog based on which toggle button changed state: */
  if (strcmp(callBackData->toggle->getName(), "ShowLightingDialog") == 0)
    {
    if (callBackData->set)
      {
      /* Open the lighting dialog at the same position as the main menu: */
      Vrui::getWidgetManager()->popupPrimaryWidget(
            lightingDialog,
            Vrui::getWidgetManager()->calcWidgetTransformation(mainMenu));
      }
    else
      {
      /* Close the lighting dialog: */
      Vrui::popdownPrimaryWidget(lightingDialog);
      }
    }
}

//----------------------------------------------------------------------------
ClippingPlane *GeometryViewer::getClippingPlanes()
{
  return this->ClippingPlanes;
}

//----------------------------------------------------------------------------
int GeometryViewer::getNumberOfClippingPlanes()
{
  return this->NumberOfClippingPlanes;
}

//----------------------------------------------------------------------------
void GeometryViewer::toolCreationCallback(
    Vrui::ToolManager::ToolCreationCallbackData *callbackData)
{
  /* Check if the new tool is a locator tool: */
  Vrui::LocatorTool *locatorTool =
      dynamic_cast<Vrui::LocatorTool*>(callbackData->tool);
  if (locatorTool != 0) {

    if (this->analysisTool == 0)
      {
      /* Create a clipping plane locator object and associate it with the
       * new tool: */
      BaseLocator *newLocator = new ClippingPlaneLocator(locatorTool, this);

      /* Add new locator to list: */
      baseLocators.push_back(newLocator);
      }
    }
}

//----------------------------------------------------------------------------
void GeometryViewer::toolDestructionCallback(
    Vrui::ToolManager::ToolDestructionCallbackData *callbackData)
{
  /* Check if the to-be-destroyed tool is a locator tool: */
  Vrui::LocatorTool *locatorTool =
      dynamic_cast<Vrui::LocatorTool*>(callbackData->tool);

  if (locatorTool != 0)
    {
    /* Find the data locator associated with the tool in the list: */
    for (BaseLocatorList::iterator blIt = baseLocators.begin();
         blIt != baseLocators.end(); ++blIt)
      {
      if ((*blIt)->getTool() == locatorTool)
        {
        /* Remove the locator: */
        delete *blIt;
        baseLocators.erase(blIt);
        break;
        }
      }
    }
}

//----------------------------------------------------------------------------
vvContextState *GeometryViewer::createContextState() const
{
  return new gvContextState;
}
