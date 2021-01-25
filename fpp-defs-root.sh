#!/bin/sh -e

# ----------------------------------------------------------------------
# fpp-defs-root.sh
# Root definitions for FPP models
# ----------------------------------------------------------------------

# Require environment variables to be set
require_vars()
{
  for var in $@
  do
    cmd='echo $'$var
    out="`eval $cmd`"
    if test -z "$out"
    then
      echo "$var not set" 1>&2
      exit 1
    fi
  done
}

# Require and canonicalize FPRIME_ROOT
require_vars FPRIME_ROOT
export FPRIME_ROOT=`cd $FPRIME_ROOT; echo $PWD`
export FPP_DEFS=$FPRIME_ROOT/defs.fpp

redo-ifchange fpp-defs.sh
redo-ifchange $FPRIME_ROOT/fpp-defs-root.sh

# Run command on all files matching regex
doall()
{
  cmd=$1
  shift
  for regex in "$@"
  do
    shift
    for file in `find . -name "$regex" -maxdepth 1`
    do
      $cmd $file
    done
  done
}

# Find and run all subdirectory targets
subdir_targets()
{
  for path in `find . -mindepth 2 -maxdepth 2 -name $2'.do'`
  do
    dir=`dirname $path`
    echo $dir/$2
  done | xargs $1
}

# Clean files
clean_do()
{
  doall 'rm -rf' '*~' depend xml
}

# Generate dependencies
depend_txt_do()
{
  require_vars FPP_FILES
  redo-ifchange $FPP_DEFS $FPP_FILES
  fpp-depend $FPP_DEFS $FPP_FILES > $3
}

# Generate depend directory
depend_do()
{
  require_vars FPP_FILES
  # Regenerate dependencies if any of these files have changed
  redo-ifchange $FPP_DEFS $FPP_FILES
  rm -rf $3
  mkdir $3
  fpp-depend -m $3/missing.txt $FPP_DEFS $FPP_FILES > $3/noinclude.txt
  fpp-depend -i $FPP_DEFS $FPP_FILES > $3/include.txt
  missing=`cat $3/missing.txt`
  if test -n "$missing"
  then
    echo "WARNING: missing dependency files" 1>&2
    echo $missing 1>&2
  fi
  redo-ifchange `cat $3/include.txt`
}

# Convert import dependencies to comma-separated format
get_comma_deps()
{
  # Recompute dependencies
  redo-ifchange depend
  # Check if dependencies have changed. If not, we are done.
  # Count included files in these dependencies
  cat depend/include.txt | redo-stamp
  # Compute the files to import
  # Don't count included dependencies
  deps=`cat depend/noinclude.txt`
  if test -n "$deps"
  then
    echo $deps | sed 's/ /,/g'
  fi
}

# Generate XML files
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

# Locate uses
locate_uses_do()
{
  comma_deps=`get_comma_deps`
  if test -n "$comma_deps"
  then
    import_deps="-i $comma_deps,$FPP_DEFS"
  else
    import_deps="-i $FPP_DEFS"
  fi
  fpp-locate-uses $import_deps $FPP_FILES
}
