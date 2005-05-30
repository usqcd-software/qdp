($gc, $lib, $infile, $outfile) = @ARGV;

if($gc eq 'g') {
  if (! $lib =~ /[df][23n]*/) {
    exit 0;
  }
} else {
  if (! $lib =~ /[df][23n]/) {
    exit 0;
  }
}

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
  $prot = substr($prot,0,$i);
  $name = $prot;
  $name =~ s/.*[\s]+([^\s]+)[\s]*/$1/;
  return($name,$args);
}

sub get_generic_name($) {
  ($name) = @_;
  $gname = $name;
  if($gc eq 'g') {
    $gname =~ s/QDP_[DF23N]+/QDP/;
  } else {
    $gname =~ s/(QDP_[DF]+)[23N]+/\1/;
  }
  return $gname;
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
    ($op, $cp) = count_paren($_);
    if($pl==0) {
      $pl += $op - $cp;
      if($pl) {
	$arg =~ s/\([^()]*$//;
      }
      $arg =~ s/.*[^\w]([\w]+)[^\w]*$/$1/;
      if($arg ne "void") { push @list, $arg; }
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
  $gname = get_generic_name($name);
  ($garglist, $arglist) = get_arg_lists($args, $pc);
  #print $gname, " : ", $arglist, "\n";
  print OUTFILE "#define ", $gname, "( ", $garglist, " ) \\\n";
  print OUTFILE "        ", $name, "( ", $arglist, " )\n";
}

print OUTFILE "\n#endif\n";

close INFILE;
close OUTFILE;
