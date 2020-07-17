LIBS = -lGL -lglfw -ldl -lm
INCS = -Iexternal/glad/include -Iexternal/stb
SRC = src/main.c src/file_ops.c external/glad/src/glad.c

CFLAGS = -std=c99 -O2 -march=znver2 -Wall

3d-textures: $(SRC)
	$(CC) -o $@ $^ $(INCS) $(LIBS) $(CFLAGS)

clean:
	rm -f 3d-textures
