#!/bin/sh -e

for var in FPRIME_ROOT FPP_FILES
do
  cmd='echo $'$var
  out="`eval $cmd`"
  if test -z "$out"
  then
    echo "$var not set" 1>&2
    exit 1
  fi
done

export FPRIME_ROOT=`cd $FPRIME_ROOT; echo $PWD`

depend_txt_do()
{
  fpp-depend $FPRIME_ROOT/defs.fpp $FPP_FILES > $3
}

get_comma_deps()
{
  redo-ifchange depend.txt
  deps=`cat depend.txt`
  if test -n "$deps"
  then
    echo $deps | sed 's/ /,/g'
  fi
}

#depend()
#{
#  fpp-depend $FPRIME_ROOT/defs.fpp $FPP_FILES
#}
#
#export DEPS=`depend`
#if test -n "$DEPS"
#then
#  export COMMA_DEPS="`echo $DEPS | sed 's/ /,/g'`"
#else
#  unset COMMA_DEPS
#fi

xml_do()
{
  comma_deps=`get_comma_deps`
  if test -n "$comma_deps"
  then
    import_deps="-i $comma_deps"
  else
    unset import_deps
  fi
  rm -rf $3
  mkdir $3
  fpp-to-xml -d $3 -p $FPRIME_ROOT $import_deps $FPP_FILES
}

locate_uses_do()
{
  comma_deps=`get_comma_deps`
  if test -n "$comma_deps"
  then
    import_deps="-i $comma_deps,$FPRIME_ROOT/defs.fpp"
  else
    import_deps="-i $FPRIME_ROOT/defs.fpp"
  fi
  fpp-locate-uses $import_deps $FPP_FILES
}
