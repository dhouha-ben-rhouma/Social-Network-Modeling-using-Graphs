#include "GRAPHE.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
// ANSI color codes
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"


#define GRAPHvIZ_PATH "C:\\Graphviz\\bin\\dot.exe"


int *ids;

// ---------------------------
// 1. FONCTIONS DE BASE
// ---------------------------
int unique(int identifiants[], int nb, int id) //r
{
    int valide = 1,i=0;
    while (i<nb && valide)
    {
        if (identifiants[i] == id )
        {
            valide = 0; // L'identifiant existe dejà
        }
        i++;
    }
    return valide;// L'identifiant est unique
}

int trouve(graphePondere g,int id) //r
{
    int t = -1,i=0;
    while (t==-1 && (i < g->v) )
    {
        if (ids[i] == id )
        {
            t=i; // L'indice
        }
        i++;
    }
    return t;
}

graphePondere creerGraphe(int v) //r
{
    if (v < 0 )
    {
        fprintf(stderr, "Erreur: Taille invalide (doit être positive)\n");
        return NULL;
    }

    graphePondere g = malloc(sizeof(*g));
    if (!g)
    {
        fprintf(stderr, "Erreur: Allocation mémoire graphe\n");
        return NULL;
    }

    g->v = v;
    g->utilisateurs = calloc(v, sizeof(ELEMENT));
    if (!g->utilisateurs)
    {
        fprintf(stderr, "Erreur: Allocation mémoire utilisateurs\n");
        free(g);
        return NULL;
    }

    g->adjList = calloc(v, sizeof(Node));
    if (!g->adjList)
    {
        fprintf(stderr, "Erreur: Allocation mémoire adjacence\n");
        free(g->utilisateurs);
        free(g);
        return NULL;
    }

    ids = malloc(v * sizeof(int)); // ← à condition que ids soit bien global
    if (!ids)
    {
        fprintf(stderr, "Erreur: Allocation mémoire ids\n");
        free(g->utilisateurs);
        free(g->adjList);
        free(g);
        return NULL;
    }

    return g;
}

void detruireGraphe(graphePondere g) //r
{
    if (!g) return;

    for (int i = 0; i < g->v; i++)
    {
        // Libère les listes d'adjacence (sans détruire les ELEMENT)
        Node courant = g->adjList[i];
        while (courant)
        {
            Node tmp = courant;
            courant = courant->next;
            free(tmp);
        }
        // Libère les utilisateurs
        if (g->utilisateurs[i])
        {
            elementDetruire(g->utilisateurs[i]);
        }
    }

    free(g->utilisateurs);
    free(g->adjList);
    free(ids);
    free(g);

}

// ---------------------------
// 2. GESTION UTILISATEURS
// ---------------------------

int ajouterUtilisateur(graphePondere g, ELEMENT user) //r
{
    if (!g)
    {
        printf("!g\n");
        return 0;
    }

    if (!user)
    {
        printf("!user\n");
        return 0;
    }

    if (user->id < 0)
    {
        printf("user->id < 0\n");
        return 0;
    }

    // Vérifie si l'utilisateur existe déjà
    if (!unique(ids, g->v, user->id))
    {
        printf("L'utilisateur existe déjà !!!\n");
        return 0;
    }

    // Tentative de réallocation mémoire
    ELEMENT* tmpUtilisateurs = realloc(g->utilisateurs, (g->v + 1) * sizeof(ELEMENT));
    if (!tmpUtilisateurs)
    {
        printf("Erreur de réallocation utilisateurs\n");
        return 0;
    }

    Node* tmpAdjList = realloc(g->adjList, (g->v + 1) * sizeof(Node));
    if (!tmpAdjList)
    {
        printf("Erreur de réallocation adjList\n");
        return 0;
    }

    int* tmpIds = realloc(ids, (g->v + 1) * sizeof(int));
    if (!tmpIds)
    {
        printf("Erreur de réallocation ids\n");
        free(tmpUtilisateurs);  // Libère mémoire allouée pour tmpUtilisateurs
        free(tmpAdjList);
        return 0;
    }

    // Mise à jour des pointeurs
    g->utilisateurs = tmpUtilisateurs;
    g->adjList = tmpAdjList;
    ids = tmpIds;

    // Ajout des valeurs
    g->utilisateurs[g->v] = user;
    g->adjList[g->v] = NULL;  // Important : initialiser la liste à NULL
    ids[g->v] = user->id;

    g->v++;

    printf("Utilisateur [ID:%d] %s ajouté (position %d)\n",
           user->id, user->nom, g->v - 1);

    return 1;
}

int supprimerUtilisateur(graphePondere g, int idUser) //r
{
    if (!g)
    {
        fprintf(stderr, "Erreur: Graphe NULL\n");
        return 0;
    }

    if (idUser < 0)
    {
        fprintf(stderr, "Erreur: ID utilisateur invalide\n");
        return 0;
    }
    int index = trouve(g, idUser);  // On cherche l'indice de l'utilisateur
    if (index == -1 || !g->utilisateurs[index])
    {
        printf("Erreur: L'utilisateur avec l'ID %d n'existe pas\n", idUser);
        return 0;
    }
    // 1. Supprime toutes les relations de l'utilisateur
    for (int i = 0; i < g->v; i++)
    {
        supprimerRelation(g, idUser, g->utilisateurs[i]->id); // Bidirectionnel
    }

    // 2. Libère l'utilisateur
    elementDetruire(g->utilisateurs[index]);
    for (int i = index; i < g->v - 1; i++)
    {
        g->utilisateurs[i] = g->utilisateurs[i + 1];
        g->adjList[i] = g->adjList[i + 1];
        ids[i] = ids[i + 1];
    }
    g->v--;
    // Réallocation (sécurisée)
    ELEMENT* tmpUsers = realloc(g->utilisateurs, g->v * sizeof(ELEMENT));
    Node* tmpAdj = realloc(g->adjList, g->v * sizeof(Node));
    int* tmpIds = realloc(ids, g->v * sizeof(int));

    if (g->v > 0 && (!tmpUsers || !tmpAdj || !tmpIds))
    {
        printf("Erreur: Réallocation après suppression\n");
        // Libération de la mémoire allouée avant l'échec
        free(g->utilisateurs);
        free(g->adjList);
        free(ids);
        return 0;
    }

    g->utilisateurs = tmpUsers;
    g->adjList = tmpAdj;
    ids = tmpIds;

    printf("Utilisateur [ID:%d] supprimé avec succès\n", idUser);
    return 1;
}

// ---------------------------
// 3. GESTION RELATIONS
// ---------------------------

int ajouterRelation(graphePondere g, int iDsrc, int iDdest, int poids) //r
{
    if (!g || iDsrc < 0 || iDdest < 0 || poids <= 0)
    {
        fprintf(stderr, "Paramètres invalides\n");
        return 0;
    }
    int indexSrc = trouve(g, iDsrc);
    int indexDest = trouve(g, iDdest);

    if (indexSrc == -1 || indexDest == -1)
    {
        printf("Un des utilisateurs n'existe pas\n");
        return 0;
    }

    // Ajoute la relation src -> dest
    Node nouveau = (Node)malloc(sizeof(structNode));
    if (!nouveau)
    {
        fprintf(stderr, "Erreur d'allocation mémoire pour la relation src -> dest\n");
        return 0;
    }

    nouveau->user = g->utilisateurs[indexDest];
    nouveau->weight = poids;
    nouveau->next = g->adjList[indexSrc];
    g->adjList[indexSrc] = nouveau;

    // Pour un graphe non orienté, ajoute aussi dest -> src
    Node nouveauInverse = (Node)malloc(sizeof(structNode));
    if (!nouveauInverse)
    {
        // Libérer la mémoire allouée pour la relation src -> dest en cas d'échec de l'allocation pour la relation inverse
        g->adjList[indexSrc] = nouveau->next;
        free(nouveau);
        fprintf(stderr, "Erreur d'allocation mémoire pour la relation dest -> src\n");
        return 0;
    }

    nouveauInverse->user = g->utilisateurs[indexSrc];
    nouveauInverse->weight = poids;
    nouveauInverse->next = g->adjList[indexDest];
    g->adjList[indexDest] = nouveauInverse;

    return 1;
}

int supprimerRelation(graphePondere g, int src, int dest) //r
{
    if (!g || src < 0 || dest < 0)
    {
        fprintf(stderr, "Erreur: Paramètres invalides\n");
        return 0;
    }

    int indexSrc = trouve(g, src);
    int indexDest = trouve(g, dest);

    if (indexSrc == -1 || indexDest == -1)
    {
        fprintf(stderr, "Erreur: Un des utilisateurs n'existe pas\n");
        return 0;
    }

    int deleted=0;
    // Supprime src -> dest
    Node *courant = &g->adjList[indexSrc];
    while (*courant && !deleted)
    {
        if ((*courant)->user && (*courant)->user->id == dest)
        {
            Node tmp = *courant;
            *courant = tmp->next;
            free(tmp);
            deleted=1;
        }
        else
            courant = &(*courant)->next;
    }

    // Supprime dest -> src (non orienté)
    int deletedInverse = 0;
    courant = &g->adjList[indexDest];
    while (*courant && !deletedInverse)
    {
        if ((*courant)->user && (*courant)->user->id == src)
        {
            Node tmp = *courant;
            *courant = tmp->next;
            free(tmp);
            deletedInverse = 1;
        }
        else
            courant = &(*courant)->next;
    }
    if (deleted && deletedInverse)
    {
        printf("La relation a été supprimée dans les deux directions\n");
        return 1;
    }
    else if (deleted)
    {
        printf("La relation a été supprimée d'une seule direction (src -> dest)\n");
        return 0;
    }
    else
    {
        printf("La relation n'existe pas\n");
        return 0;
    }
}
// ---------------------------
// 4. ALGORITHMES
// ---------------------------
// Fonction récursive DFS

void dfsRec(const graphePondere g, int idUser, int* visite)
{
    if (!g || !visite || idUser < 0) return;

    visite[idUser] = 1;
    printf("%sVisite de %s (ID %d)%s\n", GREEN, g->utilisateurs[idUser]->nom, g->utilisateurs[idUser]->id, RESET);

    Node voisin = g->adjList[idUser];
    while (voisin)
    {
        int idVoisin = voisin->user->id;
        int indexVoisin = trouve(g, idVoisin);

        if (indexVoisin != -1 && !visite[indexVoisin])
        {
            dfsRec(g, indexVoisin, visite);  // ⬅️ On passe `visite`
        }
        voisin = voisin->next;
    }
}

void dfs(const graphePondere g, int idUser)
{
    if (!g) return;

    int indexDepart = trouve(g, idUser);
    if (indexDepart == -1)
    {
        printf("%sUtilisateur avec ID %d introuvable.%s\n", RED, idUser, RESET);
        return;
    }

    int* visite = (int*)calloc(g->v, sizeof(int));
    if (!visite)
    {
        printf("%sErreur d'allocation mémoire pour le tableau visite.%s\n", RED, RESET);
        return;
    }

    printf("\n%sParcours DFS depuis %s (ID %d) :%s\n\n", BOLD, g->utilisateurs[indexDepart]->nom, idUser, RESET);
    dfsRec(g, indexDepart, visite);  // ⬅️ On passe `visite` ici aussi

    free(visite);
}


void afficherChemin(int* parent, int sommet) //r
{
    if (parent[sommet] == -1)
    {
        printf("%d", sommet);
        return;
    }
    afficherChemin(parent, parent[sommet]);
    printf(" -> %d", sommet);
}

void bfs(const graphePondere g, int idUser) //r
{
    if (!g) {
        printf("Graphe invalide.\n");
        return;
    }

    int indiceDepart = trouve(g, idUser); // on convertit l'id utilisateur → indice dans le tableau
    if (indiceDepart == -1) {
        printf("Utilisateur avec id %d non trouvé.\n", idUser);
        return;
    }

    int* visited = (int*)calloc(g->v, sizeof(int));
    int* distance = (int*)malloc(g->v * sizeof(int));
    int* parent = (int*)malloc(g->v * sizeof(int));
    int* queue = (int*)malloc(g->v * sizeof(int));
    int front = 0, rear = 0;

    // Initialisation des tableaux
    for (int i = 0; i < g->v; i++) {
        distance[i] = -1;
        parent[i] = -1;
    }

    visited[indiceDepart] = 1;
    distance[indiceDepart] = 0;
    queue[rear++] = indiceDepart;

    int maxNiveau = 0;

    while (front < rear) {
        int courant = queue[front++];
        Node voisin = g->adjList[courant];

        while (voisin) {
            // Trouver l'indice du voisin avec son identifiant
            int vid = trouve(g, voisin->user->id);  // Trouve l'indice basé sur l'identifiant

            if (vid != -1 && !visited[vid]) {  // Assurez-vous que `vid` est valide
                visited[vid] = 1;
                distance[vid] = distance[courant] + 1;
                parent[vid] = courant;
                if (distance[vid] > maxNiveau)
                    maxNiveau = distance[vid];
                queue[rear++] = vid;
            }
            voisin = voisin->next;
        }
    }

    // Affichage
    printf("\n%sArborescence BFS depuis id %d (%s) :%s\n", BOLD, idUser, g->utilisateurs[indiceDepart]->nom, RESET);

    for (int niveau = 0; niveau <= maxNiveau; niveau++) {
        printf("\n%sNiveau %d :%s\n", CYAN, niveau, RESET);
        for (int i = 0; i < g->v; i++) {
            if (distance[i] == niveau) {
                const char* color = RESET;
                switch (niveau) {
                    case 1: color = GREEN; break;
                    case 2: color = YELLOW; break;
                    case 3: color = BLUE; break;
                    case 4: color = MAGENTA; break;
                    default: color = CYAN; break;
                }
                printf("├── %s%d (%s)%s | Chemin: ", color, g->utilisateurs[i]->id, g->utilisateurs[i]->nom, RESET);
                afficherChemin(parent, i); // Affichage du chemin
                printf("\n");
            }
        }
    }

    free(visited);
    free(distance);
    free(parent);
    free(queue);
}

void dijkstra(const graphePondere g, int src_id, int dest_id) //r
{
    if (!g) {
        printf("Graphe invalide.\n");
        return;
    }

    int indiceSrc = trouve(g, src_id);
    int indiceDest = trouve(g, dest_id);

    if (indiceSrc == -1 || indiceDest == -1) {
        printf("Utilisateur(s) non trouvé(s) avec les ID spécifiés.\n");
        return;
    }

    int* dist = (int*)malloc(g->v * sizeof(int));
    int* prev = (int*)malloc(g->v * sizeof(int));
    int* visite = (int*)calloc(g->v, sizeof(int));

    // Initialisation des distances
    for (int i = 0; i < g->v; i++) {
        dist[i] = INT_MAX;
        prev[i] = -1;
    }
    dist[indiceSrc] = 0; // Distance départ = 0

    for (int count = 0; count < g->v - 1; count++) {
        // Trouve le noeud avec la distance minimale
        int u = -1;
        for (int i = 0; i < g->v; i++) {
            if (!visite[i] && dist[i] != INT_MAX && (u == -1 || dist[i] < dist[u])) {
                u = i;
            }
        }

        if (u == -1 || u == indiceDest) break;
        visite[u] = 1;

        // Parcours des voisins
        Node voisin = g->adjList[u];
        while (voisin) {
            int v = trouve(g, voisin->user->id);
            int poids = voisin->weight;

            // Mise à jour de la distance si chemin plus court trouvé
            if (!visite[v] && dist[u] != INT_MAX && dist[u] + poids < dist[v]) {
                dist[v] = dist[u] + poids;
                prev[v] = u;
            }
            voisin = voisin->next;
        }
    }

    // Affichage du résultat
    if (dist[indiceDest] != INT_MAX) {
        printf("\n%sChemin le plus court entre %s (%d) et %s (%d) - Distance totale : %d%s\n",
               BOLD, g->utilisateurs[indiceSrc]->nom, src_id,
               g->utilisateurs[indiceDest]->nom, dest_id,
               dist[indiceDest], RESET);

        // Reconstruction du chemin
        int chemin[g->v];
        int taille = 0;
        for (int v = indiceDest; v != -1; v = prev[v]) {
            chemin[taille++] = v;
        }

        printf("Chemin : ");
        for (int i = taille - 1; i >= 0; i--) {
            printf("%s%s%s", GREEN, g->utilisateurs[chemin[i]]->nom, RESET);
            if (i > 0) printf(" → ");
        }
        printf("\n");
    }
    else {
        printf("%sAucun chemin entre %s (%d) et %s (%d)%s\n",
               RED, g->utilisateurs[indiceSrc]->nom, src_id,
               g->utilisateurs[indiceDest]->nom, dest_id, RESET);
    }

    free(dist);
    free(prev);
    free(visite);
}

void dfsRecCommunautes(const graphePondere g, int idUser, int* visite, int tag) //r
{
    if (!g || !visite || idUser < 0) return;

    visite[idUser] = tag;  // Marquer avec le tag de la communauté

    Node voisin = g->adjList[idUser];
    while (voisin)
    {
        int idVoisin = voisin->user->id;
        int indexVoisin = trouve(g, idVoisin);

        if (indexVoisin != -1 && visite[indexVoisin] == 0)
        {
            dfsRecCommunautes(g, indexVoisin, visite, tag);
        }
        voisin = voisin->next;
    }
}

void trouver_communautes(const graphePondere g)
{
    if (!g) return;

    int* visite = (int*)calloc(g->v, sizeof(int));
    if (!visite)
    {
        printf("%sErreur d'allocation mémoire.%s\n", RED, RESET);
        return;
    }

    int communaute = 1;

    const char* couleurs[] = {CYAN, GREEN, YELLOW, MAGENTA, BLUE, BOLD}; // Ajoute d'autres couleurs si besoin
    int nbCouleurs = sizeof(couleurs) / sizeof(couleurs[0]);

    for (int i = 0; i < g->v; i++)
    {
        if (visite[i] == 0 && g->utilisateurs[i])
        {
            int tag = communaute;
            const char* couleur = couleurs[(communaute - 1) % nbCouleurs];

            printf("\n%sCommunauté %d :%s\n", couleur, communaute, RESET);

            dfsRecCommunautes(g, i, visite, tag);

            for (int j = 0; j < g->v; j++)
            {
                if (visite[j] == tag)
                {
                    printf("  └─ %s%s (ID %d)%s\n", couleur, g->utilisateurs[j]->nom, g->utilisateurs[j]->id, RESET);
                }
            }

            communaute++;
        }
    }

    free(visite);
}

// ---------------------------
// 5. FICHIERS & AFFICHAGE
// ---------------------------
graphePondere chargerGraphe(const char* filename) //r
{
    FILE* file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "Erreur : impossible d'ouvrir le fichier %s\n", filename);
        return NULL;
    }

    graphePondere g = creerGraphe(0);
    if (!g)
    {
        fclose(file);
        return NULL;
    }

    char buffer[256];
    int line_num = 0;

    while (fgets(buffer, sizeof(buffer), file))
    {
        line_num++;
        // Enlève les espaces inutiles ou les retours à la ligne
        buffer[strcspn(buffer, "\r\n")] = '\0';  // Supprime les retours à la ligne

        // Gestion utilisateur
        if (buffer[0] == 'U')
        {
            int id, age;
            char nom[50];

            // Parsing sécurisé
            if (sscanf(buffer + 1, "%d %49s %d", &id, nom, &age) != 3)
            {
                fprintf(stderr, "Ligne %d mal formatée (U) : %s", line_num, buffer);
                continue;
            }

            ELEMENT user = creerUtilisateur(id, nom, age);
            if (!user)
            {
                fprintf(stderr, "Erreur création utilisateur ligne %d\n", line_num);
                continue;
            }

            // Ajout unique avec gestion d'erreur
            if (!ajouterUtilisateur(g, user))
            {
                fprintf(stderr, "Échec ajout utilisateur ligne %d (ID: %d)\n", line_num, id);
                elementDetruire(user); // Nettoyage mémoire
            }
        }
        // Gestion relation
        else if (buffer[0] == 'R')
        {
            int src, dest, poids;

            // Parsing sécurisé
            if (sscanf(buffer + 1, "%d %d %d", &src, &dest, &poids) != 3)
            {
                fprintf(stderr, "Ligne %d mal formatée (R) : %s", line_num, buffer);
                continue;
            }

            // vérification existence des IDs
            if (trouve(g, src) == -1 || trouve(g, dest) == -1)
            {
                fprintf(stderr, "Relation invalide ligne %d : ID inexistant (src: %d, dest: %d)\n", line_num, src, dest);
                continue;
            }

            if (!ajouterRelation(g, src, dest, poids))
            {
                fprintf(stderr, "Échec création relation ligne %d\n", line_num);
            }
        }
        else
        {
            fprintf(stderr, "Ligne %d ignorée : format inconnu : %s\n", line_num, buffer);
        }
    }

    fclose(file);
    return g;
}

void sauvegarderGraphe(const graphePondere g, const char* filename) //r
{
    if (!g || !filename)
    {
        fprintf(stderr, "Erreur : graphe ou nom de fichier NULL\n");
        return;
    }
    FILE* file = fopen(filename, "w");
    if (!file)
    {
        fprintf(stderr, "Erreur d'ouverture du fichier %s\n", filename);
        return;
    }

    // Sauvegarde des utilisateurs
    for (int i = 0; i < g->v; i++)
    {
        if (g->utilisateurs[i])
        {
            fprintf(file, "U %d %s %d\n",
                    g->utilisateurs[i]->id,
                    g->utilisateurs[i]->nom,
                    g->utilisateurs[i]->age);
        }
    }

    // Sauvegarde des relations sans doublons
    for (int i = 0; i < g->v; i++)
    {
        Node courant = g->adjList[i];
        while (courant)
        {
            int src_id = ids[i]; // ID source
            int dest_id = courant->user->id; // ID destination

            // Éviter les doublons en comparant les IDs
            if (src_id < dest_id)
            {
                fprintf(file, "R %d %d %d\n",
                        src_id,
                        dest_id,
                        courant->weight);
            }
            courant = courant->next;
        }
    }
    fclose(file);
    printf("Graphe sauvegardé avec succès dans %s\n", filename);
}

void afficherGraphe(const graphePondere g) //r
{
    // Vérifications renforcées
    if (!g)
    {
        fprintf(stderr, "Erreur: Graphe NULL\n");
        return;
    }
    if (!g->utilisateurs || !g->adjList)
    {
        fprintf(stderr, "Erreur: Structure du graphe invalide\n");
        return;
    }
    if (g->v <= 0)
    {
        printf("Graphe vide\n");
        return;
    }

    printf("\n=========== RÉSEAU SOCIAL ===========\n");
    printf(" Utilisateurs actuels: %d\n\n", g->v);

    for (int i = 0; i < g->v; i++)    // Seulement les utilisateurs existants
    {
        // Vérification utilisateur
        ELEMENT user = g->utilisateurs[i];
        if (!user)
        {
            fprintf(stderr, " Utilisateur %d est NULL (inexistant)\n", i);
            continue;
        }

        printf(" Utilisateur %d : ID=%d | Nom=%s | Âge=%d\n",
               i, user->id, user->nom, user->age);
        // Vérification relations
        Node courant = g->adjList[i];
        if (!courant)
        {
            printf("    Aucune relation.\n\n");
            continue;
        }

        printf("    Relations:\n");
        while (courant)
        {
            if (courant->user)
            {
                printf("      → ID:%d (%s), poids: %d\n",
                       courant->user->id,
                       courant->user->nom,
                       courant->weight);
            }
            else
            {
                fprintf(stderr, "       Relation invalide (pointeur user NULL)\n");
            }
            courant = courant->next;
        }
        printf("\n");
    }

    printf("=====================================\n");
}

void exporterGraphviz(graphePondere g, const char* filename) //r
{
    if (!g || !g->utilisateurs || !g->adjList)
    {
        printf("Graphe invalide\n");
        return;
    }

    FILE* file = fopen(filename, "w");
    if (!file)
    {
        printf("Erreur lors de la création du fichier DOT.\n");
        return;
    }

    fprintf(file, "graph G {\n");

    // Style des nœuds : ovales, remplis en bleu ciel, noms en gras
    fprintf(file, "  node [shape=ellipse, style=filled, fillcolor=skyblue, fontcolor=black, fontname=\"Arial Bold\"];\n");

    // Style des arêtes
    fprintf(file, "  edge [penwidth=1.5, fontcolor=black, fontname=\"Arial\"];\n");

    for (int i = 0; i < g->v; i++)
    {
        ELEMENT e = g->utilisateurs[i];
        if (!e) continue;

        fprintf(file, "  %d [label=\"%s\"];\n", e->id, e->nom);

        Node adj = g->adjList[i];
        while (adj != NULL)
        {
            if (!adj->user)
            {
                adj = adj->next;
                continue;
            }

            ELEMENT dest = adj->user;
            if (e->id < dest->id)
            {
                const char* color;
                if (adj->weight < 10)
                    color = "green";
                else if (adj->weight <= 50)
                    color = "orange";
                else
                    color = "red";

                fprintf(file, "  %d -- %d [label=\"%d\", color=%s];\n", e->id, dest->id, adj->weight, color);
            }

            adj = adj->next;
        }
    }

    fprintf(file, "}\n");
    fclose(file);
}




int isGraphvizInstalled() //r
{
    char cmd[256];
    sprintf(cmd, "\"%s\" -v > nul 2>&1", GRAPHvIZ_PATH);
    return system(cmd) == 0;
}

void generateAndOpenImage(const char* dotFileName) //r
{
    if (!dotFileName)
    {
        fprintf(stderr, "Erreur: Nom de fichier DOT invalide\n");
        return;
    }

    char cmd[256];
    snprintf(cmd, sizeof(cmd), "dot -Tpng %s -o graphe.png", dotFileName);

    if (system(cmd) == 0)
    {
#ifdef _WIN32
        system("start graphe.png");
#else
        system("xdg-open graphe.png");
#endif
    }
    else
    {
        fprintf(stderr, "Erreur generation image. Verifiez que:\n");
        fprintf(stderr, "1. Graphviz est installe (tapez 'dot -V' dans le terminal)\n");
        fprintf(stderr, "2. Le fichier %s existe et est valide\n", dotFileName);
    }
}


void verifierGraphe(graphePondere g)
{
    printf("Vérification du graphe :\n");
    printf("Nombre d'utilisateurs (g->v) : %d\n", g->v);
    printf("Adresse utilisateurs : %p\n", (void*)g->utilisateurs);
    printf("Adresse adjList : %p\n", (void*)g->adjList);

    for (int i = 0; i < g->v; i++)
    {
        if (!g->utilisateurs[i])
        {
            printf("ERREUR: Utilisateur %d non initialisé\n", i);
            continue;
        }
        printf("Utilisateur %d: %s (ID: %d)\n",
               i, g->utilisateurs[i]->nom, g->utilisateurs[i]->id);

        Node relation = g->adjList[i];
        while (relation)
        {
            printf("  Relation avec %d (poids: %d)\n",
                   relation->user->id, relation->weight);
            relation = relation->next;
        }
    }
}

// ---------------------------
// 5. Ctrl et son
// ---------------------------

void playBeep()
{
    Beep(1000, 200);// frequency 1000 Hz, duration 200 ms
}
void playErrorBeep()
{
    Beep(400, 300);  // bip grave
    Sleep(100);      // petite pause
    Beep(400, 300);  // bip grave encore
}
void playByeByeBeep()
{
    Beep(1000, 200); // ton haut
    Sleep(70);
    Beep(800, 200);  // ton moyen
    Sleep(70);
    Beep(600, 300);  // ton bas : comme "byeeee"
}
void clearBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
int lireEntier(const char* message) {
    int n;
    char buffer[100];
    while (1) {
        printf("%s", message);

        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            // Vider le buffer *seulement* si la ligne est trop longue
            if (strchr(buffer, '\n') == NULL) {
                clearBuffer();
            }

            if (sscanf(buffer, "%d", &n) == 1 && strchr(buffer, '.') == NULL && n>=1) {
                return n;
            } else {
                playErrorBeep();
                printf(" Entrée invalide. Veuillez entrer un entier positif.\n");
            }
        }
    }
}

void lireNom(char* nom, int maxLen)
{
    char buffer[100];
    while (1)
    {
        printf("Nom : ");
        if (fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            // Supprimer le \n à la fin
            buffer[strcspn(buffer, "\n")] = '\0';

            if (strlen(buffer) > 0 && strlen(buffer) < maxLen)
            {
                strcpy(nom, buffer);
                return;
            }
            else
            {
                playErrorBeep();
                printf(" Nom invalide. Réessayez.\n");
            }
        }
    }
}
