/*
gather information into database?
*/

%include{
#define YYNOERRORRECOVERY 1
}
/* not active when YYNOERRORRECOVERY is defined */
/*%parse_failure {
	printf("parse_failure\n");
	p_s->error = 1;
}*/

%extra_context {ParserState * p_s}

%token_type {Token}
%default_type {Token}

%syntax_error {
	//printf("syntax_error\n");
	while( yypParser->yytos>yypParser->yystack ) yy_pop_parser_stack(yypParser);
}



input ::= sql_interface.

sql_interface ::= sql_interface sql.
sql_interface ::= sql.

// rewrite string with removals and '?'s for saving and comparison
// query need sub-name extracted
// SETUP just needs db name and string saved off in a list(table)
// db table PK int, db_name
// db setup pk int, db_link_num, string
// tables info pkint, table_link, type
// tables
// for each query
// 
sql ::= SETUP LPAREN IDENT(A) COMMA string RPAREN SEMI. {

	/* string is not used here, see recorded value in string.
	 * take db name, make new entry if unique, record rowid
	 * use recorded string to make set up text entry
	 * */
	s32 db_rowid;
	db_rowid = enter_db_name(A.s, A.l);
	enter_setup_text(p_s, db_rowid);
}
sql ::= query_name LPAREN IDENT(A) COMMA string RPAREN SEMI. {
	
	s32 db_rowid, qt_key;
	db_rowid = enter_db_name(A.s, A.l);
	
	qt_key = enter_query_text(p_s, db_rowid);
	
	update_query_name(p_s->qn_key, qt_key);
}

query_name ::= QUERY(B). {
	
	p_s->qn_key = enter_query_name( (B.s+11), (B.l-11) );
	
} 

// get total length of memory containing string
string ::= string QUOTE idlist QUOTE(B). {p_s->string_end=(B.s+1);}
string ::= QUOTE(A) idlist QUOTE(B). {
	p_s->string_start=(A.s);
	p_s->string_end=(B.s+1);}

idlist ::= idlist IDENT.
idlist ::= QMARK IDENT(A) COLON IDENT(B). {//printf("got read variable!\n");
	u32 type;
	type = get_type(A.s);
	enter_column_info(B.s, B.l, type, p_s->qn_key);

}
idlist ::= QMARK IDENT(A) ATSIGN IDENT(B). {//printf("got write variable!\n");
	u32 type;
	type = get_type(A.s);
	enter_bind_info(B.s, B.l, type, p_s->qn_key, 0, 0);
	
}
idlist ::= QMARK IDENT(A) ATSIGN IDENT(B) DOLLAR IDENT(C). {//printf("got write variable and size!\n");
	u32 type;
	type = get_type(A.s);
	enter_bind_info(B.s, B.l, type, p_s->qn_key, C.s, C.l);
	
}
idlist ::= idlist QMARK IDENT(A) COLON IDENT(B). {//printf("got read variable!\n");
	u32 type;
	type = get_type(A.s);
	enter_column_info(B.s, B.l, type, p_s->qn_key);

}
idlist ::= idlist QMARK IDENT(A) ATSIGN IDENT(B). {//printf("got write variable!\n");
	u32 type;
	type = get_type(A.s);
	enter_bind_info(B.s, B.l, type, p_s->qn_key, 0, 0);
	
}
idlist ::= idlist QMARK IDENT(A) ATSIGN IDENT(B) DOLLAR IDENT(C). {//printf("got write variable and size!\n");
	u32 type;
	type = get_type(A.s);
	enter_bind_info(B.s, B.l, type, p_s->qn_key, C.s, C.l);
	
}
idlist ::= idlist LPAREN.
idlist ::= idlist RPAREN.
idlist ::= idlist SEMI.
idlist ::= idlist COMMA.
idlist ::= IDENT.


//idlist(A) ::= IDENT(X). //{A = X; /*A-overwrites-X*/ } not needed as this is default



