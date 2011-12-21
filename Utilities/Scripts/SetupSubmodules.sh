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

echo "Initializing and downloading submodules..." &&
cd "${BASH_SOURCE%/*}/../.." &&
git submodule update --init &&
cd Testing/Data &&
if ! git config --get remote.origin.pushurl >/dev/null; then
  git config remote.origin.pushurl git@igstk.org:IGSTKData.git
fi
