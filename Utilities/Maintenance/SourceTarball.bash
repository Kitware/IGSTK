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

usage() {
  die 'USAGE: SourceTarball.bash [(--tgz|--txz|--zip)...] \
        [--verbose] [-v <version>] [<tag>|<commit>]'
}

info() {
  echo "$@" 1>&2
}

die() {
  echo "$@" 1>&2; exit 1
}

git_archive_tgz() {
  out="$2.tar.gz" && tmp="$out.tmp$$" &&
  git -c core.autocrlf=false archive $verbose --format=tar --prefix=$2/ $1 |
  gzip -9 > "$tmp" &&
  mv "$tmp" "$out" &&
  info "Wrote $out"
}

git_archive_txz() {
  out="$2.tar.xz" && tmp="$out.tmp$$" &&
  git -c core.autocrlf=false archive $verbose --format=tar --prefix=$2/ $1 |
  xz -9 > "$tmp" &&
  mv "$tmp" "$out" &&
  info "Wrote $out"
}

git_archive_zip() {
  out="$2.zip" && tmp="$out.tmp$$" &&
  git -c core.autocrlf=true archive $verbose --format=zip --prefix=$2/ $1 > "$tmp" &&
  mv "$tmp" "$out" &&
  info "Wrote $out"
}

#-----------------------------------------------------------------------------

formats=
commit=
version=
verbose=

# Parse command line options.
while test $# != 0; do
  case "$1" in
    --tgz) formats="$formats tgz" ;;
    --txz) formats="$formats txz" ;;
    --zip) formats="$formats zip" ;;
    --verbose) verbose=-v ;;
    --) shift; break ;;
    -v) shift; version="$1" ;;
    -*) usage ;;
    *) test -z "$commit" && commit="$1" || usage ;;
  esac
  shift
done
test $# = 0 || usage
test -n "$commit" || commit=HEAD
test -n "$formats" || formats=tgz

if ! git rev-parse --verify -q "$commit" >/dev/null ; then
  die "'$commit' is not a valid commit"
fi
if test -z "$version"; then
  desc=$(git describe --tags $commit) &&
  if test "${desc:0:1}" != "v"; then
    die "'git describe --tags $commit' is '$desc'; use -v <version>"
  fi &&
  version=${desc#v} &&
  echo "$commit is version $version" ||
  die "'git describe --tags $commit' failed."
fi

# Create temporary git index to construct source tree
export GIT_INDEX_FILE="$(pwd)/tmp-$$-index" &&
trap "rm -rf '$GIT_INDEX_FILE'" EXIT &&

info "Loading tree from $commit..." &&
git read-tree -m -i $commit &&

info "Finding submodule repos..." &&
GIT_ALTERNATE_OBJECT_DIRECTORIES=$(
sep='' && git ls-tree -r $commit | grep '^160000 ' |
while read _ _ _ path; do
  dir=$(cd "$path" && cd "$(git rev-parse --git-dir)/objects" && pwd) &&
  echo -n "$sep$dir" &&
  sep=':' || exit $?
done) &&
export GIT_ALTERNATE_OBJECT_DIRECTORIES || exit $?

info "Loading submodule trees..." &&
git ls-tree -r $commit |grep '^160000 ' |
while read _ _ sub path; do
    git update-index --force-remove -- "$path" &&
    git read-tree -i --prefix="$path"/ $sub ||
    die "Failed to load submodule tree at '$path'!"
done || exit $?

info "Generating source archive..." &&
tree=$(git write-tree) &&
result=0 &&
for fmt in $formats; do
  git_archive_$fmt $tree "IGSTK-$version" || result=1
done &&
exit $result
