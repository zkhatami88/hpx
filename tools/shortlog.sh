#!/bin/bash

# usage  : shortlog.sh $from_tag $to_tag
#
# example: shortlog.sh 0.9.9      0.9.10
#          shortlog.sh 0.9.10-rc1 0.9.10 
#
# arg $1 from tag
# arg $2 to tag
#
# if arg2 is not supplied, then log is given for all commits

if [ -n "$2" ]
 then 
  echo "================"
  echo -e $2
  echo "================"
  git log $1..$2 --reverse --no-merges --pretty=format:"commit %h%x0AAuthor: %an <%ae> %x0A%x0A %x09 %ad %s %x0A" --date=short | git shortlog -n -w130,2,13

else
  echo "================"
  echo -e $1
  echo "================"
  git log $1 --reverse --no-merges --pretty=format:"commit %h%x0AAuthor: %an <%ae> %x0A%x0A %x09 %ad %s %x0A" --date=short | git shortlog -n -w130,2,13

fi
