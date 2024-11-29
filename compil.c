#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 100  // Taille de la table de hachage

typedef struct Symbole {
    char *name;            // Nom
    char *type;            // Type (int, float, ...)
    char *form;            // Forme (variable simple, tableau, structure)
    int memoryAddress;     // Adresse mémoire
    char *scope;           // Portée (globale, locale, ...)
    int line;              // Ligne d'apparition dans le code source
    int size;              // Taille mémoire (spécifique pour tableaux ou structures)
    struct Symbole *next;  // Gestion des collisions
} Symbole;

// Table de symboles (avec gestion des portées)
typedef struct SymboleTable {
    Symbole *table[TABLE_SIZE];  // Table de hachage avec listes chaînées
    struct SymboleTable *parent; // Portée parent
} SymboleTable;

// Fonction de hachage
int hashFunction(const char *name) {
    int hash = 0;
    while (*name) {
        hash = (hash * 60 + *name) % TABLE_SIZE;
        name++;
    }
    return hash;
}

// Fonction pour créer une table de symboles
SymboleTable *createSymboleTable(SymboleTable *parent) {
    SymboleTable *table = malloc(sizeof(SymboleTable));
    for (int i = 0; i < TABLE_SIZE; i++) {
        table->table[i] = NULL;
    }
    table->parent = parent;
    return table;
}

// Fonction pour rechercher un symbole (avec gestion des portées)
Symbole *RechercheSymbole(SymboleTable *table, const char *name) {
    int index = hashFunction(name);
    Symbole *current = table->table[index];

    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }

    // Recherche dans la portée parent si non trouvé
    if (table->parent != NULL) {
        return RechercheSymbole(table->parent, name);
    }
    return NULL;
}

// Fonction pour afficher les informations d'un symbole
void AfficherSymbole(Symbole *symbole) {
    if (symbole != NULL) {
        printf("Nom : %s\n", symbole->name);
        printf("Type : %s\n", symbole->type);
        printf("Forme : %s\n", symbole->form);
        printf("Adresse mémoire : %d\n", symbole->memoryAddress);
        printf("Portée : %s\n", symbole->scope);
        printf("Ligne : %d\n", symbole->line);
        printf("Taille : %d\n", symbole->size);
    } else {
        printf("Symbole non trouvé.\n");
    }
}

// Fonction pour ajouter un symbole
void AjoutSymbole(SymboleTable *table, const char *name, const char *type, const char *form,
                  int memoryAddress, const char *scope, int line, int size) {
    int index = hashFunction(name);

    // Vérifier si le symbole existe déjà
    Symbole *current = RechercheSymbole(table, name);
    if (current != NULL) {
        printf("Symbole '%s' existe déjà dans la table.\n", name);
        return;
    }

    // Créer un nouveau symbole
    Symbole *newSymbole = malloc(sizeof(Symbole));
    newSymbole->name = strdup(name);
    newSymbole->type = strdup(type);
    newSymbole->form = strdup(form);
    newSymbole->memoryAddress = memoryAddress;
    newSymbole->scope = strdup(scope);
    newSymbole->line = line;
    newSymbole->size = size;
    newSymbole->next = table->table[index]; // Insérer en tête de liste
    table->table[index] = newSymbole;

    printf("Ajouté : %s, Type : %s, Forme : %s, Adresse mémoire : %d, Portée : %s, Ligne : %d, Taille : %d\n",
           name, type, form, memoryAddress, scope, line, size);
}

// Fonction pour mettre à jour un symbole
void UpdateSymbole(SymboleTable *table, const char *name, const char *type, const char *form,
                   int memoryAddress, const char *scope, int line, int size) {
    Symbole *current = RechercheSymbole(table, name);
    if (current == NULL) {
        printf("Symbole '%s' non trouvé dans la table pour mise à jour.\n", name);
        return;
    }

    // Mettre à jour les informations du symbole
    if (type != NULL) {
        free(current->type);
        current->type = strdup(type);
    }
    if (form != NULL) {
        free(current->form);
        current->form = strdup(form);
    }
    if (scope != NULL) {
        free(current->scope);
        current->scope = strdup(scope);
    }
    current->memoryAddress = memoryAddress;
    current->line = line;
    current->size = size;

    printf("Mis à jour : %s, Type : %s, Forme : %s, Adresse mémoire : %d, Portée : %s, Ligne : %d, Taille : %d\n",
           name, current->type, current->form, current->memoryAddress, current->scope, current->line, current->size);
}

// Fonction pour afficher les symboles dans une table
void PrintSymboleTable(SymboleTable *table) {
    printf("Table des Symboles :\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        Symbole *current = table->table[i];
        while (current != NULL) {
            printf("Nom : %s, Type : %s, Forme : %s, Adresse mémoire : %d, Portée : %s, Ligne : %d, Taille : %d\n",
                   current->name, current->type, current->form, current->memoryAddress, current->scope, current->line, current->size);
            current = current->next;
        }
    }
    if (table->parent != NULL) {
        printf("Portée parent :\n");
        PrintSymboleTable(table->parent);
    }
}

// Fonction pour entrer dans une nouvelle portée
SymboleTable *EnterScope(SymboleTable *current) {
    return createSymboleTable(current); // Crée une nouvelle table avec la table actuelle comme parent
}

// Fonction pour sortir de la portée actuelle
SymboleTable *ExitScope(SymboleTable *current) {
    SymboleTable *parent = current->parent;
    free(current); // Libérer la table actuelle
    return parent;
}
