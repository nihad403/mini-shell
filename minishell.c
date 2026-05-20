#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARGS 64

/*
 * Supprime le retour a la ligne ajoute par getline().
 * Exemple: "ls -l\n" devient "ls -l".
 */
void remove_newline(char *line)
{
    size_t len;

    len = strlen(line);
    if (len > 0 && line[len - 1] == '\n')
        line[len - 1] = '\0';
}

/*
 * Decoupe la commande en plusieurs arguments.
 * Exemple: "ls -l /home" devient:
 * args[0] = "ls", args[1] = "-l", args[2] = "/home", args[3] = NULL
 *
 * execvp() a besoin d'un tableau qui se termine par NULL.
 */
void parse_command(char *line, char **args)
{
    int i;
    char *token;

    i = 0;
    token = strtok(line, " \t");
    while (token != NULL && i < MAX_ARGS - 1)
    {
        args[i] = token;
        i++;
        token = strtok(NULL, " \t");
    }
    args[i] = NULL;
}

/*
 * Gere la commande interne cd.
 * Elle doit etre executee par le shell lui-meme, pas par un processus fils,
 * sinon le changement de dossier serait perdu apres la fin du fils.
 */
int handle_cd(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "cd: dossier manquant\n");
        return 1;
    }

    if (chdir(args[1]) != 0)
    {
        perror("cd");
        return 1;
    }

    return 1;
}

/*
 * Execute une commande externe avec fork(), execvp() et wait().
 */
void execute_command(char **args)
{
    pid_t pid;
    int status;

    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return;
    }

    if (pid == 0)
    {
        /* Processus fils: execution de la commande. */
        execvp(args[0], args);

        /* Si execvp retourne, c'est qu'il y a une erreur. */
        perror("commande invalide");
        exit(1);
    }
    else
    {
        /* Processus parent: attendre la fin du fils. */
        wait(&status);
    }
}

int main(void)
{
    char *line;
    size_t size;
    char *args[MAX_ARGS];

    line = NULL;
    size = 0;

    printf("Mini-shell en C - tapez 'exit' pour quitter\n");

    while (1)
    {
        printf("my_shell> ");
        fflush(stdout);

        /* Lecture d'une ligne complete saisie par l'utilisateur. */
        if (getline(&line, &size, stdin) == -1)
        {
            printf("\n");
            break;
        }

        remove_newline(line);

        /* Si l'utilisateur appuie seulement sur Entree. */
        if (strlen(line) == 0)
            continue;

        parse_command(line, args);

        /* Securite: si aucune commande n'a ete detectee. */
        if (args[0] == NULL)
            continue;

        /* Commande interne pour quitter le shell. */
        if (strcmp(args[0], "exit") == 0)
            break;

        /* Commande interne pour changer de dossier. */
        if (strcmp(args[0], "cd") == 0)
        {
            handle_cd(args);
            continue;
        }

        execute_command(args);
    }

    free(line);
    printf("Fin du mini-shell.\n");
    return 0;
}
