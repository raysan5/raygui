ifeq ($(shell uname -s),Darwin)
	SO_EXT := dylib
	CFLAGS := -framework OpenGL $(shell pkg-config --libs --cflags raylib)
else
	SO_EXT := so
	CFLAGS := -lraylib -lGL -lrt -lX11
endif

libraygui.$(SO_EXT): src/raygui.c
	gcc -o $@ $< -shared -fpic -DRAYGUI_IMPLEMENTATION -lm -lpthread -ldl $(CFLAGS)

src/raygui.c: src/raygui.h
	rm -f src/raygui.c
	cp src/raygui.h src/raygui.c

clean:
	rm -f src/raygui.c libraygui.$(SO_EXT)
