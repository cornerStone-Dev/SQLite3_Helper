/* here is the code start */

#include <stdio.h>
#include <stdlib.h>   
#include <string.h>
#include <stdint.h>

#define NDEBUG
#define Parse_ENGINEALWAYSONSTACK

#include "std_types.h"

typedef struct token_s{
	u8 * s;
	u32  l;
} Token;

typedef struct col_s{
	u8 * p;
	u32  l;
	u32  t;
} Column;

typedef struct bind_s{
	u8 * p;
	u8 * sp;
	u32  l;
	u32  spl;
	u32  t;
} Bind;

typedef struct parser_s{
	u8 * string_start;
	u8 * string_end;
	u32  qn_key;
} ParserState;


#include "../sqlite3/sqlite3.h"

#include "../sqlite3/sqlite3.h"
static sqlite3 * db;
#include "build_data.c"
#include "../tool_output/sqlite3_helper_gram.h"
#include "../tool_output/sqlite3_helper_lex.c"
#include "../tool_output/sqlite3_helper_gram.c"

#define INTPUT_FILE "input/test.c"
#define OUTPUT_FILE "output/sql3_macros.c"



/* stmt enum */
enum g_enum_stmts {
	g_insrt_cntype,
	g_sel_cnMemNum,
	size_of_stmt_array
};

static sqlite3_stmt * g_stmtAr[size_of_stmt_array];
//"CREATE TABLE cnt_type(typeName TEXT PRIMARY KEY, mem_num INTEGER) WITHOUT ROWID;"

#define SELECT_CMN "SELECT mem_num FROM cnt_type WHERE typeName=?;"
//static sqlite3_stmt * g_stmtArray[g_search_for_email];

#define INSERT_CNTYPE \
"INSERT INTO cnt_type VALUES(@nm, @mn);"
//static sqlite3_stmt * g_stmtArray[g_insert_registration];

#define SELECT_LOGIN_INFO \
		"SELECT Auth_id, accessLevel, sesCookie, passSalt, passHash FROM " \
		"AuthTable WHERE userEmail=? OR Auth_id=1 ORDER BY Auth_id DESC LIMIT 1;"
//static sqlite3_stmt * g_stmtArray[g_select_login_info];

#define UPDATE_PASSWORD "UPDATE AuthTable SET sesCookie=?, passHash=? WHERE Auth_id=?;"
//static sqlite3_stmt * g_stmtArray[g_update_password];

#define ALL_SQL_STATEMENTS \
		INSERT_CNTYPE \
		SELECT_CMN 

//~ static s32
//~ has_cnt_ending(u8 * mem_type)
//~ {
	//~ /* find '_' if it is there */
	//~ while ( (*mem_type!='_') && (*mem_type!=0) ) {mem_type++;}
	
	//~ if (*mem_type!='_') {
		//~ return 0;
	//~ }
	//~ mem_type++;
	//~ if (*mem_type!='c') {
		//~ return 0;
	//~ }
	//~ mem_type++;
	//~ if (*mem_type!='n') {
		//~ return 0;
	//~ }
	//~ mem_type++;
	//~ if (*mem_type!='t') {
		//~ return 0;
	//~ }
	//~ return 1;
	
//~ }

//~ static s32
//~ has_cnt_provided(ParserState * p_s, unsigned char * mem_type, u8 * cnt_type_loc, u32 mtl)
//~ {
	//~ s32 stm_state;
	//~ s32 mem_num;

	//~ sqlite3_bind_text(
		//~ g_stmtAr[g_sel_cnMemNum],
		//~ 1,
		//~ (char *)mem_type,
		//~ mtl,
		//~ SQLITE_STATIC);
	//~ stm_state = sqlite3_step(g_stmtAr[g_sel_cnMemNum]);
	//~ if(stm_state == SQLITE_DONE) {
		//~ return -1;
	//~ }
	//~ mem_num = sqlite3_column_int(g_stmtAr[g_sel_cnMemNum], 0);
	//~ sqlite3_reset(g_stmtAr[g_sel_cnMemNum]);
	//~ return mem_num;
//~ }

static void
sematic_analysis(ParserState * p_s, unsigned char * output)
{
	
}

//~ static void
//~ null_terminate(ParserState * p_s)
//~ {
	//~ for(s32 x = 0;x<p_s->n_m;x++) 
	//~ {
		//~ *(p_s->mem_type[x]+p_s->mtl[x])=0;
		//~ *(p_s->mem_name[x]+p_s->mnl[x])=0;
	//~ }
	//~ /* null terminate type name */
	//~ *(p_s->type_name+p_s->tnl)=0;
//~ }

static void 
prepare_slq_statements(void)
{
	/* alternative loop based init */
	const char * tail = 0;
	int i=0;
	
	sqlite3_prepare_v2(db,  ALL_SQL_STATEMENTS, -1, &g_stmtAr[i], &tail);
	for (i=1;i < size_of_stmt_array; i++){
		sqlite3_prepare_v2(db,  tail, -1, &g_stmtAr[i], &tail);
	}
	return;
}

static s32
execute_sql(const char * request)
{
	sqlite3_stmt * sql_statement;
	int n;
	
	/* prepare SQL query */
	sqlite3_prepare_v2(db,  request, 
						-1, &sql_statement, NULL);
	/* execute sql statement */					
	n = sqlite3_step(sql_statement);
	
	/* all work has completed */
	sqlite3_finalize(sql_statement);
	return n;
}

static void
print_setup(void)
{
	sqlite3_stmt * sql_stmt1;
	sqlite3_stmt * sql_stmt2;
	u8 * db_name, * setup_text;
	s32 n, j, db_key;
	
	/* read out db name and keys */
	
	/* prepare SQL query */
	sqlite3_prepare_v2(
		db,
		"SELECT db_key, db_name FROM db_names ORDER BY db_key ASC;",
		-1,
		&sql_stmt1,
		NULL);

	/* execute sql statement */					
	n = sqlite3_step(sql_stmt1);
	while (n==SQLITE_ROW){ // we have an entry
		db_key = sqlite3_column_int(sql_stmt1, 0);
		db_name = (u8 * )sqlite3_column_text(sql_stmt1, 1);
		printf("Database %s with key %d found\n", db_name, db_key);
		/* prepare SQL query */
		sqlite3_prepare_v2(
			db,
			"SELECT setup_text FROM setup_texts WHERE fdb_key=? ORDER BY st_key ASC;",
			-1,
			&sql_stmt2,
			NULL);
		sqlite3_bind_int(sql_stmt2, 1, db_key);
		/* execute sql statement */					
		j = sqlite3_step(sql_stmt2);
		while (j==SQLITE_ROW){ // we have an entry
			setup_text = (u8 * )sqlite3_column_text(sql_stmt2, 0);
			printf("Setup\n %s\n with fkey %d found\n", setup_text, db_key);
			j = sqlite3_step(sql_stmt2);
		}
		sqlite3_finalize(sql_stmt2);
		n = sqlite3_step(sql_stmt1);
	}
	sqlite3_finalize(sql_stmt1);
}

static void
generate_setup(u8 * out)
{
	sqlite3_stmt * sql_stmt1;
	sqlite3_stmt * sql_stmt2;
	u8 * db_name, * setup_text;
	s32 n, j, db_key, stmn_cnt=0;
	s32 def_flag=0;
	
	/* read out db name and keys */
	out+=sprintf((char*)out, "#define SQL3_SETUP(a,b)  \n");
	/* prepare SQL query */
	sqlite3_prepare_v2(
		db,
		"SELECT db_key, db_name FROM db_names ORDER BY db_key ASC;",
		-1,
		&sql_stmt1,
		NULL);

	/* execute sql statement */					
	n = sqlite3_step(sql_stmt1);
	while (n==SQLITE_ROW){ // we have an entry
		db_key = sqlite3_column_int(sql_stmt1, 0);
		db_name = (u8 * )sqlite3_column_text(sql_stmt1, 1);
		/* prepare SQL query */
		sqlite3_prepare_v2(
			db,
			"SELECT setup_text FROM setup_texts WHERE fdb_key=? ORDER BY st_key ASC;",
			-1,
			&sql_stmt2,
			NULL);
		sqlite3_bind_int(sql_stmt2, 1, db_key);
		/* execute sql statement */					
		j = sqlite3_step(sql_stmt2);
		while (j==SQLITE_ROW){ // we have an entry
			if(def_flag==0){
				def_flag=1;
				/* output number of members */
				out+=sprintf((char*)out, "#define %s_SETUPTEXT \\\n", db_name);
			}
			setup_text = (u8 * )sqlite3_column_text(sql_stmt2, 0);
			out+=sprintf((char*)out, "%s \\\n", setup_text);
			stmn_cnt++;
			j = sqlite3_step(sql_stmt2);
		}
		def_flag=0;
		// setup is in define
		out+=sprintf(
			(char*)out,
			"\n\n#define %s_SETUP() \\\n"//1
			"do{\\\n"
			"sqlite3_stmt * s;\\\n"
			"const char * t = %s_SETUPTEXT;\\\n"//1
			"for (int i=0;i < %d; i++){\\\n" //2
			"\tsqlite3_prepare_v2(%s,  t, -1, &s, &t);\\\n"//3
			"\tsqlite3_step(s);\\\n"
			"\tsqlite3_finalize(s);\\\n"
			"}\\\n"
			"sqlite3_wal_checkpoint_v2(%s,\"main\",SQLITE_CHECKPOINT_PASSIVE,0,0);\\\n"//4
			"}while(0)\n\n"
			,
			db_name,//1
			db_name,//2
			stmn_cnt,//3
			db_name,//4
			db_name);//5
		sqlite3_finalize(sql_stmt2);
		stmn_cnt=0;
		n = sqlite3_step(sql_stmt1);
	}
	sqlite3_finalize(sql_stmt1);
}

static void
generate_queries(u8 * out)
{
	sqlite3_stmt * sql_stmt1;
	sqlite3_stmt * sql_stmt2;
	u8 * db_name, * query_text;
	s32 n, j, db_key, def_flag=0,q_cnt=0;
	s32 qt_key[128];
	
	/* read out db name and keys */
	
	/* prepare SQL query */
	sqlite3_prepare_v2(
		db,
		"SELECT db_key, db_name FROM db_names ORDER BY db_key ASC;",
		-1,
		&sql_stmt1,
		NULL);

	/* execute sql statement */					
	n = sqlite3_step(sql_stmt1);
	while (n==SQLITE_ROW){ // we have an entry
		db_key = sqlite3_column_int(sql_stmt1, 0);
		db_name = (u8 * )sqlite3_column_text(sql_stmt1, 1);
		/* prepare SQL query */
		sqlite3_prepare_v2(
			db,
			"SELECT qt_key, query_text FROM query_texts WHERE fdb_key=? ORDER BY qt_key ASC;",
			-1,
			&sql_stmt2,
			NULL);
		sqlite3_bind_int(sql_stmt2, 1, db_key);
		/* execute sql statement */					
		j = sqlite3_step(sql_stmt2);
		while (j==SQLITE_ROW){ // we have an entry
			if(def_flag==0){
				def_flag=1;
				/* output number of members */
				out+=sprintf((char*)out, "#define %s_QUERYTEXT \\\n", db_name);
			}
			qt_key[q_cnt] = sqlite3_column_int(sql_stmt2, 0);
			query_text = (u8 * )sqlite3_column_text(sql_stmt2, 1);
			/* execute sql statement */
			out+=sprintf((char*)out, "%s \\\n", query_text);
			q_cnt++;
			j = sqlite3_step(sql_stmt2);
		}
		def_flag=0;
		
		// create enum
		out+=sprintf((char*)out, "\n\n");
		out+=sprintf((char*)out, "enum %s_enum_stmts {", db_name);
		for(int i = 0; i < q_cnt; i++){
			out+=sprintf((char*)out, "%s_enum%d, \\\n", db_name, qt_key[i]);
		}
		out+=sprintf((char*)out, "%s_enum_size\n", db_name);
		out+=sprintf((char*)out, "}\n\n");
		out+=sprintf((char*)out, "static sqlite3_stmt * %s_stmt_array[%d]\n", db_name, q_cnt);
		// setup is in define
		out+=sprintf(
			(char*)out,
			"\n\n#define %s_PREPARE() \\\n"//1
			"do{\\\n"
			"const char * t = %s_QUERYTEXT;\\\n"//2
			"for (int i=0;i < %d; i++){\\\n" //3
			"\tsqlite3_prepare_v2(%s,  t, -1, &%s_stmt_array[i], &t);\\\n"//4,5
			"}\\\n"
			"}while(0)\n\n"
			,
			db_name,//1
			db_name,//2
			q_cnt,//3
			db_name,//4
			db_name);//5
		sqlite3_finalize(sql_stmt2);
		q_cnt=0;
		n = sqlite3_step(sql_stmt1);
	}
	sqlite3_finalize(sql_stmt1);
}

static void
generate_macros(u8 * out)
{
	sqlite3_stmt * sql_stmt1;
	sqlite3_stmt * sql_stmt2;
	sqlite3_stmt * sql_stmt3;
	sqlite3_stmt * sql_stmt4;
	u8 * db_name, * query_text, * query_name, * varname, * varsize;
	s32 n, j, k, m, db_key, qt_key, qn_key;
	s32 col_cnt=0, bind_cnt=1;
	
	/* read out db name and keys */
	
	/* prepare SQL query */
	sqlite3_prepare_v2(
		db,
		"SELECT db_key, db_name FROM db_names ORDER BY db_key ASC;",
		-1,
		&sql_stmt1,
		NULL);

	/* execute sql statement */					
	n = sqlite3_step(sql_stmt1);
	while (n==SQLITE_ROW){ // we have an entry
		db_key = sqlite3_column_int(sql_stmt1, 0);
		db_name = (u8 * )sqlite3_column_text(sql_stmt1, 1);
		/* prepare SQL query */
		sqlite3_prepare_v2(
			db,
			"SELECT qt_key, query_text FROM query_texts WHERE fdb_key=? ORDER BY qt_key ASC;",
			-1,
			&sql_stmt2,
			NULL);
		sqlite3_bind_int(sql_stmt2, 1, db_key);
		/* execute sql statement */					
		j = sqlite3_step(sql_stmt2);
		while (j==SQLITE_ROW){ // we have an entry
			qt_key = sqlite3_column_int(sql_stmt2, 0);
			query_text = (u8 * )sqlite3_column_text(sql_stmt2, 1);
			/* prepare SQL query */
			sqlite3_prepare_v2(
				db,
				"SELECT qn_key, query_name FROM query_names WHERE fqt_key=? ORDER BY qn_key ASC;",
				-1,
				&sql_stmt3,
				NULL);
			sqlite3_bind_int(sql_stmt3, 1, qt_key);
			/* execute sql statement */					
			k = sqlite3_step(sql_stmt3);
			while (k==SQLITE_ROW){ // we have an entry
				qn_key = sqlite3_column_int(sql_stmt3, 0);
				query_name = (u8 * )sqlite3_column_text(sql_stmt3, 1);
				
				out+=sprintf((char*)out, "#define SQL3_QUERY_%s(a,b)  \n", query_name);
				out+=sprintf((char*)out,
					"#define SQL3_STEP_%s()  sqlite3_step(%s_stmt_array[%s_enum%d])\n",
					query_name,
					db_name,
					db_name,
					qt_key);
				
				out+=sprintf((char*)out,
					"#define SQL3_RESET_%s()  sqlite3_reset(%s_stmt_array[%s_enum%d])\n",
					query_name,
					db_name,
					db_name,
					qt_key);
				
				out+=sprintf((char*)out,
					"#define SQL3_CLEAR_%s()  sqlite3_clear_bindings(%s_stmt_array[%s_enum%d])\n",
					query_name,
					db_name,
					db_name,
					qt_key);
				
				out+=sprintf((char*)out,
					"#define SQL3_COL_%s()  \\\n",
					query_name);
				
				out+=sprintf((char*)out,"do{\\\n");
				
				/* prepare SQL query */
				sqlite3_prepare_v2(
					db,
					"SELECT type, varname FROM cols WHERE fqn_key=? ORDER BY col_key ASC;",
					-1,
					&sql_stmt4,
					NULL);
				sqlite3_bind_int(sql_stmt4, 1, qn_key);
				m = sqlite3_step(sql_stmt4);
				while (m==SQLITE_ROW){ // we have an entry
					m = sqlite3_column_int(sql_stmt4, 0);
					varname = (u8 * )sqlite3_column_text(sql_stmt4, 1);
					switch (m) {
						//case 'b':
						case 1:
						out+=sprintf((char*)out,
							"%s=sqlite3_column_blob(%s_stmt_array[%s_enum%d], %d); \\\n",
							varname,
							db_name,
							db_name,
							qt_key,
							col_cnt);
						break;
						
						//case 'i':
						case 2:
						out+=sprintf((char*)out,
							"%s=sqlite3_column_int64(%s_stmt_array[%s_enum%d], %d); \\\n",
							varname,
							db_name,
							db_name,
							qt_key,
							col_cnt);
						break;
						
						//case 't':
						case 3:
						out+=sprintf((char*)out,
							"%s=sqlite3_column_text(%s_stmt_array[%s_enum%d], %d); \\\n",
							varname,
							db_name,
							db_name,
							qt_key,
							col_cnt);
						break;
						
						//case 'd':
						case 4:
						out+=sprintf((char*)out,
							"%s=sqlite3_column_double(%s_stmt_array[%s_enum%d], %d); \\\n",
							varname,
							db_name,
							db_name,
							qt_key,
							col_cnt);
						break;
						
						default:
						out+=sprintf((char*)out,
							"%s=ERROR_INVALID_TYPE!!! \\\n");
						break;
					}
					
					col_cnt++;
					m = sqlite3_step(sql_stmt4);
				}
				sqlite3_finalize(sql_stmt4);
				
				out+=sprintf((char*)out,"}while(0)\n\n");
				col_cnt=0;
				
				out+=sprintf((char*)out,
					"#define SQL3_BIND_%s()  \\\n",
					query_name);
				
				out+=sprintf((char*)out,"do{\\\n");
				
				
				/* prepare SQL query */
				sqlite3_prepare_v2(
					db,
					"SELECT type, varval, varsize FROM binds WHERE fqn_key=? ORDER BY bind_key ASC;",
					-1,
					&sql_stmt4,
					NULL);
				sqlite3_bind_int(sql_stmt4, 1, qn_key);
				m = sqlite3_step(sql_stmt4);
				while (m==SQLITE_ROW){ // we have an entry
					m = sqlite3_column_int(sql_stmt4, 0);
					varname = (u8 * )sqlite3_column_text(sql_stmt4, 1);
					varsize = (u8 * )sqlite3_column_text(sql_stmt4, 2);
					
					switch (m) {
						//case 'b':
						case 1:
						out+=sprintf((char*)out,
							"%s=sqlite3_column_blob(%s_stmt_array[%s_enum%d], %d); \\\n",
							varname,
							db_name,
							db_name,
							qt_key,
							col_cnt);
						break;
						
						//case 'i':
						case 2:
						out+=sprintf((char*)out,
							"sqlite3_bind_int(%s_stmt_array[%s_enum%d], %d, ); \\\n",
							varname,
							db_name,
							db_name,
							qt_key,
							col_cnt);
						break;
						
						//case 't':
						case 3:
						out+=sprintf((char*)out,
							"%s=sqlite3_column_text(%s_stmt_array[%s_enum%d], %d); \\\n",
							varname,
							db_name,
							db_name,
							qt_key,
							col_cnt);
						break;
						
						//case 'd':
						case 4:
						out+=sprintf((char*)out,
							"%s=sqlite3_column_double(%s_stmt_array[%s_enum%d], %d); \\\n",
							varname,
							db_name,
							db_name,
							qt_key,
							col_cnt);
						break;
						
						default:
						out+=sprintf((char*)out,
							"%s=ERROR_INVALID_TYPE!!! \\\n");
						break;
					}
					
					bind_cnt++;
					
					
					m = sqlite3_step(sql_stmt4);
				}
				sqlite3_finalize(sql_stmt4);
				
				out+=sprintf((char*)out,"}while(0)\n\n");
				bind_cnt=1;
				k = sqlite3_step(sql_stmt3);
			}
			sqlite3_finalize(sql_stmt3);
			/* execute sql statement */	
			j = sqlite3_step(sql_stmt2);
		}
		sqlite3_finalize(sql_stmt2);
		n = sqlite3_step(sql_stmt1);
	}
	sqlite3_finalize(sql_stmt1);
}

static void
print_queries(void)
{
	sqlite3_stmt * sql_stmt1;
	sqlite3_stmt * sql_stmt2;
	sqlite3_stmt * sql_stmt3;
	sqlite3_stmt * sql_stmt4;
	u8 * db_name, * setup_text;
	s32 n, j, k, m, db_key, qt_key, qn_key;
	
	/* read out db name and keys */
	
	/* prepare SQL query */
	sqlite3_prepare_v2(
		db,
		"SELECT db_key, db_name FROM db_names ORDER BY db_key ASC;",
		-1,
		&sql_stmt1,
		NULL);

	/* execute sql statement */					
	n = sqlite3_step(sql_stmt1);
	while (n==SQLITE_ROW){ // we have an entry
		db_key = sqlite3_column_int(sql_stmt1, 0);
		db_name = (u8 * )sqlite3_column_text(sql_stmt1, 1);
		printf("Database %s with key %d found\n", db_name, db_key);
		/* prepare SQL query */
		sqlite3_prepare_v2(
			db,
			"SELECT qt_key, query_text FROM query_texts WHERE fdb_key=? ORDER BY qt_key ASC;",
			-1,
			&sql_stmt2,
			NULL);
		sqlite3_bind_int(sql_stmt2, 1, db_key);
		/* execute sql statement */					
		j = sqlite3_step(sql_stmt2);
		while (j==SQLITE_ROW){ // we have an entry
			qt_key = sqlite3_column_int(sql_stmt2, 0);
			setup_text = (u8 * )sqlite3_column_text(sql_stmt2, 1);
			printf("Query Text\n %s\n with qt_key %d found\n", setup_text, qt_key);
			/* prepare SQL query */
			sqlite3_prepare_v2(
				db,
				"SELECT qn_key, query_name FROM query_names WHERE fqt_key=? ORDER BY qn_key ASC;",
				-1,
				&sql_stmt3,
				NULL);
			sqlite3_bind_int(sql_stmt3, 1, qt_key);
			/* execute sql statement */					
			k = sqlite3_step(sql_stmt3);
			while (k==SQLITE_ROW){ // we have an entry
				qn_key = sqlite3_column_int(sql_stmt3, 0);
				setup_text = (u8 * )sqlite3_column_text(sql_stmt3, 1);
				printf("Query Name\n %s\n with qn_key %d found\n", setup_text, qn_key);
				
				/* prepare SQL query */
				sqlite3_prepare_v2(
					db,
					"SELECT type, varname FROM cols WHERE fqn_key=? ORDER BY col_key ASC;",
					-1,
					&sql_stmt4,
					NULL);
				sqlite3_bind_int(sql_stmt4, 1, qn_key);
				m = sqlite3_step(sql_stmt4);
				while (m==SQLITE_ROW){ // we have an entry
					m = sqlite3_column_int(sql_stmt4, 0);
					setup_text = (u8 * )sqlite3_column_text(sql_stmt4, 1);
					printf("Column variable: %s with type %d found\n", setup_text, m);
					m = sqlite3_step(sql_stmt4);
				}
				sqlite3_finalize(sql_stmt4);
				
				/* prepare SQL query */
				sqlite3_prepare_v2(
					db,
					"SELECT type, varval, varsize FROM binds WHERE fqn_key=? ORDER BY bind_key ASC;",
					-1,
					&sql_stmt4,
					NULL);
				sqlite3_bind_int(sql_stmt4, 1, qn_key);
				m = sqlite3_step(sql_stmt4);
				while (m==SQLITE_ROW){ // we have an entry
					m = sqlite3_column_int(sql_stmt4, 0);
					setup_text = (u8 * )sqlite3_column_text(sql_stmt4, 1);
					db_name = (u8 * )sqlite3_column_text(sql_stmt4, 2);
					printf("Bind varibale: %s with type %d found and size variable %s\n", setup_text, m, db_name);
					m = sqlite3_step(sql_stmt4);
				}
				sqlite3_finalize(sql_stmt4);
				
				k = sqlite3_step(sql_stmt3);
			}
			sqlite3_finalize(sql_stmt3);
			/* execute sql statement */	
			j = sqlite3_step(sql_stmt2);
		}
		sqlite3_finalize(sql_stmt2);
		n = sqlite3_step(sql_stmt1);
	}
	sqlite3_finalize(sql_stmt1);
}

int main(int argc, char **argv)
{
	
	const unsigned char * data;
	void *pEngine;     /* The LEMON-generated LALR(1) parser */
	yyParser sEngine;  /* Space to hold the Lemon-generated Parser object */
	unsigned char output_string[8192] = {0};
	unsigned char * output = output_string;
	Token token = {0};
	int tmp_token;
	ParserState p_s = {0};
	FILE * pFile, * outputFile;
	size_t lSize;
	unsigned char * buffer;
	size_t result;
	sqlite3_open(":memory:", &db);
	execute_sql("CREATE TABLE db_names(db_key INTEGER PRIMARY KEY, db_name TEXT);");
	execute_sql("CREATE TABLE setup_texts(st_key INTEGER PRIMARY KEY, fdb_key INTEGER, setup_text TEXT);");
	
	execute_sql("CREATE TABLE query_texts(qt_key INTEGER PRIMARY KEY, fdb_key INTEGER, query_text TEXT);");
	execute_sql("CREATE TABLE query_names(qn_key INTEGER PRIMARY KEY, fqt_key INTEGER, query_name TEXT);");
	execute_sql("CREATE TABLE cols(col_key INTEGER PRIMARY KEY, fqn_key INTEGER, type INTEGER, varname TEXT);");
	execute_sql("CREATE TABLE binds(bind_key INTEGER PRIMARY KEY, fqn_key INTEGER, type INTEGER, varval TEXT, varsize TEXT);");
	
	/* prepare database statements */
	prepare_slq_statements();
	
	pFile = fopen ( INTPUT_FILE, "rb" );
	if (pFile==NULL) {fputs ("File error",stderr); exit (1);}
	
	outputFile = fopen ( OUTPUT_FILE, "w" );
	if (outputFile==NULL) {fputs ("File error",stderr); exit (1);}
	
	// obtain file size:
	fseek (pFile , 0 , SEEK_END);
	lSize = ftell (pFile);
	rewind (pFile);

	// allocate memory to contain the whole file:
	buffer = (unsigned char*) malloc (sizeof(char)*lSize+1);
	if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}
	data = buffer;
	// copy the file into the buffer:
	result = fread (buffer,1,lSize,pFile);
	if (result != lSize) {fputs ("Reading error",stderr); exit (3);}
	
	buffer[lSize]=0;
	
	/*pParser = ParseAlloc( malloc, &p_s );*/
	pEngine = &sEngine;
	ParseInit(pEngine, &p_s);

#ifndef NDEBUG
	ParseTrace(stdout, "debug:: ");
#endif

	printf("starting parse\n");
	do {
		tmp_token = lex(&data, &token);

		Parse(pEngine, tmp_token, token); 
		
	} while (tmp_token != 0);
	
	/* output collected data, aka code gen */
	print_setup();
	print_queries();
	
	/* code gen */
	generate_setup(output);
	/* output to file */
	fwrite (output_string , sizeof(char), strlen((const char *)output_string), outputFile);
	/* reset to start of array */
	output = output_string;
	
	/* code gen */
	generate_queries(output);
	/* output to file */
	fwrite (output_string , sizeof(char), strlen((const char *)output_string), outputFile);
	/* reset to start of array */
	output = output_string;
	
	/* code gen */
	generate_macros(output);
	/* output to file */
	fwrite (output_string , sizeof(char), strlen((const char *)output_string), outputFile);
	/* reset to start of array */
	output = output_string;
	
	/* flush file out of cache and close both files */
	fflush (outputFile); 
	fclose (outputFile);
	fclose (pFile);
	/* free memory that stored copy of file */
	free (buffer);
	/* free parser memory */
	/*ParseFree(pParser, free );*/
	ParseFinalize(pEngine);
	
	return 0;
}
