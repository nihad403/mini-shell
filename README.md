# mini-shell
Ce projet est un mini-shell en C qui lit une commande avec getline, la découpe en arguments avec strtok, crée un processus fils avec fork, exécute la commande avec execvp, puis attend sa fin avec wait. Il gère aussi les commandes internes exit et cd.
