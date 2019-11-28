
static s32
enter_db_name(u8 * s,u32 l)
{
	//sqlite3_stmt * sql_statement;
	s32 n;
	
	SQL3_QUERY_find_db_name(db, "SELECT db_key=?i:n FROM db_names WHERE db_name=?t@s$l;");
	
	SQL3_BIND_find_db_name();
	
	/* prepare SQL query */
	n = SQL3_STEP_find_db_name();
	if (n==SQLITE_ROW){ // we have a previous entry
		//n = sqlite3_column_int(sql_statement, 0);
		SQL3_COL_find_db_name();
		//sqlite3_finalize(sql_statement);
		SQL3_RESET_find_db_name();
		return n;
	}
	/* no previous entry */
	SQL3_RESET_find_db_name();
	/* prepare SQL insert */
	SQL3_QUERY_insert_db_name(db,
		"INSERT INTO db_names VALUES("
		"NULL, ?t@s$l);");
	
	SQL3_BIND_insert_db_name();
	
	n = SQL3_STEP_insert_db_name();
	n = sqlite3_last_insert_rowid(db);
	/* all work has completed */
	SQL3_RESET_insert_db_name();
	return n;
}

static s32
enter_setup_text(ParserState * p_s, s32 db_rowid)
{
	//sqlite3_stmt * sql_statement;
	u8 * string_start = p_s->string_start;
	s32 n, str_len;
	
	/* "string"_
	^^^*^^^^^^^*
	* start and end */
	str_len = p_s->string_end - string_start;
	
	/* prepare SQL insert */
	SQL3_QUERY_insert_setup(db,
		"INSERT INTO setup_texts VALUES(NULL, "
		"?i@db_rowid, "
		"?t@string_start$str_len);");
	SQL3_BIND_insert_setup();
	n = SQL3_STEP_insert_setup();
	/* all work has completed */
	SQL3_RESET_insert_setup();
	return n;
}

static void
clean_query_text(u8 * b, u8 * s, u8 * e)
{
	u32 state=0;
	while ( (s<e) /*&& (*s!=0)*/ ) {
		switch (state) {
			case 0:
			*b = *s;
			b++;
			if(*s=='?') {
				// =?t:blah
				// ^ * 
				if(*(s+2)==':') {
					b-=2; //overwrite "=?"
				}
				state = 1;
			}
			break;
		
			case 1:
			if( (*s==' ') || (*s==',') || (*s==';') || (*s==')') || (*s=='(') || (*s=='"') ) {
				state = 0;
				*b = *s;
				b++;
			}
			break;
		}
		s++;
	}
	/* null terminate */
	*b = 0;
}

static s32
enter_query_text(ParserState * p_s, s32 db_rowid)
{
	s32 n /*,str_len*/;
	u8 text_buff[1024];
	
	/* "string"_
	^^^*^^^^^^^*
	* start and end */
	//str_len = p_s->string_end - p_s->string_start;
	
	/* copy text out to make valid sqlite3 query */
	clean_query_text(text_buff, p_s->string_start, p_s->string_end);
	
	
	/* prepare SQL query */
	SQL3_QUERY_search_qt(db,
		"SELECT qt_key=?i:n "
		"FROM query_texts WHERE "
		"query_text=?t@text_buff AND "
		"fdb_key=?i@db_rowid;");
	SQL3_BIND_search_qt();
	/* execute sql statement */					
	n = SQL3_STEP_search_qt();
	if (n==SQLITE_ROW){ // we have a previous entry for this db
		SQL3_COL_search_qt();
		SQL3_RESET_search_qt();
		return n;
	}
	/* no previous entry */
	SQL3_RESET_search_qt();
	/* prepare SQL insert */
	SQL3_QUERY_insert_qt(db,
		"INSERT INTO query_texts VALUES(NULL, "
		"?i@db_rowid, "
		"?t@text_buff);");
	
	SQL3_BIND_insert_qt();
	n = SQL3_STEP_insert_qt();
	n = sqlite3_last_insert_rowid(db);
	/* all work has completed */
	SQL3_RESET_insert_qt();
	return n;
}

static s32
enter_query_name(u8 * s, u32 l)
{
	s32 n;
	
	/* prepare SQL query */
	SQL3_QUERY_search_qn(db,
		"SELECT qn_key=?i:n FROM query_names WHERE query_name=?t@s$l;");
	/* bind variables */
	SQL3_BIND_search_qn();
	/* execute sql statement */					
	n = SQL3_STEP_search_qn();
	if (n==SQLITE_ROW){ // we have a previous entry
		SQL3_COL_search_qn();
		SQL3_RESET_search_qn();
		printf("ERROR: Query Name used twice!!!\n");
		return n;
	}
	/* no previous entry */
	SQL3_RESET_search_qn();
	n=0;
	/* prepare SQL insert */
	SQL3_QUERY_insert_qn(db,
		"INSERT INTO query_names VALUES(NULL, "
		"?i@n, "
		"?t@s$l);");
	SQL3_BIND_insert_qn();
	n = SQL3_STEP_insert_qn();
	n = sqlite3_last_insert_rowid(db);
	/* all work has completed */
	SQL3_RESET_insert_qn();
	return n;
}

static s32
update_query_name(u32 qn_key, s32 qt_key)
{
	s32 n;
	
	/* prepare SQL query */
	SQL3_QUERY_update_qn(db,
		"UPDATE query_names SET fqt_key=?i@qt_key "
		"WHERE qn_key=?i@qn_key;");
	/* bind variables */
	SQL3_BIND_update_qn();
	/* execute sql statement */					
	n = SQL3_STEP_update_qn();
	SQL3_RESET_update_qn();
	return n;
}

static u32
get_type(u8 * s)
{
	u32 type;
	switch (*s) {
		case 'b':
		type = 1;
		break;
		
		case 'i':
		type = 2;
		break;
		
		case 't':
		type = 3;
		break;
		
		case 'd':
		type = 4;
		break;
		
		default:
		type = 888;
		break;
	}
	return type;
}

static s32
enter_column_info(u8 * s, u32 l, u32 t, u32 qn_key)
{
	s32 n;
	
	/* prepare SQL insert */
	SQL3_QUERY_insert_colInfo(db,
		"INSERT INTO cols VALUES(NULL, "
		"?i@qn_key, "
		"?i@t, "
		"?t@s$l);");
	/* bind variables */
	SQL3_BIND_insert_colInfo();
	n = SQL3_STEP_insert_colInfo();
	/* all work has completed */
	SQL3_RESET_insert_colInfo();
	return n;
}

static s32
enter_bind_info(u8 * nv, u32 nl, u32 t, u32 qn_key, u8 * s, u32 sl)
{
	s32 n;
	
	/* check if size is missing */
	if ( (s==0) || (sl==0) ) {
		/* save zero */
		s=(u8 *)"0";
		sl=1;
	}
	
	/* prepare SQL insert */
	SQL3_QUERY_insert_bind(db,
		"INSERT INTO binds VALUES(NULL, "
		"?i@qn_key, "
		"?i@t, "
		"?t@nv$nl, "
		"?t@s$sl);");
	SQL3_BIND_insert_bind();
	n = SQL3_STEP_insert_bind();
	/* all work has completed */
	SQL3_RESET_insert_bind();
	return n;
}



