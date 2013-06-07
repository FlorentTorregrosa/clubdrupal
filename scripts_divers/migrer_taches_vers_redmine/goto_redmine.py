"""
Pandoc est requis pour convertir le html en textile !
"""

import url_parser  #permet de connaître les id des taches
import urllib.request #permet de récupérer une page web
import httplib2 #pour faire des requêtes http
import json
import re #pour les expressions régulières
import os #pour pouvoir faire appel à pandoc (commande system)

######## NB : cid : comment id, nid : node id, urls : urls des tâches

#Dictionnaire des gens dont on a la clé API
#id: clé
SUBMITERS = {'jenselme': '464c7c05b9bb53fb136092f1b9807ad91ec51321'}

#les entêtes des requêtes POST et PUT
Headers = {'content-type': 'application/json', 'X-Redmine-API-Key': ''}

#là où on poste les tâches
URL = 'https://forge.centrale-marseille.fr/issues'

#là où sont les tâches
LIST_TODO = 'http://localhost/portail/liste-tache'

#url de base de l’emplacement du contenu
BASE_URL = 'http://localhost/portail'
PROJECT_ID = 30
TRACKER_ID = 2


########## dictionnaires de correspondance
DONE_RATIO = {'En pause': 50, 'À commencer': 0, 'Entamée': 20, 'Bien avancée': 80, 'Terminée (success)': 100, 'Fermée (won\'t fix)': 100}
PRIORITY = {'5 - Très basse': 3, '4 - Basse': 3, '3 - Moyenne': 4, '2 - Haute': 5, '1 - Très haute': 6,\
        'Très basse': 3, 'Basse': 3, 'Moyenne': 4, 'Haute': 5, 'Très haute':6,\
        '0': 3, '1': 3, '2': 4, '3': 5, '4': 6}
STATUS = {'En cours': 2, 'Fermée': 5, 'Rejetée': 6, 'En pause': 7}
#NB sur le portail, on a les équivalences suivantes
#pour le champ version de drupal : 17 : drupal6, 18 : drupal7
DRUPAL_VERSION = {'17': 2, '18': 1}

def give_api_key(submiter):
    "Donne la clé API de submiter ou celle de jenselme si c’est la seule"
    if submiter in SUBMITERS:
        return SUBMITERS[submiter]
    else:
        return  SUBMITERS['jenselme']


def give_comments_ids(nid):
    "permet de récupérer les id des commentaires de la tâche nid"
    page = urllib.request.urlopen(BASE_URL + '/entity_json/node/' + nid).read()
    page_json = json.loads(page.decode('utf-8'))
    comments_json = page_json['comments']
    #S’il n’y a pas de commentaire, comments_json est une liste vide et pas un dictionnaire
    if comments_json:
        comments = list(comments_json.keys())
        comments.sort() #ce sont les clés d’un dictionnaire. Pas d’ordre à priori
        return comments
    else:
        return list()


def give_comments(cids):
    "Donne la liste du texte des commentaires pour chaque cid in cids"
    comments = list()
    for cid in cids:
        comment = urllib.request.urlopen(BASE_URL + '/comment/' + cid + '.json').read()
        comments.append(json.loads(comment.decode('utf-8')))
    return comments


def format(txt):
    "prend le texte en html et le renvoie en textile"
    txt.replace('\n', '')
    txt.replace('\t', '')
    with open('tmp.html', 'w') as f:
        f.write(txt)
    os.system('pandoc -f html tmp.html -t textile -o tmp.textile')
    with open('tmp.textile', 'r') as f:
        txt = f.read()
    return txt


def give_redmine_status_id(tache):
    drupal_status = ''
    for elt in tache['field_avancement']:
        if "Terminée" in elt:
            drupal_status = 'Fermée'
            break
        elif "Fermée" in elt:
            drupal_status = 'Rejetée'
            break
        elif "pause" in elt:
            drupal_status = 'En pause'
            del elt
            break
    if not drupal_status:
        drupal_status = 'En cours'
    return STATUS[drupal_status]


def give_redmine_issue(tache):
    issue = dict()
    issue['project_id'] = PROJECT_ID
    issue['tracker_id'] = TRACKER_ID
    issue['subject'] = tache['title']
    issue['description'] = format(tache['body']['value'])
    #de temps en temps, le champ priorité est vide. On met 'Normale' dans ce cas
    if tache['field_prioritaecute']:
        issue['priority_id'] = PRIORITY[tache['field_prioritaecute']]
    else:
        issue['priority_id'] = PRIORITY['3 - Moyenne']
    if tache['field_avancement']:
        issue['done_ratio'] = DONE_RATIO[tache['field_avancement'][0]]
    else:
        issue['done_ratio'] = DONE_RATIO['À commencer']
    issue['status_id'] = give_redmine_status_id(tache)
    issue['fixed_version_id'] = DRUPAL_VERSION[tache['taxonomy_vocabulary_8']['id']]
    return issue


######### Main

nids, urls = url_parser.give_json_urls(LIST_TODO, BASE_URL)

h = httplib2.Http()

for post_url in urls:
    nid = nids[urls.index(post_url)]
    print(nid)
    tache_json = urllib.request.urlopen(post_url)
    tache_drupal = json.loads(tache_json.read().decode('utf-8'))

    cids = give_comments_ids(nid)
    comments_drupal = give_comments(cids)

    issue = {}
    issue['issue'] = give_redmine_issue(tache_drupal)
    data = json.dumps(issue)

    Headers['X-Redmine-API-Key'] = SUBMITERS['jenselme']

    resp, content = h.request(URL + '.json', 'POST', body=data, headers=Headers)

    #on récupère l’issue id pour savoir où poster les commentaires
    iid = re.findall(r',"id":([0-9]*),', content.decode('utf-8'))[0]

    #on a besoin de l’url à laquelle on met les commentaires, pour changer le status
    put_url = URL + '/' + iid + '.json'
    for index, comment in enumerate(comments_drupal):
        submiter = comment['name']  #le premier est celui qui a soumis le node
        Headers['X-Redmine-API-Key'] = give_api_key(submiter)
        #si la personne n’a pas sa clé, on modifie le commentaire
        comment_body = format(comment['comment_body']['value'])
        if not submiter in SUBMITERS:
            comment_body = "_{}_ a dit que :\n\n{}".format(submiter, comment_body)
        update = {}
        update['issue'] = {'notes': comment_body}
        data = json.dumps(update)
        h.request(put_url, 'PUT', body=data, headers=Headers)

    #Les taches sont crées avec le status nouveau peu importe ce qu’il y a dans le json
    #on modifie le status après coup
    update_status = {'issue': {'status_id': issue['issue']['status_id']}}
    data = json.dumps(update_status)
    h.request(put_url, 'PUT', body=data, headers=Headers)
