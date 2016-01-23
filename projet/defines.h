/*****************************************
*  INTERPRETEUR DE REQUETES SQL          *
*   LOUE Boureima Arnaud et              *
*   OUEDRAOGO PASCAL                     *
*                                        *
******************************************/
/*variables global*/

#ifndef SQL
# define SQL
#define  TAILLE_DONNEE 100  ///LA TAILLE D'UNE DONNEE
#define  NOMBRE_ELE   40   ///LE NOMBRE MAX D'ELEMENT A EXTRAIRE

/**************************************************/


////element constituant la liste de l'entete
typedef struct Element Element;

struct Element {

char att[TAILLE_DONNEE];///nom du champ
char typ[TAILLE_DONNEE];///type du champ
char contrainte[TAILLE_DONNEE];//contrainte
Element *suivant;//element suivant
};

////structure de la liste chainee des elements de l'entete
typedef struct ListeAtt ListeAtt;

struct ListeAtt{
    int taille;
Element *premier;//pointeur sur le premier element
Element *dernier;///pointeur sur le dernier element
};

///structure donnee contenant la donnee d'un tuple
typedef struct Donnee Donnee;
struct Donnee{
char data[TAILLE_DONNEE];/// contient la donnee
Element element; ///element contenant le nom de l'attribut du type et de la contrainte
Donnee *suivant; ///pointeur sur la donnee suivante
};


////liste chainee des donnees d'un tuple
typedef struct ListeTuple ListeTuple;

struct ListeTuple{
int taille;
Donnee *premier; //pointeur sur la premiere donnee
Donnee *dernier;///pointeur sur la derniere donnee
ListeTuple *suivant;///pointeur sur la tuple suivant
};


///liste chainee de tous les tuples
typedef struct ListeAllTuple ListeAllTuple;

struct ListeAllTuple{
    int taille;
ListeTuple *premier;
ListeTuple *dernier;

};


///permet de conserver la position d'un element ou d'une donnee ou d'une liste de donnee dans une liste
typedef struct Position Position;
struct Position{

int pos;
Element *element; ///le pointeur de l'element
Donnee *donnee;///le pointeur de la donnee
ListeTuple *liste; ///le pointeur sur la liste de donnee

};

////structure contenant les pointeur sur les donnees extraient d'un fichier
typedef struct data_link data_link;

struct data_link{

ListeAllTuple *listeAll; ///pointeur sur toutes les donnees
ListeAtt *listeAt;///pointeur sur la structure de l'entete

};

/***************************************************/
//partie de definition des prototypes des fonctions
//**************************************************/

///fonction pour effectuer la jointure de deux tables
int requet_join(char *nom_table1,char * nom_table2,char *liste_chaine_aff,char *listecolonn1,char *listecolonn2);
int verifContrainte(ListeAtt *liste);

//Fonction qui recupère le nom de la table réfrencée
char * recupNomTableReference(char * chaine);

//Fonction qui récupère au fur et à mesure la valeur du champ primaire
char * extractValPrimaire(char * chaine,char *attribPrimaire);

//Fonction qui récupère l'attribut primaire
char * extractAttribPrimaire(char * chaine);

//Fonction qui gère la contrainte primaire: pas d'insertion de valeurs identiques dans un champ declaré primary key 
int gereContraintePrimaire(FILE *fichier,ListeTuple *liste);

//Fonction qui fait les contrôles consernant la gestion du foreign key
int gereContrainteForeign(ListeAtt *liste);

////Fonction qui fait les contrôles consernant la gestion du foreign key pour alter
char * gereContrainteForeign2(char *ch);

//Fonction qui verifie si une clé primaire existe
int verifIfExistPrimaire(FILE *fichier);

//Fonction qui gère l'insertion des valeurs sur un champ primaire avec update
int gereContraintePrimaireUpdate(FILE *fichier,char *champ_chang,char *new_valeur,char *op_rel);

//Fonction qui pour une table qui contient un attribut foreign key empêche d'inserrer des valeurs qui ne sont pas dans la table réferencée 
int gereContrainteForeignInsertion(ListeTuple *liste);

//Fonction qui recupère ligne par ligne les valeurs d'un champ portant une contrainte foreign key
char *extractValRef(char * ligne, char *attribRef);

// La fonction qui extrait la valeur inserée dans un attribut portant une contrainte clé primaire 
char * extractAttribReference(char * chaine);

//Fonction permettant de faire des jointure multiple
int requete_multiple_join(char *liste_chaine_aff,char *table_names,char *listecolonn1,char *listecolonn2,char *listecolonn3,char *listecolonn4);

//Fonction permettant d'extraire les resultats d'une multiple jointure
ListeAllTuple *get_join_tuple_multi(ListeTuple *tuple,char *colon,char *colon_rech,ListeAllTuple *liste);

//Fonction permettant d'afficher les résultats dune jointure multiple
void affiche_join_tuple_multi(ListeAtt *att,ListeTuple *liste1,ListeTuple *liste2,ListeTuple *liste3,char **table);

///fonction pour chercher un caractere dans une chaine
static long Cherchecar(char* buffer, long offset, int car);

///fonction pour reallouer de la memoire
static char * RedimAlloc( char *buffer, size_t dim);
///fonction pour lire une ligne dans un fichier sequentiellement
long LireLigne(FILE * f, char ** buff, long dim);

///fonction pour se placer a une position donnée dans un fichier
int gotoPosition(FILE *fichier , char * chaine);

///fonction pour verifier l'existence d'une chaine dans une autre
int ifExist(char * ligne,char * chaine2);

///fonctions pour initialiser les listes des structure de donnees
ListeAtt *initialise();
ListeTuple *initialiseListeTuple();
ListeAllTuple *initialiseAllTuple();
////////////////////////////////////////////

///fonctions d'extraction de donnee d'une chaine de caracteres
char *extractData(char *ch);
char * extractType(char *ch);
char *extracAttrib(char *ch);
char *extractContrainte(char *ch);
////////////////////////////////

///fonction pour extraire la structure d'un fichier
ListeAtt *extractStruct(FILE *fichier);

///fonction d'insertion dans les differentes listes
void insertTuple(ListeTuple *liste,Donnee *donnee);
void insertStruct(ListeAtt *liste,Element *element);
void insertAllTuple(ListeAllTuple *liste , ListeTuple *listeTup);
/////////////////////////////////////

///fonction pour afficher le contenu des differentes listes
void afficheTuple(ListeTuple *liste);
void afficheAll(ListeAllTuple *liste);
void affiche(ListeAtt *liste);
///////////////////////////////////////

///fonction pour extraire un tuple dans un fichier
ListeTuple *extractTuple(FILE *fichier);

///fonction pour extraire tous les tuples
ListeAllTuple *extractAll(FILE *fichier);

///fonction pour generer le nom d'un fichier
char *generer_nom(char *nom);

///fonction pour tester l'existence d'une table
int exist_DB(char *ch);

///fonction pour afficher la structure d'une table
void describe(char *ch);

///fonction pour extraire des sous chaine d'une chaine par rapport aux virgule
char **extract_by_virgule(char *ch);

///fonction pour creer un champ de type element a paritr d'une chaine de caractere
Element *creer_champ(char *ch);

///fonction pour cree une liste d'element a base d'un tableau de chaine de caracteres
ListeAtt *extract_champ(char **ch);

///fonction pour verifie l'existence d'une colonne dans l'entete d'un fichier 
int recherche_In_Struct(char *nom,ListeAtt *liste);

///fonction pour ecrire l'entete d'un fichier
void write_struct(FILE *fichier,ListeAtt *liste,char *nom);

///fonction pour creer un fichier xml avec son entete
int create_table(char *nom,char *ch);

///fonction pour le chargement des donnees d'un fichier dans des listes chainees
data_link *load_data(FILE *fichier);

///fonction pour ecrire un tuple dans un fichier
void write_tuple(FILE *fichier,ListeTuple *liste);

///fonction pour ecrire tous les tuples d'une liste dans un fichier
void write_all_data(FILE *fichier,ListeAllTuple *liste);

///fonction pour la sauvegarde des liste dans un fichier
void save_data(FILE *fichier,ListeAtt *listeAt,ListeAllTuple *liste,char *nom_table);

///fonction pour creer un tuple a partir d'une chaine de caractere
ListeTuple *fabrique_liste_tuple(char *data,ListeAtt *liste);

///fonction pour compter le nombre de virgule d'une chaine
int  compter_champ(char *ch);

///fonction pour verifier si une chaine est composee uniquement de chiffres
int is_number(char *chaine);

///fonction pour verifier la compatibilté des types dans un tuple a inserer avec la structure de la table
int test_type(ListeTuple *liste);

///fonction pour inserer un tuple dans la table
int requete_insert(char *nom,char *data);

///fonction pour tester l'existence d'une donnee dans un tuples
int rechercher_in_tuple(char *nom,char *valeur,ListeTuple *liste);

///fonction pour teste si c'est le premier tuple de la liste generale
int  is_first(ListeAllTuple *liste,ListeTuple *test);

///fonction pour test si c'est le dernier tuple de la liste generale
int is_last(ListeAllTuple *liste,ListeTuple *test);

///fonction pour supprimer un tuple de la table 
void delete_tuple(ListeAllTuple *liste,ListeTuple *supp);

///fonction pour supprimer tous les tuples qui verifie une condition donnée
int requete_delete(char *nom_table,char *colonne,char *valeur,char *op);

///fonction pour tester si c'est le premier element de la liste
int  is_first_elem(ListeAtt *liste,Element *test);

///fonction pour tester si c'est le dernier element de la liste
int is_last_elem(ListeAtt *liste,Element *test);

///fonction pour supprimer un element de la liste de l'entete
void supp_elem(ListeAtt *liste,Element *supp);

///fonction pour supprimer un element de l'entete
int supp_elem_from_struct(ListeAtt *liste,char *nom);

int  is_first_data(ListeTuple *liste,Donnee *test);
int is_last_data(ListeTuple *liste,Donnee *test);
void supp_data(ListeTuple *liste,Donnee *supp);
void supp_data_from_Tuple(ListeTuple *liste,char *colonne);
void supp_data_from_all(ListeAllTuple *liste,char *colonne);

//////fonctions pour modifier la structure d'une table
int requete_alter_drop(char *nom_table,char *supp);
int requete_alter_add(char *nom_table,char *nouvo_champ);
///////////////

/////fonction d'affichage d'une requete de selection avec une liste de colonne a afficher
void afficheTuple_in_req(ListeTuple *liste,char *colonne);
void afficheAll_in_req(ListeAllTuple *liste,char *colonne);
//////////////////////

///fonction pour selectionner tout les donnee d'une table
int requete_select(char *nom_table,char *colonne);

///fonctions pour affichage de donnees
void afficheTuple_in_tuple(ListeTuple *liste);
void afficheAll_in_all(ListeAllTuple *liste);
////////////////////////////////////////

///fonction pour selectionner toutes les donnees d'une table
int requete_select_all(char *nom_table);

///fonctions pour la modification d'un tuple 
int modify_tuple_egal(char *colonn_condi,char *attrib_chang,char *valeur_cher,char *new_valeur,ListeTuple *liste);
int modify_tuple_dif(char *colonn_condi,char *attrib_chang,char *valeur_cher,char *new_valeur,ListeTuple *liste);
int modif_tuple_sup(char *colonn_condi,char *attrib_chang,char *valeur_cher,char *new_valeur,ListeTuple *liste);
int modif_tuple_inf(char *colonn_condi,char *attrib_chang,char *valeur_cher,char *new_valeur,ListeTuple *liste);

///fonction pour modifier toutes les tuples qui verifie une condition donnee
int requete_update(char *nom_table,char *colonn_condi,char *attrib_chang,char *valeur_cher,char *new_valeur,char *condi);

///fonction pour la suppression d'une table
int drop_table(char *nom_table);

///fonction pour renommer une table
int rename_table(char * old_name ,char *new_name);

///fonction pour tronquer une table
int truncate(char *nom_table);

///fonction pour la gestion du manuelle d'aide
void help(char *choix);

///fonction pour obtenir le type d'une colonne
char * get_type_struc(ListeAtt  *liste,char *colonne);

///fonction pour obtenir la valeur d'une colonne
char *get_tuple_value(ListeTuple *liste,char *colonne);

///fonction pour obtenir l'ensemble des tuples qui verifie une condition
ListeAllTuple *get_liste_tuple(ListeAllTuple *liste,char *colonne,char *valeur,char *comp);

///fonction pour selectionner tous tuples qui verifie une condition
int select_where(char *nom_table,char *colonne,char *valeur,char *comp);

///fonction pour afficher des donnees dans un format donnee
void afficheTuple_in_tuple_list(ListeTuple *liste,char *list_aff);
void afficheAll_in_all_list(ListeAllTuple *liste,char *list_aff);
///////////////////////////

///fonction pour selectionne l'ensemble des tuples verifiant une condition 
int select_where_liste(char *nom_table,char *col_af,char *colonne,char *valeur,char *comp);

///fonction pour extraire la liste des colonne a afficher
Element *extract_list_aff_ele(char *chaine);
ListeAtt *extract_liste_aff(char *chaine);
///////////////////////////////

///fonction pour extraction des tuples verifiant la condition de jointure
ListeAllTuple *get_join_tuple(ListeTuple *tuple,char *colon,char *colon_rech,ListeAllTuple *liste);

///fonction pour l'affichage du resultat de la jointure
void affiche_join_tuple(ListeAtt *att,ListeTuple *liste1,ListeTuple *liste2,char *table1,char * table2);


#endif
