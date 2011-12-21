#!/usr/bin/env bash
#==========================================================================
#
#   Copyright Insight Software Consortium
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#          http://www.apache.org/licenses/LICENSE-2.0.txt
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#
#==========================================================================*/


# Run this script to set up the topic stage for pushing changes.

egrep-q() {
  egrep "$@" >/dev/null 2>/dev/null
}

die() {
  echo 1>&2 "$@" ; exit 1
}

if test -r ~/.ssh/config &&
   egrep-q 'Host[= ]igstk\.org' ~/.ssh/config; then
  echo 'Host igstk.org is already in ~/.ssh/config'
  setup=
  question='Test'
else
  echo 'Host igstk.org not found in ~/.ssh/config'
  setup=1
  question='Setup and test'
fi
echo
read -ep "${question} push access by ssh to git@igstk.org? [y/N]: " access
if [ "$access" != "y" ] && [ "$access" != "Y" ]; then
  exit 0
fi

if test -n "$setup"; then
  if ! test -d ~/.ssh; then
    mkdir -p ~/.ssh &&
    chmod 700 ~/.ssh
  fi &&
  if ! test -f ~/.ssh/config; then
    touch ~/.ssh/config &&
    chmod 600 ~/.ssh/config
  fi &&
  ssh_config='Host=igstk.org
  IdentityFile ~/.ssh/id_git_igstk' &&
  echo "Adding to ~/.ssh/config:

$ssh_config
" &&
  echo "$ssh_config" >> ~/.ssh/config &&
  if ! test -e ~/.ssh/id_git_igstk; then
    if test -f ~/.ssh/id_rsa; then
      # Take care of the common case.
      ln -s id_rsa ~/.ssh/id_git_igstk
      echo '
Assuming ~/.ssh/id_rsa is the private key corresponding to the public key for

  git@igstk.org

If this is incorrect place private key at "~/.ssh/id_git_igstk".'
    else
      echo '
Place the private key corresponding to the public key registered for

  git@igstk.org

at "~/.ssh/id_git_igstk".'
    fi
    read -e -n 1 -p 'Press any key to continue...'
  fi
fi || exit 1

echo 'Testing ssh push access to "git@igstk.org"...'
if ! ssh git@igstk.org info; then
  die 'No ssh push access to "git@igstk.org".  You may need to request access at

  https://www.kitware.com/Admin/SendPassword.cgi
'
fi
