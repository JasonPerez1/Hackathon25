//GeneticAlgorithm.cpp
#include "GeneticAlgorithm.hpp"
#include "AIControlledGame.hpp"
#include <algorithm>
#include <atomic>
#include <iostream>
using namespace std;
#include <cfloat>


// Inside GeneticAlgorithm.cpp
// Constructor
GeneticAlgorithm::GeneticAlgorithm() {
    initializePopulation();
}

// Initialize population
void GeneticAlgorithm::initializePopulation() {
    std::uniform_real_distribution<double> dist(-1.0, 1.0);
    population.resize(POP_SIZE);
    for (auto& ind : population) {
        ind.chromosome.resize(CHROM_LENGTH);
        for (auto& gene : ind.chromosome) {
            gene = dist(gen);
        }
    }
}

// Tournament selection
Individual GeneticAlgorithm::tournamentSelection() {
    std::uniform_int_distribution<int> randIndex(0, POP_SIZE - 1);
    Individual& a = population[randIndex(gen)];
    Individual& b = population[randIndex(gen)];
    return (a.fitness > b.fitness) ? a : b;
}

// Mutation
void GeneticAlgorithm::mutate(Individual& ind) {
    std::normal_distribution<double> perturb(0.0, 0.1);
    for (auto& gene : ind.chromosome) {
        if (std::uniform_real_distribution<double>(0, 1)(gen) < MUTATION_RATE) {
            gene += perturb(gen);
        }
    }
}

// Crossover
Individual GeneticAlgorithm::crossover(const Individual& p1, const Individual& p2) {
    Individual child;
    std::uniform_int_distribution<int> randPoint(0, CHROM_LENGTH - 1);
    int crossoverPt = randPoint(gen);
    child.chromosome = p1.chromosome;
    std::copy(p2.chromosome.begin() + crossoverPt, p2.chromosome.end(),
              child.chromosome.begin() + crossoverPt);
    return child;
}

void GeneticAlgorithm::evolve() {
    std::vector<Individual> newPop;
    newPop.reserve(POP_SIZE);

    // Keep top 10% elites
    std::sort(population.begin(), population.end(), 
        [](const Individual& a, const Individual& b) { return a.fitness > b.fitness; });
    int elites = POP_SIZE/10;
    for (int i = 0; i < elites; ++i) {
        newPop.push_back(population[i]);
    }

    // Fill rest with crossover and mutation
    while (newPop.size() < POP_SIZE) {
        Individual p1 = tournamentSelection();
        Individual p2 = tournamentSelection();
        Individual child = crossover(p1, p2);
        mutate(child);
        newPop.push_back(child);
    }

    population = std::move(newPop);
    generation++;
}
void GeneticAlgorithm::evaluateGeneration() {
    AIControlledGame game(false);
    double maximum = -DBL_MAX;  // Initialize properly
    int best = 0;
    float average = 0.0f;       // Initialize to 0
    
    for (int i = 0; i < POP_SIZE; i++) {
        game.runSimulation(population[i], true);
        
        // Track best individual
        if (population[i].fitness > maximum) {
            maximum = population[i].fitness;
            best = i;
        }
        average += population[i].fitness;
        
        // Progress output
        std::cout << "Evaluating: " 
                << ((i+1) * 100 / POP_SIZE) << "%\r";
        std::cout.flush();
    }
    
    std::cout << "Max: " << maximum
              << " | Avg: " << average/POP_SIZE  // Fix division
              << " | Best ID: " << best << std::endl;
}

void GeneticAlgorithm::showBest() {
    auto bestIt = std::max_element(population.begin(), population.end(),
        [](const Individual& a, const Individual& b) { return a.fitness < b.fitness; });

    if (bestIt != population.end()) {
        { // Scoped block ensures game instance is destroyed after simulation
            AIControlledGame game(true);  // Window created here
            game.runSimulation(*bestIt, true);
        } // Window automatically closes here when 'game' goes out of scope
    }
}