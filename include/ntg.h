#ifndef _NTG_H_
#define _NTG_H_

#include "nt_gfx.h"

#include "core/app/ntg_app.h"
#include "core/scene/ntg_scene.h"
#include "core/object/ntg_object.h"

// TODO:
// Separate scene drawing from scene - the scene should only create the logical
// drawing. That implies creating something like a "scene painter" or "stage"
// that will create the actual scene_drawing/stage_drawing. This also implies
// the scene graph existing inside the ntg_scene base class.

// Implement multi-root system for scenes.

#endif // _NTG_H_
