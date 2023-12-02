CC = gcc
RM = rm -rf
CFLAGS = -Wall -Wextra -g3 -O0 -lpthread
all_c_files := $(wildcard *.c)

.PHONY: all clean

all: clean
	for file in $(all_c_files); do \
		$(CC) $$file -o $$file"_exe" $(CFLAGS); \
	done

clean:
	for file in $(all_c_files); do \
		$(RM) -rf $$file"_exe"; \
	done
