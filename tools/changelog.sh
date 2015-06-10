# produces a concise log of all commits
# Logs are sorted by release tag and author
#
# usage ./changelog.sh > log.txt
#
# note: HPX releases are of the form
# major.minor.patch 
# examples
#   0.9.10
#   0.9.9
# etc, and so the match using grep '[0-9]\+\.[0-9]\+\.[0-9]\+$'
# selects only those refs which match this format for inclusion in the log.
#
# To add support for other tags/branches (any ref can be used) then simply change the regex
# to select the desired ref, the command git for-each-ref --sort='-*authordate' on its
# own will output all possible refs
#
# example, to include release candidate tags as well simply change the grep expression to
# grep -e '[0-9]\+\.[0-9]\+\.[0-9]\+$' -e '[0-9]\+\.[0-9]\+\.[0-9]\+\-rc*'
#
# The script calls shortlog.sh to generate the log for each pair of refs and it is
# generally easier to simply call shortlog for each new release
#
# example : ./shortlog 0.9.10-rc1 0.9.10
#

git for-each-ref --sort='-*authordate' | grep '[0-9]\+\.[0-9]\+\.[0-9]\+$' | awk '{printf $3 " "}' | sed 's/refs\/tags\///g' | sed 's/refs\/heads\///g' |  awk '{ for (i=1;i<NF; i++) print $(i+1) " " $i; print $NF}' | xargs -n 2 ./shortlog.sh

