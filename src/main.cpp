#include <string>
#include <vector>
#include <memory>

#include "Reseau.hpp"
#include "../vendor/csv.h"

// Fonction pour parser la date de std::string à un float
float parseDate(std::string& date_string)
{   
    // Récupère l'année
    auto year_string{ date_string.substr(0, date_string.find_first_of("-")) };
    
    // Récupère le mois et le jours dans une variable tampon
    auto temp = date_string.substr(0, date_string.find_last_of("-"));

    // Recupère le mois de la variable tampon
    auto month_string{ temp.substr(year_string.length()+1) };

    // Convertir l'année et le mois en int
    auto year{ std::stoi(year_string) };
    auto month{ std::stoi(month_string) };

    year *= 100;
    year += month;
    return static_cast<float>(year);
}

// Fonction pour normaliser les entrées (valeur entre 0 et 1)
void normalizeVector(std::vector<float>& data)
{
    float min{ data.at(0) };
    float max{ data.at(0) };

    // Récupérer les valeurs max et min
    for(auto value : data)
    {
        if(value < min)
            min = value;
        if(value > max)
            max = value;
    }

    // Appliquer la normalisation sur l'ensemble du vector
    for(auto& value : data)
    {
        value = (value - min)/(max - min);
    }
}

// Fonction qui renvoie un structure avec l'ensemble des données qui seront utilisées
std::shared_ptr<Donnees> readDataFromCSV(const std::string& filepath)
{
    // Le 4 represente le nombre de colonne qu'on utilise de notre fichier CSV, on va recup seulement les colonnes qui nous interesse et ignorer les autres
    io::CSVReader<4> file(filepath);

    // Ici, on precise quelle colonnes seront lues
    file.read_header(io::ignore_extra_column, "Date", "Consommation", "Cout", "Emission");

    // On declare les variables tampons pour qu'on utilisera plus tard - La date est recup au format string et devra etre parse pour etre utilisable je pense ? 
    std::string date{};
    float conso{};
    float cout{};
    float pollution{};

    // On déclare la structure ou on sauvegardera les elements -> shared_ptr pour eviter de casser la memoire de la pile
    auto data{ std::make_shared<Donnees>() };
    data->nb_lignes = 0;

    // Il reste plus qu'a lire le fichier en stockant a chaque fois la valeur de la ligne, on passe les variables dans l'ordre des colonnes annonce plus haut
    while(file.read_row(date, conso, cout, pollution))
    {
        data->tab_conso.push_back(conso);
        data->tab_cout.push_back(cout);
        data->tab_pollution.push_back(pollution);
        data->tab_date.push_back(parseDate(date));
        data->nb_lignes += 1;
    }

    // Normalisation des données
    normalizeVector(data->tab_conso);
    normalizeVector(data->tab_cout);
    normalizeVector(data->tab_pollution);
    normalizeVector(data->tab_date);

    return data;  
}

int main(int argc, char const *argv[])
{
    try
    {
        // On initialise la seed du random
        std::srand(std::time(0));

        // Récupérer les données normalisées depuis le fichier CSV
        auto donnees{ readDataFromCSV("data/eclairage-public.csv") };

        // Initialiser le réseau
        auto reseau{ std::make_shared<Reseau>(donnees) };

        // Une fois la première propagation de données, on peut dire à notre réseau de s'entraîner
        reseau->Apprentissage();

        return 0;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}