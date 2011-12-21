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

cd "${BASH_SOURCE%/*}/.." &&
Utilities/Scripts/SetupUser.sh && echo &&
Utilities/Scripts/SetupSubmodules.sh && echo &&
Utilities/Scripts/SetupHooks.sh && echo &&
Utilities/Scripts/SetupGitAliases.sh && echo &&
Utilities/Scripts/SetupTopicStage.sh && echo &&
(Utilities/Scripts/SetupPush.sh ||
 echo 'Failed to setup push access.  Run this again to retry.') && echo &&
Utilities/Scripts/GitTips.sh
