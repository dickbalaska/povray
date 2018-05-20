# libpovray.pro - qmake for povray/source
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

QMAKE_SUBSTITUTES += config.h.in
sDIR = ../../source

INCLUDEPATH += "../../source"
INCLUDEPATH += "../../platform/unix"
INCLUDEPATH += "../../unix/povconfig"
INCLUDEPATH += "../../vfe"

# QMAKE_CXXFLAGS_WARN_OFF -= -Wunused-parameter


SOURCES += \
	$$sDIR/backend/bounding/boundingtask.cpp \
	$$sDIR/backend/control/benchmark.cpp \
	$$sDIR/backend/control/messagefactory.cpp \
	$$sDIR/backend/control/renderbackend.cpp \
	$$sDIR/backend/control/scene.cpp \
	$$sDIR/backend/lighting/photonestimationtask.cpp \
	$$sDIR/backend/lighting/photonshootingstrategy.cpp \
	$$sDIR/backend/lighting/photonshootingtask.cpp \
	$$sDIR/backend/lighting/photonsortingtask.cpp \
	$$sDIR/backend/lighting/photonstrategytask.cpp \
	$$sDIR/backend/povray.cpp \
	$$sDIR/backend/precomp.cpp \
	$$sDIR/backend/render/radiositytask.cpp \
	$$sDIR/backend/render/rendertask.cpp \
	$$sDIR/backend/render/tracetask.cpp \
	$$sDIR/backend/scene/backendscenedata.cpp \
	$$sDIR/backend/scene/view.cpp \
	$$sDIR/backend/scene/viewthreaddata.cpp \
	$$sDIR/backend/support/task.cpp \
	$$sDIR/backend/support/taskqueue.cpp \
	$$sDIR/base/animation/animation.cpp \
	$$sDIR/base/animation/moov.cpp \
	$$sDIR/base/colour.cpp \
	$$sDIR/base/fileinputoutput.cpp \
	$$sDIR/base/fileutil.cpp \
	$$sDIR/base/font/crystal.cpp \
	$$sDIR/base/font/cyrvetic.cpp \
	$$sDIR/base/font/povlogo.cpp \
	$$sDIR/base/font/timrom.cpp \
	$$sDIR/base/image/bmp.cpp \
	$$sDIR/base/image/colourspace.cpp \
	$$sDIR/base/image/encoding.cpp \
	$$sDIR/base/image/gif.cpp \
	$$sDIR/base/image/gifdecod.cpp \
	$$sDIR/base/image/hdr.cpp \
	$$sDIR/base/image/iff.cpp \
	$$sDIR/base/image/image.cpp \
	$$sDIR/base/image/jpeg.cpp \
	$$sDIR/base/image/metadata.cpp \
	$$sDIR/base/image/openexr.cpp \
	$$sDIR/base/image/png.cpp \
	$$sDIR/base/image/ppm.cpp \
	$$sDIR/base/image/targa.cpp \
	$$sDIR/base/image/tiff.cpp \
	$$sDIR/base/mathutil.cpp \
	$$sDIR/base/messenger.cpp \
	$$sDIR/base/path.cpp \
	$$sDIR/base/platformbase.cpp \
	$$sDIR/base/pov_err.cpp \
	$$sDIR/base/pov_mem.cpp \
	$$sDIR/base/precomp.cpp \
	$$sDIR/base/stringutilities.cpp \
	$$sDIR/base/textstreambuffer.cpp \
	$$sDIR/base/textstream.cpp \
	$$sDIR/base/timer.cpp \
	$$sDIR/core/bounding/boundingbox.cpp \
	$$sDIR/core/bounding/bounding.cpp \
	$$sDIR/core/bounding/boundingcylinder.cpp \
	$$sDIR/core/bounding/boundingsphere.cpp \
	$$sDIR/core/bounding/bsptree.cpp \
	$$sDIR/core/colour/spectral.cpp \
	$$sDIR/core/lighting/lightgroup.cpp \
	$$sDIR/core/lighting/lightsource.cpp \
	$$sDIR/core/lighting/photons.cpp \
	$$sDIR/core/lighting/radiosity.cpp \
	$$sDIR/core/lighting/subsurface.cpp \
	$$sDIR/core/material/blendmap.cpp \
	$$sDIR/core/material/interior.cpp \
	$$sDIR/core/material/media.cpp \
	$$sDIR/core/material/noise.cpp \
	$$sDIR/core/material/normal.cpp \
	$$sDIR/core/material/pattern.cpp \
	$$sDIR/core/material/pigment.cpp \
	$$sDIR/core/material/portablenoise.cpp \
	$$sDIR/core/material/texture.cpp \
	$$sDIR/core/material/warp.cpp \
	$$sDIR/core/math/chi2.cpp \
	$$sDIR/core/math/hypercomplex.cpp \
	$$sDIR/core/math/jitter.cpp \
	$$sDIR/core/math/matrix.cpp \
	$$sDIR/core/math/polynomialsolver.cpp \
	$$sDIR/core/math/quaternion.cpp \
	$$sDIR/core/math/randcosweighted.cpp \
	$$sDIR/core/math/randomsequence.cpp \
	$$sDIR/core/math/spline.cpp \
	$$sDIR/core/precomp.cpp \
	$$sDIR/core/render/ray.cpp \
	$$sDIR/core/render/trace.cpp \
	$$sDIR/core/render/tracepixel.cpp \
	$$sDIR/core/scene/atmosphere.cpp \
	$$sDIR/core/scene/camera.cpp \
	$$sDIR/core/scene/object.cpp \
	$$sDIR/core/scene/scenedata.cpp \
	$$sDIR/core/scene/tracethreaddata.cpp \
	$$sDIR/core/shape/bezier.cpp \
	$$sDIR/core/shape/blob.cpp \
	$$sDIR/core/shape/box.cpp \
	$$sDIR/core/shape/cone.cpp \
	$$sDIR/core/shape/csg.cpp \
	$$sDIR/core/shape/disc.cpp \
	$$sDIR/core/shape/fractal.cpp \
	$$sDIR/core/shape/heightfield.cpp \
	$$sDIR/core/shape/isosurface.cpp \
	$$sDIR/core/shape/lathe.cpp \
	$$sDIR/core/shape/lemon.cpp \
	$$sDIR/core/shape/mesh.cpp \
	$$sDIR/core/shape/ovus.cpp \
	$$sDIR/core/shape/parametric.cpp \
	$$sDIR/core/shape/plane.cpp \
	$$sDIR/core/shape/polygon.cpp \
	$$sDIR/core/shape/polynomial.cpp \
	$$sDIR/core/shape/prism.cpp \
	$$sDIR/core/shape/quadric.cpp \
	$$sDIR/core/shape/sor.cpp \
	$$sDIR/core/shape/sphere.cpp \
	$$sDIR/core/shape/spheresweep.cpp \
	$$sDIR/core/shape/superellipsoid.cpp \
	$$sDIR/core/shape/torus.cpp \
	$$sDIR/core/shape/triangle.cpp \
	$$sDIR/core/shape/truetype.cpp \
	$$sDIR/core/support/imageutil.cpp \
	$$sDIR/core/support/octree.cpp \
	$$sDIR/core/support/statisticids.cpp \
	$$sDIR/core/support/statistics.cpp \
	$$sDIR/frontend/animationprocessing.cpp \
	$$sDIR/frontend/console.cpp \
	$$sDIR/frontend/display.cpp \
	$$sDIR/frontend/filemessagehandler.cpp \
	$$sDIR/frontend/imagemessagehandler.cpp \
	$$sDIR/frontend/imageprocessing.cpp \
	$$sDIR/frontend/parsermessagehandler.cpp \
	$$sDIR/frontend/precomp.cpp \
	$$sDIR/frontend/processoptions.cpp \
	$$sDIR/frontend/processrenderoptions.cpp \
	$$sDIR/frontend/renderfrontend.cpp \
	$$sDIR/frontend/rendermessagehandler.cpp \
	$$sDIR/frontend/shelloutprocessing.cpp \
	$$sDIR/parser/fncode.cpp \
	$$sDIR/parser/parser.cpp \
	$$sDIR/parser/parser_expressions.cpp \
	$$sDIR/parser/parser_functions.cpp \
	$$sDIR/parser/parser_functions_utilities.cpp \
	$$sDIR/parser/parser_materials.cpp \
	$$sDIR/parser/parser_obj.cpp \
	$$sDIR/parser/parser_strings.cpp \
	$$sDIR/parser/parser_tokenizer.cpp \
	$$sDIR/parser/precomp.cpp \
	$$sDIR/parser/reservedwords.cpp \
	$$sDIR/povms/povms.cpp \
	$$sDIR/povms/povmscpp.cpp \
	$$sDIR/povms/povmsutil.cpp \
	$$sDIR/povms/precomp.cpp \
	$$sDIR/vm/fnintern.cpp \
	$$sDIR/vm/fnpovfpu.cpp \
	$$sDIR/vm/precomp.cpp


HEADERS += \


unix {
    target.path = /usr/lib
    INSTALLS += target
}

