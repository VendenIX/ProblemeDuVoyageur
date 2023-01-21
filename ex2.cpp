#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <math.h>
#include <cstdlib>
#include <lpsolve/lp_lib.h>




using namespace std;

int nbLigne = 280;
int nbColonne = 3;

REAL distanceItoJ(REAL **tab,int i, int j){
    return sqrt(((tab[j][1] - tab[i][1]) * (tab[j][1] - tab[i][1])) + ((tab[j][2] - tab[i][2])*(tab[j][2] - tab[i][2])));
}

int u(int i){ return i-1;}  //fonction u

void vider_row (REAL row[], const int& taille)
{
    for (int i = 0; i < taille; i++)
    {
        row[i] = 0.0;
    }
}

int main(){

    std::ifstream fichier("a280.tsp");
    if (!fichier.is_open()){
        std::cout << "Impossible d'ouvrir le fichier du graphe." << std::endl;
        exit(EXIT_FAILURE);
    }

    string ligne;
    int cpt = 0;
    REAL** tab = new REAL*[nbLigne];

    for (int i = 0; i < nbLigne; i++) {
        tab[i] = new REAL[nbColonne];
    }

    int i = 0;

    while (getline(fichier, ligne)){
        string mot;
        string mot1;
        string mot2;




        std::istringstream iss (ligne);
        iss >> mot;
        iss >> mot1;
        iss >> mot2;
        //if ( mot != "EOF")
        //printf("%d\n",atoi(mot.c_str()));
        //std::string target = "NODE_COORD_SECTION\n";


        if (cpt >= 6 && mot!= "EOF"){

            for (int j = 0; j < nbColonne; j++){
                //std::cout << mot << std::endl;
                if (j == 0){
                    tab[i][j] = atoi(mot.c_str());
                }
                else if(j == 1){
                    tab[i][j] = atoi(mot1.c_str());
                }
                else{
                    tab[i][j] = atoi(mot2.c_str());
                }
            }

            i++;
        }
        cpt++;

    }
    std::cout << "test" << std::endl;
    for (int i = 0; i < nbLigne; i++){
        for (int j = 0; j < nbColonne; j++){
            printf("%f ", tab[i][j]);
        }
        printf("\n");

    }

    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    //printf("%f ", tab[0][2]);
    REAL valeur = distanceItoJ(tab,0,1); // distance entre la ville 1 et la ville 2
    printf("%f", valeur);
    printf("\n");
    /*
    for (int i = 0; i < nbLigne; i++) {
        delete[] tab[i];
    }
    delete[] tab;
    */

    //créer le PL
    int nbVilles =  nbLigne;
    lprec* lp;
    int nbConstraint = 2*(nbVilles)*(nbVilles-1)+(nbVilles-1)*(nbVilles-2);
    int nbVariable = (nbVilles)*(nbVilles);
    lp = make_lp(nbConstraint, nbVariable);
    std::cout << "test1" << std::endl;

    //---------contraintes sur les variables x----------
    REAL row[nbVariable + 1];

    //1ère contrainte
    //conditions pour tous les i
    vider_row(row, nbVariable+1);
    for (int i = 1 ; i <= nbVilles ; i++){
        for (int j = 1; j <= nbVilles; j++){
            if (i != j){
                int id = (i-1)*nbVilles + j;
                row[id] = 1.0;
            }
        }
        add_constraint(lp, row, EQ, 1);
        vider_row(row, nbVariable+1);
    }
    std::cout << "test2" << std::endl;
    //2ème contrainte
    //conditions pour tous les j
    vider_row(row, nbVariable+1);
    for (int j = 1 ; j <= nbVilles ; j++){
        for (int i = 1; i <= nbVilles; i++){
            if (i != j){
                int id = (i-1)*nbVilles + j;
                row[id] = 1.0;
            }
        }
        add_constraint(lp, row, EQ, 1);
        vider_row(row, nbVariable+1);
    }
    std::cout << "test3" << std::endl;

    //3ème contrainte
    //contraintes entre x u et n que
    //l'on considère comme une contrainte sur x
    //on obtient des résultats plus cohérents lorsque commentée
    /*
    vider_row(row, nbVariable+1);
    for (int i = 2 ; i <= nbVilles ; i++){
        for (int j = 2; j <= nbVilles; j++){
            if (i != j ){
            int id = (i-1)*nbVilles + j;
            row[id] = 1.0;
            REAL contrainte = (REAL)(nbVilles - 1  - u(i) + u(j))/nbVilles;
            add_constraint(lp, row, LE, contrainte);
            vider_row(row, nbVariable+1);
            }
        }
        std::cout << i << " /  "<< nbVilles << std::endl;

    }
    */
    vider_row(row, nbVariable+1);
    //CONTRAINTE VALEURS I = J INTERDITES
    //contrainte superflue
    //mais tout de même ajoutéé car on obtient malgré tout
    //des villes connectées à elles-mêmes en solution
    //malheureusement cette contrainte n'a pas corrigé ce problème
    for (int i = 1 ; i <= nbVilles ; i++){
        int id = (i-1)*nbVilles+i;
        row[id] = 1;
        add_constraint(lp, row, EQ, 0);
        vider_row(row, nbVariable+1);

    }

    //La 4ème contrainte est respectée indirectement car tous les appels vers
    //la fonction u sont entre 2 et n (donc l'ensemble d'arrivée et bien entre 1 et n-1 ).

    std::cout << "test4" << std::endl;
    //type des contraintes
    for (int i = 1; i<(nbVariable+1);i++)
    {
        set_binary(lp,i,true);
        //std::cout << "test_contrainte " << i << std::endl;
    }
    std::cout << "test5" << std::endl;

    //fonction objective
    vider_row(row, nbVariable+1);
    for (int i = 1 ; i <= nbVilles ; i++){
        for (int j = 1; j <= nbVilles; j++){
            if (i != j){
            int id = (i-1)*nbVilles + j;
            REAL coefficient = distanceItoJ(tab, i-1,j-1);
            row[id] = coefficient;
            }
        }
    }
    set_obj_fn(lp, row);
    std::cout << "test6" << std::endl;

    //probleme actuel les contraintes 1 et 2 ne sont pas respectées puisqu'on obtient les coubles x (i = j) en solution.
    //pb toujours non résolu
    //pourtant une valeur minimal cohérente est obtenue ...


    //reduction des contraintes inutiles
    //sans cette fonction la résolution n'est parfois pas possible car trop longue
    set_presolve(lp, 1, 1000);

    int ret = solve(lp);
    std::cout << "fin de resolution" << std::endl;

    //afficher resultat
    if (solve(lp) == 0){
        cout << "longueur du réseau = " << get_objective(lp) << endl;
        get_variables(lp, row);
        cout << endl;
        cout << endl;
        cout <<  "Arcs solutions : " << endl;
        cout << endl;
        int res = 0;
        for (int k = 1; k<=nbVariable+1 ; k++){
            if (row[k]==1){
                int i = (k / nbVilles)+1; //quotient
                int j = k % nbVilles; //reste
                if (j==0){
                    if (i>=1){
                        i=i-1;
                        j=nbVilles;
                    }
                }
                cout << "x ( i="<<  i <<" ; j="<< j<<" )" << endl;
                res++;

            }

        }
        cout << endl;
        cout << res << " arcs solutions au total" << endl;
        cout << endl;
    }


}
