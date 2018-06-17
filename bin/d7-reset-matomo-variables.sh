#!/bin/sh

site="$1"

###### Common to all sites
drush @"${site}" -yq en matomo
drush @"${site}" -yq vset matomo_url_http "http://piwik.centrale-marseille.fr/"
drush @"${site}" -yq vset matomo_url_https "https://piwik.centrale-marseille.fr/"
# Matomo cache.
drush @"${site}" -yq vset matomo_cache 1
drush @"${site}" -yq vset matomo_visibility_roles "1"
# Matomo is enable for everyone except the administrator
drush @"${site}" -yq vset --format=json matomo_roles '[3]'
drush @"${site}" -yq vset matomo_page_title_hierarchy 1
# Activate local search.
drush @"${site}" -yq vset matomo_site_search 1

##### Specific
# Note: you can read the matomo site id from the url in matomo
case "$1" in
    accueil)
	matomo_id=286
	;;
    agora)
	matomo_id=116
	;;
    bda)
	matomo_id=273
	;;
    bde)
	matomo_id=274
	;;
    bds)
	matomo_id=275
	;;
    cac13)
	matomo_id=276
	;;
    cheer-up)
	matomo_id=277
	;;
    clubfinance)
	matomo_id=278
	;;
    clubrobot)
	matomo_id=279
	;;
    echangesphoceens)
	matomo_id=280
	;;
    eluseleves)
	matomo_id=281
	;;
    ercm)
	matomo_id=282
	;;
    fablab)
	matomo_id=151
	;;
    forumentreprises)
	matomo_id=270
	;;
    ginfo)
	matomo_id=283
	;;
    icm)
	matomo_id=284
	;;
    isf)
	matomo_id=285
	;;
    ksi)
	matomo_id=209
	;;
    mdv)
	matomo_id=99
	;;
    tvp)
	matomo_id=110
	;;
    ftorregrosa)
	matomo_id=314
	;;
    default)
	matomo_id=101
	;;
    *)
	matomo_id=287
	;;
esac

drush @"${site}" -yq vset matomo_site_id "${matomo_id}"
