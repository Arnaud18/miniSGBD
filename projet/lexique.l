/*****************************************
* MINI INTERPRETEUR SQL                  *
* LOUE Boureima Arnaud et                *
* OUEDRAOGO Pascal                       *
*                                        *
******************************************/

                                                /*==============================================*/
                                               /*          L'analyseur lexical                  */
                                               /*                                              */
                                               /*==============================================*/






%{

#include "syntaxe.tab.h" // Inclusion du fichier d'en tête de l'ananlyseur syntaxique obtenu après compilation
extern int num_Ligne;

%}

lettre [a-zA-Z]
chiffre [0-9]
symbole [@!&_$-]
tout_Col [*]
op_relationnel [=<>]?
sign           [+-]?
ident {lettre}({lettre}|{chiffre}|(_))*({lettre}|{chiffre})*
chaine (\')({lettre}|{chiffre}|{symbole}|{espace})*(\')
integer {chiffre}+ 
number {sign}{chiffre}+(\.){chiffre}* 
jour   {chiffre}({chiffre})?
mois   {chiffre}({chiffre})?
annee  {chiffre}4
type_Integer        integer|INTEGER
val_Null            null|NULL
type_Text           text|TEXT
type_Date           date
type_Number         number
type_Boolean        boolean
cons_Primary        primary
key                 key
cons_Foreign        foreign
paren_G             "("
paren_D             ")"
slash               "/"
ref                 references
val_Date            (({jour}{slash})({mois}{slash}{annee}))
boolean [01]?
espace    [ ]
listeCreate ({ident}{espace}({type_Integer}|{type_Text}|{type_Date}|{type_Number}|{type_Boolean}))(({espace}{cons_Primary}{espace}{key})?|({espace}{cons_Foreign}{espace}{key})?{espace}{ref}{espace}{ident}{paren_G}{ident}{paren_D})(([,]({espace})*{ident}{espace}({type_Integer}|{type_Text}|{type_Date}|{type_Boolean}|{type_Number})({espace}{cons_Primary}{espace}{key})?|({espace}{cons_Foreign}{espace}{key})?{espace}{ref}{espace}{ident}{paren_G}{ident}{paren_D}))*
listeInsert	({chaine}|{val_Null}|{symbole}|{integer}|{val_Date}|{number}|{boolean})([,]({espace})*({chaine}|{val_Null}|{symbole}|{integer}|{val_Date}|{number}|{boolean}))*

listeJoin      ({ident}[.]{ident})([,]({espace})*{ident}[.]{ident})* 
tuple          ({ident}|({ident}(([,]({espace})*{ident})*))*)

%%


[ \t]			                {}
"CREATE TABLE"                          {return CMD_CREATE;}
"INSERT INTO"                           {return CMD_INSERT;}
"DELETE FROM"                           {return CMD_DELETE;}
"ALTER TABLE"                           {return CMD_ALTER;}
"SELECT"                                {return CMD_SELECT;}
"UPDATE"                                {return CMD_UPDATE;}
"RENAME"                                {return CMD_RENAME;}
"DROP TABLE"                            {return CMD_DROP;}
"DROP"                                  {return OP_DROP;}
"HELP"                                  {return CMD_HELP;}
"VALUES"                                {return OP_VALUES;}
"ADD"                                   {return CLAUSE_ADD;}
"AND"                                   {return OP_AND;}
"MODIFY"                                {return CLAUSE_MODIFY;}
"FROM"                                  {return CLAUSE_FROM;}
"WHERE"                                 {return CLAUSE_WHERE;}
"GROUP BY"                              {return CLAUSE_GROUPBY;}
"JOIN"                                  {return CLAUSE_JOIN;}
"HAVING"                                {return CLAUSE_HAVING;}
"ORDER BY"                              {return CLAUSE_ORDERBY;}
"TO"                                    {return OP_TO;} 
"OR"                                    {return OP_OR;}
"DISTINCT"                              {return OP_DISTINCT;}
"DEFAULT"                               {return OP_DEFAULT;}
"NOT NULL"                              {return OP_NOT_NULL;} 
"ASC"                                   {return ASC;}
"DESC"                                  {return CMD_DESC;}
"PRIMARY KEY"                           {yylval.chaine=NULL;yylval.chaine=(char *) malloc((1+yyleng)*sizeof(char));
                                         strcpy(yylval.chaine,yytext);return CNTR_PRIMARYKEY;}
"FOREIGN KEY"                           {yylval.chaine=NULL;yylval.chaine=(char *) malloc((1+yyleng)*sizeof(char));
                                         strcpy(yylval.chaine,yytext);return CNTR_FOREIGNKEY;}
"UNIQUE"                                {yylval.chaine=NULL;yylval.chaine=(char *) malloc((1+yyleng)*sizeof(char));
                                         strcpy(yylval.chaine,yytext);return CNTR_UNIQUE;}
"AVG"                                   {return FUNCT_AVG;}
"SUM"                                   {return FUNCT_SUM;}
"MIN"                                   {return FUNCT_MIN;}
"MAX"                                   {return FUNCT_MAX;}
";"                                     {return FIN;}
"exit"                                  {return QUIT;}
"SET"                                   {return OP_SET;}
"REFERENCES"                            {return CLAUSE_REFERENCES;}
"clear"                                 {return OP_CLEAR;}
"("                                     {return PARG;}
")"                                     {return PARD;}
","                                     {return VIRG;}
"\n"                                    {num_Ligne++; /*if(num_Ligne!=1) printf("%d \t",num_Ligne)*/;}  
"on"                                    {return OP_ON ;}
"!="|"<"|"="|">"                        {yylval.chaine=NULL;yylval.chaine=(char *) malloc((1+yyleng)*sizeof(char));
                                         strcpy(yylval.chaine,yytext);return OP_REL;}
"NULL"                                  {yylval.chaine=NULL;yylval.chaine=(char *) malloc((1+yyleng)*sizeof(char));
                                         strcpy(yylval.chaine,yytext);return VAL_NULL;}
{number}                                {yylval.chaine=NULL;yylval.chaine=(char *) malloc((1+yyleng)*sizeof(char));
                                          strcpy(yylval.chaine,yytext);  return NUMBER;}
{ident}                                 {yylval.chaine=NULL;yylval.chaine=(char *) malloc((1+yyleng)*sizeof(char));
                                             strcpy(yylval.chaine,yytext);return IDENT;}
{chaine}                                {yylval.chaine=NULL;yylval.chaine=(char *) malloc((1+yyleng)*sizeof(char));
                                          strcpy(yylval.chaine,yytext);return CHAINE;}
{boolean}                               {yylval.chaine=NULL;yylval.chaine=(char *) malloc((1+yyleng)*sizeof(char));
                                         strcpy(yylval.chaine,yytext);  return BOOLEAN;}
{integer}                               {yylval.chaine=NULL;yylval.chaine=(char *) malloc((1+yyleng)*sizeof(char));  
                                         strcpy(yylval.chaine,yytext);   return INTEGER;}
{val_Date}                              {yylval.chaine=NULL;yylval.chaine=(char *) malloc((1+yyleng)*sizeof(char));
                                          strcpy(yylval.chaine,yytext);return DATE;}
{tout_Col}                              {return ALL_COLUMN;}
{listeCreate}                           {
				          fflush(stdin);
				          yylval.chaine=NULL;
                                          yylval.chaine=(char *) malloc((1+yyleng)*sizeof(char));
                                          
			                  strcpy(yylval.chaine, yytext);
                                          return LISTE_CREATE;                                      }
                                    
{listeInsert}                           {   
				          fflush(stdin);
				          yylval.chaine=NULL;
                          	          yylval.chaine=(char *) malloc((1+yyleng)*sizeof(char));
                          	          
			                  strcpy(yylval.chaine, yytext);
                                          return LISTE_INSERT;                                       }
{listeJoin}                             { 
                                          fflush(stdin);
                                          yylval.chaine=NULL;
                          	          yylval.chaine=(char *) malloc((1+yyleng)*sizeof(char));
                          	          
			                  strcpy(yylval.chaine, yytext);
                                          return LISTE_JOIN;                                        }
{tuple}                                 { 
                                          fflush(stdin);
                                          yylval.chaine=NULL;
                          	          yylval.chaine=(char *) malloc((1+yyleng)*sizeof(char));
                          	          
			                  strcpy(yylval.chaine, yytext);
                                          return TUPLE;                                             }



%%


