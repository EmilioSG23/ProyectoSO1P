NAME = convolucion
DEPS = pgm.h filtros.h

$(NAME): $(NAME).o pgm.o filtros.o $(DEPS)
	gcc -o $@ $(NAME).o pgm.o filtros.o

test: test.o pgm.o filtros.o $(DEPS)
	gcc -o test test.o pgm.o filtros.o

%.o: %.c $(DEPS)
	gcc -c $< -g

.PHONY: clean
clean:
	rm -f $(NAME) *.o
	rm -f test *.o