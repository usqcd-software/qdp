my(%datatypes);
my(@all_types, @shift_types, @float_types, @complex_types, @color_types);
scalar eval `cat ../lib/generate/datatypes.pl`;

my @eqops  = (  "eq",  "peq",  "meq",  "eqm" );
my @veqops = ( "veq", "vpeq", "vmeq", "veqm" );

open OUTFILE, ">functions.texi";

$last_section = "";
sub comment0($) {
  if($last_section) {
    ($tag = $last_section) =~ s/ .*//;
    $tag = lc $tag;
    print OUTFILE "\@functionhook$tag \n\n";
  }
  print OUTFILE "\@section @_\n";
  ($last_section) = @_;
}

sub comment1($) {
  #print OUTFILE "\@subsection @_\n\n";
  #print OUTFILE "\@unnumberedsubsec @_\n\n";
  print OUTFILE "\@subheading @_\n\n";
}

sub comment2($) {
  ($desc) = @_;
}

sub abbr($) {
  my($t) = @_;
  my($abbr) = $datatypes{$t}{ABBR};
  return $abbr;
}

sub typelist(\@) {
  my($r) = @_;
  my(@l) = ();
  for my $t (@{$r}) {
    push @l, abbr($t);
  }
  my($s) = join(", ",@l);
  $s =~ s/,/\@comma{}/g;
  return $s;
}

sub make_functions(\%) {
  my($arg) = @_;

  $s1 = 0;
  $s2 = 0;
  $prot = "QDP";
  $args = "";
  $vargs = "";
  $macro = "\@func";
  $t = "";
  $e = "";
  $f = "";
  @vz = ();

  if($arg->{DEST_TYPES}) {
    @z = @{$arg->{DEST_TYPES}};
    if($arg->{DEST_SCALAR}) {
      $ta = "t";
      $tt = "QLA_Type";
      $tq = "QLA_";
      $tz = lc abbr($z[0]);
      $dva = "dest[]";
    } else {
      $ta = "T";
      $tt = "Type";
      $tq = "QDP_";
      $tz = abbr($z[0]);
      $dva = "*dest[]";
    }
    if($arg->{DEST_MULTI}) {
      $da = $dva;
    } else {
      $da = "*dest";
    }
    if($#z==0) {
      $prot .= "_".$tz;
      $args .= $tq.$z[0]." $da";
      $vargs .= $tq.$z[0]." $dva";
    } else {
      $prot .= "_\@var{$ta}";
      $args .= "\@var{$tt} $da";
      $vargs .= "\@var{$tt} $dva";
      $t = typelist(@z);
      $macro .= "t";
    }
  }

  if($arg->{EQ_OPS}) {
    @z = ();
    foreach (@{$arg->{EQ_OPS}}) {
      if(/^v/) {
	push @vz, $_;
      } else {
	push @z, $_;
      }
    }
    if($#z==0) {
      $prot .= "_".$z[0];
    } else {
      $prot .= "_\@var{eqop}";
      ($e = join(", ",@z)) =~ s/,/\@comma{}/g;
      $macro .= "e";
    }
  }

  $xa = $arg->{EXTRA_ARGS1};
  if($xa) {
    $vxa = $xa;
    $xa =~ s/,[ ]*$//;
    $args .= ", $xa";
    $vxa =~ s/,/[],/g;
    $vxa =~ s/,[ ]*$//;
    $vargs .= ", $vxa";
  }

  if($arg->{SRC1_TYPES}) {
    $s1 = 1;
    @z = @{$arg->{SRC1_TYPES}};
    $a = "";
    $a = "a" if($arg->{SRC1_ADJ});
    if($arg->{SRC1_SCALAR}) {
      $ta = "t";
      $tt = "QLA_Type";
      $tq = "QLA_";
    } else {
      $ta = "T";
      $tt = "Type";
      $tq = "QDP_";
    }
    if($#z==0) {
      if($z[0] eq "DEST") {
	if($t) {
	  $prot .= "_\@var{$ta}$a";
	  $args .= ", \@var{$tt} *src1";
	  $vargs .= ", \@var{$tt} *src1[]";
	} else {
	  @z = @{$arg->{DEST_TYPES}};
	}
      }
      if(!($z[0] eq "DEST")) {
	$tz = abbr($z[0]);
	if($arg->{SRC1_SCALAR}) { $tz = lc $tz; }
	$prot .= "_".$tz.$a;
	$args .= ", $tq".$z[0]." *src1";
	$vargs .= ", $tq".$z[0]." *src1[]";
      }
    } else {
      $prot .= "_\@var{$ta}$a";
      $args .= ", \@var{$tt} *src1";
      $vargs .= ", \@var{$tt} *src1[]";
      $t = typelist(@z);
      $macro .= "t";
    }
  }

  if($arg->{FUNCS}) {
    @z = @{$arg->{FUNCS}};
    if($#z==0) {
      $prot .= "_".$z[0];
    } else {
      $prot .= "_\@var{func}";
      ($f = join(", ",@{$arg->{FUNCS}})) =~ s/,/\@comma{}/g;
      $macro .= "f";
    }
  }

  $xa = $arg->{EXTRA_ARGS2};
  if($xa) {
    $vxa = $xa;
    $xa =~ s/,[ ]*$//;
    $args .= ", $xa";
    $vxa =~ s/,/[],/g;
    $vxa =~ s/,[ ]*$//;
    $vargs .= ", $vxa";
  }

  if($arg->{SRC2_TYPES}) {
    $s2 = 1;
    @z = @{$arg->{SRC2_TYPES}};
    $a = "";
    $a = "a" if($arg->{SRC2_ADJ});
    if($#z==0) {
      if(($z[0] eq "DEST")||($z[0] eq "SRC1")) {
	if($t) {
	  $prot .= "_\@var{T}$a";
	  $args .= ", \@var{Type} *src2";
	  $vargs .= ", \@var{Type} *src2[]";
	} else {
	  if($z[0] eq "DEST") {
	    @z = @{$arg->{DEST_TYPES}};
	  } else {
	    @z = @{$arg->{SRC1_TYPES}};
	  }
	}
      }
      if(!(($z[0] eq "DEST")||($z[0] eq "SRC1"))) {
	$prot .= "_".abbr($z[0]).$a;
	$args .= ", QDP_".$z[0]." *src2";
	$vargs .= ", QDP_".$z[0]." *src2[]";
      }
    } else {
      $prot .= "_\@var{T}$a";
      $args .= ", \@var{Type} *src2";
      $vargs .= ", \@var{Type} *src2[]";
      $t = typelist(@z);
      $macro .= "t";
      $macrs =~ s/ft/tf/;
    }
  }

  $xa = $arg->{EXTRA_ARGS3};
  if($xa) {
    $vxa = $xa;
    $xa =~ s/,[ ]*$//;
    $args .= ", $xa";
    $vxa =~ s/,/[],/g;
    $vxa =~ s/,[ ]*$//;
    $vargs .= ", $vxa";
  }

  if($s1==0) { $args =~ s/src2/src/g; }
  if($s2==0) { $args =~ s/src1/src/g; }
  $args =~ s/dest/r/;
  $args =~ s/src1/a/;
  $args =~ s/src2/b/;
  $args =~ s/src/a/;
  if($s1==0) { $vargs =~ s/src2/src/g; }
  if($s2==0) { $vargs =~ s/src1/src/g; }
  $vargs =~ s/dest/r/;
  $vargs =~ s/src1/a/;
  $vargs =~ s/src2/b/;
  $vargs =~ s/src/a/;
  if($arg->{DEST_MULTI}) {
    $args .= ", QDP_Subset subset[], int n";
    $vargs .= ", QDP_Subset subset[]";
    $prot .= "_multi";
  } else {
    $args .= ", \@var{subset}";
    $vargs .= ", \@var{subset}";
  }
  #($vargs = $args) =~ s/,/\[\]\@comma{}/g;
  $args =~ s/,/\@comma{}/g;
  $vargs =~ s/,/\@comma{}/g;
  if($#vz>=0) {
    $macro =~ s/func/funcv/;
    ($vprot = $prot) =~ s/_([^_]*eq[^_]*)_/_v\1_/;
    $vargs .= "\@comma{} int n";
    $args .= ",\n$vprot,\n$vargs";
  }
  ($d = $desc) =~ s/,/\@comma{}/g;
  $d =~ s/<([^>]*)>/\@var{\1}/g;

  print OUTFILE $macro, "{\n";
  print OUTFILE $prot, ",\n";
  print OUTFILE $args, ",\n";
  print OUTFILE $d;
  print OUTFILE ",\n", $t if($t);
  print OUTFILE ",\n", $e if($e);
  print OUTFILE ",\n", $f if($f);
  print OUTFILE "}\n\n";
}

# Last we actually create the functions

scalar eval `cat ../lib/generate/functions.pl`;

close OUTFILE;
