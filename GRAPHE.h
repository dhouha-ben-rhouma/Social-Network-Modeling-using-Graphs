#ifndef GRAPHE_H_INCLUDED
#define GRAPHE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

#include "GRAPHEstruct.h"


graphePondere creerGraphe(int);
graphePondere chargerGraphe(const char*);
void detruireGraphe(graphePondere);
int ajouterUtilisateur(graphePondere, ELEMENT);
int supprimerUtilisateur(graphePondere, int);
int ajouterRelation(graphePondere, int, int, int);
int supprimerRelation(graphePondere, int, int );
void sauvegarderGraphe(const graphePondere, const char*);
void afficherGraphe(const graphePondere);
void dfs(const graphePondere, int );
void bfs(const graphePondere, int);
void dijkstra(const graphePondere, int , int);
void trouver_communautes(const graphePondere);
void exporterGraphviz(graphePondere, const char*);
void generateAndOpenImage(const char*);
void verifierGraphe(graphePondere);
void playBeep(void);
void playErrorBeep(void);
void playByeByeBeep(void);
int lireEntier(const char*);
void lireNom(char*,int);
void clearBuffer(void);



#endif // GRAPHE_H_INCLUDED
