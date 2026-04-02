#ifndef ELTUSER_H_INCLUDED
#define ELTUSER_H_INCLUDED

#include "ELTUSERstruct.h"

ELEMENT elementCreer(void) ;
void elementLire(ELEMENT*);
void elementDetruire (ELEMENT);
void elementAfficher(ELEMENT);
void elementAffecter(ELEMENT*, ELEMENT);
void elementCopier(ELEMENT *, ELEMENT) ;
int elementComparer(ELEMENT, ELEMENT);
ELEMENT creerUtilisateur(int,const char*,int);


#endif // ELTUSER_H_INCLUDED
