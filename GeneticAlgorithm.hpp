//GeneticAlgorithm.hpp
#pragma once
#include <vector>
#include <random>


struct Individual {
    std::vector<double> chromosome;
    double fitness = 0;
    bool evaluated = false;
};

class GeneticAlgorithm {
public:
    GeneticAlgorithm();
    void evolve();
    void evaluateGeneration();
    void showBest();
    
    std::vector<Individual> population;
    int generation = 0;

private:
    void initializePopulation();
    Individual tournamentSelection();
    void mutate(Individual& ind);
    Individual crossover(const Individual& parent1, const Individual& parent2);

    const int POP_SIZE = 1000;
    const int CHROM_LENGTH = 200; // Was 100
    const double MUTATION_RATE = 0.05;
    std::mt19937 gen{std::random_device{}()};
};