all: build-node build-send build-recv build-ping build-hash

build-node:
	gcc -O3 src/dhtnode.c `find src/common -name "*.c"` -o bin/dhtnode

build-send:
	gcc src/dhtsend.c `find src/common -name "*.c"` -o bin/dhtsend

build-recv:
	gcc src/dhtrecv.c `find src/common -name "*.c"` -o bin/dhtrecv

build-ping:
	gcc src/dhtping.c `find src/common -name "*.c"` -o bin/dhtping

build-hash:
	gcc src/dhthash.c `find src/common -name "*.c"` -o bin/dhthash
