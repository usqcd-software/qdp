#!/usr/bin/perl

($infile, $outfile) = @ARGV;

open(INFILE, "<".$infile);
open(OUTFILE, ">>".$outfile);

sub get_name_args($) {
  ($prot) = @_;

  $len = length($prot);
  $par = 0;
  for($i=$len-1; $i>0; $i--) {
    if(substr($prot,$i,1) eq ')') {
      if($par==0) { $j = $i; }
      ++$par;
    }
    if(substr($prot,$i,1) eq '(') {
      --$par;
      last if($par==0);
    }
  }
  $args = substr($prot,$i+1,$j-$i-1);
  $name = substr($prot,0,$i);
  $name =~ s/.*[\s]+([^\s]+)[\s]*/$1/;
  return($name,$args);
}

sub count_paren($) {
  ($string) = @_;
  $string = "dummy".$string."dummy";
  my($op, $cp) = (0,0);
  my(@t);
  @t = split(/\(/, $string);
  $op = $#t;
  @t = split(/\)/, $string);
  $cp = $#t;
  return ($op, $cp);
}

sub get_arg_lists($$) {
  ($args, $pc) = @_;
  my(@list, $op, $cp);
  my($pl) = 0;
  for(split(',',$args)) {
    $arg = $_;
    ($op, $cp) = count_paren($arg);
    if($pl==0) {
      $pl += $op - $cp;
#      if($pl) {
#	$arg =~ s/\([^()]*$//;
#      }
#      $arg =~ s/\[.*\]//;
      if($pl) {
	$arg = "func";
      } else {
	$arg =~ s/.*[^\w]([\w]+)[^\w]*$/$1/;
      }
      if($arg ne "void") { push @list, "_".$arg; }
    } else {
      $pl += $op - $cp;
    }
  }
  if($pc=~/.*N/) {
    shift @list;
    @glist = @list;
    unshift @list, 'QDP_Nc';
  } else {
    @glist = @list;
  }
  return (join(",",@glist),join(",",@list));
}

sub get_nsites($$) {
  ($func, $var) = @_;

  if( $func =~ /_multi/ ) {
    $nsites = "{ int _i; for(_i=0; _i<_ns; ++_i) $var.nsites += QDP_subset_len(_subset[_i]); }";
  } elsif( $func =~ /_v[pm]?eq[pm]?_/ ) {
    $nsites = "$var.nsites += _nv*QDP_subset_len(_subset);";
  } else {
    $nsites = "$var.nsites += QDP_subset_len(_subset);";
  }
  return $nsites;
}

sub def_prof($$) {
  ($func, $args) = @_;

  #if( $func =~ /_[A-Z]_v*eq_s[A-Z]/ ) {
  if( $func =~ /_s[A-Z]/ ) {

    if( $func =~ /_multi/ ) {
      $def = "#define $func$args QDP_PROFSM($func,$args,_subset,_ns)\n";
    } elsif( $func =~ /_v[pm]?eq[pm]?_/ ) {
      $def = "#define $func$args QDP_PROFSV($func,$args,_subset,_nv)\n";
    } else {
      $def = "#define $func$args QDP_PROFSX($func,$args,_subset)\n";
    }

  } elsif( $func =~ /eq/ ) {

    if( $func =~ /_multi/ ) {
      $def = "#define $func$args QDP_PROFXM($func,$args,_subset,_ns)\n";
    } elsif( $func =~ /_v[pm]?eq[pm]?_/ ) {
      $def = "#define $func$args QDP_PROFXV($func,$args,_subset,_nv)\n";
    } else {
      $def = "#define $func$args QDP_PROFXX($func,$args,_subset)\n";
    }

  }
  return $def;
}

while(<INFILE>) {

  chomp;
  /\*\// && do {
    if($in_comment) {
      $in_comment = 0;
      s/^.*\*\///;
    } else {
      s/\/\*.*\*\///;
    }
  };
  next if $in_comment;
  /\/\*/ && do {
    $in_comment = 1;
    s/\/\*.*$//;
  };
  /^\s*$/ && next;
  /^\s*\#/ && next;
  /^\s*extern\s*"C"/ && next;
  /^\s*}/ && next;

  ($name, $args) = get_name_args($_);
  #print $name, ":", $args, "\n";
  ($pc = $name) =~ s/QDP_([DF23N]+)/\1/;
  ($garglist, $arglist) = get_arg_lists($args, $pc);
  #print $gname, " : ", $arglist, "\n";
  $arglist = "(".$arglist.")";
  print OUTFILE def_prof($name, $arglist);
}

print OUTFILE "\n#endif\n";

close INFILE;
close OUTFILE;
