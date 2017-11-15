run_length: mainRunLength.c runLength.c 
	gcc -pthread -o rL mainRunLength.c -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -lglib-2.0

encode: 
	gcc encode.c tree.c -o encode -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -lglib-2.0


decode: decode.c tree.c

