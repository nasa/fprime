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

depend()
{
  fpp-depend $FPRIME_ROOT/defs.fpp $FPP_FILES
}

export DEPS=`depend`
if test -n "$DEPS"
then
  export COMMA_DEPS="`echo $DEPS | sed 's/ /,/g'`"
else
  unset COMMA_DEPS
fi

gen_xml()
{
  if test -n "$COMMA_DEPS"
  then
    import_deps="-i $COMMA_DEPS"
  else
    unset import_deps
  fi
  fpp-to-xml -p $FPRIME_ROOT $import_deps $FPP_FILES
}

xml_do()
{
  redo-ifchange $FPRIME_ROOT/defs.fpp $DEPS $FPP_FILES
  if test -n "$COMMA_DEPS"
  then
    import_deps="-i $COMMA_DEPS"
  else
    unset import_deps
  fi
  rm -rf $3
  mkdir $3
  fpp-to-xml -d $3 -p $FPRIME_ROOT $import_deps $FPP_FILES
}

locate_uses()
{
  if test -n "$COMMA_DEPS"
  then
    import_deps="-i $COMMA_DEPS,$FPRIME_ROOT/defs.fpp"
  else
    import_deps="-i $FPRIME_ROOT/defs.fpp"
  fi
  fpp-locate-uses $import_deps $FPP_FILES
}
