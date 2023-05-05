#include <iostream>
#include <fstream>
#include <cstring>

// Question 2 : Cela envoie un signal de fin de fichier (EOF) a l'entree stdin

// Question 3 : Dans le cas où vous redirigez la sortie d'une commande externe (telle que echo) vers le programme, 
// le signal de fin de fichier (EOF) est automatiquement envoyé une fois que la commande a terminé d'exécuter et que la sortie a été entièrement lue

// Question 4 : Le message d'information « Using stdin ... » est envoyé sur la sortie d'erreur (std::cerr) et non sur la sortie standard (std::cout). 
// Lorsque nous redirigeons la sortie de notre programme vers le fichier times_2.txt

// Question 5 : Le symbole "|" est utilisé pour rediriger la sortie standard d'une commande (dans ce cas, echo) vers l'entrée standard d'une autre commande (dans ce cas, lab_ex3). 
// Cela permet de chaîner les commandes et d'effectuer des opérations plus complexes.
// Le symbole ">" est utilisé pour rediriger la sortie standard d'une commande vers un fichier. Dans ce cas, la sortie standard de lab_ex3 est redirigée vers le fichier times_2.txt.

// Question 6 : Si vous lancez la commande cat sans arguments, elle attendra une entrée de l'utilisateur, ce qui signifie que vous pourrez entrer des données et les afficher à l'écran. Pour quitter l'entrée, vous devez appuyer sur Ctrl+D.
// Si vous lancez la commande cat avec le seul argument "-", elle lira l'entrée standard (stdin) plutôt qu'un fichier, ce qui signifie que vous pourrez également entrer des données et les afficher à l'écran. Pour quitter l'entrée, vous devez appuyer sur Ctrl+D.

// Question 7 : ./lab_ex3 input.txt
// ./lab_ex3 < input.txt

// Question 8 : awk '{print $0*4}' input.txt | ./lab_ex3

Le symbole ">" est utilisé pour rediriger la sortie standard d'une commande vers un fichier. Dans ce cas, la sortie standard de lab_ex3 est redirigée vers le fichier times_2.txt.
int main(int argc, char** argv)
{
    std::ifstream file_in;

    if (argc >= 2 && (strcmp(argv[1], "-") != 0)) {
        file_in.open(argv[1]);
        if (file_in.is_open()) {
            std::cin.rdbuf(file_in.rdbuf());
            std::cerr << "Using " << argv[1] << "..." << std::endl;
        } else {
            std::cerr   << "Error: Cannot open '"
                        << argv[1] 
                        << "', using stdin (press CTRL-D for EOF)." 
                        << std::endl;
        }
    } else {
        std::cerr << "Using stdin (press CTRL-D for EOF)." << std::endl;
    }

    // recherche signal de fin de fichier (EOF) avamt de finir le programme
    while (!std::cin.eof()) {

        std::string line;

        std::getline(std::cin, line);
        if (!line.empty()) {
            int val = atoi(line.c_str());
            std::cout << val * 2 << std::endl;
        }
    }

    if (file_in.is_open()) {
        file_in.close();
    }

    return 0;
}
