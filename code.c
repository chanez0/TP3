#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

void produitMatrices(int n, int m, int p, const char *fichierA, const char *fichierB)
{
    int i, j, k;
    FILE *fichier;
    fichier = fopen("matriceC.txt", "w");
    if (fichier == NULL)
    {
        printf("Erreur: Impossible de creer le fichier matriceC.txt\n");
        return;
    }

    float A[n][m];
    float B[m][p];
    // pour stocker les valeurs de A et B
    float c; // pour stocker le resultat partiel avant de l'ecrire dans le fichier
    FILE *fa = fopen(fichierA, "r");
    FILE *fb = fopen(fichierB, "r");
    if (fa == NULL || fb == NULL)
    {
        printf("Erreur: Impossible d'ouvrir les fichiers des matrices A ou B\n");
        return;
    }

    // Ignorer les dimensions
    fscanf(fa, "%*d %*d");
    fscanf(fb, "%*d %*d");

    // Lire la matrice A
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
        {
            fscanf(fa, "%f", &A[i][j]);
        }
    }
    fclose(fa);

    // Lire la matrice B
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < p; j++)
        {
            fscanf(fb, "%f", &B[i][j]);
        }
    }
    fclose(fb);

    fprintf(fichier, "%d %d\n", n, p);

    // Calcul du produit selon la formule donnee
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < p; j++)
        {
            c = 0; /////Initialisation de c a 0
            for (k = 0; k < m; k++)
            {
                c += A[i][k] * B[k][j];
            }
            fprintf(fichier, "%.2f ", c);
        }
        fprintf(fichier, "\n");
    }
    fclose(fichier);
    printf("Produit des matrices A et B sauvegarde dans 'matriceC.txt'\n");
}

// Fonction pour afficher matrice
void afficherMatrice(const char *nom_fichier)
{
    FILE *fichier;
    int lignes, colonnes, i, j;
    float valeur;

    fichier = fopen(nom_fichier, "r");
    if (fichier == NULL)
    {
        printf("Erreur: Impossible d'ouvrir le fichier %s\n", nom_fichier);
        return;
    }

    // Lire les dimensions
    fscanf(fichier, "%d %d", &lignes, &colonnes);
    printf("\nContenu de '%s' (%dx%d):\n", nom_fichier, lignes, colonnes);

    // Lire et afficher les valeurs
    for (i = 0; i < lignes; i++)
    {
        for (j = 0; j < colonnes; j++)
        {
            fscanf(fichier, "%f", &valeur);
            printf("%8.2f ", valeur);
        }
        printf("\n");
    }

    fclose(fichier);
}

// Fonction pour vérifier si B correspond à A à la position et retourne booleen
bool verifierCorrespondance(float **A, float **B, int posI, int posJ, int nPrime, int mPrime)
{
    for (int x = 0; x < nPrime; x++)
    {
        for (int y = 0; y < mPrime; y++)
        {
            if (A[posI + x][posJ + y] != B[x][y])
            {
                return false;
            }
        }
    }
    return true;
}

// SousMat1 - Version naïve (matrices non triées)
void SousMat1(const char *fichierA, const char *fichierB)
{
    int n, m, nPrime, mPrime;
    FILE *fa = fopen(fichierA, "r");
    FILE *fb = fopen(fichierB, "r");

    if (fa == NULL || fb == NULL)
    {
        printf("Erreur: Impossible d'ouvrir les fichiers des matrices A ou B\n");
        return;
    }

    // Lire les dimensions
    fscanf(fa, "%d %d", &n, &m);
    fscanf(fb, "%d %d", &nPrime, &mPrime);

    // Vérifier les contraintes
    if (nPrime >= n || mPrime >= m)
    {
        printf("SousMat1: La matrice B n'est pas une sous-matrice de A (dimensions incorrectes)\n");
        fclose(fa);
        fclose(fb);
        return;
    }

    // Allouer et lire la matrice A car on doit la parcourir
    float **A = (float **)malloc(n * sizeof(float *));
    for (int i = 0; i < n; i++)
    {
        A[i] = (float *)malloc(m * sizeof(float));
        for (int j = 0; j < m; j++)
        {
            fscanf(fa, "%f", &A[i][j]);
        }
    }
    fclose(fa);

    // Allouer et lire la matrice B allouer car on doit la parcourir
    float **B = (float **)malloc(nPrime * sizeof(float *));
    for (int i = 0; i < nPrime; i++)
    {
        B[i] = (float *)malloc(mPrime * sizeof(float));
        for (int j = 0; j < mPrime; j++)
        {
            fscanf(fb, "%f", &B[i][j]);
        }
    }
    fclose(fb);

    // Recherche exhaustive de B dans A
    for (int i = 0; i <= n - nPrime; i++)
    {
        for (int j = 0; j <= m - mPrime; j++)
        {
            if (verifierCorrespondance(A, B, i, j, nPrime, mPrime))
            {
                printf("SousMat1: La matrice B est une sous-matrice de A à la position (%d, %d)\n", i, j);

                // Libérer la mémoire
                for (int x = 0; x < n; x++)
                    free(A[x]);
                free(A);
                for (int x = 0; x < nPrime; x++)
                    free(B[x]);
                free(B);
                return;
            }
        }
    }

    printf("SousMat1: La matrice B n'est pas une sous-matrice de A\n");

    // Libérer la mémoire
    for (int i = 0; i < n; i++)
        free(A[i]);
    free(A);
    for (int i = 0; i < nPrime; i++)
        free(B[i]);
    free(B);
}

// SousMat2 - Version optimisée (matrices avec lignes triées)
void SousMat2(const char *fichierA, const char *fichierB)
{
    int n, m, nPrime, mPrime;
    FILE *fa = fopen(fichierA, "r");
    FILE *fb = fopen(fichierB, "r");

    if (fa == NULL || fb == NULL)
    {
        printf("Erreur: Impossible d'ouvrir les fichiers des matrices A ou B\n");
        return;
    }

    // Lire les dimensions
    fscanf(fa, "%d %d", &n, &m);
    fscanf(fb, "%d %d", &nPrime, &mPrime);

    // Vérifier les contraintes
    if (nPrime >= n || mPrime >= m)
    {
        printf("SousMat2: La matrice B n'est pas une sous-matrice de A (dimensions incorrectes)\n");
        fclose(fa);
        fclose(fb);
        return;
    }

    // Allouer et lire la matrice A
    float **A = (float **)malloc(n * sizeof(float *));
    for (int i = 0; i < n; i++)
    {
        A[i] = (float *)malloc(m * sizeof(float));
        for (int j = 0; j < m; j++)
        {
            fscanf(fa, "%f", &A[i][j]);
        }
    }
    fclose(fa);

    // Allouer et lire la matrice B meme scenario que sousmat1
    float **B = (float **)malloc(nPrime * sizeof(float *));
    for (int i = 0; i < nPrime; i++)
    {
        B[i] = (float *)malloc(mPrime * sizeof(float));
        for (int j = 0; j < mPrime; j++)
        {
            fscanf(fb, "%f", &B[i][j]);
        }
    }
    fclose(fb);

    float premierElementB = B[0][0];

    // Recherche optimisée en utilisant le tri des lignes
    for (int i = 0; i <= n - nPrime; i++)
    {
        for (int j = 0; j <= m - mPrime; j++)
        {
            // Optimisation 1: si A[i][j] < B[0][0], continuer (ligne triée)
            if (A[i][j] < premierElementB)
            {
                continue;
            }

            // Optimisation 2: si A[i][j] > B[0][0], passer à la ligne suivante
            if (A[i][j] > premierElementB)
            {
                break;
            }

            // A[i][j] == B[0][0], vérifier la correspondance complète
            if (verifierCorrespondance(A, B, i, j, nPrime, mPrime))
            {
                printf("SousMat2: La matrice B est une sous-matrice de A à la position (%d, %d)\n", i, j);

                // Libérer la mémoire
                for (int x = 0; x < n; x++)
                    free(A[x]);
                free(A);
                for (int x = 0; x < nPrime; x++)
                    free(B[x]);
                free(B);
                return;
            }
        }
    }

    printf("SousMat2: La matrice B n'est pas une sous-matrice de A\n");

    // Libérer la mémoire
    for (int i = 0; i < n; i++)
        free(A[i]);
    free(A);
    for (int i = 0; i < nPrime; i++)
        free(B[i]);
    free(B);
}

int main()
{
    int n, m, p, m2, choix;
    char fichierA[100], fichierB[100];
    char continuer;
    double temps_execution;

    printf("Entrez le nom du fichier de la matrice A : ");
    scanf("%s", fichierA);

    printf("Entrez le nom du fichier de la matrice B : ");
    scanf("%s", fichierB);

    // Lire les dimensions de A et B pour le produit matriciel
    FILE *fa = fopen(fichierA, "r");
    FILE *fb = fopen(fichierB, "r");

    if (fa == NULL || fb == NULL)
    {
        printf("Erreur: Impossible d'ouvrir les fichiers\n");
        return 1;
    }

    fscanf(fa, "%d %d", &n, &m);

    fscanf(fb, "%d %d", &m2, &p);
    fclose(fa);
    fclose(fb);

    // Menu de choix avec boucle pour plusieurs opérations

    do
    {

        printf("     Choisissez une operation\n");
        printf("1. Produit matriciel (A x B)\n");
        printf("2. Recherche sous-matrice (SousMat1)\n");
        printf("3. Recherche sous-matrice (SousMat2)\n");
        printf("Votre choix : ");
        scanf("%d", &choix);

        clock_t debut = clock();

        switch (choix)
        {
        case 1:
            if (m == m2)
            {
                printf("\n=== Calcul du produit matriciel C = A x B ===\n");
                produitMatrices(n, m, p, fichierA, fichierB);
                afficherMatrice("matriceC.txt");
            }
            else
            {
                printf("\nErreur: Les matrices ne sont pas compatibles pour le produit\n");
                printf("(nombre de colonnes de A = %d ≠ nombre de lignes de B = %d)\n", m, m2);
            }
            break;

        case 2:
            printf("\nRecherche avec SousMat1 (version naive)\n");
            SousMat1(fichierA, fichierB);
            break;

        case 3:
            printf("\nRecherche avec SousMat2 (version optimisee)\n");
            SousMat2(fichierA, fichierB);
            break;

        default:
            printf("\nErreur: Choix invalide. Veuillez choisir 1, 2 ou 3.\n");
            break;
        }
        clock_t fin = clock();
        temps_execution = (double)(fin - debut) / CLOCKS_PER_SEC; // double pour faire l'operation en double pas int
        printf("Temps d'execution : %f secondes\n", temps_execution);
        printf("\nVoulez-vous effectuer une autre action? O/N : ");
        scanf(" %c", &continuer);
    } while (continuer == 'O' || continuer == 'o');

    return 0;
}
