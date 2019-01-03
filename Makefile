PROGS = fileserver fileclient

all: $(PROGS)

fileserver:
	gcc -Wall fileserver.c -o fileserver
fileclient:
	gcc -Wall fileclient.c -o fileclient 

clean:
	rm $(PROGS)
