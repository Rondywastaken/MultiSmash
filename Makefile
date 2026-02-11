SRCS = main.c connection.c menu.c tile.c player.c

linux:
	gcc $(SRCS) -o game -lraylib

windows:
	gcc $(SRCS) -o game -L lib/ -lraylib -lgdi32 -lwinmm -lws2_32
