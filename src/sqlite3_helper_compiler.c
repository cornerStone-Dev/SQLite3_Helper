/* here is the code start */

#include <stdio.h>
#include <stdlib.h>   
#include <string.h>
#include <stdint.h>
#include <dirent.h>

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
#include "../gen/sql3_macros.c"
static sqlite3 * db;
#include "build_data.c"
#include "../tool_output/sqlite3_helper_gram.h"
#include "../tool_output/sqlite3_helper_lex.c"
#include "../tool_output/sqlite3_helper_gram.c"

#define INTPUT_FILE "input/test.c"
#define OUTPUT_FILE "gen/sql3_macros.c"

#ifndef NDEBUG
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
#endif

static void
generate_setup(u8 * out)
{
	sqlite3_stmt * sql_stmt2;
	const u8 * db_name, * setup_text;
	s32 n, j, db_key, stmn_cnt=0;
	s32 def_flag=0,db_flag=0;
	
	/* read out db name and keys */
	
	/* prepare SQL query */
	SQL3_QUERY_db_name_key(db,
		"SELECT "
		"db_key=?i:db_key, "
		"db_name=?t:db_name "
		"FROM db_names ORDER BY db_key ASC;");

	/* execute sql statement */					
	n = SQL3_STEP_db_name_key();
	while (n==SQLITE_ROW){ // we have an entry
		if(db_flag==0){
			db_flag=1;
			/* output number of members */
			out+=sprintf((char*)out, "#define SQL3_SETUP(a,b) do{}while(0)\n");
		}
		SQL3_COL_db_name_key();
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
				out+=sprintf((char*)out, "const char * %s_SETUPTEXT =\\\n", db_name);
			}
			setup_text = (u8 * )sqlite3_column_text(sql_stmt2, 0);
			out+=sprintf((char*)out, "%s \\\n", setup_text);
			stmn_cnt++;
			j = sqlite3_step(sql_stmt2);
		}
		def_flag=0;
		out+=sprintf((char*)out, ";\n");
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
		n = SQL3_STEP_db_name_key();
	}
	SQL3_RESET_db_name_key();
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
				out+=sprintf((char*)out, "const char * %s_QUERYTEXT = \\\n", db_name);
			}
			qt_key[q_cnt] = sqlite3_column_int(sql_stmt2, 0);
			query_text = (u8 * )sqlite3_column_text(sql_stmt2, 1);
			/* execute sql statement */
			out+=sprintf((char*)out, "%s \\\n", query_text);
			q_cnt++;
			j = sqlite3_step(sql_stmt2);
		}
		def_flag=0;
		out+=sprintf((char*)out, ";\n");
		// create enum
		out+=sprintf((char*)out, "\n\n");
		out+=sprintf((char*)out, "enum %s_enum_stmts {", db_name);
		for(int i = 0; i < q_cnt; i++){
			out+=sprintf((char*)out, "%s_enum%d, \\\n", db_name, qt_key[i]);
		}
		out+=sprintf((char*)out, "%s_enum_size\n", db_name);
		out+=sprintf((char*)out, "};\n\n");
		out+=sprintf((char*)out, "static sqlite3_stmt * %s_stmt_array[%d];\n", db_name, q_cnt);
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
		// setup is in define
		out+=sprintf(
			(char*)out,
			"\n\n#define %s_FINALIZE() \\\n"//1
			"do{\\\n"
			"for (int i=0;i < %d; i++){\\\n" //2
			"\tsqlite3_finalize(%s_stmt_array[i]);\\\n"//3
			"}\\\n"
			"}while(0)\n\n"
			,
			db_name,//1
			q_cnt,//2
			db_name);//3
		
		sqlite3_finalize(sql_stmt2);
		q_cnt=0;
		n = sqlite3_step(sql_stmt1);
	}
	sqlite3_finalize(sql_stmt1);
}

static void
generate_macros(u8 * out, u8 * begining, FILE * outputFile)
{
	sqlite3_stmt * sql_stmt1;
	sqlite3_stmt * sql_stmt2;
	sqlite3_stmt * sql_stmt3;
	sqlite3_stmt * sql_stmt4;
	u8 * db_name, * query_name, * varname, * varsize;
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
			"SELECT qt_key FROM query_texts WHERE fdb_key=? ORDER BY qt_key ASC;",
			-1,
			&sql_stmt2,
			NULL);
		sqlite3_bind_int(sql_stmt2, 1, db_key);
		/* execute sql statement */					
		j = sqlite3_step(sql_stmt2);
		while (j==SQLITE_ROW){ // we have an entry
			qt_key = sqlite3_column_int(sql_stmt2, 0);
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
				
				out+=sprintf((char*)out, "#define SQL3_QUERY_%s(a,b) do{}while(0)\n", query_name);
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
							"ERROR_INVALID_TYPE!!! \\\n");
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
						if (*varsize!='0'){
							out+=sprintf((char*)out,
								"sqlite3_bind_blob(%s_stmt_array[%s_enum%d], %d, (const void *)%s, %s, SQLITE_STATIC); \\\n",
								db_name,
								db_name,
								qt_key,
								bind_cnt,
								varname,
								varsize);
						} else {
							printf("ERROR blob needs size*****\n");
						}
						break;
						
						//case 'i':
						case 2:
						out+=sprintf((char*)out,
							"sqlite3_bind_int64(%s_stmt_array[%s_enum%d], %d, %s); \\\n",
							db_name,
							db_name,
							qt_key,
							bind_cnt,
							varname);
						break;
						
						//case 't':
						case 3:
						if (*varsize!='0'){
							out+=sprintf((char*)out,
								"sqlite3_bind_text(%s_stmt_array[%s_enum%d], %d, (const char *)%s, %s, SQLITE_STATIC); \\\n",
								db_name,
								db_name,
								qt_key,
								bind_cnt,
								varname,
								varsize);
						} else {
							out+=sprintf((char*)out,
								"sqlite3_bind_text(%s_stmt_array[%s_enum%d], %d, (const char *)%s, -1, SQLITE_STATIC); \\\n",
								db_name,
								db_name,
								qt_key,
								bind_cnt,
								varname);
						}
						break;
						
						//case 'd':
						case 4:
						out+=sprintf((char*)out,
							"sqlite3_bind_double(%s_stmt_array[%s_enum%d], %d, %s); \\\n",
							db_name,
							db_name,
							qt_key,
							bind_cnt,
							varname);
						break;
						
						default:
						out+=sprintf((char*)out,
							"ERROR_INVALID_TYPE!!! \\\n");
						break;
					}
					
					bind_cnt++;
					
					
					m = sqlite3_step(sql_stmt4);
				}
				sqlite3_finalize(sql_stmt4);
				
				out+=sprintf((char*)out,"}while(0)\n\n");
				bind_cnt=1;
				fwrite (begining , sizeof(char), strlen((const char *)begining), outputFile);
				out = begining;
				*out = 0;
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

#ifndef NDEBUG
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
#endif

int main(int argc, char **argv)
{
	
	const unsigned char * data;
	void *pEngine;     /* The LEMON-generated LALR(1) parser */
	yyParser sEngine;  /* Space to hold the Lemon-generated Parser object */
	unsigned char output_string[32768] = {0};
	unsigned char * output = output_string;
	Token token = {0};
	int tmp_token;
	ParserState p_s = {0};
	FILE * pFile, * outputFile;
	size_t lSize;
	unsigned char * buffer;
	size_t result;
	DIR *d;
    struct dirent *dir;
	sqlite3_open(":memory:", &db);
	
	db_SETUP();
	db_PREPARE();
	
	SQL3_SETUP(db, "CREATE TABLE db_names(db_key INTEGER PRIMARY KEY, db_name TEXT);");
	SQL3_SETUP(db, "CREATE TABLE setup_texts(st_key INTEGER PRIMARY KEY, fdb_key INTEGER, setup_text TEXT);");
	
	SQL3_SETUP(db, "CREATE TABLE query_texts(qt_key INTEGER PRIMARY KEY, fdb_key INTEGER, query_text TEXT);");
	SQL3_SETUP(db, "CREATE TABLE query_names(qn_key INTEGER PRIMARY KEY, fqt_key INTEGER, query_name TEXT);");
	SQL3_SETUP(db, "CREATE TABLE cols(col_key INTEGER PRIMARY KEY, fqn_key INTEGER, type INTEGER, varname TEXT);");
	SQL3_SETUP(db, "CREATE TABLE binds(bind_key INTEGER PRIMARY KEY, fqn_key INTEGER, type INTEGER, varval TEXT, varsize TEXT);");
	
	outputFile = fopen ( OUTPUT_FILE, "w" );
	if (outputFile==NULL) {fputs ("File error",stderr); exit (1);}
	
	/*pParser = ParseAlloc( malloc, &p_s );*/
	pEngine = &sEngine;
	ParseInit(pEngine, &p_s);

#ifndef NDEBUG
	ParseTrace(stdout, "debug:: ");
#endif

	//printf("starting parse\n");
	
	/* open current directory */
	d = opendir("src");
	if (d==0)
    {
		printf("NO DIR!!!\n");
		return -1;
	}
	
	while ( ((dir = readdir(d)) != NULL) /*&& (strstr(dir->d_name, ".c")!=0)*/ )
	{
		//printf("Got in\n");
		if ( (strstr(dir->d_name, ".c")!=0) ) {
		//printf("%s\n", dir->d_name);
		
		sprintf((char *)output, "src/%s",dir->d_name);

		pFile = fopen ( (char *)output, "rb" );
		if (pFile==NULL) {fputs ("File error",stderr); exit (1);}
		
		
		
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
		
		/* null terminate buffer */
		buffer[lSize]=0;
	
		do {
			tmp_token = lex(&data, &token);

			Parse(pEngine, tmp_token, token); 
			
		} while (tmp_token != 0);
		fclose (pFile);
		/* free memory that stored copy of file */
		free (buffer);
		}
	}
	
	/* close directory */
	closedir(d);
	
	/* output collected data, aka code gen */
#ifndef NDEBUG
	print_setup();
	print_queries();
#endif

	*output=0;
	/* code gen */
	generate_setup(output);
	/* output to file */
	fwrite (output_string , sizeof(char), strlen((const char *)output_string), outputFile);
	/* reset to start of array */
	output = output_string;
	*output=0;
	
	/* code gen */
	generate_queries(output);
	/* output to file */
	fwrite (output_string , sizeof(char), strlen((const char *)output_string), outputFile);
	/* reset to start of array */
	output = output_string;
	*output=0;
	
	/* code gen */
	generate_macros(output, output_string, outputFile);
	/* output to file */
	//fwrite (output_string , sizeof(char), strlen((const char *)output_string), outputFile);
	
	/* flush file out of cache and close both files */
	fflush (outputFile); 
	fclose (outputFile);

	
	db_FINALIZE();
	/* free parser memory */
	/*ParseFree(pParser, free );*/
	ParseFinalize(pEngine);
	
	return 0;
}
