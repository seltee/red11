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
TARGET = libshine.dll
COPY = xcopy /Y
MOVE = move
else
TARGET = libshine.so
COPY = cp -f
MOVE = mv -f
endif

ifeq ($(OS),Windows_NT)
EFLAGS = -L./ -llibshine
else
EFLAGS = -L./ -lshine
endif

SRCDIR = src
EXMDIR = examples
OBJDIR = objects
BINDIR = bin
 
OBJ_FILES = ${OBJDIR}/shine.o \
			${OBJDIR}/window.o ${OBJDIR}/windowsWindow.o \
			${OBJDIR}/scene.o \
			${OBJDIR}/renderer.o ${OBJDIR}/directx9renderer.o \
			${OBJDIR}/mesh.o ${OBJDIR}/entity.o ${OBJDIR}/light.o ${OBJDIR}/camera.o \
			${OBJDIR}/material.o ${OBJDIR}/materialSimple.o \
			${OBJDIR}/actor.o \
			${OBJDIR}/component.o ${OBJDIR}/componentMesh.o ${OBJDIR}/componentLight.o \
			${OBJDIR}/utils.o ${OBJDIR}/color.o ${OBJDIR}/meshBuilder.o ${OBJDIR}/destroyable.o

EXAMPLES = 1-window${EXT}

all: engine examples

engine: $(TARGET)
${OBJDIR}/shine.o: ${SRCDIR}/shine.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/shine.o ${SRCDIR}/shine.cpp

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

${OBJDIR}/mesh.o: ${SRCDIR}/data/mesh.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/mesh.o ${SRCDIR}/data/mesh.cpp

${OBJDIR}/entity.o: ${SRCDIR}/data/entity.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/entity.o ${SRCDIR}/data/entity.cpp

${OBJDIR}/light.o: ${SRCDIR}/data/light.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/light.o ${SRCDIR}/data/light.cpp

${OBJDIR}/camera.o: ${SRCDIR}/data/camera.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/camera.o ${SRCDIR}/data/camera.cpp

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

${OBJDIR}/utils.o: ${SRCDIR}/utils/utils.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/utils.o ${SRCDIR}/utils/utils.cpp

${OBJDIR}/color.o: ${SRCDIR}/utils/color.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/color.o ${SRCDIR}/utils/color.cpp

${OBJDIR}/meshBuilder.o: ${SRCDIR}/utils/meshBuilder.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/meshBuilder.o ${SRCDIR}/utils/meshBuilder.cpp

${OBJDIR}/destroyable.o: ${SRCDIR}/utils/destroyable.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/destroyable.o ${SRCDIR}/utils/destroyable.cpp

$(TARGET): ${OBJ_FILES}
	$(LD) ${LFLAGS} ${LIBRARIES} ${OBJ_FILES} -o $(TARGET)
	${COPY} ${TARGET} "${BINDIR}/${TARGET}"

examples: ${EXAMPLES} engine
${OBJDIR}/1-window.o: ${EXMDIR}/1-window.cpp
	$(CC) $(CFLAGS) -o ${OBJDIR}/1-window.o ${EXMDIR}/1-window.cpp

1-window${EXT}: ${OBJDIR}/1-window.o
	$(LD) ${EFLAGS} ${OBJDIR}/1-window.o -o 1-window${EXT}
	${MOVE} 1-window${EXT} ${BINDIR}/1-window${EXT}

# llvm-objcopy
clean:
	$(RM) $(TARGET)