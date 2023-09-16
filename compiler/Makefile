all: frontend backend

frontend: codegen.c
	cc -o codegen codegen.c

backend: vmt.c
	cc -o vmt vmt.c

clean:
	rm *.o .*.swp codegen vmt
