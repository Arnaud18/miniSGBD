/*****************************************
* MINI INTERPRETEUR SQL                  *
* LOUE Boureima Arnaud et                *
* OUEDRAOGO Pascal                       *
*                                        *
******************************************/

                                          /*========================================================*/
                                          /*          L'analyseur Syntaxique et sémantique          */
                                          /*========================================================*/

%{
                            /* Inclusion de fichier d'en tête */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "defines.h"
#include <ctype.h>
#include "fonctions.h"   
                          /* Déclaration de variables */
extern FILE *yyin;
int num_Ligne=0;  
 
%}

%union                      /*     Définition   de types  */
{
char * chaine;
int entier;
float number;
} 
                                                  /* Défininition des lexèmes*/

    
%token CMD_CREATE CMD_INSERT CMD_SELECT CMD_DELETE CMD_ALTER CMD_UPDATE CMD_RENAME CMD_DROP CMD_HELP CMD_DESC
%token OP_VALUES CLAUSE_FROM CLAUSE_WHERE CLAUSE_JOIN CLAUSE_ORDERBY CLAUSE_GROUPBY CLAUSE_HAVING CLAUSE_ADD CLAUSE_MODIFY ASC DESC CLAUSE_RENAME
%token  OP_AND OP_OR OP_DISTINCT  OP_DEFAULT OP_NOT_NULL ALL_COLUMN OP_DROP
%token FUNCT_AVG FUNCT_SUM FUNCT_MIN FUNCT_MAX OP_SET OP_TO
%token CNTR_PRIMARYKEY CNTR_UNIQUE OP_CLEAR OP_ON
%token CNTRTAB_PRIMARYKEY CNTRTAB_UNIQUE CNTRTAB_FOREIGNKEY
%token <chaine> TYPE_NUMBER TYPE_VARCHAR TYPE_DATE  TYPE_INTEGER TYPE_TEXT TYPE_BOOLEAN
%token <chaine> IDENT LISTE_CREATE LISTE_INSERT LISTE_JOIN OP_REL TUPLE
%type  <chaine> champ valeur
%token <chaine> CHAINE DATE VAL_NULL
%token <chaine> INTEGER BOOLEAN
%token <chaine> NUMBER 
%token VIRG PARG PARD
%token FIN QUIT CLEAR
%token CNTR_FOREIGNKEY CLAUSE_REFERENCES
%start cmdSQL                  // Definition de l'axiome
%%
/*************************************************************************************************************************************************/
   
                         
cmdSQL: cmd_instr_CREATE  
      | cmd_instr_ALTER 
      | cmd_instr_INSERT 
      | cmd_instr_UPDATE
      | cmd_instr_SELECT 
      | cmd_instr_DELETE 
      | cmd_instr_DROP
      | cmd_instr_RENAME
      | cmd_instr_DESC                                             
      | QUIT FIN {   exit(1); YYACCEPT;   }
                                                         
      | OP_CLEAR FIN { system("clear");printf("\nshellSQL_> ");yyparse(); }
      | CMD_HELP IDENT FIN  {  help($2);printf("\nshellSQL_> ");yyparse();}                                                 
      | error {yyerrok;yyclearin;}
      ;

/*********************************************************Implémentation de la commande CREATE TABLE**********************************************************/

cmd_instr_CREATE: CMD_CREATE IDENT PARG LISTE_CREATE PARD FIN {create_table($2,$4); printf("\nshellSQL_> "); yyparse(); }
                              
                                                              
                ;		                                     
 
                                
                                                                                                                                         

                                                                                                                  /*********************************************************Implémentation de la commande ALTER TABLE**********************************************************/ 

cmd_instr_ALTER: alter_Add
               | alter_Drop
               ;
alter_Add: CMD_ALTER IDENT CLAUSE_ADD LISTE_CREATE FIN  { requete_alter_add($2,$4); printf("\nshellSQL_> "); yyparse();}                   
         ;
alter_Drop: CMD_ALTER IDENT OP_DROP IDENT FIN { requete_alter_drop($2,$4); printf("\nshellSQL_> "); yyparse();}   
          ; 

cmd_instr_SELECT: select_all
	        | select_all_liste
		| select_where
		| select_where_liste
		| join 
                ;
select_all: CMD_SELECT ALL_COLUMN CLAUSE_FROM IDENT FIN { requete_select_all($4); printf("\nshellSQL_> "); yyparse();}   
          ;
select_all_liste: CMD_SELECT IDENT CLAUSE_FROM IDENT FIN { requete_select($4,$2); printf("\nshellSQL_> "); yyparse();} 
                | CMD_SELECT TUPLE CLAUSE_FROM IDENT FIN { requete_select($4,$2); printf("\nshellSQL_> "); yyparse();}
                ;  
select_where: CMD_SELECT ALL_COLUMN CLAUSE_FROM IDENT CLAUSE_WHERE IDENT OP_REL valeur FIN { select_where($4,$6,$8,$7); printf("\nshellSQL_> "); 
                                                                                                   yyparse();                                    }
            ;
select_where_liste: CMD_SELECT TUPLE CLAUSE_FROM IDENT CLAUSE_WHERE IDENT OP_REL valeur FIN {select_where_liste($4,$2,$6,$8,$7); printf("\nshellSQL_> ");
                                                                                                   yyparse();                                           }
                  | CMD_SELECT IDENT CLAUSE_FROM IDENT CLAUSE_WHERE IDENT OP_REL valeur FIN {select_where_liste($4,$2,$6,$8,$7);
                                                                                                printf("\nshellSQL_>");   yyparse();             }
                  ;
join : CMD_SELECT LISTE_JOIN CLAUSE_FROM IDENT CLAUSE_JOIN IDENT CLAUSE_WHERE LISTE_JOIN OP_REL LISTE_JOIN FIN {requete_join($4,$6,$2,$8,$10);
                                                                                                                 printf("\nshellSQL_> ");      yyparse();   }
     | CMD_SELECT LISTE_JOIN CLAUSE_FROM TUPLE CLAUSE_WHERE LISTE_JOIN OP_REL LISTE_JOIN OP_AND LISTE_JOIN OP_REL LISTE_JOIN  FIN { 
                                                                                                                      requete_multiple_join($2,$4,$6,$8,$10,$12);
                                                                                                                 printf("\nshellSQL_> ");      yyparse();   }
     ;
                                                     
/*********************************************************Implémentation de la commande UPDATE*****************************************************************/

cmd_instr_UPDATE: CMD_UPDATE IDENT OP_SET IDENT OP_REL valeur CLAUSE_WHERE IDENT OP_REL valeur FIN { requete_update($2,$8,$4,$10,$6,$9);
                                                                                                       printf("\nshellSQL_> "); yyparse();}
                ;

/*********************************************************Implémentation de la commande DELETE*****************************************************************/

cmd_instr_DELETE: CMD_DELETE  IDENT CLAUSE_WHERE IDENT OP_REL valeur FIN  { requete_delete($2,$4,$6,$5); printf("\nshellSQL_> "); yyparse();}
                ;

/*********************************************************Implémentation de la commande RENAME*****************************************************************/

cmd_instr_RENAME: CMD_RENAME IDENT OP_TO IDENT FIN { rename_table($2,$4); printf("\nshellSQL_> "); 
                                                      yyparse();                                     }                                                        
                ;

/*********************************************************Implémentation de la commande INSERT*****************************************************************/

cmd_instr_INSERT: CMD_INSERT IDENT OP_VALUES PARG champ PARD FIN {requete_insert($2,$5);printf("\nshellSQL_> "); yyparse();}
                
                ;
                
/*********************************************************Implémentation de la commande DROP*******************************************************************/
                
cmd_instr_DROP: CMD_DROP IDENT FIN { drop_table($2); printf("\nshellSQL_> "); yyparse();}
              ; 
champ: valeur   
     | LISTE_INSERT {strcpy($$,$1);}
     ;
valeur: CHAINE   {strcpy($$,$1);}
      | NUMBER   {strcpy($$,$1);}
      | INTEGER  {strcpy($$,$1);}
      | DATE     {strcpy($$,$1);}
      | BOOLEAN  {strcpy($$,$1);}
      | VAL_NULL {strcpy($$,$1);}
      ;
/*********************************************************Implémentation de la commande DESC*******************************************************************/
cmd_instr_DESC: CMD_DESC IDENT FIN { describe($2); printf("\nshellSQL_> "); yyparse();}
              ; 
%%
/*********************************************************Des fonctions C*************************************************************************************/
// Redefinition de la fonction yyerror()                                 
int yyerror()
{

fprintf(stderr, "Erreur de syntaxe\a\n\n");
fprintf(stderr, "Pour avoir l'aide,taper: help nomCommande;\n");
printf("shellSQL > ");
yyparse();
return 1;
}
//definition de fonction main qui peut pendre en paramètre un fichier contenant les requêtes
    int  main(int argc, char *argv[]){
printf("********************************************************\n");
printf("*                                                      *\n");
printf("* INTERPRETEUR DE COMMANDE shellSQL                    *\n");
printf("* Version: 1.1                                         *\n");
printf("* Copyright LOUE Boureima Arnaud et OUEDRAOGO Pascal   *\n");
printf("* All rights reserved                                  *\n");
printf("*                                                      *\n");
printf("********************************************************");
printf("\n\nshellSQL > ");
    char * infile=NULL;
    char * outfile=NULL;

    infile = argv[1];
    yyin = fopen(infile,"r");

   
    if(yyin!=NULL) 
    fclose(yyin);
    

    yyparse();
    return 1;
}
                              /**************************fin de l'analyseur syntaxique et semantique*****************************/


