#!/bin/sh
#Lancer le cron pour que les sites sachent s'il y a des majs à faire
/users/guest/assos/bin/drushall -q cron 
#Supprimer le cache pour réduire de moitié la taille des bases de données sauvegardées 
/users/guest/assos/bin/drushall cc all
#Script de sauvegarde des bases de données
/bin/sh /users/guest/assos/bin/dump.sh
#Mettre à jour le code des modules et thèmes tiers
/users/guest/assos/bin/drushall --no-core -y upc
/users/guest/assos/bin/drushall -y updb
#Rapport du cron
#/users/guest/assos/bin/drushall cron | mail -s "Rapport drushall cron" assos@centrale-marseille.fr
/users/guest/assos/bin/drushall cron
#Rapport sur la taille utilisée du disque
/bin/sh /users/guest/assos/bin/taille.sh | mail -s "Rapport utilisation disque" assos@centrale-marseille.fr 



