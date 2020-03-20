# SQLite3_Helper
write SQL for sqlite3 easier with inline variables

To build the project requires gcc, make, and re2c. If on Ubunutu intall build-essential and re2c.

Uses lemon, but the Makefile will take care of that for you.

Clone from git, then `$ make`

When writing sqlite3 code it can be time consuming to write out all the variables and _prepare, _bind, _column, and _finalize statments.
This tool takes care of a lot of the work for you.
This project is partially bootstrapped to show you can co-mingle sqlite 3 api code with this tool.
By default it runs on all files ending in ".c" in directory src/. It will record all queries and set up SQL and generate macros. This allows you to write queries inline with where they are used without having a lot of clutter.

Output is put into gen/sql3_macros.c, all you do is `#include "../gen/sql3_macros.c"` at or near the top of your application(assumed from the src/ directory) for this tool to work.

Set up code is in src/sqlite3_helper_compiler.c, browse src/build_data.c for examples of usage. 
The following examples are taken from there.

Syntax breakdown of "SELECT db_key=?i:n FROM db_names WHERE db_name=?t@s$l;": 

db_key=?i:n = result of select query is type int to be put into variable "n", which must be in scope.

db_name=?t@s$l = this is a sqlite3 variable which is type TEXT, contained in "s" and optional length specified in "l".

```c
sqlite3_open(":memory:", &db);
db_SETUP(); // run ALL SQL statments for database "db" inside of SQL3_SETUP's
db_PREPARE(); // prepare all SQL3_QUERY_* statements for their respective databases.
SQL3_SETUP(db, "CREATE TABLE db_names(db_key INTEGER PRIMARY KEY, db_name TEXT);");
...
  // write query for tool to record, becomes nothing. "find_db_name" becomes the unique query name.
  SQL3_QUERY_find_db_name(db, "SELECT db_key=?i:n FROM db_names WHERE db_name=?t@s$l;"); 
  SQL3_BIND_find_db_name(); // run all bind statements
  /* prepare SQL query */
  n = SQL3_STEP_find_db_name(); // step the query
  if (n==SQLITE_ROW){ 
    SQL3_COL_find_db_name(); // run all column statements
    SQL3_RESET_find_db_name(); // reset the statement
    return n;
  }
...
// end of application
db_FINALIZE(); // finalize all statements and free memory
```

This is all translated into the following macros which you would include at the top of your file. See at gen/sql3_macros.c for the full set of generated macros used to bootstrap this tool.

```c
#define SQL3_SETUP(a,b) do{}while(0)

const char * db_SETUPTEXT =\
"CREATE TABLE db_names(db_key INTEGER PRIMARY KEY, db_name TEXT);" \
;

#define db_SETUP() \
do{\
sqlite3_stmt * s;\
const char * t = db_SETUPTEXT;\
for (int i=0;i < 1; i++){\
	sqlite3_prepare_v2(db,  t, -1, &s, &t);\
	sqlite3_step(s);\
	sqlite3_finalize(s);\
}\
sqlite3_wal_checkpoint_v2(db,"main",SQLITE_CHECKPOINT_PASSIVE,0,0);\
}while(0)

const char * db_QUERYTEXT = \
"SELECT db_key FROM db_names WHERE db_name=?;" \
;

enum db_enum_stmts {db_enum1, \
db_enum_size
};

static sqlite3_stmt * db_stmt_array[1];

#define db_PREPARE() \
do{\
const char * t = db_QUERYTEXT;\
for (int i=0;i < 1; i++){\
	sqlite3_prepare_v2(db,  t, -1, &db_stmt_array[i], &t);\
}\
}while(0)

#define db_FINALIZE() \
do{\
for (int i=0;i < 1; i++){\
	sqlite3_finalize(db_stmt_array[i]);\
}\
}while(0)

#define SQL3_QUERY_find_db_name(a,b) do{}while(0)
#define SQL3_STEP_find_db_name()  sqlite3_step(db_stmt_array[db_enum1])
#define SQL3_RESET_find_db_name()  sqlite3_reset(db_stmt_array[db_enum1])
#define SQL3_CLEAR_find_db_name()  sqlite3_clear_bindings(db_stmt_array[db_enum1])
#define SQL3_COL_find_db_name()  \
do{\
n=sqlite3_column_int64(db_stmt_array[db_enum1], 0); \
}while(0)

#define SQL3_BIND_find_db_name()  \
do{\
sqlite3_bind_text(db_stmt_array[db_enum1], 1, (const char *)s, l, SQLITE_STATIC); \
}while(0)
```
