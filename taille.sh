#!/bin/sh

##############
# nlehuby - 07 juillet 2012
# ce script permet de v�rifier la consommation d'espace disque du compte assos, et de r�p�rer d'�ventuelles irr�gularit�s (mises � jour foireuses, etc)
##############

echo "Utilisation totale du compte assos :"
du -hcs /users/guest/assos/ | grep total

echo "*Utilisation de l'installation d6 :"
du -hcs /users/guest/assos/html/sites/ | grep total

echo "*Utilisation de l'installation d7 :"
du -hcs /users/guest/assos/htmltest/sites/ | grep total

echo "D�tails pour l'installation d6 :"
cd /users/guest/assos/html/sites
for x in $(ls -1 | grep -v 'all'); do
  if [ -d $x -a ! -L $x ]; then
  du -hs $x;
    fi
done

echo "D�tails pour l'installation d7 :"
cd /users/guest/assos/htmltest/sites
for x in $(ls -1 | grep -v 'all'); do
  if [ -d $x -a ! -L $x ]; then
  du -hs $x;
    fi
done
