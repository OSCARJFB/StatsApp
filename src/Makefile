# Writen by: Oscar Bergström
# https://github.com/OSCARJFB
#	
#	MIT License
#	Copyright (c) 2023 Oscar Bergström

cc = gcc
link := -lm
cflags := -g -Wall -Wextra -pedantic
crflags := -O3 -march=native -mtune=native -flto -fomit-frame-pointer

main: 
	$(cc) main.c $(link) $(cflags) -o main.o

release:
	$(cc) main.c $(link) $(crflags) -o statsApp

clean:
	rm main.o
