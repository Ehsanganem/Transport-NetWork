#include <iostream>
#include <cstring>
#include "NetWork.h"
#include <sstream>
#include <fstream>
#include <climits>
#include <cstdlib>

// function declerations
void run(NetWork& net, const std::string& outfile);
int loadNetworkInputFiles(int argc, char* argv[], int& index, NetWork& netWork);
int loadNetworkConfigFileAndOutput(int argc, char* argv[], int& index, NetWork& netWork,
    std::string& outputFileName);

// functin implementations
void run(NetWork& net, const std::string& outfile)
{
    std::string line;
    std::getline(std::cin, line);

    while (line != "exit")
    {
        std::stringstream stream(line);
        std::string command;

        stream >> command;

        if (command == "load")
        {
            std::string filename;
            stream >> filename;

            try {
                net.loadLines(filename);
                std::cout << "Update was successful" << std::endl;
            }
            catch (const NetWork::invalidLineFile&) {
                std::cout << "ERROR opening the specified file." << std::endl;
            }
        }
        else
        {
            try
            {
                if (command == "outbound")
                {
                    std::string node;
                    stream >> node;
                    NetWork::out_bound_map out_map = net.outbound(node);

                    for (const auto& pair : out_map) {
                        std::cout << pair.first.lock()->getName() << ": ";

                        if (pair.second.empty()) {
                            std::cout << "no outbound travel";
                        }
                        else
                        {
                            for (const std::weak_ptr<Node>& currnode : pair.second) 
                            {
                                std::cout << currnode.lock()->getName() << "\t";
                            }
                        }

                        std::cout << std::endl;
                    }
                }
                else if (command == "inbound")
                {
                    std::string node;
                    stream >> node;
                    NetWork::inbound_map in_map = net.inbound(node);

                    for (const auto& pair : in_map) 
                    {
                        std::cout << pair.first.lock()->getName() << ": ";

                        if (pair.second.empty()) {
                            std::cout << "no inbound travel";
                        }
                        else 
                        {
                            for (const std::weak_ptr<Node>& currnode : pair.second) 
                            {
                                std::cout << currnode.lock()->getName() << "\t";
                            }
                        }
                        std::cout << std::endl;
                    }
                }
                else if (command == "uniExpress")
                {
                    std::string sourceNode, targetNode;
                    stream >> sourceNode >> targetNode;
                    NetWork::express_map exp_map = net.uniExpress(sourceNode, targetNode);

                    for (const auto& pair : exp_map)
                    {
                        std::cout << pair.first.lock()->getName() << ": ";
                        if (pair.second == INT_MAX)
                        {
                            std::cout << "route unavailable" << std::endl;
                        }
                        else
                        {
                            std::cout << pair.second << std::endl;
                        }
                    }
                }
                else if (command == "multiExpress")
                {
                    std::string sourceNode, targetNode;
                    stream >> sourceNode >> targetNode;
                    int res = net.multiExpress(sourceNode, targetNode);

                    if (res < INT_MAX)
                    {
                        std::cout << res << std::endl;
                    }
                    else
                    {
                        std::cout << "route unavailable" << std::endl;
                    }
                }
                else if (command == "viaExpress")
                {
                    std::string sourceNode, transitNode, targetNode;
                    stream >> sourceNode >> transitNode >> targetNode;
                    int res = net.viaExpress(sourceNode, transitNode, targetNode);

                    if (res < INT_MAX)
                    {
                        std::cout << res << std::endl;
                    }
                    else
                    {
                        std::cout << "route unavailable" << std::endl;
                    }
                }
                else if (command == "print")
                {
                    std::ofstream file(outfile);

                    if (!file)
                    {
                        std::cout << "cannot open output file" << std::endl;
                    }
                    else
                    {
                        file << net;
                        file.close();
                    }
                }
                else
                {
                    std::cout << "invalid command" << std::endl;
                }
            }
            catch (const std::invalid_argument& e) 
            {
                std::cout << e.what() << std::endl;
            }
        }

        std::getline(std::cin, line);
    }
}

// returns program exit code
int loadNetworkInputFiles(int argc, char* argv[], int& index, NetWork& netWork)
{
    if (argc < 3)
    {
        std::cerr << "not enough arguments";
        return EXIT_FAILURE;
    }
    if (strcmp(argv[1], "-i") != 0)
    {
        std::cerr << "-i flag not specified" << std::endl;
        return EXIT_FAILURE;
    }

    index = 2;

    try
    {
        while (index < argc && argv[index][0] != '-')
        {
            netWork.loadLines(argv[index]);
            index++;
        }

        return EXIT_SUCCESS;
    }
    catch (const NetWork::invalidLineFile& e)
    {
        std::cerr << "invalid input file: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

// returns program exit code
int loadNetworkConfigFileAndOutput(int argc, char* argv[], int& index, NetWork& netWork,
    std::string& outputFileName)
{
    outputFileName = "output.dat";

    try
    {
        while (index < argc)
        {
            if (strcmp(argv[index], "-c") == 0)
            {
                if (index + 1 < argc)
                {
                    netWork.loadConfiguration(argv[index + 1]);
                }
                else
                {
                    std::cerr << "wrong config argument" << std::endl;
                    return EXIT_FAILURE;
                }
            }
            else if (strcmp(argv[index], "-o") == 0)
            {
                if (index + 1 < argc)
                {
                    outputFileName = argv[index + 1];

                }
                else
                {
                    std::cerr << "wrong output argument" << std::endl;
                    return EXIT_FAILURE;
                }
            }
            else
            {
                std::cerr << "wrong flag format" << std::endl;
                return EXIT_FAILURE;
            }

            index += 2;
        }

        return EXIT_SUCCESS;
    }
    catch (const NetWork::invalidConfigFile& e)
    {
        std::cerr << "invalid file : " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}


int main(int argc, char* argv[])
{
    int index;
    NetWork network;

    if (loadNetworkInputFiles(argc, argv, index, network) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    std::string outputFileName;

    if (loadNetworkConfigFileAndOutput(argc, argv, index, network, outputFileName) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    run(network, outputFileName);

    return EXIT_SUCCESS;
}
