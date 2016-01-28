/*****************************************
* MINI INTERPRETEUR SQL                  *
* LOUE Boureima Arnaud et                *
* OUEDRAOGO Pascal                       *
*                                        *
******************************************/
/************************************************************Definition des fonctions C************************************************************************/

#include "defines.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

///extraction du type d'une donnee ex:<nom type="text" contrainte="primary key">LOUE</nom> va extraire la chaine "primary key"
char *extractContrainte(char *ch) {
char *ch1=NULL;
int i=0,nb=0,h=0,k=0,b=2;

ch1=(char *)malloc(TAILLE_DONNEE*sizeof(char));
nb=strlen(ch);
for(i=0;i<nb;i++){
if (ch[i]=='='){
k++;
}
if (k==2){
ch1[h]=ch[i+b];
h++;
}
if(ch[i]=='>'){
h=h-5;
ch1[h]='\0';
break;
}
}
return ch1;
free(ch1);
}
//FONCTION QUI EXTRAIT L'ATTRIBUT REFERENCE POUR UNE CONTRAINTE FOREIGN KEY
//Ex:alter table add id_Ecole text foreign key references ecole(id); extrait id 
char *extractAttribRef(char *ch){
    int i=0;
    int j=0;
    char * chaine=NULL;
    char *chaine1=NULL;
    chaine=(char *)malloc(sizeof(char)*TAILLE_DONNEE);
    chaine1=(char *)malloc(sizeof(char)*TAILLE_DONNEE);
for(i=strlen(ch)-2;i>=0;i--){
        chaine[j]=ch[i];
        j++;

    if(ch[i]=='('){
    chaine[--j]='\0';
    break;
    }
    }
    j=0;
    for(i=strlen(chaine)-1;i>=0;i--){
        chaine1[j]=chaine[i];
        j++;
    }
    return chaine1;
    free(chaine);
    free(chaine1);
}//La fonction qui gere la contrainte foreign Key:On s'assure que la table réfrencée existe d'abord avant d'autoriser la création se la table
int gereContrainteForeign(ListeAtt *liste){
    Element *cour=(Element *)malloc(sizeof(Element *));
    int nb=0,i=0,ok=1;
    int j=0;
    char * chaine=NULL;
    nb=liste->taille;
    cour=liste->premier;

    chaine=(char *)malloc(sizeof(char)*TAILLE_DONNEE);
    
for(i=0;i<nb;i++){
  if(ifExist(cour->contrainte,"foreign") || ifExist(cour->contrainte,"FOREIGN")){
    for(i=23;i<strlen(cour->contrainte);i++){
        chaine[j]=cour->contrainte[i];
        j++;
        if(cour->contrainte[i]=='('){
            chaine[--j]='\0';
            break;
        }
    }
    if(!exist_DB(chaine)){
   ok=0;
   break;
}
    }
cour=cour->suivant;
}
return ok;
free(chaine);
}
//La fonction qui gere la contrainte foreign Key pour un alter table:On s'assure que la table réfrencée existe d'abord avant d'autoriser la création se la table
char *  gereContrainteForeign2(char *ch){
    int i=0;
    char *retour=NULL;
    int j=0,nb=0;
    FILE *fichier=NULL;
    char * chaine=NULL;
    char * table_Name=NULL;
    char * buff=NULL;
    chaine=(char *)malloc(sizeof(char)*TAILLE_DONNEE);
    retour=(char *)malloc(sizeof(char)*5);
    retour[0]='o';retour[1]='n'; retour[2]='n';
    table_Name=(char *)malloc(sizeof(char)*TAILLE_DONNEE);
  if(ifExist(ch,"foreign key") || ifExist(ch,"FOREIGN KEY")){
     retour[2]='o';
    for(i=23;i<strlen(ch);i++){
        chaine[j]=ch[i];
        j++;
        if(ch[i]=='('){
            chaine[--j]='\0';
            break;
        }
    }
    if(!exist_DB(chaine)){
   retour[0]='n';
 }
else {
table_Name=generer_nom(chaine);
fichier=fopen(table_Name,"r"); //ON ESSAYE D'OUVIR LE FICHIER EN LECTURE
if(fichier==NULL){ 
printf("\nImpossible d'ouvrir le fichier en mode lecture");
fclose(fichier);
}
else {
nb=gotoPosition(fichier,"<struct>");///on se place a la position <struct> pour commencer a lire les donnees de l'entete
while(!ifExist(buff,"</struct>")){ ///on lit dans le fichier jusqu'à croiser la chaine </struct>
nb=LireLigne(fichier,&buff,10);
const char *lign=extracAttrib(buff);
const char *attribRef=extractAttribRef(ch);
if(strcmp(lign,attribRef)==0){
retour[1]='o';
}
}
}
}
}
  return retour;
  free(chaine);
  free(table_Name);
  free(retour);
}
/*fonction permettant de lire sequentiellement un fichier lire par ligne. elle retourne le nombre de caractere lu*/
long LireLigne(FILE * f, char ** buff, long dim);

///fonction de recherche de la position d'un caractere dans chaine 
static long Cherchecar(char* buffer, long offset, int car)
{
    char * pos = strchr(buffer+offset,car);
    return  pos != NULL ? pos - buffer : -1;
}
//fonction pour realouer dynamiquement la memoire
static char * RedimAlloc( char *buffer, size_t dim)
{
    char * q = realloc(buffer,dim);
    if (q == NULL) free(buffer);
    return q;
}

////definition de la taille a allouer par defaut
#define DIM_DEFAULT 32

/*fonction permettant de lire sequentiellement un fichier lire par ligne. elle retourne le nombre de caractere lu*/
long LireLigne(FILE * f, char ** buff, long dim)
{

    long eoln = -1;
    long zero = -1;
    char *notEOF;
    int AllocError;
    long offset = 0 ;
     free(*buff);
    if (dim<2) dim = DIM_DEFAULT;
    *buff = malloc((size_t)dim);
    AllocError = *buff == NULL;
    if (!AllocError)
    {
        notEOF= fgets(*buff,dim,f);
        if (!notEOF)
        {
            free(*buff);
            *buff = NULL;
        }
        while (notEOF && eoln < 0 && *buff != NULL)
        {
            eoln = Cherchecar(*buff,offset,'\n');
            if (eoln >=0) (*buff)[eoln] = 0;
            else
            {
                zero = Cherchecar(*buff,offset,'\0');
                if (zero == dim-1)
                {
                    offset = dim-1;
                    dim *=2;
                    *buff = RedimAlloc(*buff,(size_t)dim);
                    AllocError = *buff == NULL;
                }
                if (!AllocError)
                    if (!(notEOF = fgets(*buff+offset,offset+2,f))) eoln = zero;
            }
        }
        if (*buff!= NULL) *buff = RedimAlloc(*buff,(size_t)(eoln+1));
    }
    return *buff != NULL ? eoln
                         : AllocError ? -1 : 0;
}
#undef DIM_DEFAULT

//////test l'existence d'une chaine dans une autre
int ifExist(char * ligne,char * chaine2){
char  *pt;
if(ligne==NULL)return 0;
pt=strstr(ligne,chaine2);//fonction de recherche d'une occurence de la chaine ligne dans la chaine chaine 2
if (pt==NULL ) return 0;
else return 1;
}

///permet de palcer la tete de lecture a une position donner

int gotoPosition(FILE *fichier , char * chaine){
int i=0,nb=1;
char *buff=NULL;
rewind(fichier);
 while(nb!=0){
    nb=LireLigne(fichier,&buff,5);
    i++;
if (ifExist(buff,chaine)) return i;//si la ligne lu contient la chaine rechercher alors on quitte la fonction

    }
    return 0;
}



/*  ********************** **************
 *  Initialisation des listes chainées **
 ***************************************/
///initialisation de la liste chainee de la structure de la table
ListeAtt *initialise(){
ListeAtt *liste=NULL;
liste=(ListeAtt *)malloc(sizeof(ListeAtt *));
liste->premier=NULL;
liste->dernier=NULL;
liste->taille=0;
return liste;
}

///: initialisation de la liste chainee des element d'un tuple
ListeTuple *initialiseListeTuple(){
ListeTuple *liste=malloc(sizeof(ListeTuple));

liste->premier=NULL;
liste->dernier=NULL;
liste->taille=0;
liste->suivant=NULL;

return liste;
}

///initialise la liste globale

ListeAllTuple *initialiseAllTuple(){
 ListeAllTuple *liste=malloc(sizeof(*liste));

 liste->premier=NULL;
 liste->dernier=NULL;
 liste->taille=0;
 }


/// extrait  la donnee dans une ligne lu ex: <nom type="text">BELEM</nom> va extraire la chaine "BELEM"

char *extractData(char *ch){

char *ch1=NULL;
int nb=0,i=0,h=0,k=0;
ch1=(char *)malloc(TAILLE_DONNEE*sizeof(char));
nb=strlen(ch);

for(i=0;i<nb;i++){

if(ch[i]=='<'&&h==0){
h=1;
continue;
}
else if(ch[i]=='>'&&h==1){
h=2;
}

if(h==2&&ch[i]!='>'){

ch1[k]=ch[i];
k++;
}
if(ch[i]=='<'&&h==2)
{
    k--;
    ch1[k]='\0';
    break;

}
}


return ch1;

}


///extraction du type d'une donnee ex:<nom type="text">BELEM</nom> va extraire la chaine "text"

char * extractType(char *ch){
    int nb=0,i=0,h=0,k=0;
    char *ch1=NULL;
    nb=strlen(ch);
ch1=(char *)malloc(sizeof(char)*TAILLE_DONNEE);/// taille des donnee a extraire 100
    for(i=0;i<nb;i++){
    if(ch[i]=='"'&&h==0){
    h=1;
    continue;
    }
    if(ch[i]!='"'&&h==1){
    ch1[k]=ch[i];
    k++;

    }
    else if(ch[i]=='"'&&h==1){
    ch1[k]='\0';
    break;
    }
    }
return ch1;
free(ch1);
}


///Extrait l' attribut d'une donnee <nom type="text">LOUE</nom> va extraire la chaine "nom"
char *extracAttrib(char *ch){
char *ch1=NULL;
int i=0,nb=0,h=0,k=0;

ch1=(char *)malloc(TAILLE_DONNEE*sizeof(char));
nb=strlen(ch);
for(i=0;i<nb;i++){
if (ch[i]=='<'&&h==0){
h=1;
continue;
}
if (h==1){
ch1[k]=ch[i];
k++;
}
if (ch[i]==' '&&h==1){
    --k;
ch1[k]='\0';
break;
}

}
return ch1;
free(ch1);
}



// extrait les donnees de l'entete d'un fichier xml et les met dans une liste chainee d'element

ListeAtt *extractStruct(FILE *fichier){
char *chaine=NULL;
char *buff=NULL;
ListeAtt *liste=NULL;
Element **element=NULL;
liste=(ListeAtt *)malloc(sizeof(ListeAtt *));
chaine=(char*)malloc(TAILLE_DONNEE*sizeof(char));
buff=(char*)malloc(TAILLE_DONNEE*sizeof(char));
chaine="<struct>";
element=(Element **)malloc(NOMBRE_ELE*sizeof(Element *));
int nb=0,i=0,k=0;

nb=gotoPosition(fichier,chaine);///on se place a la position <struct> pour commencer a lire les donnees de l'entete

liste=initialise();// initialisation de la liste

while(!ifExist(buff,"</struct>")){ ///on lit dans le fichier jusqu'a croiser la chaine </struct>

nb=LireLigne(fichier,&buff,10);
if(ifExist(buff,"</struct>")){
break;
}
element[k]=NULL;
element[k]=(Element *)malloc(sizeof(Element));
strcpy(element[k]->att,extracAttrib(buff));
strcpy(element[k]->typ,extractType(buff));
strcpy(element[k]->contrainte,extractContrainte(buff));
insertStruct(liste,element[k]);
k++;
}


return liste;
free(chaine);
free(buff);
free(liste);
}


/*  ********************** **************
 *  insertion dans des listes chainées **
 ***************************************/


/// insert une donnee dans la liste chainne Tuple
void insertTuple(ListeTuple *liste,Donnee *donnee){
///si l'element a inserer est le premier de la liste
if(liste->premier==NULL){
 liste->premier=donnee;
 liste->dernier=donnee;
 donnee->suivant=NULL;
 liste->taille=liste->taille+1;
}
//insertion dans les autres cas
else{
 (liste->dernier)->suivant=donnee;
    liste->dernier=donnee;
    donnee->suivant=NULL;
    liste->taille=liste->taille+1;
}

}

///insert une structure de type element dans la liste chainee de l'entete

void insertStruct(ListeAtt *liste,Element *element){
///si l'element a inserer est le premier de la liste
if(liste->premier==NULL){
liste->premier=element;
liste->dernier=element;
element->suivant=NULL;
liste->taille=liste->taille+1;
}
//insertion dans les autres cas
else{
(liste->dernier)->suivant=element;
liste->dernier=element;
element->suivant=NULL;
liste->taille=liste->taille+1;
}
}

/// insert un tuple dans la liste global de toutes les tuples
void insertAllTuple(ListeAllTuple *liste , ListeTuple *listeTup){
///si l'element a inserer est le premier de la liste
  if(liste->premier==NULL){
   liste->premier=listeTup;
   liste->dernier=listeTup;
   listeTup->suivant=NULL;
   liste->taille=liste->taille+1;
   }
//insertion dans les autres cas
   else{
(liste->dernier)->suivant=listeTup;
liste->dernier=listeTup;
listeTup->suivant=NULL;
liste->taille=liste->taille+1;
   }
}

/// affiche un tuple

void afficheTuple(ListeTuple *liste){

Donnee *cour=malloc(sizeof(Donnee *));

cour=liste->premier;
while(cour!=NULL){

 printf("\n  attribut: %s type :  %s  donnee : %s \n",cour->element.att,cour->element.typ,cour->data);
 cour=cour->suivant;
}

}

///affiche tous les tuples de la base donnee
void afficheAll(ListeAllTuple *liste){

ListeTuple *cour=malloc(sizeof(ListeTuple *));

cour=liste->premier;
if(liste->taille==0){
printf("\n\aAucune donnee trouvee !!\n");
}
else{
while(cour!=NULL){

   afficheTuple(cour);
   cour=cour->suivant;

    }

}
    }

///affiche la structure de la table
void affiche(ListeAtt *liste){
Element *cour=(Element *)malloc(sizeof(Element *));
int nb,i,j,k,n;

if(liste==NULL) printf("\n la liste est vide\n");

else{

nb=liste->taille;
printf("\n");
cour=liste->premier;
for(i=0;i<nb;i++)
{
 	printf("| %s ",cour->att);
 	n=strlen(cour->att);
 	for(j=0;j<23-n;j++)
 	printf(" ");
 	cour=cour->suivant;
}
	printf("\n");
	for(k=0;k<nb;k++){
	for(j=0;j<23;j++)
	printf("_");
}
	printf("\n");
	cour=liste->premier;
for(i=0;i<nb;i++)
{
  
   printf("| %s ",cour->typ);
   n=strlen(cour->typ);
   for(j=0;j<23-n;j++)
   printf(" ");
   cour=cour->suivant;
}
	printf("\n");

    }
}


///fonction d'extraction d'un  tuple dans un  fichier a partir d'une position donnee

ListeTuple *extractTuple(FILE *fichier){
    ListeTuple *liste=NULL;
char *buff=NULL;
int n,k=0;
Donnee **donnee;
char ch[60];

donnee=(Donnee **)malloc(NOMBRE_ELE*sizeof(Donnee *));


liste=initialiseListeTuple();

while(!ifExist(ch,"</record>")){

n=LireLigne(fichier,&buff,10);
if(ifExist(buff,"</record>"))
break;


///si on croise </table> dans la chaine lu alors on est a la fin du fichier donc on arrete l'extraction
if(ifExist(buff,"</table>")){
liste=NULL;
return liste;
}

strcpy(ch,buff);
if(ifExist(ch,"<record>"))
continue;

donnee[k]=(Donnee *)malloc(sizeof(Donnee));

strcpy(donnee[k]->data,extractData(ch));

strcpy(donnee[k]->element.att,extracAttrib(ch));

strcpy(donnee[k]->element.typ,extractType(ch));
donnee[k]->suivant=NULL;

insertTuple(liste,donnee[k]);
k++;
}

//on retourne la liste de donnees lue
return liste;
}

///fonction permettant d'extraire toutes les tuples d'un fichier xml

ListeAllTuple *extractAll(FILE *fichier){

int i=0,n=0;
ListeAllTuple *listeAll;
  ListeTuple **liste=malloc(20*sizeof(ListeTuple *));

listeAll=initialiseAllTuple();

n=gotoPosition(fichier,"</struct>");///on se positionne a partir de </struct> pour commencer l'extraction des tuples

do{
     liste[i]=(ListeTuple *)malloc(sizeof(ListeTuple *));
//appel successif de la fonction d'extraction d'un tuple jusqu'a ce que le tuple extrait soit NULL
        liste[i]=extractTuple(fichier);
        if(liste[i]==NULL)
        break;
        insertAllTuple(listeAll,liste[i]);
        i++;
        }while(liste[i-1]!=NULL);
///retourne la liste globale
    return listeAll;
    }

/***************************************/
//partie de definition des fonctions SQL
//*************************************/

///GENERE LE NOM D'UN FICHIER XML
char *generer_nom(char *nom){
char *base;
base=(char *)malloc(30*sizeof(char));
strcpy(base,nom);
strcat(base,".xml");
return base;
}

///TESTER L'EXISTENCE D'UNE TABLE

int exist_DB(char *ch){
FILE *fichier=NULL;
char *chaine=NULL;
chaine=generer_nom(ch);
fichier=fopen(chaine,"r");//ON ESSAYE D'OUVIR LE FICHIER EN LECTURE
if(fichier==NULL){//SI IMPOSSIBLE ALORS LE FICHIER N'EXISTE PAS

return 0;
}
fclose(fichier);

return 1;
}

///AFFICHE  LA STRUCTURE DE LA TABLE
void describe(char *ch){
    FILE *fichier=NULL;
     char *nom=NULL;
     ListeAtt *liste;

if(exist_DB(ch)){
nom=generer_nom(ch);
fichier=fopen(nom,"r");
liste=extractStruct(fichier);///EXTRACTION DE LA STRUCTURE DE LA TABLE
affiche(liste);///AFFICHAGE DE LA STRUCTURE
fclose(fichier);
}
else
printf("\n Table inexistante !!!\n");

}


///EXTRACTION DES CHAINES SEPAREE PAR DES VIRGULE

char **extract_by_virgule(char *ch){
char **chaine=NULL;
int i=0,nb=0,n,k=0,h=0;
// ON RESERVE LA PLACE POUR 20 CHAINES
chaine=(char **)malloc(TAILLE_DONNEE*sizeof(char *));
for(i=0;i<20;i++){
chaine[i]=NULL;
}
n=strlen(ch);

for(i=0;i<n;i++){
if(i==0){
chaine[nb]=(char *)malloc(TAILLE_DONNEE*sizeof(char));
}
if(ch[i]==' '&&h==0){
chaine[nb][k]=' ';
k++;
 continue;
}
if(ch[i]==' '&&h==1)
continue;
if(ch[i]!=','){
chaine[nb][k]=ch[i];
h=0;
k++;
}
else
{
 chaine[nb][k]='\0';
 nb++;
 k=0;
h=1;
chaine[nb]=(char *)malloc(TAILLE_DONNEE*sizeof(char));

}

}
chaine[nb][k]='\0';
return chaine;
}


///EXTRAIT DES CHAINE SEPAREE PAR DES POINTS

char **extract_by_point(char *ch){
char **chaine=NULL;
int i=0,nb=0,n=0,k=0;
chaine=(char **)malloc(3*sizeof(char *));
for(i=0;i<3;i++){
chaine[i]=NULL;
}
n=strlen(ch);

for(i=0;i<n;i++){
if(i==0){
chaine[nb]=(char *)malloc(TAILLE_DONNEE*sizeof(char));
}

if(ch[i]!='.'){
chaine[nb][k]=ch[i];
k++;
}
else
{
 chaine[nb][k]='\0';
 nb++;
 k=0;
chaine[nb]=(char *)malloc(TAILLE_DONNEE*sizeof(char));

}

}
chaine[nb][k]='\0';
return chaine;
}


///CREE UN ELEMENT CONTENANT LE NOM,LE TYPE ET LA CONTRAINTE DE L'ATTRIBUT
Element *creer_champ(char *ch){
int i=0,n=0,k=0,g=0,c=0,d=0,h=0,trouv=0,pr=0,compt=0;
Element *element=NULL;
char *chaine1=NULL;
char *chaine2=NULL;
char *chaine3=NULL;
n=strlen(ch);
//CREATION DE L'ELEMENT
element=(Element *)malloc(sizeof(Element));
chaine1=(char *)malloc(sizeof(char)*n);
chaine2=(char *)malloc(sizeof(char)*n);
chaine3=(char *)malloc(sizeof(char)*n);

for(i=0;i<n;i++){
if(ch[i]==' '&&pr==0){
continue;}

if(ch[i]!=' '&&h==0){
chaine1[k]=ch[i];
k++;
pr=1;

}
else {

    if(ch[i]==' '&&h==0){
     chaine1[k]='\0';
      k=0;
      h=1;
}
if(ch[i]!=' '&&g==0)
{
    chaine2[k]=ch[i];
    k++;
if(ch[i]=='p' || ch[i]=='P'|| ch[i]=='f' || ch[i]=='F' || ch[i]==',') g=1;    
}
}
}
if(g) chaine2[--k]='\0';
else  chaine2[k]='\0';
if(ifExist(ch,"primary key") || ifExist(ch,"PRIMARY KEY")|| ifExist(ch,"foreign key") || ifExist(ch,"FOREIGN KEY")){
int esp=0;
for(i=0;i<n-1;i++){
if(ch[i]==' ') esp++;
if(esp==2) trouv=1;
if(trouv){
chaine3[d]=ch[i+1];
d++;
}
}
}
chaine3[d++]='\0';
strcpy(element->att,chaine1);
strcpy(element->typ,chaine2);
strcpy(element->contrainte,chaine3);
return element;

}


///PERMET D'EXTRAIRE LES CHAMPS AINSI QUE LEUR TYPE
//LA FONCTION PREND EN PARAMETRE UN TABLEAU DE CHAINE DE CARACTERE

ListeAtt *extract_champ(char **ch){
    int k=0,n=0;
    Element **element=NULL;
    ListeAtt *liste=NULL;

    while(ch[n]!=NULL)
    n++;

liste=initialise();
    element=(Element **)malloc(n*sizeof(Element *));

while(ch[k]!=NULL){
element[k]=creer_champ(ch[k]);

insertStruct(liste,element[k]);
k++;
}

return liste;
}


///FONCTION TESTANT L'EXISTENCE D'UNE COLONNE DANS LA LISTE CHAINEE DE L'ENTETE

int recherche_In_Struct(char *nom,ListeAtt *liste){

Element *cour=NULL;
char *res=NULL;
cour=liste->premier;
while(cour!=NULL){

res=strstr(cour->att,nom);
if(res!=NULL&&(strlen(cour->att)==strlen(nom))){
return 1;
}

cour=cour->suivant;
}

return 0;


}



///FONCTION PERMETTANT D'ECRIRE LA STRUCTURE DE LA TABLE DANS LE FICHIER
void write_struct(FILE *fichier,ListeAtt *liste,char *nom){
Element *cour=(Element *)malloc(sizeof(Element *));
int nb=0,i=0;

if(liste==NULL) printf("\n la liste est vide\n");

else{
 fprintf(fichier,"<?xml version=\"1.0\"  encoding=\"utf-8\"?>\n");
 fprintf(fichier,"<table name= \"%s\">\n",nom);
 fprintf(fichier,"<struct>\n");
nb=liste->taille;
cour=liste->premier;
for(i=0;i<nb;i++)
{
fprintf(fichier,"<%s type=\"%s\" contrainte=\"%s\"/>\n",cour->att,cour->typ,cour->contrainte);
cour=cour->suivant;
}
fprintf(fichier,"</struct>\n");
    }

}


///LA FONCTION DE CREATION D'UN FICHIER XML EN DONNANT EN PARAMETRE LE NOM DE LA TABLE ET SA STRUCTURE

int create_table(char *nom,char *ch){
ListeAtt *liste=NULL;
char *name=NULL;
FILE *fichier=NULL;
char **chaine=NULL;
Element *cour=NULL;
chaine=extract_by_virgule(ch);
liste=extract_champ(chaine);
int nbre_Contrainte_Primaire=0;
name=generer_nom(nom);
int ok=1,n=0,i=0;
n=liste->taille;
char *ok1=NULL;
ok1=(char *)malloc(5*sizeof(char));
ok1[0]='o';ok1[1]='n';
cour=(Element *)malloc(sizeof(Element));
cour=liste->premier;
///test si la table n'existe pas deja dans la base de donnee
if(exist_DB(nom)){
printf("\n\a Table deja existante !!!\n");
return 0;
}
ok=gereContrainteForeign(liste);
// VERIF
for(i=0;i<n;i++){
if(ifExist(cour->contrainte,"foreign key") || ifExist(cour->contrainte,"FOREIGN KEY")){
ok1=gereContrainteForeign2(cour->contrainte);
if((ok1[0]!='o' || ok1[1]!='o')){
printf("\nL'attribut reférencé n'existe pas dans la table réferencé");
return 0;
break;
}
}
else cour=cour->suivant;
}
// Appel des fonctions de contrôl
nbre_Contrainte_Primaire=verifContraintePrimaire(liste);
if(nbre_Contrainte_Primaire<2 && ok==1){
fichier=fopen(name,"w");
if(fichier==NULL){
printf("impossible de creer la table !!!");
return 0;
}

write_struct(fichier,liste,nom);
fprintf(fichier,"</table>\n");
fclose(fichier);
printf("la table %s a ete creee avec succes !!",nom);
}
else {
if(nbre_Contrainte_Primaire>1) printf("La contrainte clé primaire doit porter sur une seule colonne");
else printf("La Table reférencée n'existe pas!!");
}
}

///FONCTION POUR LE CHARGEMENT DES DONNEES EN MEMOIRE VIVE
data_link *load_data(FILE *fichier){

data_link *link=NULL;

link=(data_link *)malloc(TAILLE_DONNEE*sizeof(data_link));
link->listeAt=extractStruct(fichier);///EXTRACTION DE LA STRUCTURE DE LA TABLE

link->listeAll=extractAll(fichier);///EXTRACTION DE TOUTES LES DONNEES
return link;
}



///FONCTION POUR ECRIRE UN TUPLE DANS UN FICHIER

void write_tuple(FILE *fichier,ListeTuple *liste){

Donnee *cour=malloc(sizeof(Donnee *));

cour=liste->premier;
fprintf(fichier,"<record>\n");
while(cour!=NULL){

///ECRITURE DE CHAQUE LIGNE D'UN RECORD
fprintf(fichier,"<%s type=\"%s\">%s</%s>\n",cour->element.att,cour->element.typ,cour->data,cour->element.att);
 cour=cour->suivant;
}

fprintf(fichier,"</record>\n");
}


///FONCTION POUR ECRIRE TOUTES LES DONNEES DANS LE FICHIER
void write_all_data(FILE *fichier,ListeAllTuple *liste){

ListeTuple *cour=malloc(sizeof(ListeTuple *));
cour=liste->premier;
if(liste->taille==0){
printf("\n\a");
}
else{
while(cour!=NULL){
///APPEL SUCCESSIF DE LA FONCTION D'ECRITURE D'UN TUPLE
  write_tuple(fichier,cour);
   cour=cour->suivant;

    }
fprintf(fichier,"</table>");
}

}

/// SAUVEGARDE DES DONNEES DANS UN FICHIER

void save_data(FILE *fichier,ListeAtt *listeAt,ListeAllTuple *liste,char *nom_table){

write_struct(fichier,listeAt,nom_table);///ECRITURE DE L'ENTETE
write_all_data(fichier,liste);///ECRITURE DES TUPLES
if(liste->premier==NULL)
fprintf(fichier,"</table>");


}
///FABRIQUE UNE LISTE CHAINE DE TUPLE A PARTIR D'UNE CHAINE DE CARACTERE
ListeTuple *fabrique_liste_tuple(char *data,ListeAtt *liste){

char **chaine=NULL;
Donnee **donnee=NULL;
Element *cour=NULL;
ListeTuple *listeT=NULL;
int i=0,n=0;

n=liste->taille;
cour=(Element *)malloc(sizeof(Element));
donnee=(Donnee **)malloc(n*sizeof(Donnee *));
 chaine=extract_by_virgule(data);
 cour=liste->premier;
 listeT=initialiseListeTuple();
for(i=0;i<n;i++){
donnee[i]=(Donnee *)malloc(sizeof(Donnee));
strcpy(donnee[i]->data,chaine[i]);
strcpy(donnee[i]->element.att,cour->att);
strcpy(donnee[i]->element.typ,cour->typ);
strcpy(donnee[i]->element.contrainte,cour->contrainte);
donnee[i]->suivant=NULL;
insertTuple(listeT,donnee[i]);
cour=cour->suivant;

 }


return listeT;
}

///COMPTER LE NOMBRE DE CHAMPS DANS UNE CHAINE DE CARACTERE
int  compter_champ(char *ch)
{

    int i=0,n=0,k=0;
    n=strlen(ch);

    for(i=0;i<n;i++)
    {
       if(ch[i]==',')
       k++;
    }
return k;

}

///VERIFIE SI LA CHAINE DONNEE EN PARAMETRE CONTIENT UNIQUEMENT DES CHIFFRES 
int is_integer(char *chaine)
{
    int n=0,i=0;
    n=strlen(chaine);
    for(i=0;i<n;i++)
    {
        if(!isdigit(chaine[i]))///TEST CARACTERE PAR CARACTERE
        {
            return 0;
        }
    }
    return 1;
}
//LA FONCTION QUI VERIFIE QUE LA CHAINE DONNEE EN PARAMETRE EST UNE VALEUR NUMERIQUE
int is_number(char *chaine)
{
    int n=0,i=0;
    n=strlen(chaine);
    for(i=0;i<n;i++)
    {
        if(isdigit(chaine[i]) || chaine[i]=='.')///TEST CARACTERE PAR CARACTERE
        {
            return 1;
        }
    }
    return 0;
}

// VERIFIE QUE LA VALEUR INSERREE EST UNE VALEUR BOOLEENNE IE 0|1
int is_boolean(char *chaine)
{
    int n=0,i=0;
    n=strlen(chaine);
if(n==1){
    
       if(isdigit(*chaine) && ((*chaine=='0')||(*chaine=='1')))///TEST CARACTERE PAR CARACTERE
        {
            return 1;
        }
else return 0;
}
else{
if(strcmp(chaine,"NULL")==0 || strcmp(chaine,"null")==0) return 1;
else return 0;
}
}
//VERIFIE QUE LA VALEUR INSERREE EST UNE CHAINE DE CARACTERE IE UNE VALEUR CONTENANT DEUX '.
int is_text(char *chaine)
{
if(!strchr(chaine,'\'')){
return 0;
}
else return 1;
}



///CONTROLE LE RESPECT DES TYPES DANS UN TUPLE A INSERER DANS LA TABLE 
int test_type(ListeTuple *liste)
{
    Donnee *cour=NULL;
    int n=0,i=0;
    cour=liste->premier;
    while(cour!=NULL)
    {

        if(ifExist(cour->element.typ,"integer")||ifExist(cour->element.typ,"INTEGER"))
        {

         if(ifExist(cour->element.contrainte,"primary key")||ifExist(cour->element.contrainte,"PRIMARY KEY")){
               
            if(!is_integer(cour->data))
            {
                printf("Le champ n° %d est la clé primaire et est de type INTEGER:Veuillez inserer une valeur entière et différent de NULL",i+1);
                return 0;
            }
         }
        else {
         if(!is_integer(cour->data))
            {
         
             if(strcmp(cour->data,"null")==0 || strcmp(cour->data,"NULL")==0){
                return 1;
            }
             else{
                   printf("Veuillez inserer une valeur entière dans le champs n° %d\n",i+1);
                    return 0;
                 } 
            }
        }
        }
      if(ifExist(cour->element.typ,"boolean")||ifExist(cour->element.typ,"BOOLEAN"))
        {
            if(!is_boolean(cour->data))
            {
                printf("veuillez inserer une valeur Booléen(0 ou 1) dans le champs n° %d\n",i+1);
                return 0;
            }

        }

      if(ifExist(cour->element.typ,"text")||ifExist(cour->element.typ,"TEXT"))
        {
          if(ifExist(cour->element.contrainte,"primary key")||ifExist(cour->element.contrainte,"PRIMARY KEY")){
            
            if(!is_text(cour->data))
            {
                printf("Le champ n° %d est la clé primaire et est de type TEXT : Veuillez inserer une chaine de caractère et différent de NULL|null",i+1);
                return 0;
            }

        }
      
      else {
         if(!is_text(cour->data))
            {
         
             if(strcmp(cour->data,"null")==0 || strcmp(cour->data,"NULL")==0){
                return 1;
            }
             else{
                   printf("Veuillez inserer une chaine de caractère dans le champs n° %d\n",i+1);
                    return 0;
                 } 
            }
        }
       }
     if(ifExist(cour->element.typ,"number")||ifExist(cour->element.typ,"NUMBER"))
        {
          if(ifExist(cour->element.contrainte,"primary key")||ifExist(cour->element.contrainte,"PRIMARY KEY")){
            
            if(!is_number(cour->data))
            {
                printf("Le champ n° %d est la clé primaire et est de type NUMBER : Veuillez inserer un nombre et différent de NULL|null",i+1);
                return 0;
            }

        }
      
      else {
         if(!is_number(cour->data))
            {
         
             if(strcmp(cour->data,"null")==0 || strcmp(cour->data,"NULL")==0){
                return 1;
            }
             else{
                   printf("Veuillez inserer un nombre dans le champs n° %d\n",i+1);
                    return 0;
                 } 
            }
        }
       }
        cour=cour->suivant;
        i++;
    
}
    return 1;

}
///FONCTION D'INSERTION D'UN TUPLE DANS UN FICHIER XML
int requete_insert(char *nom,char *data){
ListeTuple *liste=NULL;
data_link *link=NULL;
FILE *fichier=NULL;
FILE *fichier_cop=NULL;
char *name=NULL;
int n_at=0,n_vir=0;
int ok=0,non=0;
Donnee *cour=NULL;
//GENERATION DU NOM DU FICHIER
name=generer_nom(nom);
fichier=fopen(name,"r");
if(fichier==NULL){
printf("\a");
printf("table inexistant !!");

return 0;
}
//CHARGEMENT DES DONNEES DU FICHIER 
link=load_data(fichier);


n_vir=compter_champ(data)+1;///DECOMPTE DU NOMBRE DE CHAMP A INSERER
n_at=link->listeAt->taille;
if(n_vir==n_at){
liste=fabrique_liste_tuple(data,link->listeAt);
cour=liste->premier;
cour=cour->suivant;
ok=gereContraintePrimaire(fichier,liste);
non=gereContrainteForeignInsertion(liste);
if(!test_type(liste))
{
   
    return 0;
}

insertAllTuple(link->listeAll,liste);
//FERMETURE DU FICHIER
fclose(fichier);
///OUVERTURE DU FICHIER DE SAUVEGARDE 
if(ok==1 && non==1){
fichier_cop=fopen("copie.xml","w");

///SAUVEGARDE  DES DONNES MODIFIEES
save_data(fichier_cop,link->listeAt,link->listeAll,nom);

fclose(fichier_cop);

///LIBERATION DE LA MEMOIRE UTILISEE
free(link->listeAll);
free(link->listeAt);
//SUPPRETION DE LA TABLE
remove(name);
//ET ON RENOMME LE FICHIER COPIE.XML COMME L'ANCIENNE TABLE
rename("copie.xml",name);
printf("1 ligne inserée !!");
}
else { 

if(ok!=1) printf("Violation de la contrainte PRIMARY KEY !!");

}
}
else{

printf("Le nombre de champs incorrect !!");

}

}




///ALGORITHME DE RECHERCHE DANS LES LISTES CHAINEES

///VERIFIE L'EXISTENCE D'UNE DONNEE DANS UN TUPLE A INSERER

int rechercher_in_tuple(char *nom,char *valeur,ListeTuple *liste){

Donnee *cour=NULL;
char *pt=NULL;
char *res=NULL;
int i=0;

cour=liste->premier;
///PERMET DE PARCOURIR TOUTES LES DONNEES DU TUPLE
while(cour!=NULL){
pt=strstr(cour->element.att,nom);
res=strstr(cour->data,valeur);
if(pt!=NULL&&res!=NULL&&(strlen(cour->data)==strlen(valeur))&&(strlen(cour->element.att)==strlen(nom))){
    break;
}
i++;
cour=cour->suivant;
}
if(pt!=NULL&&res!=NULL&&(strlen(cour->data)==strlen(valeur))&&(strlen(cour->element.att)==strlen(nom))){
//SI LA VALEUR RECHERCHER EST TROUVEE ON RETOURNE VRAI
return 1;
}

else{

return 0;
}
}


///TEST SI LE TUPLE EST LE PREMIER DES LISTE GLOBAL

int  is_first(ListeAllTuple *liste,ListeTuple *test){
if(liste->premier==test){
return 1;
}
else
return 0;
}
///TEST SI LE TUPLE EST LE DERNIER DE LA LISTE GLOBAL

int is_last(ListeAllTuple *liste,ListeTuple *test){

if(liste->dernier==test)
return 1;
else
return 0;

}


///SUPPRESION D'UN TUPLE DE LA LISTE GLOBALE

void delete_tuple(ListeAllTuple *liste,ListeTuple *supp){
ListeTuple *cour=NULL;
ListeTuple *prec=NULL;

//SI LE TUPLE  A SUPPRIMER EST LE PREMIER
if(is_first(liste,supp)){
///SI LE TUPLE  A SUPPRIMER EST L'UNIQUE DE LA TABLE
if(is_last(liste,supp)){
liste->dernier=NULL;
liste->premier=NULL;
free(supp);
}

else{
liste->premier=supp->suivant;
free(supp);
}
}
///SI LE TUPLE A SUPPRIMER N'EST PAS LE PREMIER
else{

//SI L'ELEMENT A SUPPRIMER A SUPPRIMER EST LE DERNIER
if(is_last(liste,supp)){

cour=liste->premier;
while(cour!=NULL){
if(cour->suivant==supp)
{
liste->dernier=cour;
cour->suivant=NULL;
free(supp);
break;

}

cour=cour->suivant;
}


}
///SI L'ELEMENT A SUPPRIMER N'EST NI LE PREMIER NI LE DERNIER
else{

cour=liste->premier;
prec=cour;
while(cour!=NULL)
{

    if(cour==supp)
    {
     prec->suivant=cour->suivant;
     free(cour);
     break;

    }
    prec=cour;
    cour=cour->suivant;
}

}


}

}


///FONCTION PERMETTANT DE CONNAITRE LE TYPE D'UNE COLONNE EN DONNANT LE NOM DE LA COLONNE
char * get_type_struc(ListeAtt  *liste,char *colonne)
{
    Element *cour=NULL;
    char *pt=NULL;
    cour=liste->premier;
    while(cour!=NULL)
    {
        pt=strstr(colonne,cour->att);
        if(pt!=NULL&&(strlen(colonne)==strlen(cour->att)))
        {
            return cour->typ;
        }
        cour=cour->suivant;
    }
}



//FONCTION PERMETTANT D'OBTENIR LA VALEUR D'UNE COLONNE DANS UN TUPLE
char *get_tuple_value(ListeTuple *liste,char *colonne)
{
    Donnee *cour=NULL;
    char *pt=NULL;
    char *ret=NULL;

    cour=liste->premier;
    while(cour!=NULL)
    {
        pt=strstr(cour->element.att,colonne);
        if(pt!=NULL&&(strlen(cour->element.att)==strlen(colonne)))
        {
            return cour->data;
        }
        cour=cour->suivant;
    }
    return ret;
}

///FONCTION PERMETTANT DE SUPPRIMER IN TUPLE D'UNE TABLE

int requete_delete(char *nom_table,char *colonne,char *valeur,char *op){

data_link *link=NULL;
FILE *fichier=NULL;
FILE *fichier_cop=NULL;
char *name=NULL;
char *valeur_trou=NULL;
int nbre_trou,nbre;
char *test_op=NULL;
char *test_col=NULL;
char *test_type=NULL;
ListeTuple *cour=NULL;
ListeTuple *prec=NULL;
int i=0;
int nb=0;

name=generer_nom(nom_table);
fichier=fopen(name,"r");
if(fichier==NULL){
printf("\a");
printf("table inexistante !!");
return 0;
}

///chargement des donnees
link=load_data(fichier);
fclose(fichier);
///test si la colonne exite 
if(!recherche_In_Struct(colonne,link->listeAt))
{
    printf("\aLe champs %s n'existe pas !!\n",colonne);
    return 0;
}
///verifie que les operateurs < et > sont utiliser sur une colonne de type integer ou number
if(ifExist(op,"<")||ifExist(op,">"))
{
       test_type=get_type_struc(link->listeAt,colonne);
       if(!ifExist(test_type,"integer")&&!ifExist(test_type,"INTEGER")&&!ifExist(test_type,"number")&&!ifExist(test_type,"NUMBER"))
        {
           printf("L'operateur \"%s\" ne s'appliquer sur la colonne \"%s\" !!\n",op,colonne);
           return 0;

        }
///verifie que la chaine valeur est composee de chiffres
   if(!is_integer(valeur))
      {
printf("Le type de \"%s\" incompatible avec l'operateur %s !!!",valeur,op);
return 0;
      }
      
}


test_op=strstr(op,"!=");
if(test_op){

cour=link->listeAll->premier;
while(cour!=NULL){

i=rechercher_in_tuple(colonne,valeur,cour);
if(!i){
prec=cour->suivant;
delete_tuple(link->listeAll,cour);
nb++;
cour=prec;
continue;

}
cour=cour->suivant;

}

fichier_cop=fopen("copie.xml","w");

save_data(fichier_cop,link->listeAt,link->listeAll,nom_table);
fclose(fichier_cop);
///LIBERATION DE LA MEMOIRE UTILISEE
free(link->listeAll);
free(link->listeAt);
remove(name);
rename("copie.xml",name);

printf("%d lignes supprimées !!",nb);
}
else if(ifExist(op,"=")){
cour=link->listeAll->premier;
while(cour!=NULL){

i=rechercher_in_tuple(colonne,valeur,cour);
if(i){
prec=cour->suivant;
delete_tuple(link->listeAll,cour);
nb++;
cour=prec;
continue;

}
cour=cour->suivant;

}

fichier_cop=fopen("copie.xml","w");

save_data(fichier_cop,link->listeAt,link->listeAll,nom_table);
fclose(fichier_cop);
///LIBERATION DE LA MEMOIRE UTILISEE
free(link->listeAll);
free(link->listeAt);
remove(name);
rename("copie.xml",name);
printf("%d lignes supprimées !!",nb);
}
else if(ifExist(op,"<"))
{
    cour=link->listeAll->premier;
    while(cour!=NULL)
    {
        valeur_trou=get_tuple_value(cour,colonne);
        nbre_trou=atoi(valeur_trou);///convertion la valeur de la colonne en cour en un entier
        nbre=atoi(valeur);///converti valeur en un entier
        if(nbre_trou<nbre)
        {
            prec=cour->suivant;
        delete_tuple(link->listeAll,cour);///suppression du tuple en cour 
        nb++;
       cour=prec;
       continue;

        }
cour=cour->suivant;
    }
    fichier_cop=fopen("copie.xml","w");

save_data(fichier_cop,link->listeAt,link->listeAll,nom_table);
fclose(fichier_cop);
///LIBERATION DE LA MEMOIRE UTILISEE
free(link->listeAll);
free(link->listeAt);
remove(name);
rename("copie.xml",name);

printf("%d lignes supprimées !!",nb);
}
else
{
     cour=link->listeAll->premier;
    while(cour!=NULL)
    {
        valeur_trou=get_tuple_value(cour,colonne);
        nbre_trou=atoi(valeur_trou);
        nbre=atoi(valeur);
        if(nbre_trou>nbre)
        {
            prec=cour->suivant;
        delete_tuple(link->listeAll,cour);
        nb++;
       cour=prec;
       continue;

        }
cour=cour->suivant;
    }

fichier_cop=fopen("copie.xml","w");

///sauvegarde des donnees
save_data(fichier_cop,link->listeAt,link->listeAll,nom_table);
fclose(fichier_cop);
///LIBERATION DE LA MEMOIRE UTILISEE
free(link->listeAll);
free(link->listeAt);
remove(name);
rename("copie.xml",name);

printf("%d lignes supprimées !!",nb);

}



}


///SUPPRESION D'UN CHAMP DANS LA LISTE DES CHAMPS


///TEST SI C'EST LE PREMIER ELEMENT

int  is_first_elem(ListeAtt *liste,Element *test){
if(liste->premier==test){
return 1;
}
else
return 0;
}
///TEST SI C'EST LE DERNIER ELEMENT

int is_last_elem(ListeAtt *liste,Element *test){

if(liste->dernier==test)
return 1;
else
return 0;

}
////suppression d'un element de la liste chainee des attribut de la table

void supp_elem(ListeAtt *liste,Element *supp){


Element *cour=NULL;
Element *prec=NULL;

//test si l'element a supprimer est le premier de la liste
if(is_first_elem(liste,supp)){
////teste si l'element a supprimer est l'unique element de la table
if(is_last_elem(liste,supp)){
liste->dernier=NULL;
liste->premier=NULL;
liste->taille=liste->taille-1;
free(supp);
}
else{
liste->premier=supp->suivant;
liste->taille=liste->taille-1;
free(supp);
}
}

else{
////test si l'element a supprimer est le dernier de la liste
if(is_last_elem(liste,supp)){

cour=liste->premier;
while(cour!=NULL){
if(cour->suivant==supp)
{
liste->dernier=cour;
cour->suivant=NULL;
liste->taille=liste->taille-1;
free(supp);
break;

}

cour=cour->suivant;
}


}
///cas ou l'element a supprimer est au milieu de la liste
else{

cour=liste->premier;
prec=cour;
while(cour!=NULL)
{

    if(cour==supp)
    {
     prec->suivant=cour->suivant;
     liste->taille=liste->taille-1;
     free(cour);
     break;

    }
    prec=cour;
    cour=cour->suivant;
}

}


}
}
///SUPPRIME UN ELEMENT DE LA STRUCTURE DE LA TABLE ET PREND EN PARAMETRE LE NOM DE LA COLONNE

int supp_elem_from_struct(ListeAtt *liste,char *nom){
Element *cour=NULL;
char *res=NULL;

cour=liste->premier;
///PARCOUR DE LA LISTE A LA RECHERCHE DE L'ELEMENT A SUPPRIMER
while(cour!=NULL){

res=strstr(cour->att,nom);
if(res!=NULL&&(strlen(cour->att)==strlen(nom)))
{
   ////SUPPRETION DE L'ELEMENT
    supp_elem(liste,cour);
    return 1;
}

cour=cour->suivant;

}

return 0;

}

///SUPPRESSION D'UNE DONNEE

///TEST SI C'EST LA PREMIERE DONNEE DE LA LISTE  TUPLE

int  is_first_data(ListeTuple *liste,Donnee *test){
if(liste->premier==test){
return 1;
}
else
return 0;
}
///TEST SI C'EST LA DERNIERE  DONNEE DE LA LISTE TUPLE

int is_last_data(ListeTuple *liste,Donnee *test){

if(liste->dernier==test)
return 1;
else
return 0;

}

/// SUPPRESSION D'UNE DONNEE

void supp_data(ListeTuple *liste,Donnee *supp){


Donnee *cour=NULL;
Donnee *prec=NULL;
//SUPPRIME SI L'ELEMENT A SUPPRIMER EST LE PREMIER DE LA LISTE
if(is_first_data(liste,supp)){

//SUPPRESSION SI LA DONNEE EST LE SEUL DE LA LISTE
if(is_last_data(liste,supp)){
liste->dernier=NULL;
liste->premier=NULL;
free(supp);
}
else{
liste->premier=supp->suivant;
free(supp);
}
}
else{

///SUPPRESSION SI LA DONNEE EST LA DERNIERE DE LA LISTE
if(is_last_data(liste,supp)){

cour=liste->premier;
while(cour!=NULL){
if(cour->suivant==supp)
{
liste->dernier=cour;
cour->suivant=NULL;
free(supp);
break;

}

cour=cour->suivant;
}


}

///SUPPRESSION D'UNE DONNEE SITUÉ AU MILLIEU DE LA LISTE
else{

cour=liste->premier;
prec=cour;
while(cour!=NULL)
{

    if(cour==supp)
    {
     prec->suivant=cour->suivant;
     free(cour);
     break;

    }
    prec=cour;
    cour=cour->suivant;
}

}


}
}


///FONCTION POUR SUPPRIMER UNE DONNEE D'UN TUPLES

void supp_data_from_Tuple(ListeTuple *liste,char *colonne){

Donnee *cour=NULL;
Donnee *prec=NULL;
char *res=NULL;

cour=liste->premier;
///RECHERCHE DE LA COLONNE
while(cour!=NULL){

    res=strstr(cour->element.att,colonne);

if(res!=NULL&&(strlen(cour->element.att)==strlen(colonne)))
{

    ///SUPPRIME LA DONNEE D'UNE COLONNE
    supp_data(liste,cour);
    break;
}

cour=cour->suivant;
}

}

///SUPPRIME UN CHAMP DE TOUS LES TUPLES
void supp_data_from_all(ListeAllTuple *liste,char *colonne){

ListeTuple *cour=NULL;
int i=0;

cour=liste->premier;
while(cour!=NULL)
{
    supp_data_from_Tuple(cour,colonne);
    cour=cour->suivant;
i++;

}


printf("\n %d lignes modifiées\n",i);

}


///permet de supprimer une colonne dans une table AINSI QUE DANS LES DONNEES 

int requete_alter_drop(char *nom_table,char *supp){
data_link *link=NULL;
FILE *fichier=NULL;
FILE *fichier_cop=NULL;
char *name=NULL;
int i=0;
Element *cour=NULL;
char *res=NULL;
int ok=1;

name=generer_nom(nom_table);
fichier=fopen(name,"r");
if(fichier==NULL){
printf("\a");
printf("table inexistante !!");
return 0;
}

///CHARGEMENT DE DONNEES
link=load_data(fichier);
ok=gere_Non_Suppression_Champ_Primaire(fichier,supp);
fclose(fichier);
if(!ok){
printf("Impossible de supprimer un champ portant une contrainte PRIMARY KEY!!");
return 0;
}
else {
///test si le champs a supprimer existe

if(!recherche_In_Struct(supp,link->listeAt))
{
    printf("\aLe champ %s n'existe pas !!\n",supp);
    return 0;
}

cour=link->listeAt->premier;
///RECHERCHE DE L'ELEMENT A SUPPRIMER
while(cour!=NULL){

res=strstr(cour->att,supp);
if(res!=NULL&&(strlen(cour->att)==strlen(supp)))
{
    supp_elem(link->listeAt,cour);
    break;
}

cour=cour->suivant;

}

printf("le schema de la table a ete modifié avec succès !!\n\n");

///SUPPRIMER LA DONNEE DE LA COLONNE DANS TOUTE LES TUPLES
supp_data_from_all(link->listeAll,supp);

fichier_cop=fopen("copie.xml","w");
save_data(fichier_cop,link->listeAt,link->listeAll,nom_table);
fclose(fichier_cop);
///LIBERATION DE LA MEMOIRE UTILISEE
free(link->listeAll);
free(link->listeAt);
remove(name);
rename("copie.xml",name);

printf(" Le schema de la table a ete modifiee !!");
}
}


///REQUETE ALTER TABLE ADD
int requete_alter_add(char *nom_table,char *nouvo_champ){

ListeTuple *courTup=NULL;
Donnee *donnee=NULL;
Element *element=NULL;
data_link *link=NULL;
FILE *fichier=NULL;
FILE *fichier_cop=NULL;
ListeAtt *cour=NULL;
char *name=NULL;
int i=0,ok2=1;
char *ok1=NULL;
ok1=(char *)malloc(5*sizeof(char));
ok1[0]='o';ok1[1]='n';
name=generer_nom(nom_table);
fichier=fopen(name,"r");
if(fichier==NULL){
printf("\a");
printf("table inexistante !!");
return 0;
}
///CHARGEMENT DE DONNEES
link=load_data(fichier);
// APPEL DE LA FONCTION QUI VERIFIE QU'AUCUN ATTRIBUT NE PORTE LA CONTRAINTE PRIMARY KEY
if(ifExist(nouvo_champ,"primary key") || ifExist(nouvo_champ,"PRIMARY KEY")) ok2=verifIfExistPrimaire(fichier);
fclose(fichier);

donnee=(Donnee *)malloc(sizeof(Donnee));

element=creer_champ(nouvo_champ);
strcpy(donnee->data,"NULL");
strcpy(donnee->element.att,element->att);
strcpy(donnee->element.typ,element->typ);
strcpy(donnee->element.contrainte,element->contrainte);
i=recherche_In_Struct(element->att,link->listeAt);
if(i){
printf("\a ce champs existe deja !!");
return 0;

}
///INSERTION DE LA NOUVELLE COLONNE DANS LA LISTE DES ELEMENTS DE L'ELEMENT
insertStruct(link->listeAt,element);
courTup=link->listeAll->premier;
while(courTup!=NULL){

///INSERTION DE LA NOUVELLE DONNEE DANS TOUTES LES DONNEES
insertTuple(courTup,donnee);int is_integer(char *chaine)
{
    int n=0,i=0;
    n=strlen(chaine);
    for(i=0;i<n;i++)
    {
        if(!isdigit(chaine[i]))///TEST CARACTERE PAR CARACTERE
        {
            return 0;
        }
    }
    return 1;
}

courTup=courTup->suivant;
}

//APPEL DE LA FONCTION VERIFIANT QUE S'IL EXISTE UNE CLAUSE FOREIGN KEY LA TABLE REFERENCEE DOIT EXISTER
ok1=gereContrainteForeign2(element->contrainte);
if(((ok1[0]=='o' && ok1[1]=='o') && ok2==1) || (ok1[2]=='n'&& ok2==1)){
fichier_cop=fopen("copie.xml","w");

///SAUVEGARDE DES DONNEES
save_data(fichier_cop,link->listeAt,link->listeAll,nom_table);
fclose(fichier_cop);
///LIBERATION DE LA MEMOIRE UTILISEE
free(link->listeAll);
free(link->listeAt);
remove(name);
rename("copie.xml",name);
printf(" Le schema de la table a ete modifiee !!");
}
else {
if(ok1[0]=='n'){
printf("\nLa table reférencée n'existe pas !!");
}
else {
if(ok1[1]=='n' && ok1[2]=='o') printf("\nL'attribut %s reférencé n'existe pas dans la table reférencée !!",extractAttribRef(element->contrainte));
}
}
}

///FONCTION D'AFFICHAGE D'UN TUPLE DONT LA LISTE DES CHAMPS A AFFICHER EST SPECIFIEE

void afficheTuple_in_req(ListeTuple *liste,char *colonne){

Donnee *cour=NULL;
char *pt=NULL;
char **aff=NULL;
int k=0,n=0,i=0;


cour=liste->premier;
///EXTRACTION DES CHAMPS A AFFICHER
aff=extract_by_virgule(colonne);

while(cour!=NULL){
k=0;
///AFFICHAGE DE LA VALEUR DE LA DONNEE
while(aff[k]!=NULL)
{
if(ifExist(aff[k],cour->element.att)&&(strlen(aff[k])==strlen(cour->element.att)))
{
printf("| %s ",cour->data);
n=strlen(cour->data);

for(i=0;i<23-n;i++)
printf(" ");

}
k++;
}
  
 cour=cour->suivant;
}

}

///affiche tous les tuples de la base donnee dont la liste des champs a afficher est specifiee
void afficheAll_in_req(ListeAllTuple *liste,char *colonne){

ListeTuple *cour=NULL;
int i=0,j=0,k=0,nb=0;
cour=liste->premier;
if(liste->taille==0){
printf("\n\aAucune donnee trouvee !!\n");
}
else{
///parcour la liste globale
while(cour!=NULL){


////affichage tuple par tuple
   afficheTuple_in_req(cour,colonne);
 printf("\n");
nb++;
   k=compter_champ(colonne)+1;
for(i=0;i<k;i++)
{
for(j=0;j<23;j++)
printf("_");
}
printf("\n");
   cour=cour->suivant;

    }
printf("\n%d ligne(s) trouvée(s) !!\n",nb);
}
    }




///LA REQUETE SELECT AVEC LES CHAMPS A AFFICHER

int  requete_select(char *nom_table,char *colonne){
data_link *link=NULL;
FILE *fichier=NULL;
char *name=NULL;
char **aff=NULL;
int i,k=0,n,j;
name=generer_nom(nom_table);
fichier=fopen(name,"r");
if(fichier==NULL){
printf("\a");
printf("table inexistante !!");
return 0;
}
aff=extract_by_virgule(colonne);

///AFFICHAGE DE L'ENTETE
while(aff[k]!=NULL)
{
printf("| %s ",aff[k]);
n=strlen(aff[k]);
for(i=0;i<23-n;i++)
printf(" ");
k++;
}
printf("\n");
//TRACE UNE LIGNE
for(i=0;i<k;i++)
{
for(j=0;j<23;j++)
printf("_");
}
printf("\n");
///CHARGEMENT DES DONNEES
link=load_data(fichier);
fclose(fichier);
///AFFICHAGE DE TOUTES LES DONNEES
afficheAll_in_req(link->listeAll,colonne);
///LIBERATION DE LA MEMOIRE ALLOUEE
free(link->listeAll);
free(link->listeAt);



}

/// FONCTION POUR AFFICHER TOUTES LES DONNEES D'UN TUPLE

void afficheTuple_in_tuple(ListeTuple *liste){
int n=0,i=0;
Donnee *cour=NULL;
cour=liste->premier;
while(cour!=NULL){
printf("| %s ",cour->data);
n=strlen(cour->data);
for(i=0;i<23-n;i++)
printf(" ");
 cour=cour->suivant;
}
//free(liste);
printf("\n");
}

///affiche tous les tuples de la base donnee
void afficheAll_in_all(ListeAllTuple *liste){
int i,n,j,nb=0;
ListeTuple *cour=NULL;

cour=liste->premier;
if(liste->taille==0){
printf("\n\aAucune donnee trouvee !!\n");
}
else{
while(cour!=NULL){

   afficheTuple_in_tuple(cour);
   nb++;
n=cour->taille;
for(i=0;i<n;i++)
{
for(j=0;j<23;j++)
printf("_");
}
printf("\n");
  cour=cour->suivant;
    }
printf("\n%d ligne(s) trouvée(s) !!\n",nb);
}

    }

///permet d'afficher toutes les enregistrement d'une table

int requete_select_all(char *nom_table){

data_link *link=NULL;
FILE *fichier=NULL;
char *name=NULL;
Element *cour=NULL;
int i=0,n=0,j=0;

name=generer_nom(nom_table);
fichier=fopen(name,"r");
if(fichier==NULL){
printf("\a");
printf("table inexistante !!");
return 0;
}

//CHARGEMENT DE DONNEES
link=load_data(fichier);
fclose(fichier);

cour=link->listeAt->premier;
///AFFICHAGE DE L'ENTETE
while(cour!=NULL){
printf("| %s ",cour->att);
n=strlen(cour->att);
for(i=0;i<23-n;i++)
printf(" ");
cour=cour->suivant;
}

///TRACE UNE LIGNE
printf("\n");
n=link->listeAt->taille;
for(i=0;i<n;i++)
{
for(j=0;j<23;j++)
printf("_");
}
printf("\n");
//AFFICHAGE DE TOUTES LES DONNEES
afficheAll_in_all(link->listeAll);

///LIBERATION DE LA MEMOIRE ALLOUEE
free(link->listeAll);
free(link->listeAt);

}


///MODIFIER UN TUPLE SI LA COLONNE COLONN_CONDI EST EGALE A VALEUR CHERCHER
int modify_tuple_egal(char *colonn_condi,char *attrib_chang,char *valeur_cher,char *new_valeur,ListeTuple *liste){

Donnee *cour=NULL;
Donnee *cour2=NULL;
char *pt=NULL, *pt2=NULL;
char *res=NULL;
int h=0,i=0;
char *op=NULL;


cour=liste->premier;
///PARCOUR DE TOUTES LES DONNEES DU TUPLE
while(cour!=NULL){
///TEST SI LA DONNEE COURANTE VERIFIE LA CONDITION
pt=strstr(cour->element.att,colonn_condi);
res=strstr(cour->data,valeur_cher);
if(pt!=NULL&&res!=NULL&&(strlen(cour->data)==strlen(valeur_cher))&&(strlen(cour->element.att)==strlen(colonn_condi))){
   cour2=liste->premier;
////RECHERCHE DE LA COLONNE A MODIFIER
   while(cour2!=NULL){
pt2=strstr(cour2->element.att,attrib_chang);
if(pt2!=NULL&&(strlen(cour2->element.att)==strlen(attrib_chang)))
{
///MODIFICATION DE LA DONNEE
    strcpy(cour2->data,new_valeur);
h=1;
    break;
}
cour2=cour2->suivant;

   }


}
i++;
cour=cour->suivant;
if(h==1)
break;
}

if(h==1)
return 1;
else
return 0;
}

///MODIFIE LE TUPLE SI LA VALEUR DE LA COLONN_CONDI EST DIFFERANTE DE VALEUR_CHER

int modify_tuple_dif(char *colonn_condi,char *attrib_chang,char *valeur_cher,char *new_valeur,ListeTuple *liste){

Donnee *cour=NULL;
Donnee *cour2=NULL;
char *pt=NULL, *pt2=NULL;
char *res=NULL;
int h=0,i=0;


cour=liste->premier;

while(cour!=NULL){
pt=strstr(cour->element.att,colonn_condi);
res=strstr(cour->data,valeur_cher);
//TEST SI VALEUR DE LA COLONN_CONDI EST DIFFERENTE DE LA VALEUR SPECIFIEE
if(pt!=NULL&&(strlen(cour->element.att)==strlen(colonn_condi))&&(res==NULL||(res!=NULL&&(strlen(cour->data)!=strlen(valeur_cher))))){
   cour2=liste->premier;

///RECHERCHE ET MODIFICATION DE LA COLONNE A MODIFER
   while(cour2!=NULL){
pt2=strstr(cour2->element.att,attrib_chang);
if(pt2!=NULL&&(strlen(cour2->element.att)==strlen(attrib_chang)))
{
    strcpy(cour2->data,new_valeur);
h=1;
    break;
}
cour2=cour2->suivant;

   }


}
i++;
cour=cour->suivant;
if(h==1)
break;
}

if(h==1)
return 1;
else
return 0;
}


/////MODIFIE LE TUPLE SI LA VALEUR DE LA COLONN_CONDI EST SUPERIEURE A LA VALEUR_CHER
int modif_tuple_sup(char *colonn_condi,char *attrib_chang,char *valeur_cher,char *new_valeur,ListeTuple *liste)
{

Donnee *cour=NULL;
Donnee *cour2=NULL;
char *pt=NULL, *pt2=NULL;
int nbre=0,nbre_cond=0;
 cour=liste->premier;
 while(cour!=NULL)
 {

pt=strstr(colonn_condi,cour->element.att);
if(pt!=NULL&&(strlen(colonn_condi)==strlen(cour->element.att)))
{
///CONVERTION EN  ENTIER 
nbre=atoi(cour->data);
nbre_cond=atoi(valeur_cher);
//TEST SI LA CONDITION DE MODIFICATION EST VERIFIER
if(nbre>nbre_cond)
{
    cour2=liste->premier;
    while(cour2!=NULL)
    {
        pt2=strstr(attrib_chang,cour2->element.att);
        if(pt2!=NULL&&(strlen(attrib_chang)==strlen(cour2->element.att)))
        {
            strcpy(cour2->data,new_valeur);
            return 1;
        }
      cour2=cour2->suivant;
    }
}
}
cour=cour->suivant;
 }
return 0;
}


///MODIFIE LE TUPLE SI LA VALEUR DE LA COLONN_CONDI EST INFERIEURE A LA  VALEUR_CHER
int modif_tuple_inf(char *colonn_condi,char *attrib_chang,char *valeur_cher,char *new_valeur,ListeTuple *liste)
{

Donnee *cour=NULL;
Donnee *cour2=NULL;
char *pt=NULL, *pt2=NULL;
int nbre,nbre_cond;
 cour=liste->premier;
 while(cour!=NULL)
 {

pt=strstr(colonn_condi,cour->element.att);
if(pt!=NULL&&(strlen(colonn_condi)==strlen(cour->element.att)))
{
//:CONVERTION
nbre=atoi(cour->data);
nbre_cond=atoi(valeur_cher);
///TEST SI LA CONDITION DE MODIFICATION EST VERIFIEE
if(nbre<nbre_cond)
{
    cour2=liste->premier;
    while(cour2!=NULL)
    {
        pt2=strstr(attrib_chang,cour2->element.att);
        if(pt2!=NULL&&(strlen(attrib_chang)==strlen(cour2->element.att)))
        {
            strcpy(cour2->data,new_valeur);
            return 1;
        }
      cour2=cour2->suivant;
    }
}
}
cour=cour->suivant;
 }
return 0;
}


////requete de mise a jour d'un enregistrement

int requete_update(char *nom_table,char *colonn_condi,char *attrib_chang,char *valeur_cher,char *new_valeur,char *condi){
data_link *link=NULL;
FILE *fichier=NULL;
FILE *fichier_cop=NULL;
char *name=NULL;
char *pt=NULL;
char *test_type=NULL;
int n=0,k=0,ok=0;
ListeTuple *cour=NULL;

name=generer_nom(nom_table);
fichier=fopen(name,"r");
if(fichier==NULL){
printf("\a");
printf("table inexistante !!");
return 0;
}
ok=gereContraintePrimaireUpdate(fichier,attrib_chang,new_valeur,condi);
link=load_data(fichier);
fclose(fichier);


if(!recherche_In_Struct(attrib_chang,link->listeAt))
{
    printf("\aLe champ %s n'existe pas !!\n",attrib_chang);
    return 0;
}

if(!recherche_In_Struct(colonn_condi,link->listeAt))
{
    printf("\aLe champs %s n'existe pas !!\n",colonn_condi);
    return 0;
}

if(ifExist(condi,"<")||ifExist(condi,">"))
{
       test_type=get_type_struc(link->listeAt,colonn_condi);
       if(!ifExist(test_type,"integer")&&!ifExist(test_type,"INTEGER")&&!ifExist(test_type,"number")&&!ifExist(test_type,"NUMBER"))
        {
           printf("L'operateur \"%s\" ne peut s'appliquer sur la colonne \"%s\" !!\n",condi,colonn_condi);
           return 0;

        }
   if(!is_integer(valeur_cher))
      {
printf("Le type de \"%s\" incompatible avec l'operateur %s !!!",valeur_cher,condi);
return 0;
      }
   if(!is_integer(valeur_cher))
      {
printf("Le type de \"%s\" incompatible avec l'operateur %s !!!",valeur_cher,condi);
return 0;
      }

      
}

///TEST DE LA COMPATIBILITE DES DONNEES
 test_type=get_type_struc(link->listeAt,attrib_chang);
       if(ifExist(test_type,"integer")||ifExist(test_type,"INTEGER"))
        {
           
 if(!is_integer(new_valeur))
      {
printf("la valeur \"%s\"  est incompatible avec la  colonne %s !!!",new_valeur,attrib_chang);
return 0;
      }

        }


pt=strstr(condi,"=");
if(pt!=NULL&&(strlen(condi)==strlen(pt))){
cour=link->listeAll->premier;
///PARCOUR ET MODIFE LES TUPLE EN CAS D'EGALITÉ
while(cour!=NULL){
 test_type=get_type_struc(link->listeAt,attrib_chang);
       if(ifExist(test_type,"boolean")||ifExist(test_type,"BOOLEAN"))
        {
           
 if(!is_boolean(new_valeur))
      {
printf("la valeur \"%s\"  est incompatible avec la  colonne %s de type BOOLEAN:Veuilllez inserer 0 ou 1!!!",new_valeur,attrib_chang);
return 0;
      }

        }
if(ifExist(test_type,"text")||ifExist(test_type,"TEXT"))
{
if(!is_text(new_valeur)){
printf("la valeur \"%s\"  est incompatible avec la  colonne %s de type TEXT:Veuilllez inserer une chaine de caractère!!!",new_valeur,attrib_chang);
return 0;           
}
}
n=modify_tuple_egal(colonn_condi,attrib_chang,valeur_cher,new_valeur,cour);
cour=cour->suivant;
if(n)
k++;
}

}
else if(ifExist(condi,"!=")){

cour=link->listeAll->premier;
///RECHERCHE ET MODIFICATION EN CAS DE DIFFERENCE
while(cour!=NULL){

n=modify_tuple_dif(colonn_condi,attrib_chang,valeur_cher,new_valeur,cour);
cour=cour->suivant;
if(n)
k++;
}
}
///MODIFIE SI INFERIEURE
else if(ifExist(condi,"<"))
{
    cour=link->listeAll->premier;
    while(cour!=NULL)
    {
        n=modif_tuple_inf(colonn_condi,attrib_chang,valeur_cher,new_valeur,cour);
        cour=cour->suivant;
        if(n)
        k++;
    }
}
else
{
    cour=link->listeAll->premier;
    while(cour!=NULL)
    {
        n=modif_tuple_sup(colonn_condi,attrib_chang,valeur_cher,new_valeur,cour);
        cour=cour->suivant;
        if(n)
        k++;
    }
}
if(ok==1){
fichier_cop=fopen("copie.xml","w");

save_data(fichier_cop,link->listeAt,link->listeAll,nom_table);
fclose(fichier_cop);

///LIBERATION DE LA MEMOIRE UTILISEE
free(link->listeAll);
free(link->listeAt);
remove(name);
rename("copie.xml",name);
if(k) printf(" %d lignes ont etes mises à jour !!!",k);
else printf("Aucune ligne modifiée: Une valeur spécifiée est inexistante!!!");
}
else { 
if(ok==0) printf("Violation de la contrainte PRIMARY KEY!!!");
else printf("Seul l'opérateur = peut être appliqué dans la condition car l'attribut à modifier est la clé primaire");
}
}
///FONCTION PERMETTANT DE RENOMMER UNE TABLE

int rename_table(char * old_name ,char *new_name){
FILE *fichier=NULL;

char *name=NULL;
char *name2=NULL;

name=generer_nom(old_name);
fichier=fopen(name,"r");
if(fichier==NULL){
printf("\a");
printf("table inexistante !!");
return 0;
}
name2=generer_nom(new_name);

rename(name,name2);
printf(" La table a ete renommée avec succès !!!");
}



///suppression de table

int drop_table(char *nom_table)
{
 FILE *fichier=NULL;

char *name=NULL;


name=generer_nom(nom_table);
fichier=fopen(name,"r");
if(fichier==NULL){
printf("\a");
printf("table inexistante !!");
return 0;
}
remove(name);
printf(" La table a ete supprimée avec succès !!!");
}


///permet de supprimer toutes les donnees d'une table
int truncate(char *nom_table){
data_link *link=NULL;
FILE *fichier=NULL;
FILE *fichier_cop=NULL;
char *name=NULL;
int n;
ListeTuple *cour=NULL;

name=generer_nom(nom_table);
fichier=fopen(name,"r");
if(fichier==NULL){
printf("\a");
printf("table inexistante !!");
return 0;
}

//CHARGEMENT DES DONNEES
link=load_data(fichier);
fclose(fichier);

link->listeAll->taille=0;
link->listeAll->premier=NULL;

fichier_cop=fopen("copie.xml","w");

///SAUVEGARDE DES DONNEES
save_data(fichier_cop,link->listeAt,link->listeAll,nom_table);
fclose(fichier_cop);
///LIBERATION DE LA MEMOIRE UTILISEE
free(link->listeAll);
free(link->listeAt);
remove(name);
rename("copie.xml",name);
printf(" table tronquee !!!");
}

////LE MANUEL D'AIDE
void  help(char *choix)
{
if(ifExist(choix,"create")||ifExist(choix,"CREATE")){

printf("*********************  manuel de create table ****************************\n\n");
printf("Pour creer une table on tape la commande suivante\n\n");
printf("create table nom_table (attribut1 type1 contraint1,attribut2 type2, contrainte2...,attributn typeN,contrainteN);\n\n");
printf("Les types possibles sont:\n");
printf("text\n");
printf("integer\n");
printf("number\n");
printf("boolean\n");
printf("NB: Les types sont insensibles à la casse\n\n");
printf("***************************************************************************\n\n");
}
else if(ifExist(choix,"select")||ifExist(choix,"SELECT")){

printf("*********************  manuel de select ****************************\n\n");
printf("pour faire une selection on tape la commande suivante\n\n");
printf("pour selectionner toutes les donnes d'une table\n");
printf("select * from nom_table;\n\n");
printf("pour selectionner toutes les donnes d'une table qui verifie une condition donnee\n");
printf("select * from nom_table where condition;\n\n");
printf("pour selectionner toutes les donnes d'une table en specifiant la liste des champs\n");
printf("select champ1,champ2,..,champN from nom_table;\n\n");
printf("ou\n");
printf("select champ1,champ2,..,champN  from nom_table where condition;\n\n");
printf("pour la condition on a l'egalite (=) , different (!=) ,superieur (>) et inferieur (<) \\nn");
printf("Exemples :\n");
printf("select * from test;\n\n");
printf("select * from test where nom='LOUE';\n\n");
printf("select nom,age from test;\n\n");
printf("select nom from test where age<20;\n\n");
printf("********************************************************************\n\n");

}
else if (ifExist(choix,"insert")||ifExist(choix,"INSERT")){

printf("*********************  manuel de insert ****************************\n\n");
printf("pour inserer une ligne dans une table on tape la commande suivante\n\n");
printf("insert into nom_table values (valeur1,valeur2,...,valeurN);\n\n");
printf("NB: \n");
printf("vous devez renseigner tous les champs de la table\n\n");

printf("***************************************************************************\n\n");


}
else if (ifExist(choix,"alter")||ifExist(choix,"ALTER")){

printf("*********************  manuel de alter ****************************\n\n");
printf("pour modifier le schema une table on tape la commande suivante:\n\n");
printf("si c'est un ajout de champs on a:\n");
printf("alter table nom_table add nouveau_champ type ;\n");
printf("dans ce cas toutes les tuple auront ce champ a null.\n\n");
printf("si c'est une suppression de champs on a:\n");
printf("alter table nom_table drop nom_champ;\n");
printf("***************************************************************************\n\n");
}
else if (ifExist(choix,"delete")||ifExist(choix,"DELETE")){

printf("*********************  manuel de delete ****************************\n\n");
printf("pour supprimer un enregistrement on tape la commande suivante:\n\n");
printf("delete from nom_table where condition;\n\n");
printf("pour la condition on a l'egalite (=) , different (!=) ,superieur (>) et inferieur (<) \n");
printf("exemple :\n");
printf("delete from personne where age=23;\n");
printf("delete from personne where age<30;\n");
printf("delete from personne where nom!='LOUE';\n");
printf("***************************************************************************\n\n");


}
else if (ifExist(choix,"update")||ifExist(choix,"UPDATE")){
printf("*********************  manuel de update *********************************************\n\n");
printf("pour mettre a jour un ou un ensemble d'enregistrement on tape la commande suivante:\n\n");
printf("update nom_table set nom_champ_a_modifier=valeur where condition;\n\n");
printf("pour la condition on a l'egalite (=) et different (!=)\n");
printf("exemple :\n");
printf("update personne set nom='LOUE' where age!=20 ;\n");
printf("************************************************************************************\n\n");



}

else if (ifExist(choix,"drop")||ifExist(choix,"DROP")){

printf("*********************  manuel de drop ****************************\n\n");
printf("pour supprimer une table  on tape la commande suivante:\n\n");
printf("drop table nom_table;\n\n");
printf("***************************************************************************\n\n");


}
else if (ifExist(choix,"rename")||ifExist(choix,"RENAME")){

printf("*********************  manuel de rename ***********************************\n\n");
printf("pour renommer une table  on tape la commande suivante:\n\n");
printf("rename ancien_nom to nouveau_nom ;\n\n");
printf("***************************************************************************\n\n");



}
else if (ifExist(choix,"desc")||ifExist(choix,"DESC")){

printf("*********************  manuel de desc *************************************\n\n");
printf("pour connaitre la structure d'une table on tape la commande suivante:\n\n");
printf("desc nom_table;\n\n");
printf("***************************************************************************\n\n");



}
else if (ifExist(choix,"join")||ifExist(choix,"JOIN")){

printf("*********************  manuel de la jointure  *************************************\n\n");
printf("pour effectuer une jointure sur deux tables on tape la commande suivante:\n\n");
printf("select table1.attr1,table2.attr1  from table1 join table2 on table1.attr=table2.attr ;\n\n");
printf("exemple :\n\n");
printf("select animal.nom_ani,personne.nom from personne join animal on persone.nom=animal.nom_prop;\n");
printf("pour effectuer une jointure sur trois tables on tape la commande suivante:\n\n");
printf("select table1.attribut1,table1.attribut2,...,table1.attributN,table2.attribut1...table2.attributN,table3.attribut1...,table3.attributN from"); 
printf("table1,table2,table3 WHERE table1.attributN=table2.attributN and table1.attributN=table3.attributN;\n\n");

printf("***************************************************************************\n\n");



}

else if (ifExist(choix,"help")||ifExist(choix,"HELP")){

printf("La help permet d'avoir l'aide sur l'utilisation d'une commande\n\n");
printf("***************************************************************************\n\n");



}

else{

printf("commande inexistante !!!\n\n");

printf("la liste des commandes disponibles est  :\n");
printf("create \n update \n delete\n select \n join \n insert \n alter \n rename \n truncate \n drop\n desc\n");
}

}



/// OBTENIR L'ENSEMBLE DES TUPLE DES TUPLES QUI VERIFIE UNE CONDITION 

ListeAllTuple *get_liste_tuple(ListeAllTuple *liste,char *colonne,char *valeur,char *comp)
{
    ListeAllTuple *resultat=NULL;
    ListeTuple *cour=NULL;
    ListeTuple *suiv=NULL;
    char *data=NULL;

    char *verif;
    int nombre,nb_condi,k=0,h=0;
    resultat=initialiseAllTuple();

///SI C'EST UNE CONDITION DE DIFFERENCE
    if(ifExist(comp,"!="))
    {

    cour=liste->premier;
    while(cour!=NULL)
    {

      data=get_tuple_value(cour,colonne);
      verif=strstr(data,valeur);


      if((verif==NULL)||(verif!=NULL&&(strlen(data)!=strlen(valeur))))
      {

          suiv=cour->suivant;
          h=1;
        insertAllTuple(resultat,cour);

        k++;
      }
      if(h==1){
      cour=suiv;
      h=0;
      }
      else
      cour=cour->suivant;
    }
  return resultat;
    }
///CONDITION D'EGALITÉ
    else if(ifExist(comp,"="))
    {

        cour=liste->premier;
        while(cour!=NULL)
        {
            data=get_tuple_value(cour,colonne);
            verif=strstr(data,valeur);
            if(verif!=NULL&&(strlen(data)==strlen(valeur)))
            {
                 suiv=cour->suivant;
                   h=1;
        insertAllTuple(resultat,cour);
        k++;
            }
           if(h==1){
      cour=suiv;
      h=0;
      }
      else
      cour=cour->suivant;
        }
    }
//CONDITION INFERIEUR
    else if(ifExist(comp,"<"))
    {
        cour=liste->premier;
        while(cour!=NULL)
        {
            data=get_tuple_value(cour,colonne);
            nombre=atoi(data);
            nb_condi=atoi(valeur);
            if(nombre<nb_condi)
            {
                suiv=cour->suivant;
                h=1;
        insertAllTuple(resultat,cour);
        k++;
            }
            if(h==1){
      cour=suiv;
      h=0;
      }
      else
      cour=cour->suivant;
        }
    }
///CONDITION SUPERIEURE
    else
    {
        cour=liste->premier;
        while(cour!=NULL)
        {
            data=get_tuple_value(cour,colonne);
            nombre=atoi(data);
            nb_condi=atoi(valeur);
            if(nombre>nb_condi)
            {
               suiv=cour->suivant;
               h=1;
        insertAllTuple(resultat,cour);
        k++;
            }
            if(h==1){
      cour=suiv;
      h=0;
      }
      else
      cour=cour->suivant;
        }

    }
return resultat;
}

/// AFFICHE TOUTE LES DONNEES D'UNE TABLE QUI VERIFIE UNE CONDITION 
int select_where(char *nom_table,char *colonne,char *valeur,char *comp)
{
    data_link *link=NULL;
FILE *fichier=NULL;
ListeAllTuple *resultat=NULL;
char *name=NULL;
char *pt=NULL;
Element *cour=NULL;
int n=0,i=0,j=0,nb=0;

name=generer_nom(nom_table);
fichier=fopen(name,"r");
if(fichier==NULL){
printf("\a");
printf("table inexistant !!");
getchar();
return 0;
}
///CHARGEMENT DE DONNEES
link=load_data(fichier);
fclose(fichier);
if(!recherche_In_Struct(colonne,link->listeAt))
 {
   printf("champs %s n'existe pas !!",colonne);
   return 0;
  }
if(ifExist(comp,"<")||ifExist(comp,">"))
{
    pt=get_type_struc(link->listeAt,colonne);

    if(!ifExist(pt,"INTEGER")&&(!ifExist(pt,"integer")&&!ifExist(pt,"number")&&!ifExist(pt,"NUMBER")))
    {
            printf("le type de %s est :%s\n",colonne,pt);
        printf("Les operateurs < ou > ne s'applique que sur une colonne de type integer ou number");
        return 0;
    }
}
///OBTENIR LA LISTE DES TUPLES VERIFIANT  LA CONDITION
resultat=get_liste_tuple(link->listeAll,colonne,valeur,comp);
cour=link->listeAt->premier;
///AFFICHAGE DES TUPLES 
while(cour!=NULL){
printf("| %s ",cour->att);
n=strlen(cour->att);
for(i=0;i<23-n;i++)
printf(" ");
cour=cour->suivant;
}

printf("\n");
n=link->listeAt->taille;
for(i=0;i<n;i++)
{
for(j=0;j<23;j++)
printf("_");
}
printf("\n");
afficheAll_in_all(resultat);
return 1;
}


////AFFICHAGE DES DONNEES D'UN TUPLE PAR RAPPORT A UNE LISTE DE CHAMPS
void afficheTuple_in_tuple_list(ListeTuple *liste,char *list_aff){
int n=0,i=0,h=0;
Donnee *cour=NULL;
cour=liste->premier;
char **aff=NULL;
aff=extract_by_virgule(list_aff);
while(cour!=NULL){

    i=0;
    while(aff[i]!=NULL)
    {
        if(ifExist(cour->element.att,aff[i])&&strlen(cour->element.att)==strlen(aff[i]))
        {
            h=1;
            break;
        }
        i++;
    }
 if(h){
printf("| %s ",cour->data);
n=strlen(cour->data);
for(i=0;i<23-n;i++)
printf(" ");
h=0;
 }
 cour=cour->suivant;
}
//free(liste);
printf("\n");
}

///affiche tous les tuples de la base donnee PAR rapport a une liste de champs donnee
void afficheAll_in_all_list(ListeAllTuple *liste,char *list_aff){
int i=0,n=0,j=0;
ListeTuple *cour=NULL;

cour=liste->premier;
if(liste->taille==0){
printf("\n\aAucune donnee trouvee !!\n");
}
else{
while(cour!=NULL){

   afficheTuple_in_tuple_list(cour,list_aff);

n=cour->taille;
for(i=0;i<n;i++)
{
for(j=0;j<23;j++)
printf("_");
}
printf("\n");
  cour=cour->suivant;
    }

}
    }


///SELECT  * AVEC LISTE DE CHAMP A AFFICHER
int select_where_liste(char *nom_table,char *col_af,char *colonne,char *valeur,char *comp)
{
    data_link *link=NULL;
FILE *fichier=NULL;
ListeAllTuple *resultat=NULL;
char *name=NULL;
char **liste=NULL;
char *pt=NULL;
Element *cour=NULL;
int n,i,j,h=0;

name=generer_nom(nom_table);
fichier=fopen(name,"r");
if(fichier==NULL){
printf("\a");
printf("table inexistant !!");
getchar();
return 0;
}
///chargement des donnees
link=load_data(fichier);
fclose(fichier);

if(ifExist(comp,"<")||ifExist(comp,">"))
{
    pt=get_type_struc(link->listeAt,colonne);
    printf("le type trouver est :%s\n",pt);
    if(!ifExist("INTEGER",pt)&&(!ifExist("integer",pt)&&!ifExist("NUMBER",pt)&&!ifExist("number",pt)))
    {

        printf("Les operateurs < ou > ne s'applique que sur une colonne de type integer ou number");
        return 0;
    }
}

liste=extract_by_virgule(col_af);

resultat=get_liste_tuple(link->listeAll,colonne,valeur,comp);
cour=link->listeAt->premier;
while(cour!=NULL){
    i=0;
    while(liste[i]!=NULL)
    {
        if(ifExist(cour->att,liste[i])&&strlen(cour->att)==strlen(liste[i]))
        {
            h=1;
            break;
        }
        i++;
    }
 if(h){
printf("| %s ",cour->att);
n=strlen(cour->att);
for(i=0;i<23-n;i++)
printf(" ");
h=0;
}
cour=cour->suivant;
}

printf("\n");
n=link->listeAt->taille;
for(i=0;i<n;i++)
{
for(j=0;j<23;j++)
printf("_");
}
printf("\n");
afficheAll_in_all_list(resultat,col_af);
return 1;
}


//// extraction du nom de la table et le nom de la colonne a afficher
Element *extract_list_aff_ele(char *chaine)
{
   Element *element=NULL;
   char **tab=NULL;
   element=(Element *)malloc(sizeof(Element));
   tab=extract_by_point(chaine);
   strcpy(element->att,tab[0]);
   strcpy(element->typ,tab[1]);
   return element;
}

///extraction de toutes les colonnes a afficher des deux tables
ListeAtt *extract_liste_aff(char *chaine)
{
    ListeAtt *liste=NULL;
    char **tab=NULL;
    Element **ele=NULL;
    int k=0;
    tab=extract_by_virgule(chaine);
    ele=(Element **)malloc(10*sizeof(Element *));
    liste=initialise();
    while(tab[k]!=NULL)
    {
     ele[k]=extract_list_aff_ele(tab[k]);
     insertStruct(liste,ele[k]);
     k++;

    }
return liste;
}


////permet d'obtenir pour un tuple donnee l'ensemble des tuples de l'autre table qui verifie la condition de jointure
ListeAllTuple *get_join_tuple(ListeTuple *tuple,char *colon,char *colon_rech,ListeAllTuple *liste)
{
    ListeAllTuple *resul=NULL;
    ListeTuple *cour=NULL,*suiv=NULL;
    char *valeur=NULL;
    char *valeur_cher=NULL;
    char *pt=NULL;


    valeur=get_tuple_value(tuple,colon);
    cour=liste->premier;
    resul=initialiseAllTuple();
//recherche des tuples qui verifie la condition de jointure
    while(cour!=NULL)
    {
     valeur_cher=get_tuple_value(cour,colon_rech);
     if(valeur_cher!=NULL){
       pt=strstr(valeur,valeur_cher);
      if(pt!=NULL&&(strlen(valeur)==strlen(valeur_cher)))
      {
          suiv=cour->suivant;
       insertAllTuple(resul,cour);
       cour=suiv;
       continue;
      }
   cour=cour->suivant;
}
else {
printf("\n Erreur: Un champ spécifié n'existe pas");
break;
}

    }
    return resul;
}
//FONCTION QUI RECHERCHE LES TUPLES A AFFICHER:JOINTURE MULTIPLE
ListeAllTuple *get_join_tuple_multi(ListeTuple *tuple,char *colon,char *colon_rech,ListeAllTuple *liste)
{
    ListeAllTuple *resul=NULL;
    ListeTuple *cour=NULL,*suiv=NULL;
    char *valeur=NULL;
    char *valeur_cher=NULL;
    char *pt=NULL;


    valeur=get_tuple_value(tuple,colon);
    cour=liste->premier;
    resul=initialiseAllTuple();
//recherche des tuples qui verifie la condition de jointure
    while(cour!=NULL)
    {
     valeur_cher=get_tuple_value(cour,colon_rech);
     if(valeur_cher!=NULL){
       pt=strstr(valeur,valeur_cher);
      if(pt!=NULL&&(strlen(valeur)==strlen(valeur_cher)))
      {
          suiv=cour->suivant;
       insertAllTuple(resul,cour);
       cour=suiv;
       continue;
      }
   cour=cour->suivant;
}
else {
printf("\n Erreur: Un champ spécifié n'existe pas");
break;
}

    }
    return resul;
}

////affichage d'une ligne resultante de la requete de jointure

void affiche_join_tuple(ListeAtt *att,ListeTuple *liste1,ListeTuple *liste2,char *table1,char * table2)
{
Donnee *cour1=NULL,*cour2=NULL;
Element *ele=NULL;
int h=0,i=0,n=0,f=0;
cour1=liste1->premier;
while(cour1!=NULL)
{
ele=att->premier;
while(ele!=NULL)
{
    if(ifExist(ele->att,table1)&&ifExist(ele->typ,cour1->element.att)&&strlen(ele->typ)==strlen(cour1->element.att)&&strlen(ele->att)==strlen(table1))
    {
        h=1;
        break;
    }
    ele=ele->suivant;
}



if(h){
printf("| %s ",cour1->data);
n=strlen(cour1->data);
for(i=0;i<23-n;i++)
printf(" ");
h=0;
}

cour1=cour1->suivant;
}

cour2=liste2->premier;
while(cour2!=NULL)
{
ele=att->premier;
while(ele!=NULL)
{
  if(ifExist(ele->att,table2)&&ifExist(ele->typ,cour2->element.att)&&strlen(ele->att)==strlen(table2)&&strlen(ele->typ)==strlen(cour2->element.att))
    {
        f=1;
        break;
    }
    ele=ele->suivant;
}



if(f){
printf("| %s ",cour2->data);
n=strlen(cour2->data);
for(i=0;i<23-n;i++)
printf(" ");
f=0;
}

cour2=cour2->suivant;
}
printf("\n");
}



////AFFICHAGE D'UNE LIGNE RESULTANTE DE LA REQUETE DE JOINTURE MULTIPLE

void affiche_join_tuple_multi(ListeAtt *att,ListeTuple *liste1,ListeTuple *liste2,ListeTuple *liste3,char **table)
{
Donnee *cour1=NULL,*cour2=NULL,*cour3=NULL;
Element *ele=NULL;
cour3=(Donnee *)malloc(sizeof(Donnee));
int h=0,i=0,n=0,f=0,g=0;
cour1=liste1->premier;
while(cour1!=NULL)
{
ele=att->premier;
while(ele!=NULL)
{
    if(ifExist(ele->att,table[0])&&ifExist(ele->typ,cour1->element.att)&&strlen(ele->typ)==strlen(cour1->element.att)&&strlen(ele->att)==strlen(table[0]))
    {
        h=1;
        break;
    }
    ele=ele->suivant;
}



if(h){
printf("| %s ",cour1->data);
n=strlen(cour1->data);
for(i=0;i<23-n;i++)
printf(" ");
h=0;
}

cour1=cour1->suivant;
}

cour2=liste2->premier;
while(cour2!=NULL)
{
ele=att->premier;
while(ele!=NULL)
{
  if(ifExist(ele->att,table[1])&&ifExist(ele->typ,cour2->element.att)&&strlen(ele->att)==strlen(table[1])&&strlen(ele->typ)==strlen(cour2->element.att))
    {
        f=1;
        break;
    }
    ele=ele->suivant;
}


if(f){
printf("| %s ",cour2->data);
n=strlen(cour2->data);
for(i=0;i<23-n;i++)
printf(" ");
f=0;
}
cour2=cour2->suivant;
}
cour3=liste3->premier;
while(cour3!=NULL)
{
ele=att->premier;
while(ele!=NULL)
{
    if(ifExist(ele->att,table[2])&&ifExist(ele->typ,cour3->element.att)&&strlen(ele->att)==strlen(table[2])&&strlen(ele->typ)==strlen(cour3->element.att))
    {
        g=1;
        break;
    }
    ele=ele->suivant;
}

if(g){
printf("| %s ",cour3->data);
n=strlen(cour3->data);
for(i=0;i<23-n;i++)
printf(" ");
g=0;
}
cour3=cour3->suivant;
}
printf("\n");
}

///requete de jointure simple
int requete_join(char *nom_table1,char * nom_table2,char *liste_chaine_aff,char *listecolonn1,char *listecolonn2)
{
Element *courant=NULL;
data_link *link1=NULL,*link2=NULL;
FILE *fichier1=NULL,*fichier2=NULL;
char *name1=NULL,*name2=NULL;
ListeAtt *listeatt=NULL;
char *colon_condi1=NULL,*colon_condi2=NULL;
char **tab1=NULL;
char **tab2=NULL;
ListeTuple *cour1=NULL,*cour2=NULL;
ListeAllTuple **result=NULL;
int k=0,i=0,n=0,j=0;

name1=generer_nom(nom_table1);
fichier1=fopen(name1,"r");
name2=generer_nom(nom_table2);
fichier2=fopen(name2,"r");
if(fichier1==NULL||fichier2==NULL){
printf("\a");
printf("table(s) inexistante(s) !!");
getchar();
return 0;
}

result=(ListeAllTuple **)malloc(5*sizeof(ListeAllTuple *));


listeatt=extract_liste_aff(liste_chaine_aff);

tab1=extract_by_point(listecolonn1);

colon_condi1=tab1[1];
tab2=extract_by_point(listecolonn2);
colon_condi2=tab2[1];
///chargement des donnees
link1=load_data(fichier1);
link2=load_data(fichier2);
cour1=link1->listeAll->premier;

printf("\n");
///tracer une ligne
n=listeatt->taille;
for(i=0;i<n;i++)
{
for(j=0;j<23;j++)
printf("_");
}
printf("\n");

///affichage de l'entete
printf("\n");

courant=listeatt->premier;
while(courant!=NULL)
{
    printf("| %s.%s",courant->att,courant->typ);

n=strlen(courant->att)+strlen(courant->typ);
for(i=0;i<23-n;i++)
printf(" ");
courant=courant->suivant;
}

printf("\n");

///tracer une ligne
n=listeatt->taille;
for(i=0;i<n;i++)
{
for(j=0;j<23;j++)
printf("_");
}
printf("\n");

while(cour1!=NULL)
{


result[k]=get_join_tuple(cour1,colon_condi1,colon_condi2,link2->listeAll);



cour2=result[k]->premier;
while(cour2!=NULL)
{
 affiche_join_tuple(listeatt,cour1,cour2,nom_table1,nom_table2);

n=listeatt->taille;
for(i=0;i<n;i++)
{
for(j=0;j<23;j++)
printf("_");
}
printf("\n");
 cour2=cour2->suivant;
}
k++;
//liberation de la memoire allouer
free(link1->listeAll);
free(link1->listeAt);
free(link2->listeAll);
free(link2->listeAt);
///chargement des donnees
link1=load_data(fichier1);
link2=load_data(fichier2);
cour1=cour1->suivant;
}
}

// FONCTION PERMETTANT DE FAIRE UNE JOINTURE MULTIPLE
int requete_multiple_join(char *liste_chaine_aff,char *table_names,char *listecolonn1,char *listecolonn2,char *listecolonn3,char *listecolonn4)
{
int i=0;
Element *courant=NULL;
data_link *link1=NULL,*link2=NULL,*link3=NULL,*link4=NULL;
FILE *fichier1=NULL,*fichier2=NULL,*fichier3=NULL;
char *name1=NULL,*name2=NULL, *name3=NULL;
char **nom_table=NULL;
ListeAtt *listeatt=NULL;
nom_table=(char**)malloc(4*sizeof(char*));
for(i=0;i<3;i++){
nom_table[i]=NULL;
nom_table[i]=(char *)malloc(TAILLE_DONNEE*sizeof(char));
}
char *colon_condi1=NULL,*colon_condi2=NULL,*colon_condi3=NULL,*colon_condi4=NULL;
char **tab1=NULL;
char **tab2=NULL;
char **tab3=NULL;
char **tab4=NULL;
ListeTuple *cour1=NULL,*cour2=NULL,*cour3=NULL,*cour4=NULL;
ListeAllTuple **result1=NULL;
ListeAllTuple **result2=NULL;
int k=0,n=0,j=0;
nom_table=extract_by_virgule(table_names);
name1=generer_nom(nom_table[0]);
fichier1=fopen(name1,"r");

name2=generer_nom(nom_table[1]);
fichier2=fopen(name2,"r");

name3=generer_nom(nom_table[2]);
fichier3=fopen(name3,"r");

if(fichier1==NULL||fichier2==NULL||fichier3==NULL){
printf("\a");
printf("table(s) inexistante(s) !!");
getchar();
return 0;
}

result1=(ListeAllTuple **)malloc(5*sizeof(ListeAllTuple *));
result2=(ListeAllTuple **)malloc(5*sizeof(ListeAllTuple *));


listeatt=extract_liste_aff(liste_chaine_aff);

tab1=extract_by_point(listecolonn1);
colon_condi1=tab1[1];

tab2=extract_by_point(listecolonn2);
colon_condi2=tab2[1];

tab3=extract_by_point(listecolonn3);
colon_condi3=tab3[1];

tab4=extract_by_point(listecolonn4);
colon_condi4=tab4[1];


///chargement des donnees
link1=load_data(fichier1);
link2=load_data(fichier2);
link3=load_data(fichier3);
cour1=link1->listeAll->premier;

printf("\n");
///tracer une ligne
n=listeatt->taille;
for(i=0;i<n;i++)
{
for(j=0;j<23;j++)
printf("_");
}
printf("\n");

///affichage de l'en-tete
printf("\n");

courant=listeatt->premier;
while(courant!=NULL)
{
    printf("| %s.%s",courant->att,courant->typ);

n=strlen(courant->att)+strlen(courant->typ);
for(i=0;i<23-n;i++)
printf(" ");
courant=courant->suivant;
}

printf("\n");

///tracer une ligne
n=listeatt->taille;
for(i=0;i<n;i++)
{
for(j=0;j<23;j++)
printf("_");
}
printf("\n");

while(cour1!=NULL)
{


result1[k]=get_join_tuple_multi(cour1,colon_condi1,colon_condi2,link2->listeAll);
result2[k]=get_join_tuple_multi(cour1,colon_condi3,colon_condi4,link3->listeAll);



cour2=result1[k]->premier;
cour3=result2[k]->premier;
while(cour2!=NULL)
{
if(cour3==NULL) break;
affiche_join_tuple_multi(listeatt,cour1,cour2,cour3,nom_table);
n=listeatt->taille;
for(i=0;i<n;i++)
{
for(j=0;j<23;j++)
printf("_");
}
printf("\n");
cour2=cour2->suivant;
}
k++;
//liberation de la memoire allouer
free(link1->listeAll);
free(link1->listeAt);
free(link2->listeAll);
free(link2->listeAt);
///chargement des donnees
link1=load_data(fichier1);
link2=load_data(fichier2);
link3=load_data(fichier3);
cour1=cour1->suivant;
}
}

//La fonction qui verifie que la contrainte primary key porte sur un seul attribut

int verifContraintePrimaire(ListeAtt *liste){
Element *cour=NULL;
int i=0,n=0;
int nbre_Contrainte_Primaire=0;
n=liste->taille;
cour=(Element *)malloc(sizeof(Element));
cour=liste->premier;
for(i=0;i<n;i++){
if(ifExist(cour->contrainte,"primary")||ifExist(cour->contrainte,"PRIMARY")){
nbre_Contrainte_Primaire++;
}
cour=cour->suivant;
}
return nbre_Contrainte_Primaire;
free(cour);
}
// La fonction qui extrait le nom du champ portant une contrainte clé primaire 
char * extractAttribPrimaire(char * chaine){
char * attribPrimaire=NULL;
attribPrimaire=(char *) malloc(TAILLE_DONNEE*sizeof(char));
if(ifExist(chaine,"primary") || ifExist(chaine,"PRIMARY")){
attribPrimaire=extracAttrib(chaine);
return attribPrimaire;
}
else return NULL;
free(attribPrimaire);
}


// La fonction qui extrait d'un fichier le nom d'un champ portant une contrainte clé foreign key
char * extractAttribReference(char * chaine){
char * attribRef=NULL;
attribRef=(char *) malloc(TAILLE_DONNEE*sizeof(char));
if(ifExist(chaine,"primary") || ifExist(chaine,"PRIMARY")){
attribRef=extracAttrib(chaine);
return attribRef;
}
else return NULL;
free(attribRef);
}



// La fonction qui extrait la valeur inserée dans un attribut portant une contrainte clé primaire 
char * extractValPrimaire(char * ligne, char *attribPrimaire){
char *ch1=NULL;
const char *lign=extracAttrib(ligne);
const char *attribPrimair=attribPrimaire;
if(strcmp(lign,attribPrimair)==0){
int i=0,nb=0,h=0,k=0;

ch1=(char *)malloc(TAILLE_DONNEE*sizeof(char));
nb=strlen(ligne);
for(i=0;i<nb;i++){
if (ligne[i]=='>'&&h==0){
h=1;
continue;
}
if (h==1){
ch1[k]=ligne[i];
k++;
}
if (ligne[i]=='<'&&h==1){
    --k;
ch1[k]='\0';
break;
}

}
}
return ch1;
}

// La fonction qui extrait la valeur inserée dans un attribut portant une contrainte foreign key
char * extractValRef(char * ligne, char *attribRef){
char *ch1=NULL;
const char *lign=extracAttrib(ligne);
const char *attribRefe=attribRef;
if(strcmp(lign,attribRef)==0){
int i=0,nb=0,h=0,k=0;

ch1=(char *)malloc(TAILLE_DONNEE*sizeof(char));
nb=strlen(ligne);
for(i=0;i<nb;i++){
if (ligne[i]=='>'&&h==0){
h=1;
continue;
}
if (h==1){
ch1[k]=ligne[i];
k++;
}
if (ligne[i]=='<'&&h==1){
    --k;
ch1[k]='\0';
break;
}

}

}
return ch1;
}





//La fonction qui empêche d'inserrer des valeurs identiques dans une colonne portant une contrainte clé primaire avec INSERT

int gereContraintePrimaire(FILE *fichier,ListeTuple *liste){
char *buff=NULL;
Donnee *cour=NULL;
int n=0,i=0;
int nb=0,k=0;int verif=0;
int ok=1;
cour=liste->premier;
char *valPrimaire=NULL;//Variable contenant à chaque itération la valeur de l'attribut portant la contrainte clé primaire
char * attribPrimaire=NULL;
attribPrimaire=(char *) malloc(TAILLE_DONNEE*sizeof(char));
valPrimaire=(char *) malloc(TAILLE_DONNEE*sizeof(char));
char *ch=NULL;
char *suivant=NULL;
ch=(char*)malloc(TAILLE_DONNEE*sizeof(char));
suivant=(char*)malloc(TAILLE_DONNEE*sizeof(char));
ch=cour->data;
cour=cour->suivant;
suivant=cour->data;
cour=(Donnee *)malloc(sizeof(Donnee));
do {
nb=gotoPosition(fichier,"<struct>");///on se place a la position <struct> pour commencer a lire les donnees de l'entete
while(!ifExist(buff,"</table>")){ ///on lit dans le fichier jusqu'a croiser la chaine </table>
nb=LireLigne(fichier,&buff,10);
if(!verif){
attribPrimaire=extractAttribPrimaire(buff);
verif=1;
}
if(attribPrimaire!=NULL){
valPrimaire=extractValPrimaire(buff,attribPrimaire);
if(valPrimaire!=NULL){
if(strcmp(valPrimaire,ch)==0){
ok=0;

break;
}
else{
if(suivant==NULL){
//printf("\nUne valeur null detectée");
//cour=cour->suivant;
 break;
}
//printf("\n La valeur de suivant est: %s",suivant);
}
}
if(ifExist(buff,"</table>")){
break;
}
}
//cour=cour->suivant;
}
cour=cour->suivant;
}
while(cour!=NULL);
return ok;
free(attribPrimaire);
free(valPrimaire);
free(cour);
free(ch);

}

//Fonction qui pour une table qui contient un attribut foreign key empêche d'inserrer des valeurs qui ne sont pas dans la table réferencée 

int gereContrainteForeignInsertion(ListeTuple *liste){
FILE *fichier=NULL;
Element *elmt;
char *buff=NULL;
char *nom_table=NULL;
Donnee *cour=NULL;
int n=0,i=0;
int nb=0,k=0;int j=0,nbre=0,verif=1,exist_foreign=0;
int ok=0;
cour=liste->premier;
char *valRef=NULL;//Variable contenant à chaque itération la valeur de l'attribut portant la contrainte clé primaire
char *attribRef=NULL;
char *val_courant=NULL;
attribRef=(char *) malloc(TAILLE_DONNEE*sizeof(char));
valRef=(char *) malloc(TAILLE_DONNEE*sizeof(char));
char *chaine=NULL;
char *suivant=NULL;
chaine=(char*)malloc(TAILLE_DONNEE*sizeof(char));
chaine=(char*)malloc(TAILLE_DONNEE*sizeof(char));
val_courant=(char*)malloc(TAILLE_DONNEE*sizeof(char));
nom_table=(char*)malloc(TAILLE_DONNEE*sizeof(char));
cour=(Donnee *)malloc(sizeof(Donnee));
elmt=(Element *)malloc(sizeof(Element));
cour=liste->premier;
cour=cour->suivant;
suivant=cour->data;
//cour=(Donnee *)malloc(sizeof(Donnee));

do {
 //S'il existe une contrainte contrainte sur un champ, on récupère le nom le nom de la table référencée
  if(ifExist(cour->element.contrainte,"foreign") || ifExist(cour->element.contrainte,"FOREIGN")){
     exist_foreign=1;
    for(i=23;i<strlen(cour->element.contrainte);i++){
        chaine[j]=cour->element.contrainte[i];
        j++;
        if(cour->element.contrainte[i]=='('){
            chaine[--j]='\0';
            break;
        }
    }
strcpy(nom_table,chaine);
chaine=generer_nom(chaine);
fichier=fopen(chaine,"r");

nb=gotoPosition(fichier,"<struct>");///on se place a la position <struct> pour commencer a lire les donnees de l'entete
while(!ifExist(buff,"</table>")){ ///on lit dans le fichier jusqu'a croiser la chaine </table>
nb=LireLigne(fichier,&buff,10);
if(verif) attribRef=extractAttribReference(buff);//On extrait l'attribut référencé une seule fois
if(attribRef!=NULL){
verif=0;
if(ifExist(buff,attribRef)&&!ifExist(buff,"primary key")){
valRef=extractValRef(buff,attribRef);
if(valRef!=NULL){
if(strcmp(valRef,cour->data)==0){
ok=1;
break;
}
else{
if(suivant==NULL){
 break;
}
}
}
if(ifExist(buff,"</table>")){
break;
}
}
}
}
val_courant=cour->data;
cour=cour->suivant;
}
else {
val_courant=cour->data;
cour=cour->suivant;
}
}
while(cour!=NULL);
if(!exist_foreign) ok=1;
if(!ok) printf("La valeur %s n'existe pas dans l'attribut %s de la table référencée %s",val_courant,attribRef,nom_table);
return ok;
free(attribRef);
free(valRef);
free(cour);
free(chaine);
flose(fichier);
free(nom_table);
free(val_courant);
}




//La fonction qui empêche d'inserrer des valeurs identiques dans une colonne portant une contrainte clé primaire avec UPDATE

int gereContraintePrimaireUpdate(FILE *fichier,char *champ_chang,char *new_valeur,char *op_rel){
char *buff=NULL;
int n=0,i=0;
int nb=0,k=0;int verif=0;
int ok=1;
char *valPrimaire=NULL;//Variable contenant à chaque itération la valeur de l'attribut portant la contrainte clé primaire
char * attribPrimaire=NULL;
attribPrimaire=(char *) malloc(TAILLE_DONNEE*sizeof(char));
valPrimaire=(char *) malloc(TAILLE_DONNEE*sizeof(char));
char *ch=NULL;
ch=(char*)malloc(TAILLE_DONNEE*sizeof(char));

nb=gotoPosition(fichier,"<struct>");///on se place a la position <struct> pour commencer a lire les donnees de l'entete
while(!ifExist(buff,"</table>")){ ///on lit dans le fichier jusqu'a croiser la chaine </table>
nb=LireLigne(fichier,&buff,10);
if(!verif){
attribPrimaire=extractAttribPrimaire(buff);
verif=1;
}
if(attribPrimaire!=NULL){
if(strcmp(attribPrimaire,champ_chang)==0){
if(strcmp(op_rel,"=")==0){
valPrimaire=extractValPrimaire(buff,attribPrimaire);
if(valPrimaire!=NULL){
if(strcmp(valPrimaire,new_valeur)==0){
ok=0;
break;
}
}
}
else ok=-1;
}

}
if(ifExist(buff,"</table>")){
break;
}
}
return ok;
free(attribPrimaire);
free(valPrimaire);
}







int verifIfExistPrimaire(FILE * fichier){
int n=0,i=0;
int nb=0,k=0;int verif=0;
int ok=1;
char *buff=NULL;
char *attribPrimaire=NULL;
attribPrimaire=(char*)malloc(TAILLE_DONNEE*sizeof(char));
nb=gotoPosition(fichier,"<struct>");///on se place a la position <struct> pour commencer a lire les donnees de l'entete
while(!ifExist(buff,"</struct>")){ ///on lit dans le fichier jusqu'a croiser la chaine </table>
nb=LireLigne(fichier,&buff,10);
if(ifExist(buff,"primary key")){
ok=0;
attribPrimaire=extracAttrib(buff);
printf("\nUn attribut nommé %s porte dejà une contrainte Primary Key",attribPrimaire);
break;
}
else ok=-1;
}
return ok;
}
//gere le fait qu'on ne puisse pas supprimer un champ portant une contrainte PRIMARY KEY
int gere_Non_Suppression_Champ_Primaire(FILE *fichier,char *champ_A_Supprimer){
int ok=1,nb=0;
char * attribPrimaire=NULL;
attribPrimaire=(char *) malloc(TAILLE_DONNEE*sizeof(char));
char * buff=NULL;
buff=(char *) malloc(TAILLE_DONNEE*sizeof(char));
nb=gotoPosition(fichier,"<struct>");///on se place a la position <struct> pour commencer a lire les donnees de l'entete
while(!ifExist(buff,"</table>")){ ///on lit dans le fichier jusqu'a croiser la chaine </table>
nb=LireLigne(fichier,&buff,10);
attribPrimaire=extractAttribPrimaire(buff);
if(attribPrimaire!=NULL){
if(strcmp(attribPrimaire,champ_A_Supprimer)==0) ok=0;
}

if(ifExist(buff,"</table>")) break;
}
return ok;
free(attribPrimaire);
}
