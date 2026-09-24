#include "../includes/Environment.hpp"
#include <sstream>
#include <iomanip>
#include <fstream>

//fonction de test (vibe  codee bien vu ) a supprimer 
static void printComponentsData(const std::map<std::string, t_compdata>& components)
{
    std::cout << "=======================================================================\n";
    std::cout << "                       COMPONENTS DATA (CSV LOADED)                    \n";
    std::cout << "=======================================================================\n";
    
    if (components.empty())
    {
        std::cout << " [!] La map est vide.\n";
        std::cout << "=======================================================================\n";
        return;
    }

    // Entête du tableau avec alignement des colonnes
    std::cout << std::left 
              << std::setw(25) << "Component Key" 
              << std::setw(15) << "Embodied" 
              << std::setw(15) << "Lifetime" 
              << std::setw(15) << "Margin" << "\n";
    std::cout << "-----------------------------------------------------------------------\n";

    // Parcours et affichage de la map
    for (const auto& [name, data] : components)
    {
        std::cout << std::left 
                  << std::setw(25) << name 
                  << std::setw(15) << data.embodied 
                  << std::setw(15) << data.lifetime 
                  << std::setw(15) << data.margin << "\n";
    }

    std::cout << "=======================================================================\n";
    std::cout << " Total éléments : " << components.size() << "\n";
    std::cout << "=======================================================================\n";
}
#include <iostream>
#include <iomanip>
#include <vector>

static void printNodesData(const std::vector<Node>& nodes)
{
    std::cout << "========================================================================================\n";
    std::cout << "                                 NODES DATA (PARSED)                                    \n";
    std::cout << "========================================================================================\n";

    if (nodes.empty())
    {
        std::cout << " [!] Le vecteur de nœuds est vide.\n";
        std::cout << "========================================================================================\n";
        return;
    }

    // Configuration du format d'affichage pour les doubles (4 décimales pour voir le drate)
    std::cout << std::fixed << std::setprecision(4);

    // Entête du tableau
    std::cout << std::left 
              << std::setw(8)  << "ID" 
              << std::setw(20) << "Embodied (gCO2e)" 
              << std::setw(22) << "drate (gCO2e/s)" 
              << std::setw(18) << "Mnode (gCO2e)" 
              /*<< std::setw(12) << "Data Logs" */<< "\n";
    std::cout << "----------------------------------------------------------------------------------------\n";

    // Parcours des nœuds
    for (const auto& node : nodes)
    {
        std::cout << std::left 
                  << std::setw(8)  << node.getId() 
                  << std::setw(20) << node.getInitialEmbodied() 
                  << std::fixed << std::setprecision(9) << std::setw(22) << node.getDrate() << std::setprecision(6)
                  << std::setw(18) << node.getMnode() << std::setprecision(0)
                  /*<< std::setw(12) << node.getData().size() */<< "\n";
    }

    std::cout << "========================================================================================\n";
    std::cout << " Total nœuds (Gateway incluse) : " << nodes.size() << "\n";
    std::cout << "========================================================================================\n";
}

int main ()
{
    std::map<std::string, t_compdata> ComponentsData = loadCSV(std::string("../testdata/boavizta_impact_data.csv"));
    printComponentsData(ComponentsData);
    std::ifstream fileGrid("test/scenario_12345.grid.json");
    std::ifstream fileTopo("test/scenario_12345.topo.json");
    std::ifstream coojalogs("test/logs_12345/COOJA.testlog");

    if (!fileGrid.is_open() || !fileTopo.is_open()) {
        std::cerr << "Cannot open Json files !" << std::endl;
        return 1;
    }
    Environment environement = parseJson(fileGrid, fileTopo, ComponentsData);
    //la simu se produit ici imo;

    printNodesData(environement.getNodes());
    parseCoojaLogs(environement, coojalogs);
    return (0);
}