#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <getopt.h>
#include <algorithm>

using namespace std;

// Cli options holder
struct CliOptions {
    bool isIndex = false;
    bool isSearch = false;
    string compressionAlgorithm = "";
    string indexAlgorithm = "";
    bool count = false;
    vector<string> patterns;
    vector<string> files;
};

// CLI options
static const string shortCliOptions = "ha:e:cp:";
static const vector<option> longCliOptions = {
        {"help",        no_argument,       nullptr, 'h'},
        {"compression", required_argument, nullptr, 'e'},
        {"indextype",   required_argument, nullptr, 'i'},
        {"count",       no_argument,       nullptr, 'c'},
        {"pattern",     required_argument, nullptr, 'p'},
        {nullptr,       0,                 nullptr, 0}};

static const string indexOperation = "index";
static const string searchOperation = "search";

// Options arguments
static const vector<string> compressionAlgorithmArguments = {"lz77"};
static const vector<string> indexAlgorithmArguments = {"suffixarray"};

// Help message
static const string help =
        "Usage:\n"
                "ipmt index [<option>] textfile ->  generates index file\n"
                "ipmt search [<option>] pattern indexfile  ->  search indexfile\n"
                "ipmt <option> <-h | --help>  ->  show global help\n"
                "ipmt <-h | --help>  -> show option help\n"
                "Options:\n"
                "-h,--help                  Display this information\n"
                "-e,--compression <arg>     Specify the compression algorithm\n"
                "-i,--indextype <arg>       Specify the indexing algorithm\n"
                "-c,--count                 Display only the number of found occurrences\n"
                "-p,--pattern <arg>         Specify a file with patterns to search\n"
                "                           If specified, the first global argument (pattern) is treated as a text file\n"
                "\n"
                "Examples:\n"
                "ipmt index moby-dick.txt\n"
                "ipmt search moby moby-dick.idx\n"
                "\n"
                "\n"
                "-comp,--compression <arg>  Specify the compression algorithm\n"
                "arg options:\n"
                "\tlz77\n"
                "-i,--indextype <arg>       Specify the indexing algorithm\n"
                "arg options:\n"
                "\tsuffixarray\n"
                "\n";

bool isInVector(const string &text, vector<string> vector) {
    for (auto &textInVector : vector)
        if (text == textInVector)
            return true;
    return false;
}

CliOptions parseCommand(int argc, char **argv) {
    CliOptions options = {};
    int optionFlag, uselessIndex;
    while ((optionFlag = getopt_long(argc, argv, shortCliOptions.c_str(), &longCliOptions[0], &uselessIndex)) != -1) {
        switch (optionFlag) {
            case 'h':
                cout << help;
                exit(0);
            case 'i':
                options.indexAlgorithm = optarg;
                if (!isInVector(optarg, indexAlgorithmArguments)) {
                    cerr << "ERR: illegal index algorithm name" << endl;
                    exit(1);
                }
                break;
            case 'e':
                options.compressionAlgorithm = optarg;
                if (!isInVector(optarg, compressionAlgorithmArguments)) {
                    cerr << "ERR: illegal compression algorithm name" << endl;
                    exit(1);
                }
                break;
            case 'c':
                options.count = true;
                break;
            case 'p': {
                ifstream patternsFileStream(optarg);
                if (!patternsFileStream.is_open()) {
                    cerr << "ERR: can not open patterns file" << endl;
                    exit(1);
                }
                for (string line; getline(patternsFileStream, line);)
                    options.patterns.push_back(line);
                patternsFileStream.close();
            }
                break;
            default:
                cerr << "ERR: option not found" << endl;
                exit(1);
        }
    }

    // Get global arguments
    char **arguments = argv + optind;
    int argumentCount = argc - optind;

    if (argumentCount == 0) {
        cerr << "ERR: invalid operation" << endl;
        exit(1);
    }

    if (searchOperation.compare(arguments[0]) == 0) {
        options.isSearch = true;
    } else if (indexOperation.compare(arguments[0]) == 0) {
        options.isIndex = true;
    } else {
        cerr << "ERR: invalid operation" << endl;
        exit(1);
    }

    if (argumentCount == 1 && options.patterns.empty()) {
        cerr << "ERR: missing pattern" << endl;
        exit(1);
    }

    if (options.patterns.empty()) {
        options.patterns.emplace_back(arguments[1]);
    }

    for (int i = 2; i < argumentCount; i++) {
        options.files.emplace_back(arguments[i]);
    }

    if (options.files.empty()) {
        cerr << "ERR: missing file" << endl;
        exit(1);
    }

    options.files.resize(unique(options.files.begin(), options.files.end()) - options.files.begin());

    // Default options
    if (options.compressionAlgorithm.empty()) {
        options.compressionAlgorithm = "lz77";
    }

    if (options.indexAlgorithm.empty()) {
        options.indexAlgorithm = "suffixarray";
    }

    return options;
}
