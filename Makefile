LDFLAGS = -L/usr/local/lib
LIBS = -lglfw -lm
INCS = -Iexternal/file_ops -Iexternal/glad/include -Iexternal/stb -I/usr/local/include
CFLAGS = -std=c99 -O2 -march=native -Wall

SRC_ROTATE = src/main_rotate.c external/file_ops/file_ops.c external/glad/src/glad.c
SRC_CAMERA = src/main_camera.c external/file_ops/file_ops.c external/glad/src/glad.c src/camera.c

rotate: ${SRC_ROTATE}
	${CC} -o $@ ${SRC_ROTATE} ${INCS} ${LDFLAGS} ${LIBS} ${CFLAGS}

camera: ${SRC_CAMERA}
	${CC} -o $@ ${SRC_CAMERA} ${INCS} ${LDFLAGS} ${LIBS} ${CFLAGS}

tags:
	ctags `find src -name "*.c"`

clean:
	rm -f rotate camera tags
