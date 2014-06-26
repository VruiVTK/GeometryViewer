/*
 * ClippingPlane.h - Class for ...
 *
 * Author: Patrick O'Leary
 * Created: May 27, 2008
 * Copyright: 2008. All rights reserved.
 */

#ifndef CLIPPINGPLANE_H_
#define CLIPPINGPLANE_H_

/* Vrui includes */
#include <Geometry/Plane.h>
#include <Vrui/Geometry.h>

class ClippingPlane {
public:
	ClippingPlane(void);
	~ClippingPlane(void);
	bool isActive(void);
	void setActive(bool _active);
	bool isAllocated(void);
	void setAllocated(bool _allocated);
	Vrui::Plane getPlane(void);
	void setPlane(Vrui::Plane plane);
private:
	bool active;
	bool allocated;
	Vrui::Plane plane;
};

#endif /*CLIPPINGPLANE_H_*/
