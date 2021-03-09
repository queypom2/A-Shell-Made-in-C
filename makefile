# compile files
myshell: src/myshell.c src/read_parse.c src/builtin.c src/execute.c src/myshell.h src/read_parse.h src/builtin.h src/execute.h
	gcc src/myshell.c src/read_parse.c src/builtin.c src/execute.c -o bin/myshell

