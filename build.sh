gcc -g main.c -o main
valgrind --leak-check=full ./main
cppcheck main.c
./main