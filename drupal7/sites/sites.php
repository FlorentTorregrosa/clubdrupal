<?php
/**
 * @file
 * Configuration file for Drupal's multi-site directory aliasing feature.
 *
 * This file allows you to define a set of aliases that map hostnames, ports, and
 * pathnames to configuration directories in the sites directory. These aliases
 * are loaded prior to scanning for directories, and they are exempt from the
 * normal discovery rules. See default.settings.php to view how Drupal discovers
 * the configuration directory when no alias is found.
 *
 * Aliases are useful on development servers, where the domain name may not be
 * the same as the domain of the live server. Since Drupal stores file paths in
 * the database (files, system table, etc.) this will ensure the paths are
 * correct when the site is deployed to a live server.
 *
 * To use this file, copy and rename it such that its path plus filename is
 * 'sites/sites.php'. If you don't need to use multi-site directory aliasing,
 * then you can safely ignore this file, and Drupal will ignore it too.
 *
 * Aliases are defined in an associative array named $sites. The array is
 * written in the format: '<port>.<domain>.<path>' => 'directory'. As an
 * example, to map http://www.drupal.org:8080/mysite/test to the configuration
 * directory sites/example.com, the array should be defined as:
 * @code
 * $sites = array(
 *   '8080.www.drupal.org.mysite.test' => 'example.com',
 * );
 * @endcode
 * The URL, http://www.drupal.org:8080/mysite/test/, could be a symbolic link or
 * an Apache Alias directive that points to the Drupal root containing
 * index.php. An alias could also be created for a subdomain. See the
 * @link http://drupal.org/documentation/install online Drupal installation guide @endlink
 * for more information on setting up domains, subdomains, and subdirectories.
 *
 * The following examples look for a site configuration in sites/example.com:
 * @code
 * URL: http://dev.drupal.org
 * $sites['dev.drupal.org'] = 'example.com';
 *
 * URL: http://localhost/example
 * $sites['localhost.example'] = 'example.com';
 *
 * URL: http://localhost:8080/example
 * $sites['8080.localhost.example'] = 'example.com';
 *
 * URL: http://www.drupal.org:8080/mysite/test/
 * $sites['8080.www.drupal.org.mysite.test'] = 'example.com';
 * @endcode
 *
 * @see default.settings.php
 * @see conf_path()
 * @see http://drupal.org/documentation/install/multi-site
 */
$sites['forum.centrale-marseille.fr'] = 'assos.centrale-marseille.fr.agora';
$sites['assos.centrale-marseille.fr.annales'] = 'assos.centrale-marseille.fr.annales';
$sites['assos.centrale-marseille.fr.aoudad'] = 'assos.centrale-marseille.fr.aoudad';
$sites['assos.centrale-marseille.fr.apocaliste'] = 'assos.centrale-marseille.fr.apocaliste';
$sites['assos.centrale-marseille.fr.bda'] = 'assos.centrale-marseille.fr.bda';
$sites['assos.centrale-marseille.fr.bde'] = 'assos.centrale-marseille.fr.bde';
$sites['assos.centrale-marseille.fr.bds'] = 'assos.centrale-marseille.fr.bds';
$sites['assos.centrale-marseille.fr.cheer-up'] = 'assos.centrale-marseille.fr.cheer-up';
$sites['assos.centrale-marseille.fr.clubfinance'] = 'assos.centrale-marseille.fr.clubfinance';
$sites['assos.centrale-marseille.fr.clubrobot'] = 'assos.centrale-marseille.fr.clubrobot';
$sites['assos.centrale-marseille.fr.echangesphoceens'] = 'assos.centrale-marseille.fr.echangesphoceens';
$sites['assos.centrale-marseille.fr.election'] = 'assos.centrale-marseille.fr.election';
$sites['assos.centrale-marseille.fr.eluseleves'] = 'assos.centrale-marseille.fr.eluseleves';
$sites['assos.centrale-marseille.fr.ercm'] = 'assos.centrale-marseille.fr.ercm';
$sites['fablab.centrale-marseille.fr'] = 'assos.centrale-marseille.fr.fablab';
$sites['forum-foceen.centrale-marseille.fr'] = 'assos.centrale-marseille.fr.forumentreprises';
$sites['assos.centrale-marseille.fr.ftorregrosa'] = 'assos.centrale-marseille.fr.ftorregrosa';
$sites['assos.centrale-marseille.fr.icm'] = 'assos.centrale-marseille.fr.icm';
$sites['assos.centrale-marseille.fr.isf'] = 'assos.centrale-marseille.fr.isf';
$sites['assos.centrale-marseille.fr.jpennec'] = 'assos.centrale-marseille.fr.jpennec';
$sites['assos.centrale-marseille.fr.lessive'] = 'assos.centrale-marseille.fr.lessive';
$sites['assos.centrale-marseille.fr.mdv'] = 'assos.centrale-marseille.fr.mdv';
$sites['tvp.centrale-marseille.fr'] = 'assos.centrale-marseille.fr.tvp';
$sites['assos.centrale-marseille.fr'] = 'default';
$sites['assos.centrale-marseille.fr.charlist'] = 'assos.centrale-marseille.fr.charlist';
$sites['assos.centrale-marseille.fr.atrinhtest'] = 'assos.centrale-marseille.fr.atrinhtest';
$sites['assos.centrale-marseille.fr.ginfotest'] = 'assos.centrale-marseille.fr.ginfotest';
$sites['assos.centrale-marseille.fr.expat'] = 'assos.centrale-marseille.fr.expat';
$sites['assos.centrale-marseille.fr.cneuvilletest'] = 'assos.centrale-marseille.fr.cneuvilletest';
$sites['assos.centrale-marseille.fr.mschiano'] = 'assos.centrale-marseille.fr.mschiano';
$sites['assos.centrale-marseille.fr.djulientest'] = 'assos.centrale-marseille.fr.djulientest';
$sites['assos.centrale-marseille.fr.forumfoceentest'] = 'assos.centrale-marseille.fr.forumfoceentest';
$sites['assos.centrale-marseille.fr.echangesphotest'] = 'assos.centrale-marseille.fr.echangesphotest';
$sites['assos.centrale-marseille.fr.accueiltest'] = 'assos.centrale-marseille.fr.accueiltest';
$sites['assos.centrale-marseille.fr.rgrondin'] = 'assos.centrale-marseille.fr.rgrondin';
$sites['assos.centrale-marseille.fr.clubdrupal'] = 'assos.centrale-marseille.fr.clubdrupal';
$sites['assos.centrale-marseille.fr.accueilold'] = 'assos.centrale-marseille.fr.accueilold';
$sites['assos.centrale-marseille.fr.forumtest'] = 'assos.centrale-marseille.fr.forumtest';
$sites['assos.centrale-marseille.fr.epold'] = 'assos.centrale-marseille.fr.epold';
$sites['assos.centrale-marseille.fr.uaecm'] = 'assos.centrale-marseille.fr.uaecm';
$sites['assos.centrale-marseille.fr.mlannes'] = 'assos.centrale-marseille.fr.mlannes';
$sites['assos.centrale-marseille.fr.acohen'] = 'assos.centrale-marseille.fr.acohen';
$sites['assos.centrale-marseille.fr.ancienbde'] = 'assos.centrale-marseille.fr.ancienbde';
