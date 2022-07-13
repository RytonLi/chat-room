src = $(wildcard *.c)
obj = $(patsubst %.c, %, $(src))

args = -Wall -g -lrt

ALL: $(obj)

$(obj): %: %.c
	g++ $< -o $@ $(args)

clean:
	rm -rf $(obj)

.PHONY: clean ALL
