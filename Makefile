src = $(wildcard *.c)
obj = $(patsubst %.c, %, $(src))

args = -Wall -g

ALL: $(obj)

$(obj): %: %.c
	gcc $< -o $@ $(args)

clean:
	rm -rf $(obj)

.PHONY: clean ALL
