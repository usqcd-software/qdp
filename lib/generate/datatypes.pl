%datatypes = (
	      RandomState => {
			      ABBR => "S",
			      NO_ADJ => 1,
			      NO_PRECISION => 1,
			      NO_COLOR => 1,
			     },
	      Int => {
		      ABBR => "I",
		      NO_ADJ => 1,
		      NO_PRECISION => 1,
		      NO_COLOR => 1,
		     },
	      Real => {
		       ABBR => "R",
		       NO_ADJ => 1,
		       NO_COLOR => 1,
		      },
	      Complex => {
			  ABBR => "C",
			  NO_COLOR => 1,
			 },
	      ColorVector => {
			      ABBR => "V",
			     },
	      HalfFermion => {
			       ABBR => "H",
			      },
	      DiracFermion => {
			       ABBR => "D",
			      },
	      ColorMatrix => {
			      ABBR => "M",
			     },
	      DiracPropagator => {
			       ABBR => "P",
			      },
	     );

#@all_types = ( keys %datatypes );
@all_types = ( "RandomState", "Int", "Real", "Complex", "ColorVector", "HalfFermion", "DiracFermion", "ColorMatrix", "DiracPropagator" );
for(@all_types) {
  if(!$datatypes{$_}{NO_SHIFT}) {
    push @shift_types, $_;
  }
  if(!$datatypes{$_}{NO_PRECISION}) {
    push @float_types, $_;
  }
  if(!$datatypes{$_}{NO_ADJ}) {
    push @complex_types, $_;
  }
  if(!$datatypes{$_}{NO_COLOR}) {
    push @color_types, $_;
  }
}

1;
