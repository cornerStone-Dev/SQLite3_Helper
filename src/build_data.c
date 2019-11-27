
static s32
enter_db_name(u8 * s,u32 l)
{
	sqlite3_stmt * sql_statement;
	s32 n;
	
	/* prepare SQL query */
	sqlite3_prepare_v2(
		db,
		"SELECT db_key FROM db_names WHERE db_name=?;",
		-1,
		&sql_statement,
		NULL);
	/* bind in text */
	sqlite3_bind_text(
		sql_statement,
		1,
		(const char *)s, 
		l,
		SQLITE_STATIC);
	/* execute sql statement */					
	n = sqlite3_step(sql_statement);
	if (n==SQLITE_ROW){ // we have a previous entry
		n = sqlite3_column_int(sql_statement, 0);
		sqlite3_finalize(sql_statement);
		return n;
	}
	/* no previous entry */
	sqlite3_finalize(sql_statement);
	/* prepare SQL insert */
	sqlite3_prepare_v2(
		db,
		"INSERT INTO db_names VALUES(NULL, @db_name);",
		-1,
		&sql_statement,
		NULL);
	/* bind in text */
	sqlite3_bind_text(
		sql_statement,
		1,
		(const char *)s, 
		l,
		SQLITE_STATIC);
	n = sqlite3_step(sql_statement);
	n = sqlite3_last_insert_rowid(db);
	/* all work has completed */
	sqlite3_finalize(sql_statement);
	return n;
}

static s32
enter_setup_text(ParserState * p_s, s32 db_rowid)
{
	sqlite3_stmt * sql_statement;
	s32 n, str_len;
	
	/* "string"_
	^^^*^^^^^^^*
	* start and end */
	str_len = p_s->string_end - p_s->string_start;
	
	/* prepare SQL insert */
	sqlite3_prepare_v2(
		db,
		"INSERT INTO setup_texts VALUES(NULL, @db_rowid, @setup_text);",
		-1,
		&sql_statement,
		NULL);
	/* bind in int */
	sqlite3_bind_int(
		sql_statement,
		1,
		db_rowid);
	/* bind in text */
	sqlite3_bind_text(
		sql_statement,
		2,
		(const char *)p_s->string_start, 
		str_len,
		SQLITE_STATIC);
	n = sqlite3_step(sql_statement);
	/* all work has completed */
	sqlite3_finalize(sql_statement);
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
			if( (*s==' ') || (*s==',') || (*s==';') || (*s==')') || (*s=='(') ) {
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
	sqlite3_stmt * sql_statement;
	s32 n /*,str_len*/;
	u8 text_buff[1024];
	
	/* "string"_
	^^^*^^^^^^^*
	* start and end */
	//str_len = p_s->string_end - p_s->string_start;
	
	/* copy text out to make valid sqlite3 query */
	clean_query_text(text_buff, p_s->string_start, p_s->string_end);
	
	
	/* prepare SQL query */
	sqlite3_prepare_v2(
		db,
		"SELECT qt_key FROM query_texts WHERE query_text=? AND fdb_key=?;",
		-1,
		&sql_statement,
		NULL);
	/* bind in text */
	sqlite3_bind_text(
		sql_statement,
		1,
		(const char *)text_buff, 
		-1,
		SQLITE_STATIC);
	/* bind in int */
	sqlite3_bind_int(
		sql_statement,
		2,
		db_rowid);
	/* execute sql statement */					
	n = sqlite3_step(sql_statement);
	if (n==SQLITE_ROW){ // we have a previous entry for this db
		n = sqlite3_column_int(sql_statement, 0);
		sqlite3_finalize(sql_statement);
		return n;
	}
	/* no previous entry */
	sqlite3_finalize(sql_statement);
	/* prepare SQL insert */
	sqlite3_prepare_v2(
		db,
		"INSERT INTO query_texts VALUES(NULL, @db_rowid, @query_text);",
		-1,
		&sql_statement,
		NULL);
	/* bind in int */
	sqlite3_bind_int(
		sql_statement,
		1,
		db_rowid);
	/* bind in text */
	sqlite3_bind_text(
		sql_statement,
		2,
		(const char *)text_buff, 
		-1,
		SQLITE_STATIC);
	n = sqlite3_step(sql_statement);
	n = sqlite3_last_insert_rowid(db);
	/* all work has completed */
	sqlite3_finalize(sql_statement);
	return n;
}

static s32
enter_query_name(u8 * s, u32 l)
{
	sqlite3_stmt * sql_statement;
	s32 n;
	
	/* prepare SQL query */
	sqlite3_prepare_v2(
		db,
		"SELECT qn_key FROM query_names WHERE query_name=?;",
		-1,
		&sql_statement,
		NULL);
	/* bind in text */
	sqlite3_bind_text(
		sql_statement,
		1,
		(const char *)s, 
		l,
		SQLITE_STATIC);
	/* execute sql statement */					
	n = sqlite3_step(sql_statement);
	if (n==SQLITE_ROW){ // we have a previous entry
		n = sqlite3_column_int(sql_statement, 0);
		sqlite3_finalize(sql_statement);
		printf("ERROR: Query Name used twice!!!\n");
		return n;
	}
	/* no previous entry */
	sqlite3_finalize(sql_statement);
	/* prepare SQL insert */
	sqlite3_prepare_v2(
		db,
		"INSERT INTO query_names VALUES(NULL, @qt_rowid, @query_name);",
		-1,
		&sql_statement,
		NULL);
	/* bind in int */
	sqlite3_bind_int(
		sql_statement,
		1,
		0);
	/* bind in text */
	sqlite3_bind_text(
		sql_statement,
		2,
		(const char *)s, 
		l,
		SQLITE_STATIC);
	n = sqlite3_step(sql_statement);
	n = sqlite3_last_insert_rowid(db);
	/* all work has completed */
	sqlite3_finalize(sql_statement);
	return n;
}

static s32
update_query_name(u32 qn_key, s32 qt_key)
{
	sqlite3_stmt * sql_statement;
	s32 n;
	
	/* prepare SQL query */
	sqlite3_prepare_v2(
		db,
		"UPDATE query_names SET fqt_key=? WHERE qn_key=?;",
		-1,
		&sql_statement,
		NULL);
	/* bind in int */
	sqlite3_bind_int(
		sql_statement,
		1,
		qt_key);
	/* bind in int */
	sqlite3_bind_int(
		sql_statement,
		2,
		qn_key);
	/* execute sql statement */					
	n = sqlite3_step(sql_statement);
	sqlite3_finalize(sql_statement);
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
	sqlite3_stmt * sql_statement;
	s32 n;
	
	/* prepare SQL insert */
	sqlite3_prepare_v2(
		db,
		"INSERT INTO cols VALUES(NULL, @fqn_key, @type, @varname);",
		-1,
		&sql_statement,
		NULL);
	/* bind in int */
	sqlite3_bind_int(
		sql_statement,
		1,
		qn_key);
	/* bind in int */
	sqlite3_bind_int(
		sql_statement,
		2,
		t);
	/* bind in text */
	sqlite3_bind_text(
		sql_statement,
		3,
		(const char *)s, 
		l,
		SQLITE_STATIC);
	n = sqlite3_step(sql_statement);
	/* all work has completed */
	sqlite3_finalize(sql_statement);
	return n;
}

static s32
enter_bind_info(u8 * nv, u32 nl, u32 t, u32 qn_key, u8 * s, u32 sl)
{
	sqlite3_stmt * sql_statement;
	s32 n;
	
	/* prepare SQL insert */
	sqlite3_prepare_v2(
		db,
		"INSERT INTO binds VALUES(NULL, @fqn_key, @type, @varval, @varsize);",
		-1,
		&sql_statement,
		NULL);
	/* bind in int */
	sqlite3_bind_int(
		sql_statement,
		1,
		qn_key);
	/* bind in int */
	sqlite3_bind_int(
		sql_statement,
		2,
		t);
	/* bind in text */
	sqlite3_bind_text(
		sql_statement,
		3,
		(const char *)nv, 
		nl,
		SQLITE_STATIC);
	if ( (s==0) || (sl==0) ) {
		/* bind in text */
		sqlite3_bind_text(
			sql_statement,
			4,
			"0", 
			1,
			SQLITE_STATIC);
	} else {
		/* bind in text */
		sqlite3_bind_text(
			sql_statement,
			4,
			(const char *)s, 
			sl,
			SQLITE_STATIC);
	}
	
	n = sqlite3_step(sql_statement);
	/* all work has completed */
	sqlite3_finalize(sql_statement);
	return n;
}



