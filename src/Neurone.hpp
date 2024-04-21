#ifndef NEURONE_HPP
#define NEURONE_HPP
#pragma once

#include <vector>
#include <stdexcept>
#include <time.h>
#include <math.h>
#include <iostream>

// Structure contenant l'ensemble des données du fichier CSV
struct Donnees
{
    std::vector<float> tab_date;
    std::vector<float> tab_conso;
    std::vector<float> tab_cout;
    std::vector<float> tab_pollution;
    unsigned int nb_lignes;
};

class Neurone
{
    private:
        std::vector<float> _tableau_poids;
        std::vector<float> _entrees;

        float _sortie = 0.f;
        float _derive_sigmoide = 0.f;
        float _erreur = 0.f;
    public:
        Neurone(const unsigned int& nombre_poids)
        {
            // Initialiser les poids de manière aléatoire
            for(unsigned int i = 0; i < nombre_poids; i++)
            {
                auto random = static_cast<float> (rand()) / static_cast <float> (RAND_MAX/0.2f) - 0.1f;
                this->_tableau_poids.push_back(random);
            }
        }

        // Fonction pour le sigmoïde
        float Sigmoide(const float& x)
        {
            return 1 / (1 + std::exp(-(x)));
        }

        // Fonction d'activation du neurone - Calcul la sortie du neurone
        void ActivationSigmoide(const std::vector<float>& data)
        {
            // Si le nombre de donnees et le nombre de poids sont différents, on renvoie une exception
            if(data.size() != this->_tableau_poids.size())
                throw std::invalid_argument("Le tableau de donnee et le tableau de poids ne font pas la même taille");

            // On nettoie le tableau d'entrée du dernier passage
            this->_entrees.clear();

            // On calcule la somme des (poids * entrée) et on ajoute le tableau d'entrée à noitre neurone (pour calculer le delta poids)
            float somme{ 0.f };
            for (unsigned int i = 0; i < data.size();i++)
            {
                this->_entrees.push_back(data[i]);
                somme += data[i] * this->_tableau_poids[i];
            }

            // Puis on applique la fonction sigmoide, qui est notre fonction d'activation
            this->_sortie = Sigmoide(somme);

            // On calcule en avance la derivée de la sigmoide pour la retropropagation de l'erreur
            this->_derive_sigmoide = Sigmoide(somme) * (1 - Sigmoide(somme));
        }

        // Fonction de mise à jour du poids sur le neurone
        void UpdatePoids(const bool& last_couche)
        {
            float coeff_apprentissage{ 0.25f };

            // On mets à jour l'ensemble des poids de notre neurone
            for(unsigned int i = 0; i < this->_tableau_poids.size(); i++)
            {
                float delta{ 0.0f };
                if(last_couche)
                {
                    delta = coeff_apprentissage * this->_erreur * this->_entrees.at(i) * this->_derive_sigmoide;
                }
                else
                {
                    delta = coeff_apprentissage * this->_erreur * this->_entrees.at(i);
                }
                this->_tableau_poids.at(i) += delta;
            }
        }

        inline float getSortie() { return this->_sortie; }
        inline float getErreur() { return this->_erreur; }
        inline std::vector<float> getTabPoids() { return this->_tableau_poids; }
        inline float getDeriveSigmoide() { return this->_derive_sigmoide; }

        inline void setErreur(const float& err) { this->_erreur = err; }
};

#endif