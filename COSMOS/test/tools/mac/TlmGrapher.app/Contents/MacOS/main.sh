#!/bin/bash -l
TOOLNAME=TlmGrapher
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
rm $DIR/$TOOLNAME
if hash rbenv 2>/dev/null; then
  ln -s `rbenv which ruby` $DIR/$TOOLNAME
else
  ln -s `which ruby` $DIR/$TOOLNAME
fi
exec $DIR/$TOOLNAME $DIR/$TOOLNAME.rb "$@" &
