#define SQL3_SETUP(a,b) do{}while(0)
const char * db_SETUPTEXT =\
"CREATE TABLE db_names(db_key INTEGER PRIMARY KEY, db_name TEXT);" \
"CREATE TABLE setup_texts(st_key INTEGER PRIMARY KEY, fdb_key INTEGER, setup_text TEXT);" \
"CREATE TABLE query_texts(qt_key INTEGER PRIMARY KEY, fdb_key INTEGER, query_text TEXT);" \
"CREATE TABLE query_names(qn_key INTEGER PRIMARY KEY, fqt_key INTEGER, query_name TEXT);" \
"CREATE TABLE cols(col_key INTEGER PRIMARY KEY, fqn_key INTEGER, type INTEGER, varname TEXT);" \
"CREATE TABLE binds(bind_key INTEGER PRIMARY KEY, fqn_key INTEGER, type INTEGER, varval TEXT, varsize TEXT);" \
;


#define db_SETUP() \
do{\
sqlite3_stmt * s;\
const char * t = db_SETUPTEXT;\
for (int i=0;i < 6; i++){\
	sqlite3_prepare_v2(db,  t, -1, &s, &t);\
	sqlite3_step(s);\
	sqlite3_finalize(s);\
}\
sqlite3_wal_checkpoint_v2(db,"main",SQLITE_CHECKPOINT_PASSIVE,0,0);\
}while(0)

const char * db_QUERYTEXT = \
"SELECT db_key FROM db_names WHERE db_name=?;" \
"INSERT INTO db_names VALUES("
		"NULL, ?);" \
"INSERT INTO setup_texts VALUES(NULL, "
		"?, "
		"?);" \
"SELECT qt_key "
		"FROM query_texts WHERE "
		"query_text=? AND "
		"fdb_key=?;" \
"INSERT INTO query_texts VALUES(NULL, "
		"?, "
		"?);" \
"SELECT qn_key FROM query_names WHERE query_name=?;" \
"INSERT INTO query_names VALUES(NULL, "
		"?, "
		"?);" \
"UPDATE query_names SET fqt_key=? "
		"WHERE qn_key=?;" \
"INSERT INTO cols VALUES(NULL, "
		"?, "
		"?, "
		"?);" \
"INSERT INTO binds VALUES(NULL, "
		"?, "
		"?, "
		"?, "
		"?);" \
;


enum db_enum_stmts {db_enum1, \
db_enum2, \
db_enum3, \
db_enum4, \
db_enum5, \
db_enum6, \
db_enum7, \
db_enum8, \
db_enum9, \
db_enum10, \
db_enum_size
};

static sqlite3_stmt * db_stmt_array[10];


#define db_PREPARE() \
do{\
const char * t = db_QUERYTEXT;\
for (int i=0;i < 10; i++){\
	sqlite3_prepare_v2(db,  t, -1, &db_stmt_array[i], &t);\
}\
}while(0)



#define db_FINALIZE() \
do{\
for (int i=0;i < 10; i++){\
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

#define SQL3_QUERY_insert_db_name(a,b) do{}while(0)
#define SQL3_STEP_insert_db_name()  sqlite3_step(db_stmt_array[db_enum2])
#define SQL3_RESET_insert_db_name()  sqlite3_reset(db_stmt_array[db_enum2])
#define SQL3_CLEAR_insert_db_name()  sqlite3_clear_bindings(db_stmt_array[db_enum2])
#define SQL3_COL_insert_db_name()  \
do{\
}while(0)

#define SQL3_BIND_insert_db_name()  \
do{\
sqlite3_bind_text(db_stmt_array[db_enum2], 1, (const char *)s, l, SQLITE_STATIC); \
}while(0)

#define SQL3_QUERY_insert_setup(a,b) do{}while(0)
#define SQL3_STEP_insert_setup()  sqlite3_step(db_stmt_array[db_enum3])
#define SQL3_RESET_insert_setup()  sqlite3_reset(db_stmt_array[db_enum3])
#define SQL3_CLEAR_insert_setup()  sqlite3_clear_bindings(db_stmt_array[db_enum3])
#define SQL3_COL_insert_setup()  \
do{\
}while(0)

#define SQL3_BIND_insert_setup()  \
do{\
sqlite3_bind_int64(db_stmt_array[db_enum3], 1, db_rowid); \
sqlite3_bind_text(db_stmt_array[db_enum3], 2, (const char *)string_start, str_len, SQLITE_STATIC); \
}while(0)

#define SQL3_QUERY_search_qt(a,b) do{}while(0)
#define SQL3_STEP_search_qt()  sqlite3_step(db_stmt_array[db_enum4])
#define SQL3_RESET_search_qt()  sqlite3_reset(db_stmt_array[db_enum4])
#define SQL3_CLEAR_search_qt()  sqlite3_clear_bindings(db_stmt_array[db_enum4])
#define SQL3_COL_search_qt()  \
do{\
n=sqlite3_column_int64(db_stmt_array[db_enum4], 0); \
}while(0)

#define SQL3_BIND_search_qt()  \
do{\
sqlite3_bind_text(db_stmt_array[db_enum4], 1, (const char *)text_buff, -1, SQLITE_STATIC); \
sqlite3_bind_int64(db_stmt_array[db_enum4], 2, db_rowid); \
}while(0)

#define SQL3_QUERY_insert_qt(a,b) do{}while(0)
#define SQL3_STEP_insert_qt()  sqlite3_step(db_stmt_array[db_enum5])
#define SQL3_RESET_insert_qt()  sqlite3_reset(db_stmt_array[db_enum5])
#define SQL3_CLEAR_insert_qt()  sqlite3_clear_bindings(db_stmt_array[db_enum5])
#define SQL3_COL_insert_qt()  \
do{\
}while(0)

#define SQL3_BIND_insert_qt()  \
do{\
sqlite3_bind_int64(db_stmt_array[db_enum5], 1, db_rowid); \
sqlite3_bind_text(db_stmt_array[db_enum5], 2, (const char *)text_buff, -1, SQLITE_STATIC); \
}while(0)

#define SQL3_QUERY_search_qn(a,b) do{}while(0)
#define SQL3_STEP_search_qn()  sqlite3_step(db_stmt_array[db_enum6])
#define SQL3_RESET_search_qn()  sqlite3_reset(db_stmt_array[db_enum6])
#define SQL3_CLEAR_search_qn()  sqlite3_clear_bindings(db_stmt_array[db_enum6])
#define SQL3_COL_search_qn()  \
do{\
n=sqlite3_column_int64(db_stmt_array[db_enum6], 0); \
}while(0)

#define SQL3_BIND_search_qn()  \
do{\
sqlite3_bind_text(db_stmt_array[db_enum6], 1, (const char *)s, l, SQLITE_STATIC); \
}while(0)

#define SQL3_QUERY_insert_qn(a,b) do{}while(0)
#define SQL3_STEP_insert_qn()  sqlite3_step(db_stmt_array[db_enum7])
#define SQL3_RESET_insert_qn()  sqlite3_reset(db_stmt_array[db_enum7])
#define SQL3_CLEAR_insert_qn()  sqlite3_clear_bindings(db_stmt_array[db_enum7])
#define SQL3_COL_insert_qn()  \
do{\
}while(0)

#define SQL3_BIND_insert_qn()  \
do{\
sqlite3_bind_int64(db_stmt_array[db_enum7], 1, n); \
sqlite3_bind_text(db_stmt_array[db_enum7], 2, (const char *)s, l, SQLITE_STATIC); \
}while(0)

#define SQL3_QUERY_update_qn(a,b) do{}while(0)
#define SQL3_STEP_update_qn()  sqlite3_step(db_stmt_array[db_enum8])
#define SQL3_RESET_update_qn()  sqlite3_reset(db_stmt_array[db_enum8])
#define SQL3_CLEAR_update_qn()  sqlite3_clear_bindings(db_stmt_array[db_enum8])
#define SQL3_COL_update_qn()  \
do{\
}while(0)

#define SQL3_BIND_update_qn()  \
do{\
sqlite3_bind_int64(db_stmt_array[db_enum8], 1, qt_key); \
sqlite3_bind_int64(db_stmt_array[db_enum8], 2, qn_key); \
}while(0)

#define SQL3_QUERY_insert_colInfo(a,b) do{}while(0)
#define SQL3_STEP_insert_colInfo()  sqlite3_step(db_stmt_array[db_enum9])
#define SQL3_RESET_insert_colInfo()  sqlite3_reset(db_stmt_array[db_enum9])
#define SQL3_CLEAR_insert_colInfo()  sqlite3_clear_bindings(db_stmt_array[db_enum9])
#define SQL3_COL_insert_colInfo()  \
do{\
}while(0)

#define SQL3_BIND_insert_colInfo()  \
do{\
sqlite3_bind_int64(db_stmt_array[db_enum9], 1, qn_key); \
sqlite3_bind_int64(db_stmt_array[db_enum9], 2, t); \
sqlite3_bind_text(db_stmt_array[db_enum9], 3, (const char *)s, l, SQLITE_STATIC); \
}while(0)

#define SQL3_QUERY_insert_bind(a,b) do{}while(0)
#define SQL3_STEP_insert_bind()  sqlite3_step(db_stmt_array[db_enum10])
#define SQL3_RESET_insert_bind()  sqlite3_reset(db_stmt_array[db_enum10])
#define SQL3_CLEAR_insert_bind()  sqlite3_clear_bindings(db_stmt_array[db_enum10])
#define SQL3_COL_insert_bind()  \
do{\
}while(0)

#define SQL3_BIND_insert_bind()  \
do{\
sqlite3_bind_int64(db_stmt_array[db_enum10], 1, qn_key); \
sqlite3_bind_int64(db_stmt_array[db_enum10], 2, t); \
sqlite3_bind_text(db_stmt_array[db_enum10], 3, (const char *)nv, nl, SQLITE_STATIC); \
sqlite3_bind_text(db_stmt_array[db_enum10], 4, (const char *)s, sl, SQLITE_STATIC); \
}while(0)

