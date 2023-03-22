name:=main-v2

all: clean build exec

build:
	clang -Wall -Wextra -Wpedantic -Wformat=2 -Wno-unused-parameter -Wshadow -Wwrite-strings -Wstrict-prototypes -Wold-style-definition -Wredundant-decls -Wnested-externs -Wmissing-include-dirs `pkg-config --cflags --libs gtk+-3.0` -c $(name).c -o $(name).o
	clang -Wall -Wextra -Wpedantic -Wformat=2 -Wno-unused-parameter -Wshadow -Wwrite-strings -Wstrict-prototypes -Wold-style-definition -Wredundant-decls -Wnested-externs -Wmissing-include-dirs  `pkg-config --cflags --libs gtk+-3.0` -lm $(name).o -o $(name)

.PHONY: clean
clean:
	rm -rf $(name).o
	rm -rf $(name)

.PHONY: exec
exec:
	./$(name)