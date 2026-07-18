#include "./Generate_Orders/GenerateOrders.hpp"
#include "./Process_Orders/OrderPipeline.hpp"
#include "./Limit_Order_Book/Book.hpp"
#include "./Limit_Order_Book/Limit.hpp"
#include "./Limit_Order_Book/Order.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <filesystem>
#include <fstream>

namespace {
std::string resolveInputPath(const std::string& relativePath) {
    std::filesystem::path base = std::filesystem::current_path();
    std::vector<std::filesystem::path> candidates = {
        base / relativePath,
        base / ".." / relativePath,
        base / ".." / ".." / relativePath
    };

    for (const auto& candidate : candidates) {
        if (std::filesystem::exists(candidate)) {
            return candidate.string();
        }
    }

    return relativePath;
}
}

int main() {
    Book* book = new Book();

    OrderPipeline orderPipeline(book);
    GenerateOrders generateOrders(book);

    const std::string initialOrdersPath = resolveInputPath("Generate_Orders/initialOrders.txt");
    orderPipeline.processOrdersFromFile(initialOrdersPath);

    const std::string ordersPath = resolveInputPath("Orders.txt");
    if (std::filesystem::exists(ordersPath)) {
        auto start = std::chrono::high_resolution_clock::now();
        orderPipeline.processOrdersFromFile(ordersPath);
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        std::cout << "Time taken to process orders: " << duration.count() << " milliseconds" << std::endl;
    } else {
        std::cout << "Generating a fresh Orders.txt file..." << std::endl;
        generateOrders.createOrders(100000);

        const std::string generatedPath = resolveInputPath("Orders.txt");
        if (std::filesystem::exists(generatedPath)) {
            auto start = std::chrono::high_resolution_clock::now();
            orderPipeline.processOrdersFromFile(generatedPath);
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
            std::cout << "Time taken to process orders: " << duration.count() << " milliseconds" << std::endl;
        } else {
            std::cout << "Orders.txt generation failed." << std::endl;
        }
    }

    delete book;
    return 0;
}