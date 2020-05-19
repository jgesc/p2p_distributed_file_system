build-server:
	gcc main.c common/*.c common/data_structures/*.c common/networking/*.c -o dhtserver

build-ping:
	gcc dhtping.c common/*.c common/data_structures/*.c common/networking/*.c -o dhtping
