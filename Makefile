build-server:
	gcc -O3 src/dhtserver.c `find src/common -name "*.c"` -o bin/dhtserver

build-ping:
	gcc src/dhtping.c `find src/common -name "*.c"` -o bin/dhtping

build-hash:
	gcc src/dhthash.c `find src/common -name "*.c"`c -o bin/dhthash
