# source.pro - qmake for povray/source
# builds: libpovray.a - The main povray library
# Copyright 2018 BuckoSoft
#

QT       -= gui

TARGET = povray
TEMPLATE = lib
CONFIG += staticlib warn_off
CONFIG += object_parallel_to_source

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES += OPENEXR_MISSING
DEFINES += BUILD_ARCH="\"$$QMAKE_HOST.arch\""
QMAKE_CLEAN += libpovray.a

INCLUDEPATH += "../platform/unix"
INCLUDEPATH += "../unix/povconfig"
INCLUDEPATH += "../vfe"

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
	povcore/bounding/boundingbox.cpp \
	povcore/bounding/bounding.cpp \
	povcore/bounding/boundingcylinder.cpp \
	povcore/bounding/boundingsphere.cpp \
	povcore/bounding/bsptree.cpp povcore/colour/spectral.cpp \
	povcore/lighting/lightgroup.cpp \
	povcore/lighting/lightsource.cpp \
	povcore/lighting/photons.cpp \
	povcore/lighting/radiosity.cpp \
	povcore/lighting/subsurface.cpp \
	povcore/material/blendmap.cpp \
	povcore/material/interior.cpp povcore/material/media.cpp \
	povcore/material/noise.cpp povcore/material/normal.cpp \
	povcore/material/pattern.cpp \
	povcore/material/pigment.cpp \
	povcore/material/portablenoise.cpp \
	povcore/material/texture.cpp povcore/material/warp.cpp \
	povcore/math/chi2.cpp povcore/math/hypercomplex.cpp \
	povcore/math/jitter.cpp povcore/math/matrix.cpp \
	povcore/math/polynomialsolver.cpp \
	povcore/math/quaternion.cpp \
	povcore/math/randcosweighted.cpp \
	povcore/math/randomsequence.cpp povcore/math/spline.cpp \
	povcore/precomp.cpp povcore/render/ray.cpp \
	povcore/render/trace.cpp povcore/render/tracepixel.cpp \
	povcore/scene/atmosphere.cpp povcore/scene/camera.cpp \
	povcore/scene/object.cpp povcore/scene/scenedata.cpp \
	povcore/scene/tracethreaddata.cpp \
	povcore/shape/bezier.cpp povcore/shape/blob.cpp \
	povcore/shape/box.cpp povcore/shape/cone.cpp \
	povcore/shape/csg.cpp povcore/shape/disc.cpp \
	povcore/shape/fractal.cpp povcore/shape/heightfield.cpp \
	povcore/shape/isosurface.cpp povcore/shape/lathe.cpp \
	povcore/shape/lemon.cpp povcore/shape/mesh.cpp \
	povcore/shape/ovus.cpp povcore/shape/parametric.cpp \
	povcore/shape/plane.cpp povcore/shape/polygon.cpp \
	povcore/shape/polynomial.cpp povcore/shape/prism.cpp \
	povcore/shape/quadric.cpp povcore/shape/sor.cpp \
	povcore/shape/sphere.cpp povcore/shape/spheresweep.cpp \
	povcore/shape/superellipsoid.cpp povcore/shape/torus.cpp \
	povcore/shape/triangle.cpp povcore/shape/truetype.cpp \
	povcore/support/imageutil.cpp povcore/support/octree.cpp \
	povcore/support/statisticids.cpp \
	povcore/support/statistics.cpp \
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

