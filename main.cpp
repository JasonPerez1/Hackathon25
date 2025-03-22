//main.cpp
//g++ -std=c++17 -Iinclude/ src/*.cpp -o mario_ai -lsfml-graphics -lsfml-window -lsfml-system
#include "GeneticAlgorithm.hpp"
#include "AIControlledGame.hpp"
#include <algorithm>
#include <iostream>
using namespace std;
#include "GeneticAlgorithm.hpp"

int main() {
    GeneticAlgorithm ga;
    
    for(int gen = 0; gen < 10000000; gen++) {
        std::cout << "\nGeneration " << gen << std::endl;
        
        // Parallel evaluation
        ga.evaluateGeneration();
        
        // Show best performer
        ga.showBest();
        
        
        // Evolve population
        ga.evolve();
    }
    
    return 0;
}