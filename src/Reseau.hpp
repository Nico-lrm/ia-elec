#ifndef RESEAU_HPP
#define RESEAU_HPP
#pragma once

#include <list>
#include <memory>
#include <tuple>
#include <iostream>

#include "Couche.hpp"

class Reseau
{
private:
    // Pointeur vers les données qui seront utilisées
    std::shared_ptr<Donnees> _donnees;

    // Liste des couches du réseau de neurones
    std::list<std::shared_ptr<Couche>> _liste_couche;

    // Tableau d'indice où il y a eu une erreur
    std::vector<unsigned int> _tableau_indice_erreur;

public:
    Reseau(const std::shared_ptr<Donnees> donnees)
    {
        // Attacher les données qui seront utilisées au réseau
        this->_donnees = donnees;

        // Préparer les couches de neurones
        auto couche_1{std::make_shared<Couche>(4, 3)};

        auto couche_2{std::make_shared<Couche>(3, 4, couche_1)};
        couche_1->setNextCouche(couche_2);

        auto couche_3{std::make_shared<Couche>(2, 3, couche_2)};
        couche_2->setNextCouche(couche_3);

        auto couche_4{std::make_shared<Couche>(1, 2, couche_3)};
        couche_3->setNextCouche(couche_4);

        // Les ajouter à la liste de couche
        this->_liste_couche.push_back(couche_1);
        this->_liste_couche.push_back(couche_2);
        this->_liste_couche.push_back(couche_3);
        this->_liste_couche.push_back(couche_4);

        // Effectuer la première propagation
        for (unsigned int i = 0; i < this->_donnees->nb_lignes; i++)
        {
            Propagation(i);
        }
    }

    // Fonction utilisé par le réseau pour propager les données sur les neurones
    void Propagation(const unsigned int &i)
    {
        // On récupère la première couche grâce à la méthode front() des listes
        auto couche_front{this->_liste_couche.front()};

        // On créé le tableau qui va contenir les données de la ligne actuelle
        std::vector<float> donnees_actuelles{
            this->_donnees->tab_conso.at(i),
            this->_donnees->tab_date.at(i),
            this->_donnees->tab_pollution.at(i)};

        // La fonction de propagation nous renvoie le coût estimé pour la ligne
        float estimation_cout{couche_front->Propagation(donnees_actuelles)};

        // Affichage de test
        std::cout << i << " : "
                  << "Estimation : " << estimation_cout << " ------- Cout attendu : " << this->_donnees->tab_cout.at(i) << "\n";

        // Si l'écart entre l'estimation et la valeur attendu est trop grand, alors on ajoute une pair indice_ligne_donnee/erreur dans le tableau associé
        if (std::abs(this->_donnees->tab_cout.at(i) - estimation_cout) > 0.1)
            this->_tableau_indice_erreur.push_back(i);

        // On récupère la dernière couche
        auto couche_back{this->_liste_couche.back()};

        // L'erreur de sortie = cout_reel - cout_estimé
        couche_back->getTableauNeurone().at(0)->setErreur(this->_donnees->tab_cout.at(i) - estimation_cout);
    }

    // Rétropropagation de l'erreur
    void Retropropagation()
    {
        // On dernière la première couche grâce à la méthode back() des listes
        auto couche{this->_liste_couche.back()};
        couche->Retropropagation();
    }

    // Mise à jour de l'ensemble des neurones du réseau
    void UpdatePoids()
    {
        for (auto couche : this->_liste_couche)
        {
            couche->UpdatePoids();
        }
    }

    // Boucle d'apprentissage du neurone
    void Apprentissage()
    {
        // Tant que notre nombre d'erreur est au dessus de 10%
        while (this->_tableau_indice_erreur.size() > (this->_donnees->nb_lignes / 10))
        {
            // On choisit une valeur aléatoire dans le tableau des indices d'erreur
            auto indice_erreur_utilise{rand() % this->_tableau_indice_erreur.size()};

            // On propage cette donnée dans le système afin de remettre les bonnes valeurs d'entrées par rapport à l'exemple choisi
            Propagation(indice_erreur_utilise);

            // On applique la rétropropagation
            Retropropagation();

            // On mets à jour les poids
            UpdatePoids();

            // On nettoie le tableau d'indice d'erreur
            this->_tableau_indice_erreur.clear();

            // On relance la propagation sur l'ensemble des données
            for (unsigned int i = 0; i < this->_donnees->nb_lignes; i++)
                Propagation(i);
        }
    }
};

#endif