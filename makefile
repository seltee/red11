CC = clang++
LD = clang++

ifeq ($(OS),Windows_NT)
CFLAGS = -Isrc -Wall -c -std=c++17 -mfpmath=sse -fdeclspec -g -O3
else
CFLAGS = -Isrc -Wall -c -std=c++17 -fdeclspec -g -O3
endif

ifeq ($(OS),Windows_NT)
LIBRARIES = -lkernel32 -luser32 -lgdi32 -lwinspool \
			-lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32 -lopengl32
else
LIBRARIES = -lSDL2 -lSDL2main -lSDL2_ttf -framework OpenGL -framework
endif

ifeq ($(OS),Windows_NT)
EXT = ".exe"
else
EXT = ""
endif

LFLAGS = -shared -Wall -g

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
EFLAGS = -L./ -llibred11
else
EFLAGS = -L./ -lred11
endif

SRCDIR = src
EXMDIR = examples
OBJDIR = objects
BINDIR = bin
 
OBJ_FILES = ${OBJDIR}/red11.o \
			${OBJDIR}/window.o ${OBJDIR}/windowsWindow.o \
			${OBJDIR}/scene.o \
			${OBJDIR}/renderer.o \
			${OBJDIR}/directx9renderer.o ${OBJDIR}/directx9meshRenderData.o ${OBJDIR}/directx9textureRenderData.o  \
			${OBJDIR}/mesh.o ${OBJDIR}/meshObject.o ${OBJDIR}/entity.o ${OBJDIR}/light.o ${OBJDIR}/camera.o ${OBJDIR}/texture.o ${OBJDIR}/textureFile.o ${OBJDIR}/inputProvider.o \
			${OBJDIR}/deform.o ${OBJDIR}/boneTransform.o ${OBJDIR}/animation.o ${OBJDIR}/animationTarget.o ${OBJDIR}/animator.o ${OBJDIR}/animationTrack.o \
			${OBJDIR}/data3DFile.o \
			${OBJDIR}/material.o ${OBJDIR}/materialSimple.o \
			${OBJDIR}/actor.o \
			${OBJDIR}/component.o ${OBJDIR}/componentMesh.o ${OBJDIR}/componentLight.o ${OBJDIR}/componentMeshGroup.o ${OBJDIR}/componentCamera.o \
			${OBJDIR}/utils.o ${OBJDIR}/color.o ${OBJDIR}/meshBuilder.o ${OBJDIR}/meshCombiner.o ${OBJDIR}/destroyable.o ${OBJDIR}/stb_image.o \
			${OBJDIR}/deltaCounter.o \
			${OBJDIR}/loaderFBX.o ${OBJDIR}/FBXNode.o ${OBJDIR}/FBXAnimationStack.o ${OBJDIR}/FBXAnimationLayer.o ${OBJDIR}/FBXAnimationCurve.o ${OBJDIR}/FBXAnimationCurveNode.o \
			${OBJDIR}/FBXDeform.o ${OBJDIR}/FBXGeometry.o ${OBJDIR}/FBXModel.o ${OBJDIR}/FBXAttribute.o

EXAMPLES = 1-window${EXT} 2-textures${EXT} 3-animation${EXT} 4-bones${EXT}

all: engine examples

engine: $(TARGET)
${OBJDIR}/red11.o: ${SRCDIR}/red11.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/red11.o ${SRCDIR}/red11.cpp

${OBJDIR}/window.o: ${SRCDIR}/window/window.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/window.o ${SRCDIR}/window/window.cpp
	
${OBJDIR}/windowsWindow.o: ${SRCDIR}/window/windows/windowsWindow.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/windowsWindow.o ${SRCDIR}/window/windows/windowsWindow.cpp

${OBJDIR}/scene.o: ${SRCDIR}/scene/scene.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/scene.o ${SRCDIR}/scene/scene.cpp

${OBJDIR}/renderer.o: ${SRCDIR}/renderer/renderer.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/renderer.o ${SRCDIR}/renderer/renderer.cpp
	
${OBJDIR}/directx9renderer.o: ${SRCDIR}/renderer/directx9/directx9renderer.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/directx9renderer.o ${SRCDIR}/renderer/directx9/directx9renderer.cpp

${OBJDIR}/directx9meshRenderData.o: ${SRCDIR}/renderer/directx9/directx9meshRenderData.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/directx9meshRenderData.o ${SRCDIR}/renderer/directx9/directx9meshRenderData.cpp

${OBJDIR}/directx9textureRenderData.o: ${SRCDIR}/renderer/directx9/directx9textureRenderData.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/directx9textureRenderData.o ${SRCDIR}/renderer/directx9/directx9textureRenderData.cpp

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

${OBJDIR}/texture.o: ${SRCDIR}/data/texture.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/texture.o ${SRCDIR}/data/texture.cpp

${OBJDIR}/textureFile.o: ${SRCDIR}/data/textureFile.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/textureFile.o ${SRCDIR}/data/textureFile.cpp

${OBJDIR}/inputProvider.o: ${SRCDIR}/data/inputProvider.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/inputProvider.o ${SRCDIR}/data/inputProvider.cpp

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

${OBJDIR}/data3DFile.o: ${SRCDIR}/data/data3DFile.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/data3DFile.o ${SRCDIR}/data/data3DFile.cpp

${OBJDIR}/material.o: ${SRCDIR}/data/material/material.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/material.o ${SRCDIR}/data/material/material.cpp

${OBJDIR}/materialSimple.o: ${SRCDIR}/data/material/materialSimple.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/materialSimple.o ${SRCDIR}/data/material/materialSimple.cpp

${OBJDIR}/actor.o: ${SRCDIR}/actor/actor.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/actor.o ${SRCDIR}/actor/actor.cpp

${OBJDIR}/component.o: ${SRCDIR}/actor/components/component.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/component.o ${SRCDIR}/actor/components/component.cpp

${OBJDIR}/componentMesh.o: ${SRCDIR}/actor/components/componentMesh.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/componentMesh.o ${SRCDIR}/actor/components/componentMesh.cpp

${OBJDIR}/componentLight.o: ${SRCDIR}/actor/components/componentLight.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/componentLight.o ${SRCDIR}/actor/components/componentLight.cpp

${OBJDIR}/componentMeshGroup.o: ${SRCDIR}/actor/components/componentMeshGroup.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/componentMeshGroup.o ${SRCDIR}/actor/components/componentMeshGroup.cpp

${OBJDIR}/componentCamera.o: ${SRCDIR}/actor/components/componentCamera.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/componentCamera.o ${SRCDIR}/actor/components/componentCamera.cpp

${OBJDIR}/utils.o: ${SRCDIR}/utils/utils.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/utils.o ${SRCDIR}/utils/utils.cpp

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

${OBJDIR}/deltaCounter.o: ${SRCDIR}/utils/deltaCounter.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/deltaCounter.o ${SRCDIR}/utils/deltaCounter.cpp

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

# llvm-objcopy
clean:
	$(RM) $(TARGET)