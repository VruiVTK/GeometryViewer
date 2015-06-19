// GeometryViewer includes
#include "GeometryViewer.h"

#include "BaseLocator.h"

// Vrui includes
#include <Vrui/LocatorTool.h>


/*
 * BaseLocator
 *
 * parameter _locatorTool - Vrui::LocatorTool*
 * parameter _geometryViewer - GeometryViewer*
 */
BaseLocator::BaseLocator(Vrui::LocatorTool* _locatorTool, GeometryViewer* _geometryViewer) :
	Vrui::LocatorToolAdapter(_locatorTool) {
	geometryViewer = _geometryViewer;
} // end BaseLocator()

/*
 * ~BaseLocator - Destructor
 */
BaseLocator::~BaseLocator(void) {
	geometryViewer = 0;
} // end ~BaseLocator()

/*
 * highlightLocator - Render actual locator
 *
 * parameter glContextData - GLContextData&
 */
void BaseLocator::highlightLocator(GLContextData& glContextData) const {
} // end highlightLocator()

/*
 * glRenderAction - Render opaque elements of locator
 *
 * parameter glContextData - GLContextData&
 */
void BaseLocator::glRenderAction(GLContextData& glContextData) const {
} // end glRenderAction()

/*
 * glRenderActionTransparent - Render transparent elements of locator
 *
 * parameter glContextData - GLContextData&
 */
void BaseLocator::glRenderActionTransparent(GLContextData& glContextData) const {
} // end glRenderActionTransparent()

void BaseLocator::getName(std::string& name) const
{
}
