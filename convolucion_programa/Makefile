NAME = convolucion
DEPS = pgm.h pgm_image.h filtros.h

$(NAME): $(NAME).o pgm.o pgm_image.o filtros.o $(DEPS)
	gcc -o $@ $(NAME).o pgm.o pgm_image.o filtros.o

test: test.o pgm.o pgm_image.o filtros.o $(DEPS)
	gcc -o test test.o pgm.o pgm_image.o filtros.o

%.o: %.c $(DEPS)
	gcc -c $< -g

.PHONY: clean
clean:
	rm -f $(NAME) *.o
	rm -f test *.o