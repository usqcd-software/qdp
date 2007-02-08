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
      if($pl) {
	$arg =~ s/\([^()]*$//;
      }
      $arg =~ s/\[.*\]//;
      $arg =~ s/.*[^\w]([\w]+)[^\w]*$/$1/;
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
  return (join(", ",@glist),join(", ",@list));
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

    $nsites = get_nsites($func, "qp[QDP_restart]");

    $def = <<"  EOF;";
#define $func$args { \\
  if(QDP_prof_level>0) { \\
    static QDP_prof qp[2]; \\
    static int first_time=1; \\
    double _time; \\
    if(first_time) { \\
      first_time = 0; \\
      qp[0].count = 0; \\
      qp[0].time = 0; \\
      qp[0].comm_time = 0; \\
      qp[0].math_time = 0; \\
      qp[0].nsites = 0; \\
      qp[0].func = \"$func(no restart)\"; \\
      qp[0].caller = __func__; \\
      qp[0].line = __LINE__; \\
      QDP_register_prof(&qp[0]); \\
      qp[1].count = 0; \\
      qp[1].time = 0; \\
      qp[1].comm_time = 0; \\
      qp[1].math_time = 0; \\
      qp[1].nsites = 0; \\
      qp[1].func = \"$func(restart)\"; \\
      qp[1].caller = __func__; \\
      qp[1].line = __LINE__; \\
      QDP_register_prof(&qp[1]); \\
    } \\
    QDP_keep_time = 1; \\
    QDP_comm_time = 0; \\
    QDP_math_time = 0; \\
    QDP_restart = 0; \\
    _time = QDP_time(); \\
    $func$args; \\
    qp[QDP_restart].time += QDP_time() - _time; \\
    QDP_keep_time = 0; \\
    qp[QDP_restart].comm_time += QDP_comm_time; \\
    qp[QDP_restart].math_time += QDP_math_time; \\
    qp[QDP_restart].count++; \\
    $nsites \\
  } else { \\
    $func$args; \\
  } \\
}
  EOF;

  } elsif( $func =~ /eq/ ) {

    $nsites = get_nsites($func, "qp");

    $def = <<"  EOF;";
#define $func$args { \\
  if(QDP_prof_level>0) { \\
    static QDP_prof qp; \\
    static int first_time=1; \\
    if(first_time) { \\
      first_time = 0; \\
      qp.count = 0; \\
      qp.time = 0; \\
      qp.comm_time = 0; \\
      qp.math_time = 0; \\
      qp.nsites = 0; \\
      qp.func = \"$func\"; \\
      qp.caller = __func__; \\
      qp.line = __LINE__; \\
      QDP_register_prof(&qp); \\
    } \\
    QDP_keep_time = 1; \\
    QDP_comm_time = 0; \\
    QDP_math_time = 0; \\
    qp.count++; \\
    $nsites \\
    qp.time -= QDP_time(); \\
    $func$args; \\
    qp.time += QDP_time(); \\
    QDP_keep_time = 0; \\
    qp.comm_time += QDP_comm_time; \\
    qp.math_time += QDP_math_time; \\
  } else { \\
    $func$args; \\
  } \\
}
  EOF;

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
