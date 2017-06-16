#!/tps/bin/perl -w

$maxlen = 0;

$FILE = "file";
$PCT = "pct";
$TESTED = "tested";
$UNTESTED = "untested";
$UNC_ASSERT = "unc_assert";
$LINES = "lines";

$GCOV = "gcov";
$OBJDIR = "linux-linux-x86-debug-gnu-4.4.0-ut";
$SRCDIR = ".";
$GCOVOUTPUT = "";

while (@ARGV) {
  $_ = shift(@ARGV);

  if ($_ eq "-g") {
    die "No argument provided for -gcov option" if (!@ARGV);
    $GCOV = shift(@ARGV);
  } elsif ($_ eq "-o") {
    die "No argument provided for -o option" if (!@ARGV);
    $OBJDIR = shift(@ARGV);
  } elsif ($_ eq "-s") {
    die "No argument provided for -s option" if (!@ARGV);
    $SRCDIR = shift(@ARGV);
  } elsif ($_ eq "-l") {
    die "No argument provided for -l option" if (!@ARGV);
    $GCOVOUTPUT = shift(@ARGV);
  } elsif ($_ eq "-h" || $_ eq "--help") {
    usage(0);
  } else {
    print STDERR "Unrecognized option $_";
    usage(-1);
  }
}


if (!$GCOVOUTPUT) {
  # Read all the *.c and *.cc files in the SRCDIR directory.
  #
  # We use "opendir" instead of `ls` to avoid warnings about
  # *.cc not matching any files.

  if (opendir (DIR, $SRCDIR)) {
    map {
      my($file) = $_;

      if ($file =~ /\.(c|cc)$/) {

        ### Simple "grep" on readdir wouldn't include the SRCDIR path, so add it here.

        push @files, "$SRCDIR/$file";
      }
    } sort readdir DIR;
    closedir DIR;
  } 
  if (!@files) {
    print "No input files provided\n";
    usage(-1);
  }
  my($gcov_command) = "$GCOV -o $OBJDIR " . join (" ", sort @files );
  @gcov_output = `$gcov_command`;
} else {
  @gcov_output = `/bin/cat $GCOVOUTPUT`;
} 

for (@gcov_output) {
  chomp;
  if (/File/) {
    # eat everything up to the name of the file
    ($file = $_) =~ s|^.*/||g;
    $file =~ s/\'//g;
    my $len = length($file);
    $maxlen = $len if ($len > $maxlen);
  } elsif (/Lines executed/) {

    # eat everything up to the colon, leaving "xx.xx% of yyy"
    s/^.*://g;
    my @array = split(/\% of /);
    my %rec;

    $rec{$FILE} = $file;
    $rec{$PCT} = $array[0];
    $rec{$LINES} = $array[1];
    $rec{$TESTED} = int($array[0]*$array[1]/100.0);
    $rec{$UNTESTED} = $rec{$LINES} - $rec{$TESTED};

    filter_assertion_lines(\%rec);

    if ($file =~ /_ac_.*hsm.*/) {
        push(@required_results, \%rec);
    } elsif ($file =~ /_ac_(.*_)?(ipc_init|next_msg|ext\.c|int\.c|params|dispatch\.c|dp\.c|eha\.[ch])/) {
        push(@ac_results, \%rec);
    } elsif ($file !~ /\.h/) {
        push(@required_results, \%rec);
    }
    push (@results, \%rec);
  }
}

summarize_results("All", @results);
summarize_results("Autocoded", @ac_results);
summarize_results("Required", @required_results);

# If a file does not have 100% coverage, count how many lines are
# FSW_ASSERTs that always file.
sub filter_assertion_lines {
  my $rec = $_[0];
  
  if ($rec->{$PCT} != 100.0) {
    my $cmd = 'pcregrep -c "^\s*#+:\s*\d+:\s*FSW_ASSERT(_\d)?\s*\((0|FALSE)(,|\))" ' . $rec->{$FILE} . ".gcov";
    @unconditional_assertions = `$cmd`;
    $rec->{$UNC_ASSERT} = $unconditional_assertions[0];
    $rec->{$PCT} = 100.0*($rec->{$TESTED} + $rec->{$UNC_ASSERT}) / $rec->{$LINES};
    $rec->{$UNTESTED} -= $rec->{$UNC_ASSERT};
  } else {
    $rec->{$UNC_ASSERT} = 0;
  }
}

sub summarize_results {
  my $group = shift(@_);
  my @res = @_;
  my $rec;
  my $total_lines = 0;
  my $tested_lines = 0;
  
  return if (!@res);
  
  print "\n#################### $group files #######################\n";

  for $rec (@res) {
    $total_lines += $rec->{$LINES};
    $tested_lines += $rec->{$PCT}*$rec->{$LINES}/100.0;
  }
  my @sorted_results = sort { -($a->{$PCT} <=> $b->{$PCT}) } @res;
  
  
  printf "%30s \t%6s  \t%6s  \t%s \t%s    %s\n", "Filename", "Percent", "Lines", "Lines",  "Lines",   "Unconditional";
  printf "%30s \t%6s  \t%6s  \t%s \t%s %s\n", "",         "tested",  "total", "tested", "untested", "assertion lines";
  
  for (@sorted_results) {
    printf "%30s \t%-3.2f%%   \t%6s  \t%6d \t%6d\t %6d\n", 
      $_->{$FILE}, $_->{$PCT}, $_->{$LINES}, $_->{$TESTED}, $_->{$UNTESTED}, $_->{$UNC_ASSERT};
  }
  $tested_lines = int($tested_lines);
  my $tested_pct = int(100*$tested_lines/$total_lines);
  print "\n$group coverage:  $tested_lines / $total_lines  lines  ($tested_pct\%)\n";
}


sub usage {
print <<END;

Usage: $0 -s <source-file-dir> [-g <gcov-path>] [-o <obj-dir>] [-h]

  Run gcov on source files for an executable that has been run with code coverage
  enabled.  Either -s or -h must be specified.  -g and -o are optional.

  Options:

  -s <source-file-dir>  Run gcov on .c files found in source-file-dir.
  -h                    Print this informative message and exit.
  -o <obj-dir>          Pass -obj-dir as the -o option to gcov
  -g <gcov-path>        Use alternate gcov binary found at <gcov-path>
  
END

  exit ($_[0]);
}
