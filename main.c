#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // Ajouté pour strcpy
#include "ELTUSER.h"

#include "GRAPHE.h"
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

void menu()
{
    printf(BOLD CYAN "\n╔═════════════════════════════════════════════════════════╗\n");
    printf("║              MENU DU " YELLOW "RÉSEAU SOCIAL" CYAN "                      ║\n");
    printf("╠═════════════════════════════════════════════════════════╣\n");
    printf(RESET BOLD);
    printf("║  " GREEN "1" RESET BOLD "   - Charger le graphe depuis le fichier " YELLOW "-reseau.txt-" RESET BOLD " ║\n");
    printf("║  " GREEN "2" RESET BOLD "   - Afficher le graphe                               ║\n");
    printf("║  " GREEN "3" RESET BOLD "   - Ajouter un utilisateur                           ║\n");
    printf("║  " GREEN "4" RESET BOLD "   - Supprimer un utilisateur                         ║\n");
    printf("║  " GREEN "5" RESET BOLD "   - Ajouter une relation                             ║\n");
    printf("║  " GREEN "6" RESET BOLD "   - Supprimer une relation                           ║\n");
    printf("║  " GREEN "7" RESET BOLD "   - Trouver les amis (DFS)                           ║\n");
    printf("║  " GREEN "8" RESET BOLD "   - Trouver les amis à une distance N (BFS)          ║\n");
    printf("║  " GREEN "9" RESET BOLD "   - Trouver le chemin le plus fort (Dijkstra)        ║\n");
    printf("║ " GREEN "10" RESET BOLD "   - Détecter les communautés                         ║\n");
    printf("║ " GREEN "11" RESET BOLD "   - Sauvegarder                                      ║\n");
    printf("║  " RED "0" RESET BOLD "   - Quitter                                          ║\n");
    printf(CYAN "╚═════════════════════════════════════════════════════════╝\n" RESET);
}



int main()
{
    graphePondere g = NULL;
    int choix, id, id1, id2, poids, result;
    char nom[50];
    int age;
    g = chargerGraphe("reseau.txt");
    system("cls");
    do
    {
        system("chcp 65001 > nul");
        menu();
        playBeep();
        do
        {
            printf("- Choix : ");

            result = scanf("%d", &choix);

            if (result != 1)
            {
                playErrorBeep();
                printf(" Donner un entier valide (entre 0 et 11) !\n");
                clearBuffer(); // Discard invalid input
            }
            playBeep();

        }
        while (result != 1);
        switch (choix)
        {
        case 1:
            Sleep(100);
            system("cls");
            g = chargerGraphe("reseau.txt");
            if (g == NULL)
            {
                printf("Erreur de chargement du graphe.\n");
            }
            else
            {
                printf("Graphe chargé avec succes.\n");
            }
            printf("\nAppuyez sur Entrée pour continuer...");
            getchar();
            getchar();
            Sleep(400);
            system("cls");
            break;
        case 2:
            Sleep(100);
            system("cls");
            if (g == NULL)
            {
                printf("Erreur : graphe non chargé. Veuillez d'abord charger le graphe avec l'option 1.\n");
            }
            else
            {
                verifierGraphe(g);
                Sleep(1000);
                printf("\nAppuyez sur Entrée pour voir le graphe");
                getchar();
                getchar();
                remove("graphe.png");
                exporterGraphviz(g,"graphe.dot");
                generateAndOpenImage("graphe.dot");
            }
            Sleep(1000);
            system("cls");
            break;
        case 3:
        {
            system("cls");

            printf("la liste des id reserves: \n <");
            for(int i=0;i<g->v;i++)
                printf("%d\t",g->utilisateurs[i]->id);
            printf(">\n");
            clearBuffer();
            id = lireEntier("ID de l'utilisateur à ajouter : ");
            lireNom(nom, 50);
            age = lireEntier("Âge : ");
            ELEMENT user = creerUtilisateur(id, nom, age);
            if (ajouterUtilisateur(g,user))
                    printf("Relation ajoutee avec succes");
            printf("\nAppuyez sur Entrée pour continuer...");
            getchar();// pause manuelle
            sauvegarderGraphe(g,"reseau.txt");
            Sleep(1000);
            system("cls");
            break;
        }
        case 4:
            system("cls");
            afficherGraphe(g);
            clearBuffer();
            id = lireEntier("ID de l'utilisateur à supprimer : ");
            supprimerUtilisateur(g,id);
            printf("\nAppuyez sur Entrée pour continuer...");
            getchar();// pause manuelle
            sauvegarderGraphe(g,"reseau.txt");
            Sleep(1000);
            system("cls");
            break;
        case 5:
            Sleep(100);
            system("cls");
            afficherGraphe(g);
            clearBuffer();
            id1 = lireEntier("ID source : ");
            id2 = lireEntier("ID destination : ");
            poids = lireEntier("poids : ");
            ajouterRelation(g, id1, id2, poids);
            printf("\nAppuyez sur Entrée pour continuer...");
            getchar();// pause manuelle
            sauvegarderGraphe(g,"reseau.txt");
            Sleep(1000);
            system("cls");
            break;
        case 6:
            Sleep(100);
            system("cls");
            afficherGraphe(g);
            clearBuffer();
            id1 = lireEntier("ID source : ");
            id2 = lireEntier("ID destination : ");
            supprimerRelation(g, id1, id2);
            printf("\nAppuyez sur Entrée pour continuer...");
            getchar();// pause manuelle
            sauvegarderGraphe(g,"reseau.txt");
            Sleep(1000);
            system("cls");
            break;
        case 7:
        {
            Sleep(100);
            system("cls");
            afficherGraphe(g);
            clearBuffer();
            id = lireEntier("ID de l'utilisateur pour DFS : ");
            int* visite = (int*)calloc(g->v, sizeof(int));
            dfs(g, id);
            free(visite);
            printf("\nAppuyez sur Entrée pour continuer...");
            getchar();// pause manuelle
            Sleep(100);
            system("cls");
            break;
        }
        case 8:
            Sleep(100);
            system("cls");
            afficherGraphe(g);
            clearBuffer();
            id = lireEntier("ID de l'utilisateur pour BFS : ");
            bfs(g, id);
            printf("\nAppuyez sur Entrée pour continuer...");
            getchar();
            Sleep(400);
            system("cls");
            break;
        case 9:
            Sleep(100);
            system("cls");
            afficherGraphe(g);
            clearBuffer();
            id1 = lireEntier("ID source : ");
            id2 = lireEntier("ID destination : ");
            printf("\nAppuyez sur Entrée pour continuer...");
            dijkstra(g,id1,id2);
            getchar();
            Sleep(400);
            system("cls");
            break;
        case 10:
            Sleep(100);
            system("cls");
            trouver_communautes(g);
            printf("\nAppuyez sur Entrée pour continuer...");
            getchar();
            getchar();
            Sleep(400);
            system("cls");
            break;
        case 11:
            sauvegarderGraphe(g,"reseau.txt");
            printf("\nAppuyez sur Entrée pour continuer...");
            getchar();
            getchar();// pause manuelle
            Sleep(100);
            system("cls");
            break;
        case 0:
            Sleep(100);
            system("cls");
            printf("Merci ! Fin du programme.\n");
            playByeByeBeep();
            if (g != NULL)
            {
                detruireGraphe(g);
            }
            break;
        default:
            playErrorBeep();
            Sleep(100);
            system("cls");
            printf("Choix invalide. Reesayer!!!\n");
            Sleep(2500);
        }
    }
    while (choix != 0);

    return 0;
}
