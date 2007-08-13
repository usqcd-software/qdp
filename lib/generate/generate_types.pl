#!/usr/bin/perl

if($ARGV[0] eq "-c") {
  ($cflag, $lib, $dir, @srcs) = @ARGV;
  if ( $lib =~ /^[df][df]/ ) {
    exit 0;
  }
} else {
  if($#ARGV!=2) {
    print "$0 <lib> <template> <dest>\n";
    exit 1;
  }
  ($lib, $template, $dest) = @ARGV;
}

$lib = uc $lib;
$llib = lc $lib;
if(($llib=~/df/)||($llib=~/int/)) {
  $elib = $clib = '';
} else {
  ($elib=$llib) =~ s/^f/d/ || $elib =~ s/^d/q/;
  ($clib=$llib) =~ s/^f/df/ || $clib =~ s/^d/dq/;
}

if($lib eq "INT") { $pc = ''; }
else { $pc = "_".$lib; }

($precision = $lib) =~ s/([DF]*)[^DF]*/$1/;
if($lib eq "INT") { $color = ''; }
else { ($color = $lib) =~ s/[^23N]*([23N]*)/$1/; }

$porpc = 'P';
if($color) { $porpc .= 'C'; }

($thisdir = $0) =~ s/[^\/]*$//;
scalar eval `cat ${thisdir}datatypes.pl`;

sub compatible_pc($$$) {
  ($type, $p, $c) = @_;
  if($p) {
    return 0 if($datatypes{$type}{NO_PRECISION});
  } else {
    return 0 if(!$datatypes{$type}{NO_PRECISION});
  }
  if($c) {
    return 0 if($datatypes{$type}{NO_COLOR});
  } else {
    return 0 if(!$datatypes{$type}{NO_COLOR});
  }
  return 1;
}

$us = "_";

if($cflag) {

  for $file (@srcs) {
    for $dt (@all_types) {
      next if(!compatible_pc($dt,$precision,$color));

      #$infile_mtime = (stat($file))[9];
      open(INFILE, "<".$file);
      ($fn = $file) =~ s/.*[\/]//;
      $save = 1;
      $text = "";
      $gauge_mult = 0;
      while(<INFILE>) {

	if(/^!GAUGEMULT1/) {
	  $gauge_mult = 1;
	  if($datatypes{$dt}{NO_COLOR}) { $save = 0; }
	} elsif(/^!GAUGEMULT2/) {
	  $gauge_mult = 2;
	  if($datatypes{$dt}{NO_COLOR}) { $save = 0; }
	} elsif(/^!GAUGEMULT/) {
	  $gauge_mult = 3;
	  if($datatypes{$dt}{NO_COLOR}) { $save = 0; }
	} elsif(/^!PCSHIFTTYPES/) {
	  if($datatypes{$dt}{NO_SHIFT}) { $save = 0; }
	} elsif(/^!PCNOSHIFTTYPES/) {
	  if(!$datatypes{$dt}{NO_SHIFT}) { $save = 0; }
	} elsif(/^!END/) {
	  $save = 1;
	} else {
	  if($save) { $text .= $_; };
	}

      }

      if($text) {
	if($gauge_mult) {
	  $t1 = 'ColorMatrix';
	  $t2 = $t3 = $dt;
	  if($dt eq 'ColorMatrix') { @s2a = ('','a'); } else { @s2a = (''); }
	  for $s1adj ('','a') {
	    for $s2adj (@s2a) {

	      $outfn = $dir."/".$fn;
	      $outfn =~ s/QDP/QDP$pc/;
	      $outfn =~ s/T1/$datatypes{$t1}{ABBR}$s1adj/g;
	      $outfn =~ s/T2/$datatypes{$t2}{ABBR}$s2adj/g;
	      $outfn =~ s/T3/$datatypes{$t3}{ABBR}/g;

	      if($color eq 'N') {
		$ncarg = "int nc, ";
		$ncvoid = "int nc";
		$ncvar = "nc, ";
	      } else {
		$ncarg = "";
		$ncvoid = "void";
		$ncvar = "";
	      }
	      $temp = $text;
	      $temp =~ s/\$ABBR1/$datatypes{$t1}{ABBR}/g;
	      $temp =~ s/\$ABBR2/$datatypes{$t2}{ABBR}/g;
	      $temp =~ s/\$ABBR3/$datatypes{$t3}{ABBR}/g;
	      $temp =~ s/\$ADJ1/$s1adj/g;
	      $temp =~ s/\$ADJ2/$s2adj/g;
	      $temp =~ s/\$QDPPCTYPE1/QDP$pc$us$t1/g;
	      $temp =~ s/\$QDPPCTYPE2/QDP$pc$us$t2/g;
	      $temp =~ s/\$QDPPCTYPE3/QDP$pc$us$t3/g;
	      $temp =~ s/\$QLAPCTYPE1/QLA$pc$us$t1/g;
	      $temp =~ s/\$QLAPCTYPE2/QLA$pc$us$t2/g;
	      $temp =~ s/\$QLAPCTYPE3/QLA$pc$us$t3/g;
	      $temp =~ s/\$NCVOID/$ncvoid/g;
	      $temp =~ s/\$NCVAR/$ncvar/g;
	      $temp =~ s/\$NC/$ncarg/g;
	      $temp =~ s/\$PC/$pc/g;
	      $temp =~ s/\$PORPC/$porpc/g;
	      $temp =~ s/\$LIB/$lib/g;
	      $temp =~ s/\$lib/$llib/g;
	      for $eqop ('eq','peq','meq','eqm') {
		$ttemp = $temp;
		$ttemp =~ s/\$EQOP/$eqop/g;
		$toutfn = $outfn;
		$toutfn =~ s/EQOP/$eqop/;
		#if((stat($toutfn))[9]<$infile_mtime) {
	        open(OUTFILE, ">".$toutfn);
	        print OUTFILE $ttemp;
                close(OUTFILE);
		#}
	      }
	    }
	  }
	} else { # not $gauge_mult
	  $outfn = $dir."/".$fn;
	  $outfn =~ s/QDP/QDP$pc/;
	  $outfn =~ s/T/$datatypes{$dt}{ABBR}/g;

	  if($color eq 'N') {
	    $ncarg = "int nc, ";
	    $ncvoid = "int nc";
	    $ncvar = "nc, ";
	    $qdp_nc = "nc";
	    $cn = "_N";
	  } else {
	    $ncarg = "";
	    $ncvoid = "void";
	    $ncvar = "";
	    $qdp_nc = $color;
	    if(!$qdp_nc) { $qdp_nc = "0"; }
	    $cn = "";
	  }
	  $temp = $text;
	  $temp =~ s/\$TYPE/$dt/g;
	  $temp =~ s/\$ABBR/$datatypes{$dt}{ABBR}/g;
	  $qlaabbr = lc($datatypes{$dt}{ABBR});
	  $temp =~ s/\$QLAABBR/$qlaabbr/g;
	  $temp =~ s/\$QDPTYPE/QDP$us$dt/g;
	  $temp =~ s/\$QDPPTYPE/QDP$us$precision$us$dt/g;
	  $temp =~ s/\$QDPPCTYPE/QDP$pc$us$dt/g;
	  $temp =~ s/\$QLAPCTYPE/QLA$pc$us$dt/g;
	  $temp =~ s/\$NCVOID/$ncvoid/g;
	  $temp =~ s/\$NCVAR/$ncvar/g;
	  $temp =~ s/\$NC/$ncarg/g;
	  $temp =~ s/\$QDP_NC/$qdp_nc/g;
	  $temp =~ s/\$PC/$pc/g;
	  $temp =~ s/\$PORPC/$porpc/g;
	  $temp =~ s/\$LIB/$lib/g;
	  $temp =~ s/\$lib/$llib/g;
	  $temp =~ s/\$P/$precision/g;
	  $temp =~ s/\$C/$color/g;
	  $temp =~ s/\$N/$cn/g;
          #if((stat($outfn))[9]<$infile_mtime) {
	    open(OUTFILE, ">".$outfn);
	    print OUTFILE $temp;
            close(OUTFILE);
          #}
	}
      }

    }
  }

} else { # not cflag

  open(INFILE, "<".$template);
  open(OUTFILE, ">".$dest);

  $begin_types = 0;
  $begin_eqops = 0;
  $begin_qlaelibs = 0;
  while(<INFILE>) {

    if(/^!END/) {
      if($begin_qlaelibs) {
	if($elib) {
	  ($temp=$text) =~ s/\$elib/$elib/g;
	  print OUTFILE $temp;
	  ($temp=$text) =~ s/\$elib/$clib/g;
	  print OUTFILE $temp;
	  if($llib=~/[23n]$/) {
	    ($tlib=$llib) =~ s/[23n]$//g;
	    ($temp=$text) =~ s/\$elib/$tlib/g;
	    print OUTFILE $temp;
	    ($tlib=$elib) =~ s/[23n]$//g;
	    ($temp=$text) =~ s/\$elib/$tlib/g;
	    print OUTFILE $temp;
	    ($tlib=$clib) =~ s/[23n]$//g;
	    ($temp=$text) =~ s/\$elib/$tlib/g;
	    print OUTFILE $temp;
	  }
	}
	$begin_qlaelibs = 0;
      } else {
	if($begin_types) { @ta = (@all_types); }
	else { @ta = ('ColorMatrix'); $begin_types = 8; }
	for $dt (@ta) {
	  for $tp ('','F','D') {
	    for $tc ('','2','3','N') {
	      next if(!compatible_pc($dt,$tp,$tc));
	      next if((($begin_types==2)||($begin_types==4))&&($datatypes{$dt}{NO_SHIFT}));
	      next if((($begin_types==3)||($begin_types==4)||($begin_types==6)||($begin_types==8))&&
		      (($tp ne $precision)||($tc ne $color)));
	      next if(($begin_types==5)&&((($tp)&&($tp ne $precision))||(($tc)&&($tc ne $color))));
	      next if((($begin_types==6)||($begin_types==7))&&(!$datatypes{$dt}{NO_SHIFT}));
	      next if(($begin_types==8)&&($datatypes{$dt}{NO_COLOR}));
	      $tpc = $tp.$tc;
	      if($tpc) { $tpc = "_".$tpc; }
	      if($tc eq 'N') {
		$ncarg = "int nc, ";
		$ncvoid = "int nc";
		$qdpncvar = "QDP_Nc, ";
	      } else {
		$ncarg = "";
		$ncvoid = "void";
		$qdpncvar = "";
	      }
	      $temp = $text;
	      $temp =~ s/\$PC/$tpc/g;
	      $temp =~ s/\$P/$tp/g;
	      $temp =~ s/\$TYPE/$dt/g;
	      $temp =~ s/\$ABBR/$datatypes{$dt}{ABBR}/g;
	      $qlaabbr = lc($datatypes{$dt}{ABBR});
	      $temp =~ s/\$QLAABBR/$qlaabbr/g;
	      $temp =~ s/\$QDPTYPE/QDP$us$dt/g;
	      $temp =~ s/\$QDPPTYPE/QDP$us$precision$us$dt/g;
	      $temp =~ s/\$QDPPCTYPE/QDP$tpc$us$dt/g;
	      $temp =~ s/\$QLAPCTYPE/QLA$tpc$us$dt/g;
	      $temp =~ s/\$NCVOID/$ncvoid/g;
	      $temp =~ s/\$QDPNCVAR/$qdpncvar/g;
	      $temp =~ s/\$NC/$ncarg/g;
	      if($begin_eqops) {
		for $eqop ('eq','peq','meq','eqm') {
		  $ttemp = $temp;
		  $ttemp =~ s/\$EQOP/$eqop/g;
		  print OUTFILE $ttemp;
		}
	      } else {
		print OUTFILE $temp;
	      }
	    }
	  }
	}
	$begin_types = 0;
	$begin_eqops = 0;
      }
    } elsif(/^!/) {
      if(/^!ALLTYPES/) { # all types
	$begin_types = 1;
	$text = '';
      } elsif(/^!SHIFTTYPES/) { # shiftable types
	$begin_types = 2;
	$text = '';
      } elsif(/^!PCTYPES/) { # types with same PC as lib
	$begin_types = 3;
	$text = '';
      } elsif(/^!PCSHIFTTYPES/) { # shiftable types with same PC as lib
	$begin_types = 4;
	$text = '';
      } elsif(/^!PCSUBTYPES/) { # types that are compatible with PC of lib
	$begin_types = 5;
	$text = '';
      } elsif(/^!PCNOSHIFTTYPES/) { # non-shiftable types with same PC as lib
	$begin_types = 6;
	$text = '';
      } elsif(/^!NOSHIFTTYPES/) { # non-shiftable types
	$begin_types = 7;
	$text = '';
      } elsif(/^!PCCOLORTYPES/) { # colored types
	$begin_types = 8;
	$text = '';
      } elsif(/^!EQOPS/) {
	$begin_eqops = 1;
	$text = '';
      } elsif(/^!QLAELIBS/) {
	$begin_qlaelibs = 1;
	$text = '';
      } else {
	die "error unknown tag in generate_types.pl:\n $_";
      }
    } elsif($begin_types||$begin_qlaelibs||$begin_eqops) {
      $text .= $_;
    } else {
      $line = $_;
      $line =~ s/\$PC/$pc/g;
      $line =~ s/\$PORPC/$porpc/g;
      $line =~ s/\$P/$precision/g;
      $line =~ s/\$LIB/$lib/g;
      $line =~ s/\$lib/$llib/g;
      $line =~ s/\$clib/$clib/g;
      print OUTFILE $line;
    }

  }

}
