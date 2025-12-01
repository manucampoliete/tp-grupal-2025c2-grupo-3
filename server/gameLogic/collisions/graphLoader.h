#ifndef GRAPH_LOADER_H
#define GRAPH_LOADER_H

#include <yaml-cpp/yaml.h>  // For YAML parsing
#include <vector>           // For std::vector
#include <utility>          // For std::move

#include <iostream>

struct GraphNode {
    int id;
    float x;
    float y;
};

struct Graph {
    int mapId;
    std::vector<GraphNode> nodes;
    std::vector<std::pair<int,int>> edges;
};

class GraphLoader {
public:
    static Graph LoadGraph(const std::string& graphYamlPath) {
        Graph graph;

        YAML::Node config;

        try {
            config = YAML::LoadFile(graphYamlPath);
        } catch (const std::exception& e) {
            std::cerr << "[GRAPH_LOADER] Error loading graph YAML: " << e.what() << std::endl;
            return graph;
        }

        if (!config["graph"]) {
            std::cerr << "[GRAPH_LOADER] YAML missing 'graph' root key in: " << graphYamlPath << std::endl;
            return graph;
        }

        YAML::Node g = config["graph"];

        try {
            graph.mapId = g["map_id"].as<int>();
        } catch (const std::exception& e) {
            std::cerr << "[GRAPH_LOADER] Error loading map_id: " << e.what() << std::endl;
            return graph;
        }

        YAML::Node nodes = g["nodes"];

        if (nodes) {
            for (const auto& n : nodes) {
                GraphNode node;
                node.id = n["id"].as<int>();
                node.x  = n["x"].as<float>();
                node.y  = n["y"].as<float>();
                graph.nodes.push_back(node);
            }
        } else {
            std::cerr << "[GRAPH_LOADER] There is no 'nodes' section" << std::endl;
        }

        YAML::Node edges = g["edges"];
        if(edges) {
            for (const auto& e : edges) {
                // asumo que ya vienen con la forma (u,v)
                int u = e[0].as<int>();
                int v = e[1].as<int>();
                graph.edges.emplace_back(u, v);
            }
        } else {
            std::cerr << "[GRAPH_LOADER] There is no 'edges' section" << std::endl;
        }

        return graph;
    }
};

#endif
