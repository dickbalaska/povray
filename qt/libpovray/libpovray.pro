# libpovray.pro - qmake for povray/source
# builds: libpovray.a - The main povray library
# Copyright 2018 BuckoSoft
#

QT       -= gui

TARGET = povray
TEMPLATE = lib
CONFIG += staticlib warn_off
CONFIG += object_parallel_to_source
CONFIG += c++11
CONFIG += precompile_header

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# DEFINES += OPENEXR_MISSING
#DEFINES += BUILD_ARCH="\"$$QMAKE_HOST.arch\""
QMAKE_CLEAN += libpovray.a


sDIR = ../../source
cDIR = ../../unix/povconfig

INCLUDEPATH += "../../source"
INCLUDEPATH += "../../platform/unix"
INCLUDEPATH += "../../platform/x86"
INCLUDEPATH += "../../unix/povconfig"
INCLUDEPATH += "../../vfe"

# QMAKE_CXXFLAGS_WARN_OFF -= -Wunused-parameter
PRECOMPILED_HEADER = $$sDIR/base/precomp.h

SOURCES += \
	$$sDIR/backend/bounding/boundingtask.cpp \
	$$sDIR/backend/control/benchmark_ini.cpp \
	$$sDIR/backend/control/benchmark_pov.cpp \
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
	$$sDIR/base/data/bluenoise64a.cpp \
	$$sDIR/base/fileinputoutput.cpp \
	$$sDIR/base/fileutil.cpp \
	$$sDIR/base/font/crystal.cpp \
	$$sDIR/base/font/cyrvetic.cpp \
	$$sDIR/base/font/povlogo.cpp \
	$$sDIR/base/font/timrom.cpp \
	$$sDIR/base/image/bmp.cpp \
	$$sDIR/base/image/colourspace.cpp \
	$$sDIR/base/image/dither.cpp \
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
	$$sDIR/parser/reservedwords.cpp \
	$$sDIR/povms/povms.cpp \
	$$sDIR/povms/povmscpp.cpp \
	$$sDIR/povms/povmsutil.cpp \
	$$sDIR/vm/fnintern.cpp \
	$$sDIR/vm/fnpovfpu.cpp \


HEADERS += \
	$$sDIR/backend/bounding/boundingtask.h \
	$$sDIR/backend/control/benchmark_ini.h \
	$$sDIR/backend/control/benchmark_pov.h \
	$$sDIR/backend/control/benchmark.h \
	$$sDIR/backend/control/messagefactory.h \
	$$sDIR/backend/control/renderbackend.h \
	$$sDIR/backend/control/scene.h \
	$$sDIR/backend/lighting/photonestimationtask.h \
	$$sDIR/backend/lighting/photonshootingstrategy.h \
	$$sDIR/backend/lighting/photonshootingtask.h \
	$$sDIR/backend/lighting/photonsortingtask.h \
	$$sDIR/backend/lighting/photonstrategytask.h \
	$$sDIR/backend/render/radiositytask.h \
	$$sDIR/backend/render/rendertask.h \
	$$sDIR/backend/render/tracetask.h \
	$$sDIR/backend/scene/backendscenedata.h \
	$$sDIR/backend/scene/view.h \
	$$sDIR/backend/scene/viewthreaddata.h \
	$$sDIR/backend/support/task.h \
	$$sDIR/backend/support/taskqueue.h \
	$$sDIR/backend/frame.h \
	$$sDIR/backend/povray.h \
	$$sDIR/base/animation/animation.h \
	$$sDIR/base/animation/moov.h \
	$$sDIR/base/build.h \
	$$sDIR/base/colour.h \
	$$sDIR/base/configbase.h \
	$$sDIR/base/data/bluenoise64a.h \
	$$sDIR/base/fileinputoutput.h \
	$$sDIR/base/fileutil.h \
	$$sDIR/base/font/crystal.h \
	$$sDIR/base/font/cyrvetic.h \
	$$sDIR/base/font/povlogo.h \
	$$sDIR/base/font/timrom.h \
	$$sDIR/base/image/bmp.h \
	$$sDIR/base/image/colourspace.h \
	$$sDIR/base/image/dither.h \
	$$sDIR/base/image/encoding.h \
	$$sDIR/base/image/gif.h \
	$$sDIR/base/image/hdr.h \
	$$sDIR/base/image/iff.h \
	$$sDIR/base/image/image.h \
	$$sDIR/base/image/jpeg_pov.h \
	$$sDIR/base/image/metadata.h \
	$$sDIR/base/image/openexr.h \
	$$sDIR/base/image/png_pov.h \
	$$sDIR/base/image/ppm.h \
	$$sDIR/base/image/targa.h \
	$$sDIR/base/image/tiff_pov.h \
	$$sDIR/base/mathutil.h \
	$$sDIR/base/messenger.h \
	$$sDIR/base/path.h \
	$$sDIR/base/platformbase.h \
	$$sDIR/base/pov_err.h \
	$$sDIR/base/pov_mem.h \
	$$sDIR/base/safemath.h \
	$$sDIR/base/stringutilities.h \
	$$sDIR/base/textstreambuffer.h \
	$$sDIR/base/textstream.h \
	$$sDIR/base/timer.h \
	$$sDIR/base/types.h \
	$$sDIR/base/version_info.h \
	$$sDIR/base/version.h \
	$$sDIR/core/bounding/bounding.h \
	$$sDIR/core/bounding/boundingbox.h \
	$$sDIR/core/bounding/boundingcylinder.h \
	$$sDIR/core/bounding/boundingsphere.h \
	$$sDIR/core/bounding/bsptree.h \
	$$sDIR/core/colour/spectral.h \
	$$sDIR/core/lighting/lightgroup.h \
	$$sDIR/core/lighting/lightsource.h \
	$$sDIR/core/lighting/photons.h \
	$$sDIR/core/lighting/radiosity.h \
	$$sDIR/core/lighting/subsurface.h \
	$$sDIR/core/material/blendmap.h \
	$$sDIR/core/material/interior.h \
	$$sDIR/core/material/media.h \
	$$sDIR/core/material/noise.h \
	$$sDIR/core/material/normal.h \
	$$sDIR/core/material/pattern.h \
	$$sDIR/core/material/pigment.h \
	$$sDIR/core/material/portablenoise.h \
	$$sDIR/core/material/texture.h \
	$$sDIR/core/material/warp.h \
	$$sDIR/core/math/chi2.h \
	$$sDIR/core/math/hypercomplex.h \
	$$sDIR/core/math/jitter.h \
	$$sDIR/core/math/matrix.h \
	$$sDIR/core/math/polynomialsolver.h \
	$$sDIR/core/math/quaternion.h \
	$$sDIR/core/math/randcosweighted.h \
	$$sDIR/core/math/randomsequence.h \
	$$sDIR/core/math/spline.h \
	$$sDIR/core/render/ray.h \
	$$sDIR/core/render/trace.h \
	$$sDIR/core/render/tracepixel.h \
	$$sDIR/core/scene/atmosphere.h \
	$$sDIR/core/scene/camera.h \
	$$sDIR/core/scene/object.h \
	$$sDIR/core/scene/scenedata.h \
	$$sDIR/core/scene/tracethreaddata.h \
	$$sDIR/core/shape/bezier.h \
	$$sDIR/core/shape/blob.h \
	$$sDIR/core/shape/box.h \
	$$sDIR/core/shape/cone.h \
	$$sDIR/core/shape/csg.h \
	$$sDIR/core/shape/disc.h \
	$$sDIR/core/shape/fractal.h \
	$$sDIR/core/shape/heightfield.h \
	$$sDIR/core/shape/isosurface.h \
	$$sDIR/core/shape/lathe.h \
	$$sDIR/core/shape/lemon.h \
	$$sDIR/core/shape/mesh.h \
	$$sDIR/core/shape/ovus.h \
	$$sDIR/core/shape/parametric.h \
	$$sDIR/core/shape/plane.h \
	$$sDIR/core/shape/polygon.h \
	$$sDIR/core/shape/polynomial.h \
	$$sDIR/core/shape/prism.h \
	$$sDIR/core/shape/quadric.h \
	$$sDIR/core/shape/sor.h \
	$$sDIR/core/shape/sphere.h \
	$$sDIR/core/shape/spheresweep.h \
	$$sDIR/core/shape/superellipsoid.h \
	$$sDIR/core/shape/torus.h \
	$$sDIR/core/shape/triangle.h \
	$$sDIR/core/shape/truetype.h \
	$$sDIR/core/support/imageutil.h \
	$$sDIR/core/support/octree.h \
	$$sDIR/core/support/statisticids.h \
	$$sDIR/core/support/statistics.h \
	$$sDIR/core/configcore.h \
	$$sDIR/core/coretypes.h \
	$$sDIR/frontend/animationprocessing.h \
	$$sDIR/frontend/console.h \
	$$sDIR/frontend/display.h \
	$$sDIR/frontend/filemessagehandler.h \
	$$sDIR/frontend/imagemessagehandler.h \
	$$sDIR/frontend/imageprocessing.h \
	$$sDIR/frontend/parsermessagehandler.h \
	$$sDIR/frontend/processoptions.h \
	$$sDIR/frontend/processrenderoptions.h \
	$$sDIR/frontend/renderfrontend.h \
	$$sDIR/frontend/rendermessagehandler.h \
	$$sDIR/frontend/shelloutprocessing.h \
	$$sDIR/parser/configparser.h \
	$$sDIR/parser/fncode.h \
	$$sDIR/parser/parser.h \
	$$sDIR/parser/reservedwords.h \
	$$sDIR/povms/configpovms.h \
	$$sDIR/povms/povms.h \
	$$sDIR/povms/povmsid.h \
	$$sDIR/povms/povmscpp.h \
	$$sDIR/povms/povmsutil.h \
	$$sDIR/vm/fnintern.h \
	$$sDIR/vm/fnpovfpu.h \
	$$cDIR/syspovconfig.h \

#unix {
#    target.path = /usr/lib
#    INSTALLS += target
#}

linux-g++ {
	DEFINES += TRY_OPTIMIZED_NOISE
	DEFINES += BUILD_X86
}

