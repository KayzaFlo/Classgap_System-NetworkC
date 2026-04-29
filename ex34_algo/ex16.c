typedef struct {
    int    lignes;
    int    colonnes;
    double** data;   // tableau 2D alloué dynamiquement
} matrice_t;


// TEST
matrice_t* creerMatrice(int lignes, int colonnes) {
    matrice_t* m = malloc(sizeof(matrice_t));
    m->lignes   = lignes;
    m->colonnes = colonnes;

    // On alloue un tableau de pointeurs (un par ligne)
    m->data = malloc(lignes * sizeof(double*));
    for (int i = 0; i < lignes; i++)
        m->data[i] = malloc(colonnes * sizeof(double));

    return m;
}

// Accès à une case : m->data[i][j]

// La clé est d'allouer le tableau de données dynamiquement pour gérer n'importe quelle dimension.

// L'idée du double** c'est un tableau de tableaux,
// chaque ligne est un tableau indépendant,
// ce qui permet des matrices de toutes tailles sans rien connaître à la compilation.