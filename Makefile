run_length: mainRunLength.c runLength.c
	gcc -pthread -o rL mainRunLength.c `pkg-config --cflags --libs glib-2.0`

debug: mainRunLength.c runLength.c
	gcc -pthread -g -o rL mainRunLength.c `pkg-config --cflags --libs glib-2.0`

lzw: lzw.c
	gcc -g -o lzw lzw.c bitfile.c `pkg-config --cflags --libs glib-2.0`

server: server.c
	clang -pthread server.c -o server -std=c99 -c -MMD -MP -D_GNU_SOURCE

client: client.c
	gcc client.c -o client -std=c99 -c -MMD -MP -D_GNU_SOURCE
