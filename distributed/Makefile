MAINFILE := main
BINFILE := app
INCFOLDER := inc
SRCFOLDER := src
OBJFOLDER := obj

CC := gcc 
CFLAGS := -Wextra -Wall -pedantic
LFLAGS := -lwiringPi -lpthread
SRCFILES := $(wildcard $(SRCFOLDER)/*.c)

.DELETE_ON_ERROR:
all: clean $(SRCFILES:$(SRCFOLDER)/%.c=$(OBJFOLDER)/%.o)
	$(CC) $(CFLAGS) -c $(MAINFILE).c -o $(OBJFOLDER)/$(MAINFILE).o $(LFLAGS)
	$(CC) $(CFLAGS) $(OBJFOLDER)/*.o -o $(BINFILE) $(LFLAGS)

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@ -I./$(INCFOLDER) $(LFLAGS)

run: $(BINFILE)
	./$(BINFILE) $(CFG)

.PHONY: clean
clean:
	rm -rf ./${OBJFOLDER}/*.o
	rm -rf ./$(BINFILE)