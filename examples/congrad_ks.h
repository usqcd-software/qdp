#include <qdp.h>

extern int
congrad(QDP_ColorVector  *result,
	QDP_ColorMatrix  **gauge,
	QDP_ColorVector  *rhs,
	QLA_Real         mass,
	int              max_iter,
	double           epsilon);

extern void prepare_dslash(void);
extern void cleanup_dslash(void);
extern void dslash(QDP_ColorVector  *result,
		   QDP_ColorMatrix  **gauge,
		   QDP_ColorVector  *psi,
                   QDP_Subset subset);
