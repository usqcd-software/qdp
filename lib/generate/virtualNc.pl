#!/usr/bin/perl

($infile, $outfile) = @ARGV;

($def = $outfile) =~ s|.*/(qdp_.*).h|_$1_H|;
$def = uc $def;
($p = $outfile) =~ s|.*qdp_([dfq]+).*|$1|;
$P = uc $p;
($P0 = $P) =~ s/(.).*/$1/;
($P1 = $P) =~ s/.*(.)/$1/;

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
  $name =~ s/.*[\s]+[*]*([^\s]+)[\s]*/$1/;
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
  ($args, $name) = @_;
  my(@list, $op, $cp);
  my($pl) = 0;
  for(split(',',$args)) {
    $arg = $_;
    ($op, $cp) = count_paren($_);
    $pl += $op - $cp;
    if($pl==0) {
      $arg =~ s/.*[^\w]([\w]+)[^\w]*$/$1/;
      if($arg ne "void") { push @list, $arg; }
    }
  }
  $has_nc = 0;
  if($list[0] eq "nc") {
    shift @list;
    $has_nc = 1;
  }
  @olist = @list;

  if( ($name=~/spproj/) || ($name=~/sprecon/)  || ($name=~/colorvec/) ||
      ($name=~/elem/)   || ($name=~/spintrace/) || ($name=~/antiherm/) ||
      ($name=~/inverse/) || ($name=~/invsqrt/)  || ($name=~/invsqrtPH/) ||
      ($name=~/times_nV/) || ($name=~/P_times/) || ($name=~/Pa_times/) ||
      ($name=~/create/) || ($name=~/destroy/) || ($name=~/get/) ||
      ($name=~/expose/) || ($name=~/reset/) || ($name=~/extract/) ||
      ($name=~/site_ptr/) || ($name=~/discard/) || ($name=~/func/) ||
      ($name=~/_s/) || ($name=~/insert/)
      ) {
    $oname = $name;
    if($#list>=0) {
      @list = ( "..." );
      @olist = ( "__VA_ARGS__" );
    }
    if($has_nc==1) { unshift @olist, '1'; }
  } else {
    $hasspin = 0;
    @on = ();
    $an = 0;
    $veq = "";
    for(split('_',$name)) {
      /^v[mp]?eq[m]?$/ && do { $veq = "*"; };
    }
    for(split('_',$name)) {
      $t = $_;
      /^[s]?[HDP][a]?$/ && $hasspin++;
      /^[hdp][a]?$/ && $hasspin++;
      /^[s]?[VM][a]?$/ && do {
	$t =~ s/[VM]/C/;
	($ppn = $name) =~ s/.*QDP_([DFQ]+).*/$1/;
	if($an==0) { ($pn = $ppn) =~ s/(.).*/$1/; }
	else { ($pn = $ppn) =~ s/.*(.)/$1/; }
	$ct = "QDP_${pn}_Complex *$veq";
	$olist[$an] = "($ct)($olist[$an])";
      };
      /^[vm][a]?$/ && do {
	$t =~ s/[vm]/c/;
	($ppn = $name) =~ s/.*QDP_([DFQ]+).*/$1/;
	if($an==0) { ($pn = $ppn) =~ s/(.).*/$1/; }
	else { ($pn = $ppn) =~ s/.*(.)/$1/; }
	$ct = "QLA_${pn}_Complex *$veq";
	$olist[$an] = "($ct)($olist[$an])";
      };
      /^[s]?[ISRCVHDMP][a]?$/ && $an++;
      /^[srcvhdmp][a]?$/ && $an++;
      push @on, $t;
    }
    if($hasspin>0) {
      if($has_nc==1) { unshift @olist, '1'; }
    } else {
      $on[1] =~ s/N//;
    }
    $oname = join("_",@on);
    $oname =~ s/transpose_C/C/;
    $oname =~ s/conj_C/Ca/;
    $oname =~ s/trace_C/C/;
    $oname =~ s/det_C/C/;
    $oname =~ s/eigenvals_C/C/;
    $oname =~ s/eigenvalsH_C/C/;
    $oname =~ s/sqrt_C/csqrt_C/;
    $oname =~ s/sqrtPH_C/csqrt_C/;
    $oname =~ s/exp_C/cexp_C/;
    $oname =~ s/expA_C/cexp_C/;
    $oname =~ s/expTA_C/cexp_C/;
    $oname =~ s/log_C/clog_C/;
  }
  return (join(", ",@list),join(", ",@olist),$oname);
}

open(INFILE, "<".$infile);
open(OUTFILE, ">".$outfile);

print OUTFILE "#ifndef $def\n";
print OUTFILE "#define $def\n\n";
print OUTFILE "#include <qdp_f.h>\n";
print OUTFILE "#include <qdp_d.h>\n";
print OUTFILE "#include <qdp_df.h>\n";
print OUTFILE "#include <qdp_fn.h>\n";
print OUTFILE "#include <qdp_dn.h>\n";
print OUTFILE "#include <qdp_dfn.h>\n";

print OUTFILE <<EOF;

#define QDP_${P}1_ColorVector QDP_${P}N_ColorVector
#define QDP_${P}1_HalfFermion QDP_${P}N_HalfFermion
#define QDP_${P}1_DiracFermion QDP_${P}N_DiracFermion
#define QDP_${P}1_ColorMatrix QDP_${P}N_ColorMatrix
#define QDP_${P}1_DiracPropagator QDP_${P}N_DiracPropagator

#define QDP_${P}1_read_V(qr,md,field) QDP_${P}N_read_V(qr,md,(QDP_${P}N_ColorVector*)(field))
#define QDP_${P}1_write_V(qw,md,field) QDP_${P}N_write_V(qw,md,(QDP_${P}N_ColorVector*)(field))
#define QDP_${P}1_vread_V(qr,md,field,n) QDP_${P}N_vread_V(qr,md,(QDP_${P}N_ColorVector**)(field),n)
#define QDP_${P}1_vwrite_V(qw,md,field,n) QDP_${P}N_vwrite_V(qw,md,(QDP_${P}N_ColorVector**)(field),n)
#define QDP_${P}1_vread_v(qr,md,field,n) QDP_${P}N_vread_v(1,qr,md,(QLA_${P}N_ColorVector(1,(*)))(field),n)
#define QDP_${P}1_vwrite_v(qw,md,field,n) QDP_${P}N_vwrite_v(1,qw,md,(QLA_${P}N_ColorVector(1,(*)))(field),n)

#define QDP_${P}1_read_H(qr,md,field) QDP_${P}N_read_H(qr,md,(QDP_${P}N_HalfFermion*)(field))
#define QDP_${P}1_write_H(qw,md,field) QDP_${P}N_write_H(qw,md,(QDP_${P}N_HalfFermion*)(field))
#define QDP_${P}1_vread_H(qr,md,field,n) QDP_${P}N_vread_H(qr,md,(QDP_${P}N_HalfFermion**)(field),n)
#define QDP_${P}1_vwrite_H(qw,md,field,n) QDP_${P}N_vwrite_H(qw,md,(QDP_${P}N_HalfFermion**)(field),n)
#define QDP_${P}1_vread_h(qr,md,field,n) QDP_${P}N_vread_h(1,qr,md,(QLA_${P}N_HalfFermion(1,(*)))(field),n)
#define QDP_${P}1_vwrite_h(qw,md,field,n) QDP_${P}N_vwrite_h(1,qw,md,(QLA_${P}N_HalfFermion(1,(*)))(field),n)

#define QDP_${P}1_read_D(qr,md,field) QDP_${P}N_read_D(qr,md,(QDP_${P}N_DiracFermion*)(field))
#define QDP_${P}1_write_D(qw,md,field) QDP_${P}N_write_D(qw,md,(QDP_${P}N_DiracFermion*)(field))
#define QDP_${P}1_vread_D(qr,md,field,n) QDP_${P}N_vread_D(qr,md,(QDP_${P}N_DiracFermion**)(field),n)
#define QDP_${P}1_vwrite_D(qw,md,field,n) QDP_${P}N_vwrite_D(qw,md,(QDP_${P}N_DiracFermion**)(field),n)
#define QDP_${P}1_vread_d(qr,md,field,n) QDP_${P}N_vread_d(1,qr,md,(QLA_${P}N_DiracFermion(1,(*)))(field),n)
#define QDP_${P}1_vwrite_d(qw,md,field,n) QDP_${P}N_vwrite_d(1,qw,md,(QLA_${P}N_DiracFermion(1,(*)))(field),n)

#define QDP_${P}1_read_M(qr,md,field) QDP_${P}N_read_M(qr,md,(QDP_${P}N_ColorMatrix*)(field))
#define QDP_${P}1_write_M(qw,md,field) QDP_${P}N_write_M(qw,md,(QDP_${P}N_ColorMatrix*)(field))
#define QDP_${P}1_vread_M(qr,md,field,n) QDP_${P}N_vread_M(qr,md,(QDP_${P}N_ColorMatrix**)(field),n)
#define QDP_${P}1_vwrite_M(qw,md,field,n) QDP_${P}N_vwrite_M(qw,md,(QDP_${P}N_ColorMatrix**)(field),n)
#define QDP_${P}1_vread_m(qr,md,field,n) QDP_${P}N_vread_m(1,qr,md,(QLA_${P}N_ColorMatrix(1,(*)))(field),n)
#define QDP_${P}1_vwrite_m(qw,md,field,n) QDP_${P}N_vwrite_m(1,qw,md,(QLA_${P}N_ColorMatrix(1,(*)))(field),n)

#define QDP_${P}1_read_P(qr,md,field) QDP_${P}N_read_P(qr,md,(QDP_${P}N_DiracPropagator*)(field))
#define QDP_${P}1_write_P(qw,md,field) QDP_${P}N_write_P(qw,md,(QDP_${P}N_DiracPropagator*)(field))
#define QDP_${P}1_vread_P(qr,md,field,n) QDP_${P}N_vread_P(qr,md,(QDP_${P}N_DiracPropagator**)(field),n)
#define QDP_${P}1_vwrite_P(qw,md,field,n) QDP_${P}N_vwrite_P(qw,md,(QDP_${P}N_DiracPropagator**)(field),n)
#define QDP_${P}1_vread_p(qr,md,field,n) QDP_${P}N_vread_p(1,qr,md,(QLA_${P}N_DiracPropagator(1,(*)))(field),n)
#define QDP_${P}1_vwrite_p(qw,md,field,n) QDP_${P}N_vwrite_p(1,qw,md,(QLA_${P}N_DiracPropagator(1,(*)))(field),n)

EOF

while(<INFILE>) {

  chomp;
  /\*\// && do { # possible end comment
    if($in_comment) {
      $in_comment = 0;
      s/^.*\*\///;
    } else {
      s/\/\*.*\*\///;
    }
  };
  next if $in_comment;
  /\/\*/ && do { # possible begin comment
    $in_comment = 1;
    s/\/\*.*$//;
  };
  /^\s*$/ && next;
  /^\s*\#/ && next;
  /^\s*extern\s*"C"/ && next;
  /^\s*}/ && next;

  ($name, $args) = get_name_args($_);
  #print $name, ":", $args, "\n";
  ($nname = $name) =~ s/(QDP_[FDQ]+)N/${1}1/;
  ($narglist, $oarglist, $oname) = get_arg_lists($args, $name);
  #print $gname, " : ", $arglist, "\n";
  print OUTFILE "#define ", $nname, "( ", $narglist, " ) \\\n";
  print OUTFILE "        ", $oname, "( ", $oarglist, " )\n";
}

if($P0 eq $P1) {
  print OUTFILE <<EOF;

/* Translation to precision-generic names */
#if QDP_Precision == '$P'
#include <qdp_${p}1_precision_generic.h>
#endif

/* Translation to color-generic names */
#if QDP_Colors == 1
#include <qdp_${p}1_color_generic.h>
#endif

/* Translation to fully generic names */
#if QDP_Precision == '${P}' && QDP_Colors == 1
#include <qdp_${p}1_generic.h>
#endif

#endif /* $def */
EOF
} else {
  print OUTFILE <<EOF;

/* Translation to color-generic names */
#if QDP_Colors == 1
#include <qdp_${p}1_color_generic.h>
#endif

#endif /* $def */
EOF
}

close INFILE;
close OUTFILE;
