# define generated functions
# parsed by generate_qdp.pl

my $pr;
if($precision) {
  $pr = $precision."_";
} else {
  $pr = "";
}


comment0("Fills and random numbers");
comment1("Zero fills");
comment2("r = 0");

make_functions(%{{
  (
   DEST_TYPES  => [ @shift_types ],
   EQ_OPS      => [ 'eq' ],
   FUNCS       => [ 'zero' ],
  )}});

comment1("Constant fills");
comment2("r = a");

make_functions(%{{
  (
   DEST_TYPES  => [ @shift_types ],
   EQ_OPS      => [ 'eq' ],
   SRC1_TYPES  => [ 'DEST' ],
   SRC1_SCALAR => 1,
  )}});

comment1("Fill color matrix with constant times identity");
comment2("r = a I");

make_functions(%{{
  (
   DEST_TYPES  => [ 'ColorMatrix' ],
   EQ_OPS      => [ 'eq' ],
   SRC1_TYPES  => [ 'Complex' ],
   SRC1_SCALAR => 1,
  )}});

comment1("Seeding the random number generator field from an integer field");
comment2("seed r from constant c and field a");

make_functions(%{{
  (
   DEST_TYPES  => [ 'RandomState' ],
   EQ_OPS      => [ 'eq' ],
   FUNCS       => [ 'seed_i' ],
   EXTRA_ARGS2 => "QLA_Int c, ",
   SRC2_TYPES  => [ 'Int' ],
  )}});

comment1("Uniform random number fills");
comment2("r = uniform random number in (0,1) from seed a");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Real' ],
   EQ_OPS      => [ 'eq' ],
   FUNCS       => [ 'random' ],
   SRC2_TYPES  => [ 'RandomState' ],
  )}});

comment1("Gaussian random number fills");
comment2("r = normal gaussian from seed a");

make_functions(%{{
  (
   DEST_TYPES  => [ @float_types ],
   EQ_OPS      => [ 'eq' ],
   FUNCS       => [ 'gaussian' ],
   SRC2_TYPES  => [ 'RandomState' ],
  )}});


comment0("Unary Operations");
comment1("Bitwise not");
comment2("r = not(a)");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Int' ],
   EQ_OPS      => [ 'eq' ],
   FUNCS       => [ 'not' ],
   SRC2_TYPES  => [ 'DEST' ],
  )}});

comment1("Elementary unary functions on reals");
comment2("r = <func>(a)");

make_functions(%{{
  DEST_TYPES => [ 'Real' ],
  EQ_OPS     => [ 'eq' ],
  FUNCS      => [ 'sin','cos','tan','asin','acos','atan','sqrt','fabs','exp',
		  'log','sign','ceil','floor','sinh','cosh','tanh','log10' ],
  SRC2_TYPES => [ 'DEST' ],
}});

comment1("Elementary unary functions real to complex");
comment2("r = exp(ia)");

make_functions(%{{
  DEST_TYPES => [ 'Complex' ],
  EQ_OPS     => [ 'eq' ],
  FUNCS      => [ 'cexpi' ],
  SRC2_TYPES => [ 'Real' ],
}});

comment1("Elementary unary functions complex to real");
comment2("r = <func>(a)");

make_functions(%{{
  (
   DEST_TYPES => [ 'Real' ],
   EQ_OPS     => [ 'eq' ],
   FUNCS      => [ 'norm','arg' ],
   SRC2_TYPES => [ 'Complex' ],
  )}});

comment1("Elementary unary functions on complex values");
comment2("r = <func>(a)");

make_functions(%{
  {
  (
   DEST_TYPES => [ 'Complex' ],
   EQ_OPS     => [ 'eq' ],
   FUNCS      => [ 'cexp','csqrt','clog' ],
   SRC2_TYPES => [ 'Complex' ],
  )}});

comment1("Copying");
comment2("r = a");

make_functions(%{{
  (
   DEST_TYPES  => [ @all_types ],
   EQ_OPS      => [ 'eq','veq' ],
   SRC1_TYPES  => [ 'DEST' ],
  )}});

comment1("Incrementing");
comment2("r <eqop> a");

make_functions(%{{
  (
   DEST_TYPES  => [ @shift_types ],
   EQ_OPS      => [ 'eqm','peq','meq','veqm','vpeq','vmeq' ],
   SRC1_TYPES  => [ 'DEST' ],
  )}});

comment1("Transpose");
comment2("r <eqop> transpose(a)");

make_functions(%{{
  (
   DEST_TYPES  => [ 'ColorMatrix','DiracPropagator' ],
   EQ_OPS      => [ @eqops ],
   FUNCS       => [ 'transpose' ],
   SRC2_TYPES  => [ 'DEST' ],
  )}});

comment1("Complex conjugate");
comment2("r <eqop> conjugate(a)");

make_functions(%{{
  (
   DEST_TYPES  => [ @complex_types ],
   EQ_OPS      => [ @eqops ],
   FUNCS       => [ 'conj' ],
   SRC2_TYPES  => [ 'DEST' ],
  )}});

comment1("Hermitian conjugate");
comment2("r <eqop> adjoint(a)");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Complex','ColorMatrix','DiracPropagator' ],
   EQ_OPS      => [ @eqops ],
   SRC1_TYPES  => [ 'DEST' ],
   SRC1_ADJ    => 1,
  )}});

comment1("Local squared norm: uniform precision");
comment2("r = norm2(a)");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Real' ],
   EQ_OPS      => [ 'eq' ],
   FUNCS       => [ 'norm2' ],
   SRC2_TYPES  => [ @complex_types ],
  )}});


comment0("Type conversion and component extraction and insertion");
comment1("Convert float to double");
comment2("r = a");

make_functions(%{{
  (
   DEST_TYPES  => [ @float_types ],
   EQ_OPS      => [ 'eq' ],
   SRC1_TYPES  => [ 'DEST' ],
   DO_DF       => 1,
  )}});

comment1("Convert double to float");
comment2("r = a");

make_functions(%{{
  (
   DEST_TYPES  => [ @float_types ],
   EQ_OPS      => [ 'eq' ],
   SRC1_TYPES  => [ 'DEST' ],
   DO_FD       => 1,
  )}});

comment1("Convert real to complex (zero imaginary part)");
comment2("r = a + i0");

make_functions(%{{
  (
   DEST_TYPES => [ 'Complex' ],
   EQ_OPS     => [ 'eq' ],
   SRC1_TYPES => [ 'Real' ],
  )}});

comment1("Convert real and imaginary to complex");
comment2("r = a + i b");

make_functions(%{{
  (
   DEST_TYPES => [ 'Complex' ],
   EQ_OPS     => [ 'eq' ],
   SRC1_TYPES => [ 'Real' ],
   FUNCS      => [ 'plus_i' ],
   SRC2_TYPES => [ 'Real' ],
  )}});

comment1("Real/Imaginary part of complex");
comment2("r = <func>(a)");

make_functions(%{{
  (
   DEST_TYPES => [ 'Real' ],
   EQ_OPS     => [ 'eq' ],
   FUNCS      => [ 're','im' ],
   SRC2_TYPES => [ 'Complex' ],
  )}});

comment1("Integer to real");
comment2("r = a");

make_functions(%{{
  (
   DEST_TYPES => [ 'Real' ],
   EQ_OPS     => [ 'eq' ],
   SRC1_TYPES => [ 'Int' ],
  )}});

comment1("Real to integer (truncate/round)");
comment2("r = <func>(a)");

make_functions(%{{
  (
   DEST_TYPES => [ 'Int' ],
   EQ_OPS     => [ 'eq' ],
   FUNCS      => [ 'trunc','round' ],
   SRC2_TYPES => [ 'Real' ],
  )}});

comment1("Accessing a color matrix element");
comment2("r = a[i, j]");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Complex' ],
   EQ_OPS      => [ 'eq' ],
   FUNCS       => [ 'elem' ],
   SRC2_TYPES  => [ 'ColorMatrix' ],
   EXTRA_ARGS3 => "int i, int j, ",
  )}});

comment1("Inserting a color matrix element");
comment2("r[i, j] = a");

make_functions(%{{
  (
   DEST_TYPES  => [ 'ColorMatrix' ],
   EQ_OPS      => [ 'eq' ],
   FUNCS       => [ 'elem' ],
   SRC2_TYPES  => [ 'Complex' ],
   EXTRA_ARGS3 => "int i, int j, ",
  )}});

comment1("Accessing a half fermion or Dirac fermion spinor element");
comment2("r = a[color, spin]");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Complex' ],
   EQ_OPS      => [ 'eq' ],
   FUNCS       => [ 'elem' ],
   SRC2_TYPES  => [ 'HalfFermion','DiracFermion' ],
   EXTRA_ARGS3 => "int color, int spin, ",
  )}});

comment1("Inserting a half fermion or Dirac fermion spinor element");
comment2("r[color, spin] = a");

make_functions(%{{
  (
   DEST_TYPES  => [ 'HalfFermion','DiracFermion' ],
   EQ_OPS      => [ 'eq' ],
   FUNCS       => [ 'elem' ],
   SRC2_TYPES  => [ 'Complex' ],
   EXTRA_ARGS3 => "int color, int spin, ",
  )}});

comment1("Accessing a color vector element");
comment2("r = a[i]");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Complex' ],
   EQ_OPS      => [ 'eq' ],
   FUNCS       => [ 'elem' ],
   SRC2_TYPES  => [ 'ColorVector' ],
   EXTRA_ARGS3 => "int i, ",
  )}});

comment1("Inserting a color vector element");
comment2("r[i] = a");

make_functions(%{{
  (
   DEST_TYPES  => [ 'ColorVector' ],
   EQ_OPS      => [ 'eq' ],
   FUNCS       => [ 'elem' ],
   SRC2_TYPES  => [ 'Complex' ],
   EXTRA_ARGS3 => "int i, ",
  )}});

comment1("Accessing a Dirac propagator matrix element");
comment2("r = a[ic, is, jc, js]");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Complex' ],
   EQ_OPS      => [ 'eq' ],
   FUNCS       => [ 'elem' ],
   SRC2_TYPES  => [ 'DiracPropagator' ],
   EXTRA_ARGS3 => "int ic, int is, int jc, int js, ",
  )}});

comment1("Inserting a Dirac propagator matrix element");
comment2("r[ic, is, jc, js] = a");

make_functions(%{{
  (
   DEST_TYPES  => [ 'DiracPropagator' ],
   EQ_OPS      => [ 'eq' ],
   FUNCS       => [ 'elem' ],
   SRC2_TYPES  => [ 'Complex' ],
   EXTRA_ARGS3 => "int ic, int is, int jc, int js, ",
  )}});

comment1("Extracting a color vector from a color matrix column");
comment2("r[i] = a[i, j] (for all i)");

make_functions(%{{
  (
   DEST_TYPES  => [ 'ColorVector' ],
   EQ_OPS      => [ 'eq' ],
   FUNCS       => [ 'colorvec' ],
   SRC2_TYPES  => [ 'ColorMatrix' ],
   EXTRA_ARGS3 => "int j, ",
  )}});

comment1("Inserting a color vector into a color matrix column");
comment2("r[i, j] = a[i] (for all i)");

make_functions(%{{
  (
   DEST_TYPES  => [ 'ColorMatrix' ],
   EQ_OPS      => [ 'eq' ],
   FUNCS       => [ 'colorvec' ],
   SRC2_TYPES  => [ 'ColorVector' ],
   EXTRA_ARGS3 => "int j, ",
  )}});

comment1("Extracting a color vector from a half fermion or Dirac fermion");
comment2("r[color] = a[color, spin] (for all color)");

make_functions(%{{
  (
   DEST_TYPES  => [ 'ColorVector' ],
   EQ_OPS      => [ 'eq' ],
   FUNCS       => [ 'colorvec' ],
   SRC2_TYPES  => [ 'HalfFermion','DiracFermion' ],
   EXTRA_ARGS3 => "int spin, ",
  )}});

comment1("Inserting a color vector into a half fermion or Dirac fermion");
comment2("r[color, spin] = a[color] (for all color)");

make_functions(%{{
  (
   DEST_TYPES  => [ 'HalfFermion','DiracFermion' ],
   EQ_OPS      => [ 'eq' ],
   FUNCS       => [ 'colorvec' ],
   SRC2_TYPES  => [ 'ColorVector' ],
   EXTRA_ARGS3 => "int spin, ",
  )}});

comment1("Extracting a Dirac vector from a Dirac propagator matrix column");
comment2("r[ic, is] = a[ic, is, jc, js] (for all ic, is)");

make_functions(%{{
  (
   DEST_TYPES  => [ 'DiracFermion' ],
   EQ_OPS      => [ 'eq' ],
   FUNCS       => [ 'diracvec' ],
   SRC2_TYPES  => [ 'DiracPropagator' ],
   EXTRA_ARGS3 => "int jc, int js, ",
  )}});

comment1("Inserting a Dirac vector into a Dirac propagator matrix column");
comment2("r[ic, is, jc, js] = a[ic, is] (for all ic, is)");

make_functions(%{{
  (
   DEST_TYPES  => [ 'DiracPropagator' ],
   EQ_OPS      => [ 'eq' ],
   FUNCS       => [ 'diracvec' ],
   SRC2_TYPES  => [ 'DiracFermion' ],
   EXTRA_ARGS3 => "int jc, int js, ",
  )}});

comment1("Trace of color matrix");
comment2("r = trace(a)");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Complex' ],
   EQ_OPS      => [ 'eq' ],
   FUNCS       => [ 'trace' ],
   SRC2_TYPES  => [ 'ColorMatrix' ],
  )}});

comment1("Real/Imaginary part of trace of color matrix");
comment2("r = <func>(a)");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Real' ],
   EQ_OPS      => [ 'eq' ],
   FUNCS       => [ 're_trace','im_trace' ],
   SRC2_TYPES  => [ 'ColorMatrix' ],
  )}});

comment1("Traceless antihermitian part of color matrix");
#comment2("\@math{r = (a - a^\\dagger)/2 - i Im Tr a/n_c}");
#comment2("t^d");
comment2("r = (a - a^\\dagger)/2 - i Im Tr a/n_c");

make_functions(%{{
  (
   DEST_TYPES  => [ 'ColorMatrix' ],
   EQ_OPS      => [ 'eq' ],
   FUNCS       => [ 'antiherm' ],
   SRC2_TYPES  => [ 'ColorMatrix' ],
  )}});

comment1("Spin trace of Dirac propagator");
comment2("r[ic, jc] = Sum_is a[ic, is, jc, is]");

make_functions(%{{
  (
   DEST_TYPES  => [ 'ColorMatrix' ],
   EQ_OPS      => [ 'eq' ],
   FUNCS       => [ 'spintrace' ],
   SRC2_TYPES  => [ 'DiracPropagator' ],
  )}});

comment1("Dirac spin projection");
comment2("r = spin project(a, dir, sign)");

make_functions(%{{
  (
   DEST_TYPES  => [ 'HalfFermion' ],
   EQ_OPS      => [ @eqops, @veqops ],
   FUNCS       => [ 'spproj' ],
   SRC2_TYPES  => [ 'DiracFermion' ],
   EXTRA_ARGS3 => "int dir, int sign, ",
  )}});

comment1("Dirac spin reconstruction");
comment2("r = spin reconstruct(a, dir, sign)");

make_functions(%{{
  (
   DEST_TYPES  => [ 'DiracFermion' ],
   EQ_OPS      => [ @eqops, @veqops ],
   FUNCS       => [ 'sprecon' ],
   SRC2_TYPES  => [ 'HalfFermion' ],
   EXTRA_ARGS3 => "int dir, int sign, ",
  )}});

comment1("Dirac spin projection with reconstruction");
comment2("r = spin reconstruct(spin project(a, dir, sign), dir, sign)");

make_functions(%{{
  (
   DEST_TYPES  => [ 'DiracFermion' ],
   EQ_OPS      => [ @eqops, @veqops ],
   FUNCS       => [ 'spproj' ],
   SRC2_TYPES  => [ 'DiracFermion' ],
   EXTRA_ARGS3 => "int dir, int sign, ",
  )}});

comment1("Matrix multiply and Dirac spin projection");
comment2("r = spin project(a*b, dir, sign)");

make_functions(%{{
  (
   DEST_TYPES  => [ 'HalfFermion' ],
   EQ_OPS      => [ map($_."_spproj",( @eqops, @veqops )) ],
   SRC1_TYPES  => [ 'ColorMatrix' ],
   SRC1_DO_ADJ => 1,
   FUNCS       => [ 'times' ],
   SRC2_TYPES  => [ 'DiracFermion' ],
   EXTRA_ARGS3 => "int dir, int sign, ",
  )}});

comment1("Matrix multiply and Dirac spin reconstruction");
comment2("r = spin reconstruct(a*b, dir, sign)");

make_functions(%{{
  (
   DEST_TYPES  => [ 'DiracFermion' ],
   EQ_OPS      => [ map($_."_sprecon",( @eqops, @veqops )) ],
   SRC1_TYPES  => [ 'ColorMatrix' ],
   SRC1_DO_ADJ => 1,
   FUNCS       => [ 'times' ],
   SRC2_TYPES  => [ 'HalfFermion' ],
   EXTRA_ARGS3 => "int dir, int sign, ",
  )}});

comment1("Matrix multiply and Dirac spin projection with reconstruction");
comment2("r = spin reconstruct(spin project(a*b, dir, sign), dir, sign)");

make_functions(%{{
  (
   DEST_TYPES  => [ 'DiracFermion' ],
   EQ_OPS      => [ map($_."_spproj",( @eqops, @veqops )) ],
   SRC1_TYPES  => [ 'ColorMatrix' ],
   SRC1_DO_ADJ => 1,
   FUNCS       => [ 'times' ],
   SRC2_TYPES  => [ 'DiracFermion' ],
   EXTRA_ARGS3 => "int dir, int sign, ",
  )}});


comment0("Binary operations with constants");
comment1("Multiplication by integer constant");
comment2("r <eqop> a * b");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Int' ],
   EQ_OPS      => [ @eqops ],
   SRC1_TYPES  => [ 'Int' ],
   SRC1_SCALAR => 1,
   FUNCS       => [ 'times' ],
   SRC2_TYPES  => [ 'DEST' ],
  )}});

comment1("Multiplication by real constant");
comment2("r <eqop> a * b");

make_functions(%{{
  (
   DEST_TYPES  => [ @float_types ],
   EQ_OPS      => [ @eqops, @veqops ],
   SRC1_TYPES  => [ 'Real' ],
   SRC1_SCALAR => 1,
   FUNCS       => [ 'times' ],
   SRC2_TYPES  => [ 'DEST' ],
  )}});

comment1("Multiplication by complex constant");
comment2("r <eqop> a * b");

make_functions(%{{
  (
   DEST_TYPES  => [ @complex_types ],
   EQ_OPS      => [ @eqops, @veqops ],
   SRC1_TYPES  => [ 'Complex' ],
   SRC1_SCALAR => 1,
   FUNCS       => [ 'times' ],
   SRC2_TYPES  => [ 'DEST' ],
  )}});

comment1("Multiplication by i");
comment2("r <eqop> i a");

make_functions(%{{
  (
   DEST_TYPES  => [ @complex_types ],
   EQ_OPS      => [ @eqops ],
   FUNCS       => [ 'i' ],
   SRC2_TYPES  => [ 'DEST' ],
  )}});

comment1("Left multiplication by gamma matrix");
comment2("r = gamma(i) * a");

make_functions(%{{
  (
   DEST_TYPES  => [ 'DiracFermion','DiracPropagator' ],
   EQ_OPS      => [ 'eq' ],
   FUNCS       => [ 'gamma_times' ],
   SRC2_TYPES  => [ 'DEST' ],
   EXTRA_ARGS3 => "int i, ",
  )}});

comment1("Right multiplication by gamma matrix");
comment2("r = a * gamma(i)");

make_functions(%{{
  (
   DEST_TYPES  => [ 'DiracPropagator' ],
   EQ_OPS      => [ 'eq' ],
   SRC1_TYPES  => [ 'DEST' ],
   FUNCS       => [ 'times_gamma' ],
   EXTRA_ARGS2 => "int i, ",
  )}});


comment0("Binary operations with fields");
comment1("Elementary binary functions on integers");
comment2("r = a <func> b");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Int' ],
   EQ_OPS      => [ 'eq' ],
   SRC1_TYPES  => [ 'DEST' ],
   FUNCS       => [ 'lshift','rshift','mod','max','min','or','and','xor' ],
   SRC2_TYPES  => [ 'DEST' ],
  )}});

comment1("Elementary binary functions on reals");
comment2("r = a <func> b");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Real' ],
   EQ_OPS      => [ 'eq' ],
   SRC1_TYPES  => [ 'DEST' ],
   FUNCS       => [ 'mod','max','min','pow','atan2' ],
   SRC2_TYPES  => [ 'DEST' ],
  )}});

comment1("Multiplying real by integer power of 2");
comment2("r = a * 2^b");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Real' ],
   EQ_OPS      => [ 'eq' ],
   SRC1_TYPES  => [ 'DEST' ],
   FUNCS       => [ 'ldexp' ],
   SRC2_TYPES  => [ 'Int' ],
  )}});

comment1("Addition");
comment2("r = a + b");

make_functions(%{{
  (
   DEST_TYPES  => [ @shift_types ],
   EQ_OPS      => [ 'eq', 'veq' ],
   SRC1_TYPES  => [ 'DEST' ],
   FUNCS       => [ 'plus' ],
   SRC2_TYPES  => [ 'DEST' ],
  )}});

comment1("Subtraction");
comment2("r = a - b");

make_functions(%{{
  (
   DEST_TYPES  => [ @shift_types ],
   EQ_OPS      => [ 'eq', 'veq' ],
   SRC1_TYPES  => [ 'DEST' ],
   FUNCS       => [ 'minus' ],
   SRC2_TYPES  => [ 'DEST' ],
  )}});

comment1("Multiplication: uniform types");
comment2("r <eqop> a * b");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Int','Real','Complex','DiracPropagator' ],
   EQ_OPS      => [ @eqops ],
   SRC1_TYPES  => [ 'DEST' ],
   FUNCS       => [ 'times' ],
   SRC2_TYPES  => [ 'DEST' ],
  )}});

comment1("Division of integer, real, and complex fields");
comment2("r = a / b");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Int','Real','Complex' ],
   EQ_OPS      => [ 'eq' ],
   SRC1_TYPES  => [ 'DEST' ],
   FUNCS       => [ 'divide' ],
   SRC2_TYPES  => [ 'DEST' ],
  )}});

comment1("Left multiplication by color matrix");
comment2("r <eqop> a * b");

make_functions(%{{
  (
   DEST_TYPES  => [ @color_types ],
   EQ_OPS      => [ @eqops, @veqops ],
   SRC1_TYPES  => [ 'ColorMatrix' ],
   FUNCS       => [ 'times' ],
   SRC2_TYPES  => [ 'DEST' ],
  )}});

comment1("Left multiplication by adjoint of color matrix");
comment2("r <eqop> adjoint(a) * b");

make_functions(%{{
  (
   DEST_TYPES  => [ @color_types ],
   EQ_OPS      => [ @eqops, @veqops ],
   SRC1_TYPES  => [ 'ColorMatrix' ],
   SRC1_ADJ    => 1,
   FUNCS       => [ 'times' ],
   SRC2_TYPES  => [ 'DEST' ],
  )}});

comment1("Right multiplication by color matrix");
comment2("r <eqop> a * b");

make_functions(%{{
  (
   DEST_TYPES  => [ 'DiracPropagator' ],
   EQ_OPS      => [ @eqops ],
   SRC1_TYPES  => [ 'DEST' ],
   FUNCS       => [ 'times' ],
   SRC2_TYPES  => [ 'ColorMatrix' ],
  )}});

comment1("Right multiplication by adjoint of color matrix");
comment2("r <eqop> a * adjoint(b)");

make_functions(%{{
  (
   DEST_TYPES  => [ 'ColorMatrix','DiracPropagator' ],
   EQ_OPS      => [ @eqops ],
   SRC1_TYPES  => [ 'DEST' ],
   FUNCS       => [ 'times' ],
   SRC2_TYPES  => [ 'ColorMatrix' ],
   SRC2_ADJ    => 1,
  )}});

comment1("Adjoint of color matrix times adjoint of color matrix");
comment2("r <eqop> adjoint(a) * adjoint(b)");

make_functions(%{{
  (
   DEST_TYPES  => [ 'ColorMatrix' ],
   EQ_OPS      => [ @eqops ],
   SRC1_TYPES  => [ 'DEST' ],
   SRC1_ADJ    => 1,
   FUNCS       => [ 'times' ],
   SRC2_TYPES  => [ 'DEST' ],
   SRC2_ADJ    => 1,
  )}});

comment1("Local inner product");
comment2("r = Tr adjoint(a) * b");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Complex' ],
   EQ_OPS      => [ 'eq' ],
   SRC1_TYPES  => [ @complex_types ],
   FUNCS       => [ 'dot' ],
   SRC2_TYPES  => [ 'SRC1' ],
  )}});

comment1("Real part of local inner product");
comment2("r = Re Tr adjoint(a) * b");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Real' ],
   EQ_OPS      => [ 'eq_re' ],
   SRC1_TYPES  => [ @complex_types ],
   FUNCS       => [ 'dot' ],
   SRC2_TYPES  => [ 'SRC1' ],
  )}});

comment1("Color matrix from outer product");
comment2("r[i, j] <eqop> a[i] * b[j]");

make_functions(%{{
  (
   DEST_TYPES  => [ 'ColorMatrix' ],
   EQ_OPS      => [ @eqops, @veqops ],
   SRC1_TYPES  => [ 'ColorVector' ],
   FUNCS       => [ 'times' ],
   SRC2_TYPES  => [ 'ColorVector' ],
   SRC2_ADJ    => 1,
  )}});


comment0("Ternary operations with fields");
comment1("Addition or subtraction with integer scalar multiplication");
comment2("r = c * a +/- b");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Int' ],
   EQ_OPS      => [ 'eq_i_times' ],
   EXTRA_ARGS1 => "QLA_Int *c, ",
   SRC1_TYPES  => [ 'DEST' ],
   FUNCS       => [ 'plus','minus' ],
   SRC2_TYPES  => [ 'DEST' ],
  )}});

comment1("Addition or subtraction with real scalar multiplication");
comment2("r = c * a +/- b");

make_functions(%{{
  (
   DEST_TYPES  => [ @float_types ],
   EQ_OPS      => [ 'eq_r_times', 'veq_r_times' ],
   EXTRA_ARGS1 => "QLA_".$pr."Real *c, ",
   SRC1_TYPES  => [ 'DEST' ],
   FUNCS       => [ 'plus','minus' ],
   SRC2_TYPES  => [ 'DEST' ],
  )}});


comment1("Addition or subtraction with complex scalar multiplication");
comment2("r = c * a +/- b");

make_functions(%{{
  (
   DEST_TYPES  => [ @complex_types ],
   EQ_OPS      => [ 'eq_c_times', 'veq_c_times' ],
   EXTRA_ARGS1 => "QLA_".$pr."Complex *c, ",
   SRC1_TYPES  => [ 'DEST' ],
   FUNCS       => [ 'plus','minus' ],
   SRC2_TYPES  => [ 'DEST' ],
  )}});


comment0("Boolean operations");
comment1("Comparisons of integers and reals");
comment2("r = a <func> b");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Int' ],
   EQ_OPS      => [ 'eq' ],
   SRC1_TYPES  => [ 'Int','Real' ],
   FUNCS       => [ 'eq','ne','gt','lt','ge','le' ],
   SRC2_TYPES  => [ 'SRC1' ],
  )}});

comment1("Copy under a mask");
comment2("r = a (if b is not 0)");

make_functions(%{{
  (
   DEST_TYPES  => [ @shift_types ],
   EQ_OPS      => [ 'eq' ],
   SRC1_TYPES  => [ 'DEST' ],
   FUNCS       => [ 'mask' ],
   SRC2_TYPES  => [ 'Int' ],
  )}});


comment0("Reductions");
comment1("Global squared norm: uniform precision");
comment2("r = Sum norm2(a)");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Real' ],
   DEST_SCALAR => 1,
   EQ_OPS      => [ 'eq','veq' ],
   FUNCS       => [ 'norm2' ],
   SRC2_TYPES  => [ @shift_types ],
  )}});

comment1("Global inner product");
comment2("r = Sum Tr a * b");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Real' ],
   DEST_SCALAR => 1,
   EQ_OPS      => [ 'eq','veq' ],
   SRC1_TYPES  => [ 'Int','Real' ],
   FUNCS       => [ 'dot' ],
   SRC2_TYPES  => [ 'SRC1' ],
  )}});

comment2("r = Sum Tr adjoint(a) * b");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Complex' ],
   DEST_SCALAR => 1,
   EQ_OPS      => [ 'eq','veq' ],
   SRC1_TYPES  => [ @complex_types ],
   FUNCS       => [ 'dot' ],
   SRC2_TYPES  => [ 'SRC1' ],
  )}});

comment1("Real part of global inner product");
comment2("r = Sum Re Tr adjoint(a) * b");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Real' ],
   DEST_SCALAR => 1,
   EQ_OPS      => [ 'eq_re','veq_re' ],
   SRC1_TYPES  => [ @complex_types ],
   FUNCS       => [ 'dot' ],
   SRC2_TYPES  => [ 'SRC1' ],
  )}});

comment1("Global sums");
comment2("r = Sum a");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Real' ],
   DEST_SCALAR => 1,
   EQ_OPS      => [ 'eq' ],
   FUNCS       => [ 'sum' ],
   SRC2_TYPES  => [ 'Int' ],
  )}});

make_functions(%{{
  (
   DEST_TYPES  => [ @float_types ],
   DEST_SCALAR => 1,
   EQ_OPS      => [ 'eq' ],
   FUNCS       => [ 'sum' ],
   SRC2_TYPES  => [ 'DEST' ],
  )}});

comment1("Multisubset Norms");
comment2("r[i] = Sum_subset[i] norm2(a)");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Real' ],
   DEST_SCALAR => 1,
   DEST_MULTI  => 1,
   EQ_OPS      => [ 'eq','veq' ],
   FUNCS       => [ 'norm2' ],
   SRC2_TYPES  => [ @shift_types ],
  )}});

comment1("Multisubset inner products");
comment2("r[i] = Sum_subset[i] a * b");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Real' ],
   DEST_SCALAR => 1,
   DEST_MULTI  => 1,
   EQ_OPS      => [ 'eq','veq' ],
   SRC1_TYPES  => [ 'Int','Real' ],
   FUNCS       => [ 'dot' ],
   SRC2_TYPES  => [ 'SRC1' ],
  )}});

comment2("r[i] = Sum_subset[i] adjoint(a) * b");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Complex' ],
   DEST_SCALAR => 1,
   DEST_MULTI  => 1,
   EQ_OPS      => [ 'eq','veq' ],
   SRC1_TYPES  => [ @complex_types ],
   FUNCS       => [ 'dot' ],
   SRC2_TYPES  => [ 'SRC1' ],
  )}});

comment1("Multisubset real part of global inner product");
comment2("r = Sum Re Tr adjoint(a) * b");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Real' ],
   DEST_SCALAR => 1,
   DEST_MULTI  => 1,
   EQ_OPS      => [ 'eq_re','veq_re' ],
   SRC1_TYPES  => [ @complex_types ],
   FUNCS       => [ 'dot' ],
   SRC2_TYPES  => [ 'SRC1' ],
  )}});

comment1("Multisubset global sums");
comment2("r[i] = Sum_subset[i] a");

make_functions(%{{
  (
   DEST_TYPES  => [ 'Real' ],
   DEST_SCALAR => 1,
   DEST_MULTI  => 1,
   EQ_OPS      => [ 'eq' ],
   FUNCS       => [ 'sum' ],
   SRC2_TYPES  => [ 'Int' ],
  )}});

make_functions(%{{
  (
   DEST_TYPES  => [ @float_types ],
   DEST_SCALAR => 1,
   DEST_MULTI  => 1,
   EQ_OPS      => [ 'eq' ],
   FUNCS       => [ 'sum' ],
   SRC2_TYPES  => [ 'DEST' ],
  )}});
