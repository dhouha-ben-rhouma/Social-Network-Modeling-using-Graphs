#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "ELTUSER.h"

ELEMENT elementCreer ()
{
    ELEMENT e;
    e = (ELEMENT)malloc(sizeof(utilisateurStruct));
    if (e == NULL)
    {
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        exit(EXIT_FAILURE);
    }
    e ->id = 0;
    e ->age = 0;
    e->nom[0] = '\0';
    return e;
}

void elementLire (ELEMENT *e)
{
    printf("donner l'identifiant unique de l'utilisateur: ");
    while(scanf("%d", &(*e)->id) != 1)
    {
        printf("Erreur. Veuillez entrer un nombre valide: ");
        while(getchar() != '\n'); // Vide le buffer
    }

    fflush(stdin);
    printf("donner le nom de l'utilisateur: ");
    fgets((*e)->nom,sizeof((*e)->nom),stdin);
    // Supprimer le '\n' si présent
    size_t len = strlen((*e)->nom);
    if (len > 0 && (*e)->nom[len - 1] == '\n')
    {
        (*e)->nom[len - 1] = '\0';
    }

    printf("donner l'age de l'utilisateur (16 ans minimum): ");
    while((scanf("%d", &(*e)->age) != 1) || ((*e)-> age)<16 )
    {
        printf("Erreur. Veuillez entrer un âge valide(16+): ");
        while(getchar() != '\n');
    }
}

void elementDetruire (ELEMENT e)
{
    free (e);
}

void elementAfficher ( ELEMENT e)
{
    if (e == NULL)
    {
        fprintf(stderr, "Erreur : pointeur invalide\n");
        return;
    }
    printf(" id: %i\n nom: %s\n age: %i", e->id, e->nom,e->age);
}

void elementAffecter ( ELEMENT *e1, ELEMENT e2 )
{
    if (!e1)
        return;
    *e1 = e2;
}


void elementCopier(ELEMENT *e1, ELEMENT e2)
{
    if (!*e1) *e1 = elementCreer();
    if (e1 == NULL || *e1 == NULL || e2 == NULL)
    {
        fprintf(stderr, "Erreur : pointeur invalide\n");
        return;
    }
    (*e1)->id=e2->id;
    strncpy((*e1)->nom,e2->nom,sizeof((*e1)->nom));
    (*e1)->age=e2->age;
}

int elementComparer(ELEMENT e1, ELEMENT e2)
{
    return (e1->id == e2->id);
}

ELEMENT creerUtilisateur(int id, const char* nom, int age) {
    ELEMENT e = (ELEMENT)malloc(sizeof(utilisateurStruct));
    if (e) {
        e->id = id;
        strncpy(e->nom, nom, 49);
        e->nom[49] = '\0';
        e->age = age;
    }
    return e;
}
