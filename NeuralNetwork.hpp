// NeuralNetwork.hpp
#pragma once
#include <vector>

class NeuralNetwork {
public:
    struct Layer {
        std::vector<std::vector<double>> weights;
        std::vector<double> biases;
    };

    explicit NeuralNetwork(const std::vector<double>& chromosome, const std::vector<int>& topology);
    std::vector<double> activate(const std::vector<double>& inputs);
    bool jumpReleased = true;

private:
    std::vector<Layer> layers;
    double sigmoid(double x) const;  // Add const herew
};