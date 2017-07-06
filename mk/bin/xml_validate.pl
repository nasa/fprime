#!/usr/bin/env perl

use strict;
use Cwd;

my(@files);
my($java_args);

### MIST has JING installed in two places, but mslfswtbgds1 only has
### the 2008 version.

my(@jing_path) =
    (
     "/dsw/jing-20030619/bin",
     "/dsw/jing-20081028/bin"
     );
my($java) = "java";
my(@paths) = split ":", $ENV{'PATH'};
my($tmp_file) = "/tmp/#xml_validate__$$";
my($remove_file) = $tmp_file;
my(%xml_files);
my($verbose) = 0;

while ($#ARGV >= 0) {
  if ($ARGV[0] =~ /^-j/i && $#ARGV) {
    $java = $ARGV[1];
    print "== Will run java as \"$java\"\n"
	if $verbose;
    shift @ARGV;
  } elsif ($ARGV[0] =~ /^-a/i && $#ARGV) {
    $java_args = $ARGV[1];
    print "== Will use java arguments \"$java_args\"\n"
	if $verbose;
    shift @ARGV;
  } elsif ($ARGV[0] =~ /^-p/i && $#ARGV) {
    unshift @paths, $ARGV[1];
    print "== Adding directory \"$ARGV[1]\" to head of search path\n"
	if $verbose;
    shift @ARGV;
  } elsif ($ARGV[0] =~ /^-t/i && $#ARGV) {
    $tmp_file = $ARGV[1];
    print "== Will write to temp file \"$ARGV[1]\"\n"
	if $verbose;
    shift @ARGV;
  } elsif ($ARGV[0] =~ /^-v/i) {
    $verbose = $ARGV[1];
    print "== Verbosity now $verbose\n"
	if $verbose;
    shift @ARGV;
  } elsif ($ARGV[0] =~ /^-h/i) {
    do_show_help ();
  } elsif (-f $ARGV[0]) {
    push @files, $ARGV[0];
  } else {
    print STDERR "** Skipping unknown argument \"$ARGV[0]\"\n";
  }

  shift @ARGV;
}

### Add STDIN to the tail of the list if it's being redirected (not a TTY)

if (! -t STDIN) {
  push @ARGV, "-";
}

### Find the java executable

if (! -x $java) {

### FIXME Doesn't search for binary subdirectories yet, thanks to the
### "undef".  Happily, it does find the newest version in the right
### place

### Need to find version 1.5 (at least) for MIST environment

  my($version_regexp) = "/1\\.[5-9]/";

  my($path) = find_in_path ($java, \@paths, undef, $verbose,
			    $version_regexp, "-version");

  if (!defined $path ||
      ! -x $path) {
    print STDERR "*** Failed to find executable \"$java\" version $version_regexp in path\n\t" .
	join ("\n\t", @paths) . "\n";
    exit 1;
  }
  $java = $path;
}

### Find JING

my($envar) = $ENV{'JING'};

if (defined ($envar) &&
    -r $envar) {
  $java_args = "-jar $envar";
}

if (!defined $java_args) {
  my($jing) = find_in_path ("jing.jar", \@jing_path, undef,
			    $verbose);

  if (!defined $jing) {
    print (STDERR "*** Failed to locate jing.jar along path in %s\n",
	   join (", ", @jing_path));
    exit 1;
  }

  $java_args = "-jar $jing";
}


while (scalar(@files)) {
  handle ($files[0]);
  shift @files;
}


sub handle {
  my($filename) = @_;
  my($file);

### Find the name of the schema

  if (!read_complete_file ($filename, \$file)) {
    print STDERR "*** Cannot read \"$filename\"\n";
    exit 1;
  }

  my(@pieces) = split /RNGSchema=/i, $file;

  if (scalar(@pieces) != 2) {
    printf STDERR "*** Failed to extract unique Schema name, found %d != 2 pieces\n",
    scalar(@pieces);
    exit 1;
  }

  if ($pieces[1] =~ /^[\"\']*([^\"\'\<]+)[\"\']*[\s\<]/) {
    my($schema) = $1;

### Remove "file:" prefix
    $schema =~ s/^\s*file:\s*//g;
    $schema = correct_pathname ($schema);
    print "== Running against schema file \"$schema\"\n";

    my($cmd) = sprintf "%s %s -c %s %s >$tmp_file 2>&1",
    $java, $java_args, $schema, $filename;

    print "== Running XML check command \"$cmd\"\n"
	if $verbose;

    my($status) = system($cmd) >> 8;

### Read through the temp file, annotating things along the way

    annotate_results_in_file ($tmp_file);

    if ($status == 0) {
      print "== XML check passed!\n"
	  if $verbose;
    } else {
      print STDERR "*** \"$filename\" failed XML validation!\n";
    }

    if (-f $remove_file) {
      unlink ($remove_file);
    }

    exit ($status);

  } else {
    print STDERR "*** Failed to extract Schema name from \"$filename\"\n";
    exit 1;
  }
}



sub annotate_results_in_file {
  my($filename) = @_;

  if (!open (OUTPUTFILE, "<$filename")) {
    print STDERR "*** Failed to open results file \"$filename\"\n";
  } else {
    my($file) = join ("", <OUTPUTFILE>);

    map {
      my($outer_line) = $_;

      if ($outer_line =~ /^\s*([^\s:]+)\:(\d+)\:(\d+)\:(.*)$/) {
	my($xmlfilename,$lineno,$char,$rest) = ($1, $2, $3, $4);
	my($fileref) = cache_xml_fileref ($xmlfilename);
	my($linenoless1) = $lineno - 1;

### Jump to the line

	my(@lines) = split ("\n", $$fileref);

	if (scalar (@lines) > $linenoless1) {
	  my($line) = $lines[$linenoless1];
	  my($charless1) = $char - 1;

	  print STDERR "Searching \"$line\"\n"
	      if $verbose;

### Grab any preceeding/following tags too

	  if ($line =~ /^(.{$charless1})(.? [^\<\>]*)(.*)?$/x) {
	    my($prefix,$tag,$suffix) = ($1, $2, $3);

	    $prefix =~ s/^\s*//;
	    $suffix =~ s/\s*$//;

### See if this is one of those "I'm pointing at the end of all the useful stuff" errors.

	    if (length ($suffix) == 0 &&
		length ($tag) == 0 &&
		$line =~ /^(.*[<>])([^<>]*)?([<>][^<>]*[<>])$/) {
	      print STDERR "Resampling this \"$prefix\" \"$tag\" \"$suffix\"\n"
		  if $verbose;

	      ($prefix, $tag, $suffix) = ($1, $2, $3);

	      $prefix =~ s/^\s*//;
	      $suffix =~ s/\s*$//;

	      print STDERR "      into this \"$prefix\" \"$tag\" \"$suffix\"\n"
		  if $verbose;
	    }

	    if (length ($prefix) > 20) {
	      $prefix = "..." . substr ($prefix, length($prefix) - 20);
	    }

	    if (length ($suffix) > 20) {
	      $suffix = substr ($suffix, 0, 20) . "...";
	    }

	    $outer_line .= "\t$prefix  \"$tag\"  $suffix";
	  }
	}
      }
      print $outer_line, "\n";
    } split ("\n", $file);

    close OUTPUTFILE;
  }
}



sub cache_xml_fileref {
  my($filename) = @_;

  if (!exists $xml_files{$filename}) {
    my($file);

    if (!read_complete_file ($filename, \$file)) {
      print STDERR "*** Failed to read \"$filename\", skipping\n";
    } else {
      $xml_files{$filename} = \$file;
    }
  }
  return $xml_files{$filename};
}



##############################################################################
### Fix misguided "../.." references

sub correct_pathname {
  my($path) = @_;

### If this path already exists, we're done

  return $path
      if -r $path;

  my($this_dir) = Cwd::cwd;
  my($try) = $path;

### Else try removing "../" prefixes, one by one

  while ($try =~ /^\.\.\//) {
    $try =~ s:^\.\.\/::;
    return $try
	if -r $try;
  }

### If it still doesn't match, AND we're in a source tree somewhere,
### try relative to the src.

  if ($this_dir =~ /^(.*\/src)\//) {
    my($src_dir) = $1;

    my($try) = $path;

    return "$src_dir/$try"
	if -r "$src_dir/$try";

### Else try removing "../" prefixes, one by one, AND prefixing the source dir

    while ($try =~ /^\.\.\//) {
      $try =~ s:^\.\.\/::;
      return $try
	  if -r $try;
    }
  }

  print STDERR "Could not find the RNG Schema file \"$path\" anywhere, sorry\n";
  exit 1;

}


sub do_show_help {
  print <<END;
$0 : Validate XML file

SWITCHES:
    -args  str		Set JAVA ARGUMENTS [\"$java_args\"]
    -java  name		Set JAVA binary name [\"$java\"]
    -path  path		Add to head of SEARCH PATH
    -temp  path		Write TEMPORARY results here [\"$tmp_file\"]
    -verbose \#		Set VERBOSITY (0-10)
END
}




##############################################################################
### FIND_IN_PATH
###
###    Locate a given file somewhere in the list of input paths.  Also
### search in any subdirectories named by $machines, e.g.:
###
###		@paths = ("/home/mwm", "/proj/mer/dev/mwm");
###		@machines = ("linux", "linux2")
###		$fullpath = find_in_path ($file,
###				          \@paths,

###					  \@machines,
###					  $D->{'_verbose'},
###					  "2.4",
###					  "-v");
###
###   Returns   undef   if not found.
###
##############################################################################

sub find_in_path {
  my($file,$path,$machines,$verbose,$version_regexp,$version_argument) = @_;
 
  if ($file =~ /^\// &&
      (-f $file || -d $file)) {
    $file = check_version ($file, $version_regexp, $version_argument)
	if defined $version_regexp;

    if (defined $file) {
      print " == find_in_path: Found \"$file\" without searching\n"
	  if $verbose > 2;
      return $file;
    }
  }

  ### Eliminate any leading path component
  $file =~ s:^.*/::;
 
  ### Search all directories
  my($dir);
 
  if (ref $path ne "ARRAY") {
    print STDERR "Find_in_path: Expecting ARRAY REF paths, got ",
    ref $path, "\n";
    return undef;
  }

  for $dir (@$path) {
    if (-f "$dir/$file" || -d "$dir/$file") {
      my($result) = "$dir/$file";

      $result = check_version ($result, $version_regexp, $version_argument);
      if (defined $result) {
	print " == Found \"$result\" in search path\n"
	    if $verbose > 2;
	return $result;
      }
    }
    my($arch);

    ### Check each possible architecture-based subdirectory
   
    if (defined $machines) {
      if (ref $machines ne "ARRAY") {
	print STDERR "Find_in_path: Expecting ARRAY REF machines, got ",
	ref $machines, "\n";
	return undef;
      }
      for $arch (@$machines) {
	if (-f "$dir/$arch/$file" || -d "$dir/$arch/$file") {
	  my($result) = "$dir/$arch/$file";

	  $result = check_version ($result, $version_regexp, $version_argument);
	  if (defined $result) {
	    print " == Found \"$result\" in search path\n"
		if $verbose > 2;
	    return $result;
	  }
	}
      }
    }
  }

  print STDERR "warning: cannot find $file\n" if $verbose;
  return undef;
}




##############################################################################
### When pasting filenames into system() calls, we have to make sure
### any shell metachars are quoted

sub shell_quote {
  my($str) = @_;

  if ($str !~ /^[-a-z\dA-Z_\.\/]*$/i) {
    print "shell_quote:  Given \"$str\"\n"
	if $verbose > 7;

    my(@a) = split /([^-a-z\dA-Z_:\.\/])/, $str;

    map {
      print "    \"$a[$_*2+1]\" => "
	  if $verbose > 8;
      $a[$_*2+1] = "\\" . $a[$_*2+1];
      print " \"$a[$_*2+1]\"\n"
	  if $verbose > 8;
    } 0..($#a-1)/2;

    $str = join ("", @a);
    print "shell_quote:  Yields \"$str\"\n"
	if $verbose > 7;
  }

  return $str;
}



##############################################################################
### Check the version number of any executable program.
### 	$fullpath -- Full pathname of tool to be checked
###	$version_regexp -- UNDEF, or regexp (MUST INCLUDE "/" CHARS)
###			   describing a successful match
###	$version_argument -- UNDEF, or argument to pass to $fullpath to
###			   cause it to generate a version # (default is
###			   "--version")
sub check_version {
  my($fullpath, $version_regexp, $version_argument) = @_;

  if (!defined $version_regexp) {
    print "  == Skipping version check, no check defined for \"$fullpath\"\n"
	if $verbose > 6;
    return $fullpath;
  }

  print "== Checking \"$fullpath\" for version regexp \"$version_regexp\"\n"
      if $verbose > 4;
  if (!defined $version_argument) {
    $version_argument = "--version";
    print "  == Using default switch \"$version_argument\"\n"
	if $verbose > 8;
  }

  my($cmd) = shell_quote ($fullpath) . " $version_argument ";
  print "== Running \"$cmd\"\n" if $verbose > 2;
  my($result) = join ("", `$cmd 2>&1 < /dev/null`);
  my($status) =  eval "((\$result =~ $version_regexp) ? 1 : 0)";
  print "== Complete text result is \"$result\"\n"
      if $verbose > 9;
  print "== Status is \"$status\" from \"$cmd\"\n"
      if $verbose > 1;
  return $status ? $fullpath : undef;
}



sub read_complete_file {
  my($filename,$ref) = @_;
  my($len);

  if (!sysopen (FILE, "$filename", "O_RDONLY")) {
    print STDERR "*** Failed to open \"$filename\", nothing read!\n";
    return undef;
  }

### Read the whole file at once if possible.  Using sysread() instead
### of <FILE> speeds this up by at least 20%

  my($request) = 1024 * 1024 * 32;

  if (($len = sysread (FILE, ${$ref}, $request)) =~ /^0$/) {
    print STDERR "*** \"$filename\": empty file?!?\n";
    return undef;
  }

### if not possible, keep reading.  sysread returns undef on error,
### 0 on EOF.  CANNOT use eof() here, since that's based on stdio.

  while (defined $len && $len >= $request) {
    my($fragment);

    $len = sysread (FILE, $fragment, $request);
    if (defined $len &&
	$len !~ /^0$/) {
      ${$ref} .= $fragment;
    }
  }

  close FILE;
  return 1;
}

