CC = clang++
LD = clang++

ifeq ($(OS),Windows_NT)
CFLAGS = -Isrc -Wall -c -std=c++17 -mfpmath=sse -fdeclspec -g -O3
else
CFLAGS = -Isrc -Wall -c -std=c++17 -fdeclspec -g -O3
endif

ifeq ($(OS),Windows_NT)
LIBRARIES = -lkernel32 -luser32 -lgdi32 -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32 -ldsound.lib
endif

ifeq ($(OS),Windows_NT)
EXT = ".exe"
else
EXT = ""
endif

LFLAGS = -shared -Wall -g -Xlinker /subsystem:windows -Xlinker /subsystemversion:6.01

# The build target 
ifeq ($(OS),Windows_NT)
TARGET = libred11.dll
COPY = xcopy /Y
MOVE = move
else
TARGET = libred11.so
COPY = cp -f
MOVE = mv -f
endif

ifeq ($(OS),Windows_NT)
EFLAGS = -L./ -llibred11 -Xlinker /subsystem:windows -Xlinker /subsystemversion:6.01
else
EFLAGS = -L./ -lred11
endif

SRCDIR = src
EXMDIR = examples
OBJDIR = objects
BINDIR = bin
 
OBJ_FILES = ${OBJDIR}/red11.o \
			${OBJDIR}/window.o ${OBJDIR}/gamepad.o ${OBJDIR}/windowsWindow.o ${OBJDIR}/windowsWindowUtils.o ${OBJDIR}/windowsGamepad.o \
			${OBJDIR}/audio.o ${OBJDIR}/audioSource.o ${OBJDIR}/audioWindows.o \
			${OBJDIR}/scene.o \
			${OBJDIR}/ui.o ${OBJDIR}/uiContext.o ${OBJDIR}/uiNode.o ${OBJDIR}/uiNodeDisplay.o \
			${OBJDIR}/renderer.o \
			${OBJDIR}/directx9renderer.o ${OBJDIR}/directx9meshRenderData.o ${OBJDIR}/directx9textureRenderData.o ${OBJDIR}/directx9materialRenderData.o  \
			${OBJDIR}/directx9data.o ${OBJDIR}/directx9shader.o \
			${OBJDIR}/mesh.o ${OBJDIR}/meshObject.o ${OBJDIR}/entity.o ${OBJDIR}/light.o ${OBJDIR}/camera.o ${OBJDIR}/spline.o \
			${OBJDIR}/inputProvider.o \
			${OBJDIR}/texture.o ${OBJDIR}/textureFile.o ${OBJDIR}/textureFileHDR.o \
			${OBJDIR}/sound.o ${OBJDIR}/soundFile.o \
			${OBJDIR}/font.o \
			${OBJDIR}/deform.o ${OBJDIR}/boneTransform.o ${OBJDIR}/animation.o ${OBJDIR}/animationTarget.o ${OBJDIR}/animator.o ${OBJDIR}/animationTrack.o \
			${OBJDIR}/physicsWorld.o ${OBJDIR}/physicsBody.o ${OBJDIR}/physicsForm.o ${OBJDIR}/physicsUtils.o \
			${OBJDIR}/constraint.o ${OBJDIR}/constraintAxis.o \
			${OBJDIR}/collisionDispatcher.o ${OBJDIR}/collisionSolver.o ${OBJDIR}/collisionHandler.o \
			${OBJDIR}/shape.o ${OBJDIR}/shapePlain.o ${OBJDIR}/shapeSphere.o ${OBJDIR}/shapeOBB.o ${OBJDIR}/shapeCapsule.o ${OBJDIR}/shapeConvex.o ${OBJDIR}/shapeMesh.o \
			${OBJDIR}/data3DFile.o ${OBJDIR}/debugEntities.o \
			${OBJDIR}/material.o ${OBJDIR}/materialSimple.o \
			${OBJDIR}/actor.o ${OBJDIR}/actorTemporary.o \
			${OBJDIR}/component.o ${OBJDIR}/componentMesh.o ${OBJDIR}/componentText.o ${OBJDIR}/componentLight.o ${OBJDIR}/componentMeshGroup.o ${OBJDIR}/componentCamera.o \
			${OBJDIR}/componentSpline.o \
			${OBJDIR}/utils.o ${OBJDIR}/resourceManager.o ${OBJDIR}/sysinfo.o ${OBJDIR}/color.o ${OBJDIR}/meshBuilder.o ${OBJDIR}/meshCombiner.o ${OBJDIR}/destroyable.o \
			${OBJDIR}/stb_image.o ${OBJDIR}/stb_vorbis.o ${OBJDIR}/stb_truetype.o ${OBJDIR}/convhull_3d.o \
			${OBJDIR}/deltaCounter.o ${OBJDIR}/jobQueue.o ${OBJDIR}/logger.o ${OBJDIR}/hullCliping.o \
			${OBJDIR}/loaderFBX.o ${OBJDIR}/FBXNode.o ${OBJDIR}/FBXAnimationStack.o ${OBJDIR}/FBXAnimationLayer.o ${OBJDIR}/FBXAnimationCurve.o ${OBJDIR}/FBXAnimationCurveNode.o \
			${OBJDIR}/FBXDeform.o ${OBJDIR}/FBXGeometry.o ${OBJDIR}/FBXModel.o ${OBJDIR}/FBXAttribute.o \
			${OBJDIR}/networkMessage.o ${OBJDIR}/messageProcessor.o ${OBJDIR}/networkApi.o ${OBJDIR}/client.o ${OBJDIR}/server.o ${OBJDIR}/connection.o \
			${OBJDIR}/windowsClient.o ${OBJDIR}/windowsServer.o ${OBJDIR}/windowsConnection.o \

EXAMPLES = 	1-window${EXT} 2-textures${EXT} 3-animation${EXT} 4-bones${EXT} 5-physics${EXT} 6-collisionEvents${EXT} 7-ui${EXT} 8-resourceManagment${EXT} 9-customShaders${EXT} \
			10-splines${EXT} 11-networkServer${EXT} 12-networkClient${EXT} 13-gamepad${EXT} demo-1${EXT}

all: engine examples

engine: $(TARGET)
${OBJDIR}/red11.o: ${SRCDIR}/red11.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/red11.o ${SRCDIR}/red11.cpp

${OBJDIR}/window.o: ${SRCDIR}/window/window.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/window.o ${SRCDIR}/window/window.cpp

${OBJDIR}/gamepad.o: ${SRCDIR}/window/gamepad.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/gamepad.o ${SRCDIR}/window/gamepad.cpp

${OBJDIR}/windowsWindow.o: ${SRCDIR}/window/windows/windowsWindow.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/windowsWindow.o ${SRCDIR}/window/windows/windowsWindow.cpp

${OBJDIR}/windowsWindowUtils.o: ${SRCDIR}/window/windows/windowsWindowUtils.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/windowsWindowUtils.o ${SRCDIR}/window/windows/windowsWindowUtils.cpp

${OBJDIR}/windowsGamepad.o: ${SRCDIR}/window/windows/windowsGamepad.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/windowsGamepad.o ${SRCDIR}/window/windows/windowsGamepad.cpp

${OBJDIR}/audio.o: ${SRCDIR}/audio/audio.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/audio.o ${SRCDIR}/audio/audio.cpp

${OBJDIR}/audioSource.o: ${SRCDIR}/audio/audioSource.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/audioSource.o ${SRCDIR}/audio/audioSource.cpp

${OBJDIR}/audioWindows.o: ${SRCDIR}/audio/windows/audioWindows.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/audioWindows.o ${SRCDIR}/audio/windows/audioWindows.cpp

${OBJDIR}/scene.o: ${SRCDIR}/scene/scene.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/scene.o ${SRCDIR}/scene/scene.cpp

${OBJDIR}/ui.o: ${SRCDIR}/ui/ui.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/ui.o ${SRCDIR}/ui/ui.cpp

${OBJDIR}/uiContext.o: ${SRCDIR}/ui/uiContext.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/uiContext.o ${SRCDIR}/ui/uiContext.cpp

${OBJDIR}/uiNode.o: ${SRCDIR}/ui/uiNode.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/uiNode.o ${SRCDIR}/ui/uiNode.cpp

${OBJDIR}/uiNodeDisplay.o: ${SRCDIR}/ui/uiNodeDisplay.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/uiNodeDisplay.o ${SRCDIR}/ui/uiNodeDisplay.cpp

${OBJDIR}/renderer.o: ${SRCDIR}/renderer/renderer.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/renderer.o ${SRCDIR}/renderer/renderer.cpp
	
${OBJDIR}/directx9renderer.o: ${SRCDIR}/renderer/directx9/directx9renderer.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/directx9renderer.o ${SRCDIR}/renderer/directx9/directx9renderer.cpp

${OBJDIR}/directx9meshRenderData.o: ${SRCDIR}/renderer/directx9/directx9meshRenderData.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/directx9meshRenderData.o ${SRCDIR}/renderer/directx9/directx9meshRenderData.cpp

${OBJDIR}/directx9textureRenderData.o: ${SRCDIR}/renderer/directx9/directx9textureRenderData.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/directx9textureRenderData.o ${SRCDIR}/renderer/directx9/directx9textureRenderData.cpp

${OBJDIR}/directx9materialRenderData.o: ${SRCDIR}/renderer/directx9/directx9materialRenderData.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/directx9materialRenderData.o ${SRCDIR}/renderer/directx9/directx9materialRenderData.cpp

${OBJDIR}/directx9data.o: ${SRCDIR}/renderer/directx9/directx9data.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/directx9data.o ${SRCDIR}/renderer/directx9/directx9data.cpp

${OBJDIR}/directx9shader.o: ${SRCDIR}/renderer/directx9/directx9shader.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/directx9shader.o ${SRCDIR}/renderer/directx9/directx9shader.cpp

${OBJDIR}/mesh.o: ${SRCDIR}/data/mesh.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/mesh.o ${SRCDIR}/data/mesh.cpp

${OBJDIR}/meshObject.o: ${SRCDIR}/data/meshObject.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/meshObject.o ${SRCDIR}/data/meshObject.cpp

${OBJDIR}/entity.o: ${SRCDIR}/data/entity.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/entity.o ${SRCDIR}/data/entity.cpp

${OBJDIR}/light.o: ${SRCDIR}/data/light.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/light.o ${SRCDIR}/data/light.cpp

${OBJDIR}/camera.o: ${SRCDIR}/data/camera.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/camera.o ${SRCDIR}/data/camera.cpp

${OBJDIR}/spline.o: ${SRCDIR}/data/spline.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/spline.o ${SRCDIR}/data/spline.cpp

${OBJDIR}/texture.o: ${SRCDIR}/data/texture.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/texture.o ${SRCDIR}/data/texture.cpp

${OBJDIR}/textureFile.o: ${SRCDIR}/data/textureFile.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/textureFile.o ${SRCDIR}/data/textureFile.cpp

${OBJDIR}/textureFileHDR.o: ${SRCDIR}/data/textureFileHDR.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/textureFileHDR.o ${SRCDIR}/data/textureFileHDR.cpp

${OBJDIR}/inputProvider.o: ${SRCDIR}/data/inputProvider.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/inputProvider.o ${SRCDIR}/data/inputProvider.cpp

${OBJDIR}/sound.o: ${SRCDIR}/data/sound.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/sound.o ${SRCDIR}/data/sound.cpp

${OBJDIR}/soundFile.o: ${SRCDIR}/data/soundFile.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/soundFile.o ${SRCDIR}/data/soundFile.cpp

${OBJDIR}/font.o: ${SRCDIR}/data/font.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/font.o ${SRCDIR}/data/font.cpp

${OBJDIR}/deform.o: ${SRCDIR}/data/deform.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/deform.o ${SRCDIR}/data/deform.cpp

${OBJDIR}/boneTransform.o: ${SRCDIR}/data/boneTransform.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/boneTransform.o ${SRCDIR}/data/boneTransform.cpp

${OBJDIR}/animation.o: ${SRCDIR}/data/animation/animation.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/animation.o ${SRCDIR}/data/animation/animation.cpp

${OBJDIR}/animationTarget.o: ${SRCDIR}/data/animation/animationTarget.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/animationTarget.o ${SRCDIR}/data/animation/animationTarget.cpp

${OBJDIR}/animator.o: ${SRCDIR}/data/animation/animator.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/animator.o ${SRCDIR}/data/animation/animator.cpp

${OBJDIR}/animationTrack.o: ${SRCDIR}/data/animation/animationTrack.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/animationTrack.o ${SRCDIR}/data/animation/animationTrack.cpp

${OBJDIR}/physicsWorld.o: ${SRCDIR}/physics/physicsWorld.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/physicsWorld.o ${SRCDIR}/physics/physicsWorld.cpp

${OBJDIR}/physicsBody.o: ${SRCDIR}/physics/physicsBody.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/physicsBody.o ${SRCDIR}/physics/physicsBody.cpp

${OBJDIR}/physicsForm.o: ${SRCDIR}/physics/physicsForm.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/physicsForm.o ${SRCDIR}/physics/physicsForm.cpp

${OBJDIR}/physicsUtils.o: ${SRCDIR}/physics/physicsUtils.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/physicsUtils.o ${SRCDIR}/physics/physicsUtils.cpp

${OBJDIR}/constraint.o: ${SRCDIR}/physics/constraints/constraint.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/constraint.o ${SRCDIR}/physics/constraints/constraint.cpp

${OBJDIR}/constraintAxis.o: ${SRCDIR}/physics/constraints/constraintAxis.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/constraintAxis.o ${SRCDIR}/physics/constraints/constraintAxis.cpp

${OBJDIR}/collisionDispatcher.o: ${SRCDIR}/physics/collisionDispatcher.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/collisionDispatcher.o ${SRCDIR}/physics/collisionDispatcher.cpp

${OBJDIR}/collisionSolver.o: ${SRCDIR}/physics/collisionSolver.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/collisionSolver.o ${SRCDIR}/physics/collisionSolver.cpp

${OBJDIR}/collisionHandler.o: ${SRCDIR}/physics/collisionHandler.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/collisionHandler.o ${SRCDIR}/physics/collisionHandler.cpp

${OBJDIR}/shape.o: ${SRCDIR}/physics/shapes/shape.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/shape.o ${SRCDIR}/physics/shapes/shape.cpp

${OBJDIR}/shapePlain.o: ${SRCDIR}/physics/shapes/shapePlain.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/shapePlain.o ${SRCDIR}/physics/shapes/shapePlain.cpp

${OBJDIR}/shapeSphere.o: ${SRCDIR}/physics/shapes/shapeSphere.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/shapeSphere.o ${SRCDIR}/physics/shapes/shapeSphere.cpp

${OBJDIR}/shapeOBB.o: ${SRCDIR}/physics/shapes/shapeOBB.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/shapeOBB.o ${SRCDIR}/physics/shapes/shapeOBB.cpp

${OBJDIR}/shapeCapsule.o: ${SRCDIR}/physics/shapes/shapeCapsule.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/shapeCapsule.o ${SRCDIR}/physics/shapes/shapeCapsule.cpp

${OBJDIR}/shapeConvex.o: ${SRCDIR}/physics/shapes/shapeConvex.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/shapeConvex.o ${SRCDIR}/physics/shapes/shapeConvex.cpp

${OBJDIR}/shapeMesh.o: ${SRCDIR}/physics/shapes/shapeMesh.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/shapeMesh.o ${SRCDIR}/physics/shapes/shapeMesh.cpp

${OBJDIR}/data3DFile.o: ${SRCDIR}/data/data3DFile.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/data3DFile.o ${SRCDIR}/data/data3DFile.cpp

${OBJDIR}/debugEntities.o: ${SRCDIR}/data/debugEntities.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/debugEntities.o ${SRCDIR}/data/debugEntities.cpp

${OBJDIR}/material.o: ${SRCDIR}/data/material/material.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/material.o ${SRCDIR}/data/material/material.cpp

${OBJDIR}/materialSimple.o: ${SRCDIR}/data/material/materialSimple.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/materialSimple.o ${SRCDIR}/data/material/materialSimple.cpp

${OBJDIR}/actor.o: ${SRCDIR}/actor/actor.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/actor.o ${SRCDIR}/actor/actor.cpp

${OBJDIR}/actorTemporary.o: ${SRCDIR}/actor/actorTemporary.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/actorTemporary.o ${SRCDIR}/actor/actorTemporary.cpp

${OBJDIR}/component.o: ${SRCDIR}/actor/components/component.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/component.o ${SRCDIR}/actor/components/component.cpp

${OBJDIR}/componentMesh.o: ${SRCDIR}/actor/components/componentMesh.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/componentMesh.o ${SRCDIR}/actor/components/componentMesh.cpp

${OBJDIR}/componentText.o: ${SRCDIR}/actor/components/componentText.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/componentText.o ${SRCDIR}/actor/components/componentText.cpp

${OBJDIR}/componentLight.o: ${SRCDIR}/actor/components/componentLight.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/componentLight.o ${SRCDIR}/actor/components/componentLight.cpp

${OBJDIR}/componentMeshGroup.o: ${SRCDIR}/actor/components/componentMeshGroup.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/componentMeshGroup.o ${SRCDIR}/actor/components/componentMeshGroup.cpp

${OBJDIR}/componentCamera.o: ${SRCDIR}/actor/components/componentCamera.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/componentCamera.o ${SRCDIR}/actor/components/componentCamera.cpp

${OBJDIR}/componentSpline.o: ${SRCDIR}/actor/components/componentSpline.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/componentSpline.o ${SRCDIR}/actor/components/componentSpline.cpp

${OBJDIR}/utils.o: ${SRCDIR}/utils/utils.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/utils.o ${SRCDIR}/utils/utils.cpp

${OBJDIR}/resourceManager.o: ${SRCDIR}/utils/resourceManager.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/resourceManager.o ${SRCDIR}/utils/resourceManager.cpp

${OBJDIR}/sysinfo.o: ${SRCDIR}/utils/sysinfo.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/sysinfo.o ${SRCDIR}/utils/sysinfo.cpp

${OBJDIR}/color.o: ${SRCDIR}/utils/color.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/color.o ${SRCDIR}/utils/color.cpp

${OBJDIR}/meshBuilder.o: ${SRCDIR}/utils/meshBuilder.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/meshBuilder.o ${SRCDIR}/utils/meshBuilder.cpp

${OBJDIR}/meshCombiner.o: ${SRCDIR}/utils/meshCombiner.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/meshCombiner.o ${SRCDIR}/utils/meshCombiner.cpp

${OBJDIR}/destroyable.o: ${SRCDIR}/utils/destroyable.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/destroyable.o ${SRCDIR}/utils/destroyable.cpp

${OBJDIR}/stb_image.o: ${SRCDIR}/utils/image/stb_image.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/stb_image.o ${SRCDIR}/utils/image/stb_image.cpp

${OBJDIR}/stb_vorbis.o: ${SRCDIR}/utils/sound/stb_vorbis.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/stb_vorbis.o ${SRCDIR}/utils/sound/stb_vorbis.cpp

${OBJDIR}/stb_truetype.o: ${SRCDIR}/utils/font/stb_truetype.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/stb_truetype.o ${SRCDIR}/utils/font/stb_truetype.cpp

${OBJDIR}/convhull_3d.o: ${SRCDIR}/utils/convex/convhull_3d.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/convhull_3d.o ${SRCDIR}/utils/convex/convhull_3d.cpp

${OBJDIR}/deltaCounter.o: ${SRCDIR}/utils/deltaCounter.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/deltaCounter.o ${SRCDIR}/utils/deltaCounter.cpp

${OBJDIR}/jobQueue.o: ${SRCDIR}/utils/jobQueue.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/jobQueue.o ${SRCDIR}/utils/jobQueue.cpp

${OBJDIR}/logger.o: ${SRCDIR}/utils/logger.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/logger.o ${SRCDIR}/utils/logger.cpp

${OBJDIR}/hullCliping.o: ${SRCDIR}/utils/hullCliping.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/hullCliping.o ${SRCDIR}/utils/hullCliping.cpp

${OBJDIR}/loaderFBX.o: ${SRCDIR}/utils/FBX/loaderFBX.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/loaderFBX.o ${SRCDIR}/utils/FBX/loaderFBX.cpp

${OBJDIR}/FBXNode.o: ${SRCDIR}/utils/FBX/FBXNode.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/FBXNode.o ${SRCDIR}/utils/FBX/FBXNode.cpp

${OBJDIR}/FBXAnimationStack.o: ${SRCDIR}/utils/FBX/FBXAnimationStack.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/FBXAnimationStack.o ${SRCDIR}/utils/FBX/FBXAnimationStack.cpp

${OBJDIR}/FBXAnimationLayer.o: ${SRCDIR}/utils/FBX/FBXAnimationLayer.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/FBXAnimationLayer.o ${SRCDIR}/utils/FBX/FBXAnimationLayer.cpp

${OBJDIR}/FBXAnimationCurve.o: ${SRCDIR}/utils/FBX/FBXAnimationCurve.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/FBXAnimationCurve.o ${SRCDIR}/utils/FBX/FBXAnimationCurve.cpp

${OBJDIR}/FBXAnimationCurveNode.o: ${SRCDIR}/utils/FBX/FBXAnimationCurveNode.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/FBXAnimationCurveNode.o ${SRCDIR}/utils/FBX/FBXAnimationCurveNode.cpp

${OBJDIR}/FBXDeform.o: ${SRCDIR}/utils/FBX/FBXDeform.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/FBXDeform.o ${SRCDIR}/utils/FBX/FBXDeform.cpp

${OBJDIR}/FBXGeometry.o: ${SRCDIR}/utils/FBX/FBXGeometry.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/FBXGeometry.o ${SRCDIR}/utils/FBX/FBXGeometry.cpp

${OBJDIR}/FBXModel.o: ${SRCDIR}/utils/FBX/FBXModel.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/FBXModel.o ${SRCDIR}/utils/FBX/FBXModel.cpp

${OBJDIR}/FBXAttribute.o: ${SRCDIR}/utils/FBX/FBXAttribute.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/FBXAttribute.o ${SRCDIR}/utils/FBX/FBXAttribute.cpp

${OBJDIR}/networkMessage.o: ${SRCDIR}/network/networkMessage.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/networkMessage.o ${SRCDIR}/network/networkMessage.cpp

${OBJDIR}/messageProcessor.o: ${SRCDIR}/network/messageProcessor.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/messageProcessor.o ${SRCDIR}/network/messageProcessor.cpp

${OBJDIR}/networkApi.o: ${SRCDIR}/network/networkApi.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/networkApi.o ${SRCDIR}/network/networkApi.cpp

${OBJDIR}/client.o: ${SRCDIR}/network/client.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/client.o ${SRCDIR}/network/client.cpp

${OBJDIR}/server.o: ${SRCDIR}/network/server.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/server.o ${SRCDIR}/network/server.cpp

${OBJDIR}/connection.o: ${SRCDIR}/network/connection.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/connection.o ${SRCDIR}/network/connection.cpp

${OBJDIR}/windowsClient.o: ${SRCDIR}/network/windows/windowsClient.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/windowsClient.o ${SRCDIR}/network/windows/windowsClient.cpp

${OBJDIR}/windowsServer.o: ${SRCDIR}/network/windows/windowsServer.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/windowsServer.o ${SRCDIR}/network/windows/windowsServer.cpp

${OBJDIR}/windowsConnection.o: ${SRCDIR}/network/windows/windowsConnection.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/windowsConnection.o ${SRCDIR}/network/windows/windowsConnection.cpp

$(TARGET): ${OBJ_FILES}
	$(LD) ${LFLAGS} ${LIBRARIES} ${OBJ_FILES} -o $(TARGET)
	${COPY} ${TARGET} "${BINDIR}/${TARGET}"

examples: ${EXAMPLES} engine
${OBJDIR}/1-window.o: ${EXMDIR}/1-window.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/1-window.o ${EXMDIR}/1-window.cpp

1-window${EXT}: ${OBJDIR}/1-window.o
	$(LD) ${EFLAGS} ${OBJDIR}/1-window.o -o 1-window${EXT}
	${MOVE} 1-window${EXT} ${BINDIR}/1-window${EXT}

${OBJDIR}/2-textures.o: ${EXMDIR}/2-textures.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/2-textures.o ${EXMDIR}/2-textures.cpp

2-textures${EXT}: ${OBJDIR}/2-textures.o
	$(LD) ${EFLAGS} ${OBJDIR}/2-textures.o -o 2-textures${EXT}
	${MOVE} 2-textures${EXT} ${BINDIR}/2-textures${EXT}

${OBJDIR}/3-animation.o: ${EXMDIR}/3-animation.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/3-animation.o ${EXMDIR}/3-animation.cpp

3-animation${EXT}: ${OBJDIR}/3-animation.o
	$(LD) ${EFLAGS} ${OBJDIR}/3-animation.o -o 3-animation${EXT}
	${MOVE} 3-animation${EXT} ${BINDIR}/3-animation${EXT}

${OBJDIR}/4-bones.o: ${EXMDIR}/4-bones.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/4-bones.o ${EXMDIR}/4-bones.cpp

4-bones${EXT}: ${OBJDIR}/4-bones.o
	$(LD) ${EFLAGS} ${OBJDIR}/4-bones.o -o 4-bones${EXT}
	${MOVE} 4-bones${EXT} ${BINDIR}/4-bones${EXT}

${OBJDIR}/5-physics.o: ${EXMDIR}/5-physics.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/5-physics.o ${EXMDIR}/5-physics.cpp

5-physics${EXT}: ${OBJDIR}/5-physics.o
	$(LD) ${EFLAGS} ${OBJDIR}/5-physics.o -o 5-physics${EXT}
	${MOVE} 5-physics${EXT} ${BINDIR}/5-physics${EXT}

${OBJDIR}/6-collisionEvents.o: ${EXMDIR}/6-collisionEvents.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/6-collisionEvents.o ${EXMDIR}/6-collisionEvents.cpp

6-collisionEvents${EXT}: ${OBJDIR}/6-collisionEvents.o
	$(LD) ${EFLAGS} ${OBJDIR}/6-collisionEvents.o -o 6-collisionEvents${EXT}
	${MOVE} 6-collisionEvents${EXT} ${BINDIR}/6-collisionEvents${EXT}

${OBJDIR}/7-ui.o: ${EXMDIR}/7-ui.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/7-ui.o ${EXMDIR}/7-ui.cpp

7-ui${EXT}: ${OBJDIR}/7-ui.o
	$(LD) ${EFLAGS} ${OBJDIR}/7-ui.o -o 7-ui${EXT}
	${MOVE} 7-ui${EXT} ${BINDIR}/7-ui${EXT}

${OBJDIR}/8-resourceManagment.o: ${EXMDIR}/8-resourceManagment.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/8-resourceManagment.o ${EXMDIR}/8-resourceManagment.cpp

8-resourceManagment${EXT}: ${OBJDIR}/8-resourceManagment.o
	$(LD) ${EFLAGS} ${OBJDIR}/8-resourceManagment.o -o 8-resourceManagment${EXT}
	${MOVE} 8-resourceManagment${EXT} ${BINDIR}/8-resourceManagment${EXT}

${OBJDIR}/9-customShaders.o: ${EXMDIR}/9-customShaders.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/9-customShaders.o ${EXMDIR}/9-customShaders.cpp

9-customShaders${EXT}: ${OBJDIR}/9-customShaders.o
	$(LD) ${EFLAGS} ${OBJDIR}/9-customShaders.o -o 9-customShaders${EXT}
	${MOVE} 9-customShaders${EXT} ${BINDIR}/9-customShaders${EXT}

${OBJDIR}/10-splines.o: ${EXMDIR}/10-splines.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/10-splines.o ${EXMDIR}/10-splines.cpp

10-splines${EXT}: ${OBJDIR}/10-splines.o
	$(LD) ${EFLAGS} ${OBJDIR}/10-splines.o -o 10-splines${EXT}
	${MOVE} 10-splines${EXT} ${BINDIR}/10-splines${EXT}

${OBJDIR}/11-networkServer.o: ${EXMDIR}/11-networkServer.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/11-networkServer.o ${EXMDIR}/11-networkServer.cpp

11-networkServer${EXT}: ${OBJDIR}/11-networkServer.o
	$(LD) ${EFLAGS} ${OBJDIR}/11-networkServer.o -o 11-networkServer${EXT}
	${MOVE} 11-networkServer${EXT} ${BINDIR}/11-networkServer${EXT}

${OBJDIR}/12-networkClient.o: ${EXMDIR}/12-networkClient.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/12-networkClient.o ${EXMDIR}/12-networkClient.cpp

12-networkClient${EXT}: ${OBJDIR}/12-networkClient.o
	$(LD) ${EFLAGS} ${OBJDIR}/12-networkClient.o -o 12-networkClient${EXT}
	${MOVE} 12-networkClient${EXT} ${BINDIR}/12-networkClient${EXT}

${OBJDIR}/13-gamepad.o: ${EXMDIR}/13-gamepad.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/13-gamepad.o ${EXMDIR}/13-gamepad.cpp

13-gamepad${EXT}: ${OBJDIR}/13-gamepad.o
	$(LD) ${EFLAGS} ${OBJDIR}/13-gamepad.o -o 13-gamepad${EXT}
	${MOVE} 13-gamepad${EXT} ${BINDIR}/13-gamepad${EXT}

${OBJDIR}/demo-1.o: ${EXMDIR}/demo-1.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/demo-1.o ${EXMDIR}/demo-1.cpp

demo-1${EXT}: ${OBJDIR}/demo-1.o
	$(LD) ${EFLAGS} ${OBJDIR}/demo-1.o -o demo-1${EXT}
	${MOVE} demo-1${EXT} ${BINDIR}/demo-1${EXT}

# llvm-objcopy
clean:
	$(RM) $(TARGET)