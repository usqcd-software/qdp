#include <qdp.h>

extern int
congrad(QDP_DiracFermion  *result,
	QDP_ColorMatrix   **gauge,
	QDP_DiracFermion  *rhs,
	QLA_Real          kappa,
	int               max_iter,
	double            epsilon);

extern void prepare_dslash(void);
extern void cleanup_dslash(void);
extern void dslash(QDP_DiracFermion  *result,
		   QDP_ColorMatrix   **gauge,
		   QDP_DiracFermion  *psi,
		   QLA_Real          kappa,
		   int               sign);
