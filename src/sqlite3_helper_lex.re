/*!re2c re2c:flags:i = 1; */         // re2c block with configuration that turns off #line directives
									 //
//#include <stdio.h>                   //    C/C++ code


/*!max:re2c*/                        // directive that defines YYMAXFILL (unused)
/*!re2c                              // start of re2c block
	
	mcm = "/*" ([^*] | ("*" [^/]))* "*""/";
	scm = "//" [^\n]* "\n";
	wsp = ([ \t\v\n\r] | scm | mcm)+;
	query =  "SQL3_QUERY_" [a-zA-Z_0-9]*;
	step =   "SQL3_STEP_";
	column = "SQL3_COL_";
	reset =  "SQL3_RESET_";
	clear =  "SQL3_CLEAR_";
	setup =  "SQL3_SETUP";
	lparen = "(";
	rparen = ")";
	semi =   ";";
	atsign = "@";
	colon =  ":";
	comma =  ",";
	dollar = "$";
	qmark =  "?";
	quote =  ["];
	id = [a-zA-Z_][a-zA-Z_0-9]*;
	
*/                                   // end of re2c block

static int lex(const u8 **YYCURSOR_p, Token * t) // YYCURSOR is defined as a function parameter
{                                    //
	const u8 * YYMARKER;    // YYMARKER is defined as a local variable
	//const u8 * YYCTXMARKER; // YYCTXMARKER is defined as a local variable
	const u8 * YYCURSOR;    // YYCURSOR is defined as a local variable
	const u8 * start;
	start = *YYCURSOR_p;
	YYCURSOR = *YYCURSOR_p;

loop: // label for looping within the lexxer

	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* {  start =YYCURSOR; goto loop; }//   default rule with its semantic action
	[\x00] { return 0; }             // EOF rule with null sentinal
	
	wsp {
		start =YYCURSOR;
		goto loop;
	}
	
	query {
		// record query 
		t->s = (u8 *)start;
		t->l = (u32)(YYCURSOR - start); 
		*YYCURSOR_p = YYCURSOR;
		return QUERY;
	}
	
	setup {
		*YYCURSOR_p = YYCURSOR;
		return SETUP;
	}
	
	lparen {
		*YYCURSOR_p = YYCURSOR;
		return LPAREN;
	}
	
	rparen { 
		*YYCURSOR_p = YYCURSOR;
		return RPAREN;
	}
	
	semi {
		*YYCURSOR_p = YYCURSOR;
		return SEMI;
	}
	
	atsign {
		*YYCURSOR_p = YYCURSOR;
		return ATSIGN;
	}
	
	colon {
		*YYCURSOR_p = YYCURSOR;
		return COLON;
	}

	comma {
		*YYCURSOR_p = YYCURSOR;
		return COMMA;
	}

	dollar {
		*YYCURSOR_p = YYCURSOR;
		return DOLLAR;
	}

	qmark {
		*YYCURSOR_p = YYCURSOR;
		return QMARK;
	}

	quote {
		// record quote 
		t->s = (u8 *)start;
		*YYCURSOR_p = YYCURSOR;
		return QUOTE;
	}
	
	id {
		// record string 
		t->s = (u8 *)start;
		t->l = (u32)(YYCURSOR - start); 
		*YYCURSOR_p = YYCURSOR;
		return IDENT;
	}

	*/                               // end of re2c block
}  

