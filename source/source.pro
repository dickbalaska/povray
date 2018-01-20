# source.pro - qmake for povray/source
# builds: libpovray.a - The main povray library
# Copyright 2018 BuckoSoft
#

QT       -= gui

TARGET = povray
TEMPLATE = lib
CONFIG += staticlib warn_off

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_CLEAN += libpovray.a

INCLUDEPATH += "expat/lib"

# QMAKE_CXXFLAGS_WARN_OFF -= -Wunused-parameter

SOURCES += \
	backend/bounding/boundingtask.cpp \
	backend/control/benchmark.cpp \
	backend/control/messagefactory.cpp \
	backend/control/renderbackend.cpp \
	backend/control/scene.cpp \
	backend/lighting/photonestimationtask.cpp \
	backend/lighting/photonshootingstrategy.cpp \
	backend/lighting/photonshootingtask.cpp \
	backend/lighting/photonsortingtask.cpp \
	backend/lighting/photonstrategytask.cpp \
	backend/povray.cpp backend/precomp.cpp \
	backend/render/radiositytask.cpp \
	backend/render/rendertask.cpp \
	backend/render/tracetask.cpp \
	backend/scene/backendscenedata.cpp \
	backend/scene/view.cpp \
	backend/scene/viewthreaddata.cpp \
	backend/support/task.cpp \
	backend/support/taskqueue.cpp \
	base/animation/animation.cpp \
	base/animation/moov.cpp base/colour.cpp \
	base/fileinputoutput.cpp base/fileutil.cpp \
	base/font/crystal.cpp base/font/cyrvetic.cpp \
	base/font/povlogo.cpp base/font/timrom.cpp \
	base/image/bmp.cpp base/image/colourspace.cpp \
	base/image/encoding.cpp base/image/gif.cpp \
	base/image/gifdecod.cpp base/image/hdr.cpp \
	base/image/iff.cpp base/image/image.cpp \
	base/image/jpeg.cpp base/image/metadata.cpp \
	base/image/openexr.cpp base/image/png.cpp \
	base/image/ppm.cpp base/image/targa.cpp \
	base/image/tiff.cpp base/mathutil.cpp \
	base/messenger.cpp base/path.cpp \
	base/platformbase.cpp base/pov_err.cpp \
	base/pov_mem.cpp base/precomp.cpp \
	base/stringutilities.cpp base/textstreambuffer.cpp \
	base/textstream.cpp base/timer.cpp \
	core/bounding/boundingbox.cpp \
	core/bounding/bounding.cpp \
	core/bounding/boundingcylinder.cpp \
	core/bounding/boundingsphere.cpp \
	core/bounding/bsptree.cpp core/colour/spectral.cpp \
	core/lighting/lightgroup.cpp \
	core/lighting/lightsource.cpp \
	core/lighting/photons.cpp \
	core/lighting/radiosity.cpp \
	core/lighting/subsurface.cpp \
	core/material/blendmap.cpp \
	core/material/interior.cpp core/material/media.cpp \
	core/material/noise.cpp core/material/normal.cpp \
	core/material/pattern.cpp \
	core/material/pigment.cpp \
	core/material/portablenoise.cpp \
	core/material/texture.cpp core/material/warp.cpp \
	core/math/chi2.cpp core/math/hypercomplex.cpp \
	core/math/jitter.cpp core/math/matrix.cpp \
	core/math/polynomialsolver.cpp \
	core/math/quaternion.cpp \
	core/math/randcosweighted.cpp \
	core/math/randomsequence.cpp core/math/spline.cpp \
	core/precomp.cpp core/render/ray.cpp \
	core/render/trace.cpp core/render/tracepixel.cpp \
	core/scene/atmosphere.cpp core/scene/camera.cpp \
	core/scene/object.cpp core/scene/scenedata.cpp \
	core/scene/tracethreaddata.cpp \
	core/shape/bezier.cpp core/shape/blob.cpp \
	core/shape/box.cpp core/shape/cone.cpp \
	core/shape/csg.cpp core/shape/disc.cpp \
	core/shape/fractal.cpp core/shape/heightfield.cpp \
	core/shape/isosurface.cpp core/shape/lathe.cpp \
	core/shape/lemon.cpp core/shape/mesh.cpp \
	core/shape/ovus.cpp core/shape/parametric.cpp \
	core/shape/plane.cpp core/shape/polygon.cpp \
	core/shape/polynomial.cpp core/shape/prism.cpp \
	core/shape/quadric.cpp core/shape/sor.cpp \
	core/shape/sphere.cpp core/shape/spheresweep.cpp \
	core/shape/superellipsoid.cpp core/shape/torus.cpp \
	core/shape/triangle.cpp core/shape/truetype.cpp \
	core/support/imageutil.cpp core/support/octree.cpp \
	core/support/statisticids.cpp \
	core/support/statistics.cpp \
	frontend/animationprocessing.cpp \
	frontend/console.cpp frontend/display.cpp \
	frontend/filemessagehandler.cpp \
	frontend/imagemessagehandler.cpp \
	frontend/imageprocessing.cpp \
	frontend/parsermessagehandler.cpp \
	frontend/precomp.cpp frontend/processoptions.cpp \
	frontend/processrenderoptions.cpp \
	frontend/renderfrontend.cpp \
	frontend/rendermessagehandler.cpp \
	frontend/shelloutprocessing.cpp parser/fncode.cpp \
	parser/parser.cpp parser/parser_expressions.cpp \
	parser/parser_functions.cpp \
	parser/parser_functions_utilities.cpp \
	parser/parser_materials.cpp parser/parser_obj.cpp \
	parser/parser_strings.cpp \
	parser/parser_tokenizer.cpp parser/precomp.cpp \
	parser/reservedwords.cpp povmain.cpp \
	povms/povms.cpp povms/povmscpp.cpp \
	povms/povmsutil.cpp povms/precomp.cpp \
	vm/fnintern.cpp vm/fnpovfpu.cpp \
	vm/precomp.cpp


HEADERS += \


unix {
    target.path = /usr/lib
    INSTALLS += target
}

