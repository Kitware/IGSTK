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

# This script makes optional suggestions for working with git.

egrep-q() {
  egrep "$@" >/dev/null 2>/dev/null
}

if test -z "$(git config --get color.ui)"; then
  echo '
One may enable color output from Git commands with

  git config --global color.ui auto
'
fi

if ! bash -i -c 'echo $PS1' | egrep-q '__git_ps1'; then
  echo '
A dynamic, informative Git shell prompt can be obtained by sourcing the git
bash-completion script in your ~/.bashrc.  Set the PS1 environmental variable as
suggested in the comments at the top of the bash-completion script.  You may
need to install the bash-completion package from your distribution to obtain it.
'
fi
