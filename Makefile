build-node:
	gcc -O3 src/dhtnode.c `find src/common -name "*.c"` -o bin/dhtnode

build-client:
	gcc src/dhtclient.c `find src/common -name "*.c"` -o bin/dhtclient

build-ping:
	gcc src/dhtping.c `find src/common -name "*.c"` -o bin/dhtping

build-hash:
	gcc src/dhthash.c `find src/common -name "*.c"` -o bin/dhthash
