
all: bin tool tool_output sqlite3 src gen bin/sqlite3Helper

bin/sqlite3Helper: src/*.c tool_output/sqlite3_helper_gram.c tool_output/sqlite3_helper_lex.c sqlite3/sqlite3.o
#	./bin/sqlite3Helper
	gcc -O2 -s -o bin/sqlite3Helper src/sqlite3_helper_compiler.c sqlite3/sqlite3.o -Wall -ldl

tool_output/sqlite3_helper_gram.c: tool/lemon src/sqlite3_helper_gram.y
	./tool/lemon src/sqlite3_helper_gram.y -s -dtool_output
	sed -i 's/void Parse/static void Parse/g' tool_output/sqlite3_helper_gram.c

tool/lemon: tool/lemon.c tool/lempar.c
	gcc -O2 tool/lemon.c -o tool/lemon

tool/lemon.c:
	curl https://raw.githubusercontent.com/sqlite/sqlite/master/tool/lemon.c > tool/lemon.c

tool/lempar.c:
	curl https://raw.githubusercontent.com/sqlite/sqlite/master/tool/lempar.c > tool/lempar.c

tool_output/sqlite3_helper_lex.c: src/sqlite3_helper_lex.re
	re2c -W --eager-skip src/sqlite3_helper_lex.re -o tool_output/sqlite3_helper_lex.c

sqlite3/sqlite3.o:
	rm -f sqlite-autoconf-3300100.tar.gz
	wget "https://www.sqlite.org/2019/sqlite-autoconf-3300100.tar.gz"
	tar -xvzf sqlite-autoconf-3300100.tar.gz
	rm -f sqlite-autoconf-3300100.tar.gz
	(cd sqlite-autoconf-3300100 && ./configure)
	sed -i 's/-g -O2/-O2/' sqlite-autoconf-3300100/Makefile
	sed -i 's/-DSQLITE_THREADSAFE=1 -DSQLITE_ENABLE_FTS4 -DSQLITE_ENABLE_FTS5 -DSQLITE_ENABLE_JSON1 -DSQLITE_ENABLE_RTREE/-DSQLITE_THREADSAFE=0 -DSQLITE_DQS=0 -DSQLITE_DEFAULT_MEMSTATUS=0 -DSQLITE_LIKE_DOESNT_MATCH_BLOBS -DSQLITE_MAX_EXPR_DEPTH=0 -DSQLITE_OMIT_DECLTYPE -DSQLITE_OMIT_DEPRECATED -DSQLITE_OMIT_PROGRESS_CALLBACK -DSQLITE_OMIT_SHARED_CACHE/g' sqlite-autoconf-3300100/Makefile
	(cd sqlite-autoconf-3300100 && make sqlite3.o)
	cp sqlite-autoconf-3300100/sqlite3.o sqlite3/sqlite3.o
	cp sqlite-autoconf-3300100/sqlite3.h sqlite3/sqlite3.h
	rm -f -R sqlite-autoconf-3300100

bin:
	mkdir bin

tool:
	mkdir tool

tool_output:
	mkdir tool_output

sqlite3:
	mkdir sqlite3

src:
	mkdir src

gen:
	mkdir gen

clean:
	rm bin/sqlite3Helper
	rm -f tool_output/sqlite3_helper_gram.c
	rm -f tool_output/sqlite3_helper_lex.c
