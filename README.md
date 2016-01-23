# miniSGBD
miniSGBD c'est ainsi que je l'ai baptisé, est un petit programme qui se comporte comme un SGBD, une sorte de mini SGBD.
Ce programme je l'ai fait dans le cadre d'un projet de classe dans un module intitulé "Techiniques de compilation" alors que j'étais encore en troisième année d'informatique.

En fait il nous été demandé de concevoir un interpréteur de commandes SQL avec les grandes
fonctionnalités suivantes :
 Fonctionnalité de création de table (avec posssibilité de définir des contraintes d'intégrités): génération d'un fichier XML
 Fonctionnalité de modification de la structure de la table (la commande alter table)
 Fonctionnalité d'affichage du schéma d'une table (la commande desc)
 Fonctionnalité d'insertion de tuples dans une table (la commande insert into)
 Fonctionnalité de mise à jour de tuples dans une table (la commande update)
 Fonctionnalité de suppression de tuples dans une table (la commande delete
 Fonctionnalité de sélection de tuples dans une table ou dans deux à plusieurs tables : les sélections
simples, les jointures et jointures multiples.
Pour ce faire nous devions utiliser les outils flex et bison dans leur version attribuée au langage C.
Flex est un analyseur lexical il fait de définir l'ensemble des mots de notre langage.
Voir:https://fr.wikipedia.org/wiki/Flex_(logiciel)
Bison est un analyseur syntaxique, il permet de définir la syntaxe de notre langage.
Voir:https://fr.wikipedia.org/wiki/GNU_Bison
