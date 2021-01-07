LIBS = -lGL -lglfw -ldl -lm
INCS = -Iexternal/glad/include -Iexternal/stb
CFLAGS = -std=c99 -O2 -march=znver1 -Wall

SRC_ROTATE = src/main_rotate.c src/file_ops.c external/glad/src/glad.c
SRC_CAMERA = src/main_camera.c src/file_ops.c external/glad/src/glad.c src/camera.c

gentags:
	ctags `find src -name "*.c"`

rotate: $(SRC_ROTATE)
	$(CC) -o $@ $^ $(INCS) $(LIBS) $(CFLAGS)

camera: $(SRC_CAMERA)
	$(CC) -o $@ $^ $(INCS) $(LIBS) $(CFLAGS)

clean:
	rm -f rotate camera tags
