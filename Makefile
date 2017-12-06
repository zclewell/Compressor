run_length: mainRunLength.c runLength.c
	gcc -pthread -o rL mainRunLength.c `pkg-config --cflags --libs glib-2.0`

debug: mainRunLength.c runLength.c
	gcc -pthread -g -o rL mainRunLength.c `pkg-config --cflags --libs glib-2.0`

lzw: lzw.c
	gcc -g -o lzw lzw.c bitfile.c `pkg-config --cflags --libs glib-2.0`
