#!/bin/sh
PATH=/usr/local/bin:/usr/bin:/bin:/users/guest/assos/bin

##############
# nlehuby - 05 juillet 2012
# ce script permet de nettoyer p�riodiquement les diff�rents r�pertoires o� sont enregistr�s les sauvegardes de base de donn�es, afin de lib�rer de l'espace.
##############

######## sauvegardes individuelles ########
#compte le nombre de sites d7 (en excluant les liens symboliques)
cd /users/guest/assos/htmltest/sites
let "nbsitesd7 = `ls -lF | grep -v 'all' | grep -v @ | wc -l`" >/dev/null ;
#explications : l'option -F dans le ls permet de marquer les liens symboliques avec un @ ; le "grep -v @" permet donc d'ignorer les liens symboliques dans la liste fournie par ls. La commande wc permet de compter le nombre de lignes retourn�es. Le let permet de d�finir une variable qui soit un nombre (et pas une chaine de caract�res)

#va dans le dossier des sauvegardes individuelles
cd /users/guest/assos/Desktop/dump_individuels

#s'il y a plus de 2 x (le nombre de site d7) - 1 fichiers alors
let "c=2*$nbsitesd7 - 1" >/dev/null  ;
if [ $(ls -l | wc -l)  -gt  $c ] ; then
    echo "je supprime des sauvegardes isol�es"
#supprime (le nombre de site d7) plus vieux fichiers
    ls -tr | head -$nbsitesd7 | xargs rm;

#sinon, alerte
else
    echo "La purge des sauvegardes des sites individuelles d7 n'a pas pu �tre effectu�e car il n'y avait pas assez de fichiers dans le dossier. Il faudrait v�rifier que le script de sauvegarde s'ex�cute correctement" | mail -s "[dump assos] On a des soucis avec les sauvegardes de bdd" assos@centrale-marseille.fr;

fi
echo `ls -l | wc -l `
######## sauvegardes compl�tes d6 ########
#va dans le site des sauvegardes compl�tes d6
cd /users/guest/assos/Desktop/dump_d6

#s'il y a plus de 15 fichiers alors
if [ $(ls -l | wc -l)  -gt 15 ] ; then
    echo "je supprime du d6"
#supprime les 3 fichiers les plus vieux
    ls -tr | head -3 | xargs rm;

#sinon, alerte
else
    echo "La purge des sauvegardes compl�tes des bases de donn�es drupal 6 n'a pas pu �tre effectu�e car il n'y avait pas assez de fichiers dans le dossier. Il faudrait v�rifier que le script de sauvegarde s'ex�cute correctement" | mail -s "[dump assos] On a des soucis avec les sauvegardes de bdd" assos@centrale-marseille.fr ;

fi
echo `ls -l | wc -l `
######## sauvegardes compl�tes d7 ########
#va dans le site des sauvegardes compl�tes d7
cd /users/guest/assos/htmltest/sites
let "nbsitesd7 = `ls -lF | grep -v 'all' | grep -v @ | wc -l`" >/dev/null ;
#explications : l'option -F dans le ls permet de marquer les liens symboliques avec un @ ; le "grep -v @" permet donc d'ignorer les liens symboliques dans la liste fournie par ls. La commande wc permet de compter le nombre de lignes retourn�es. Le let permet de d�finir une variable qui soit un nombre (et pas une chaine de caract�res)

#va dans le dossier des sauvegardes individuelles
cd /users/guest/assos/Desktop/dump_d7

#s'il y a plus de 3 x (le nombre de site d7)
let "c=2*$nbsitesd7" >/dev/null  ;
if [ $(ls -l | wc -l)  -gt  $c ] ; then
    echo "je supprime des sauvegardes isol�es"
#supprime (le nombre de site d7) plus vieux fichiers
    ls -tr | head -$nbsitesd7 | xargs rm;

#sinon, alerte
else
    echo "La purge des sauvegardes automatiques des sites d7 n'a pas pu �tre effectu�e car il n'y avait pas assez de fichiers dans le dossier. Il faudrait v�rifier que le script de sauvegarde s'ex�cute correctement" | mail -s "[dump assos] On a des soucis avec les sauvegardes de bdd" assos@centrale-marseille.fr;

fi
echo `ls -l | wc -l `
