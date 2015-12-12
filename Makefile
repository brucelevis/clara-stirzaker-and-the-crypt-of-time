default:run

all:native web

sources = main.c editor.c menu.c easing.c game.c sprite.c assets.c
output = ld34

optimization  = -O0
#optimization  = -O1 -fsanitize=address -fno-omit-frame-pointer
#optimization  = -O1 -fsanitize=memory -fsanitize-memory-track-origins -fno-omit-frame-pointer
#optimization  = -O3

web:
	source emsdk_portable/emsdk_set_env.sh;\
	emcc -O3 $(sources) -o $(output).html -s USE_SDL=2 -s USE_SDL_TTF=2 \
	-s ALLOW_MEMORY_GROWTH=1 --preload-file data/
	cp $(output).html $(output).data $(output).js $(output).html.mem site
	@rclone copy site dropbox:Public/ld34

web-devel:
	source emsdk_portable/emsdk_set_env.sh;\
	emcc $(sources) -o $(output).html -s USE_SDL=2 -s USE_SDL_TTF=2 \
	-s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=2 --preload-file data/

native:
	clang $(sources) -g $(optimization) -o $(output) -lSDL2 -lSDL2_ttf -lm
	
windows:
	i686-w64-mingw32-clang $(sources) -g -O0 -o $(output).exe -lSDL2 -lSDL2_ttf -lm -Wl,-subsystem,windows
	zip -r $(output).zip *.exe *.dll
	cp $(output).zip site
	@rclone copy site dropbox:Public/ld34

run:native
	optirun ./$(output);echo a
