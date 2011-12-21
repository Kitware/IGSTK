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


# Run this script to set up the git hooks for committing changes to IGSTK.
# For more information, see:
#   http://www.itk.org/Wiki/ITK/Git#Hooks
#   http://www.itk.org/Wiki/Git/Hooks

egrep-q() {
  egrep "$@" >/dev/null 2>/dev/null
}

die() {
	echo 1>&2 "$@" ; exit 1
}

echo 'Setting up git hooks...' &&
cd "${BASH_SOURCE%/*}" &&
git_dir=$(git rev-parse --git-dir) &&
cd "$git_dir/hooks" &&
if ! test -e .git; then
  git init -q || die 'Could not run git init for hooks.'
fi &&
if GIT_DIR=.. git for-each-ref refs/remotes/origin/hooks 2>/dev/null |
  egrep-q 'refs/remotes/origin/hooks$'; then
  git fetch -q .. remotes/origin/hooks
else
  git fetch -q http://igstk.org/IGSTK.git hooks
fi &&
git reset -q --hard FETCH_HEAD || die 'Failed to install hooks'
