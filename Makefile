run_length: mainRunLength.c runLength.c
	gcc -pthread -o rL mainRunLength.c `pkg-config --cflags --libs glib-2.0`

debug: mainRunLength.c runLength.c
<<<<<<< HEAD
	gcc -pthread -g -o rL mainRunLength.c `pkg-config --cflags --libs glib-2.0`

lzw: lzw.c
	gcc -g -o lzw lzw.c bitfile.c `pkg-config --cflags --libs glib-2.0`
=======
	gcc -pthread -g -o rL mainRunLength.c -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -lglib-2.0

server: server.c
	gcc server.c -o server -pthread -std=c99 -c -MMD -MP -D_GNU_SOURCE

client: client.c
	gcc client.c -o client -std=c99 -c -MMD -MP -D_GNU_SOURCE
>>>>>>> 7126e2a66954f30d5d1d6a4fc44100483e5cdf41
