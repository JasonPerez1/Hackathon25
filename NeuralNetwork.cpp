// src/NeuralNetwork.cpp
#include "NeuralNetwork.hpp"
#include <vector>  
#include <cmath>  
#include <iostream>

NeuralNetwork::NeuralNetwork(const std::vector<double>& chromosome, const std::vector<int>& topology) {
    int idx = 0;
    for(size_t i = 0; i < topology.size() - 1; i++) {
        Layer layer;
        int input_size = topology[i];
        int output_size = topology[i+1];
        
        layer.weights.resize(output_size, std::vector<double>(input_size));
        for(auto& row : layer.weights) {
            for(auto& val : row) {
                val = chromosome[idx++];
            }
        }
        
        layer.biases.resize(output_size);
        for(auto& bias : layer.biases) {
            bias = chromosome[idx++];
        }
        
        layers.push_back(layer);
    }
}

std::vector<double> NeuralNetwork::activate(const std::vector<double>& inputs) {
    std::vector<double> current = inputs;
    for(const auto& layer : layers) {
        std::vector<double> next(layer.biases.size());
        for(size_t i = 0; i < layer.weights.size(); i++) {
            double sum = layer.biases[i];
            for(size_t j = 0; j < current.size(); j++) {
                sum += layer.weights[i][j] * current[j];
            }
            next[i] = sigmoid(sum);
        }
        current = next;
    }
    return current;
}

double NeuralNetwork::sigmoid(double x) const {
    return 1.0 / (1.0 + exp(-x));
}