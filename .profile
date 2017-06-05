export MAILDIR=~/Maildir

####### Composer bin directory.
export PATH="$HOME/.composer/vendor/bin:$PATH"

####### ALIASES
. ~/.aliases

###### FUNCTIONS
. ~/.functions

###### DRUSH AUTOCOMPLETION
if [ -f "~/.bashrc" ] ; then
    . ~/.bashrc
fi
