assembler: main.o preprocessor.o first_pass.o second_pass.o base32.o handlers.o helpers.o list.o macro_symbol.o operations.o validators.o  
	gcc -ansi -Wall -pedantic main.o preprocessor.o first_pass.o second_pass.o base32.o handlers.o helpers.o list.o macro_symbol.o operations.o validators.o -o assembler.bin

main.o: main.c list.h helpers.h passes.h base32.h globals.h
	gcc -c -ansi -Wall -pedantic main.c -o main.o 

preprocessor.o: preprocessor.c helpers.h passes.h globals.h
	gcc -c -ansi -Wall -pedantic preprocessor.c -o preprocessor.o 

first_pass.o: first_pass.c globals.h handlers.h passes.h helpers.h validators.h base32.h
	gcc -c -ansi -Wall -pedantic first_pass.c -o first_pass.o 

second_pass.o: second_pass.c globals.h passes.h base32.h validators.h helpers.h handlers.h
	gcc -c -ansi -Wall -pedantic second_pass.c -o second_pass.o 
	
base32.o: base32.c base32.h
	gcc -c -ansi -Wall -pedantic base32.c -o base32.o
	
handlers.o: handlers.c globals.h helpers.h
	gcc -c -ansi -Wall -pedantic handlers.c -o handlers.o 

helpers.o: helpers.c globals.h helpers.h
	gcc -c -ansi -Wall -pedantic helpers.c -o helpers.o 
	
list.o: list.c list.h
	gcc -c -ansi -Wall -pedantic list.c -o list.o 
	
macro_symbol.o: macro_symbol.c globals.h
	gcc -c -ansi -Wall -pedantic macro_symbol.c -o macro_symbol.o 
	
operations.o: operations.c globals.h
	gcc -c -ansi -Wall -pedantic operations.c -o operations.o 
	
validators.o: validators.c validators.h helpers.h
	gcc -c -ansi -Wall -pedantic validators.c -o validators.o 