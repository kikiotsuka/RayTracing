all:
	clang++ *.cpp -lsfml-graphics -lsfml-window -lsfml-system -o raytracing -g
clean:
	rm raytracing
