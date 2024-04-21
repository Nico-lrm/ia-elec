#ifndef COUCHE_HPP
#define COUCHE_HPP
#pragma once

#include <memory>
#include <vector>

#include "Neurone.hpp"

class Couche
{
    private:
        std::shared_ptr<Couche> _prev;
        std::shared_ptr<Couche> _next;
        std::vector<std::shared_ptr<Neurone>> _tableau_neurone;
    public:
        Couche(const unsigned int& nombre_neurone, const unsigned int& nombre_poids_neurone, const std::shared_ptr<Couche> prev = nullptr)
        {
            this->_prev = prev;
            this->_next = nullptr;

            for(unsigned int i = 0; i < nombre_neurone; i++)
                this->_tableau_neurone.push_back(std::make_shared<Neurone>(nombre_poids_neurone));
        }

        // Fonction de propagation des données - Doit être lancé sur la première couche pour éviter de fausser le calcul ou de créer une exception
        // Pour la première couche, il s'agit des entrées du fichier
        // Pour les autres, il s'agit des sorties de la couche précédente
        float Propagation(const std::vector<float>& data)
        {
            // On prépare un tableau qui sera utilisé pour transférer les sorties des neurones
            std::vector<float> sorties_neurones;

            // On envoie les données précédentes sur chaque neurones pour calculer la sortie et la stocker dans le tableau
            for(auto neurone : this->_tableau_neurone)
            {
                neurone->ActivationSigmoide(data);
                sorties_neurones.push_back(neurone->getSortie());
            }

            // On vérifie la taille du tableau de neurone
            // Si sorties_neurones > 1 -> On relance la fonction avec la couche suivante et le tableau de sorties en paramètre
            // Sinon, on renvoie la valeur du premier indice du tableau
            if(sorties_neurones.size() > 1 && this->_next != nullptr)
                return 0.f + this->_next->Propagation(sorties_neurones);

            return sorties_neurones.at(0);
        }

        // Fonction qui calcule l'erreur précédente grâce à la somme des (erreurs * poids) * dérivé sigmoïde
        // Doit être lancé depuis la dernière couche pour qu'elle s'applique sur l'ensemble des couches précédentes
        void Retropropagation()
        {
            // Si on lance l'application depuis la dernière couche, on passe directement à la couche précédente pour calculer son erreur
            if(this->_next != nullptr)
            {
                // Récupérer les neurones de la couche suivante
                auto neurones_suivants{ this->_next->getTableauNeurone() };

                // On parcourt les neurones de la couche actuelle
                for(unsigned int i = 0; i < this->_tableau_neurone.size(); i++)
                {
                    // On récupère les poids de connexion entre l'erreur du neurone et la somme des poids de la connexion avec celui-ci
                    float somme{ 0.f };

                    // On fait la somme des poids * erreur
                    for(unsigned int j = 0; j < neurones_suivants.size(); j++)
                    {
                        // Récupérer le poids du neurone actuel (i) pour le neurone (j) de la couche suivante
                        somme += neurones_suivants.at(j)->getTabPoids().at(i) * neurones_suivants.at(j)->getErreur();
                    }

                    // Attribué l'erreur au neurone (dérivé sigmoide * sortie * somme(poids*err))
                    this->_tableau_neurone.at(i)->setErreur(this->_tableau_neurone.at(i)->getDeriveSigmoide() * somme);
                }
            }
            if(this->_prev != nullptr)
            {
                this->_prev->Retropropagation();
            }
        }

        // Mise à jour des poids des neurones d'une couche, si c'est la denrière couche on le précise car
        // On doit prendre en compte la dérivé de la sigmoïde dans le calcul
        void UpdatePoids()
        {
            bool last_couche = (this->_next == nullptr);
            for(auto neurone : this->_tableau_neurone)
            {
                neurone->UpdatePoids(last_couche);
            }
        }

        inline void setNextCouche(const std::shared_ptr<Couche> next) { this->_next = next; }
        inline unsigned int getNombreNeurone() { return static_cast<unsigned int>(this->_tableau_neurone.size()); }
        inline std::vector<std::shared_ptr<Neurone>> getTableauNeurone() { return this->_tableau_neurone; }
};

#endif