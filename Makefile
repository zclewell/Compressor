run_length: mainRunLength.c runLength.c
	gcc -pthread -o rL mainRunLength.c -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -lglib-2.0

debug: mainRunLength.c runLength.c
	gcc -pthread -g -o rL mainRunLength.c -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -lglib-2.0

server: server.c
	gcc server.c -o server -pthread -std=c99 -c -MMD -MP -D_GNU_SOURCE

client: client.c
	gcc client.c -o client -std=c99 -c -MMD -MP -D_GNU_SOURCE