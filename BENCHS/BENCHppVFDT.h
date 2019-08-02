//
// Created by nassimaab on 1/13/19.
//

#ifndef DAPPLE_BENCHPPVFDT_H
#define DAPPLE_BENCHPPVFDT_H

#include "../PPML/PPVFDT/PPVFDT_Classifier.h"
#include "../LIB/config4cpp/include/config4cpp/Configuration.h"

class BENCHppVFDT {
    config4cpp::Configuration *  cfg = config4cpp::Configuration::create();
    const char *scope = "dataset";
    const char *configFile = "../CONFIG/DAPPLE.config";
    string  path;
    bool full;
    int classNumber, grace = 0;
    int max_depth = 4;
    int thresholdNumber = 2;
    float tau, delta;
    string chunk_prefix = "";
    double err = 0.0;
    int dimension = 0;
    int numbTh = 1;

    gmp_randstate_t randstate;
    DTPKC dtpkc;
    int nb_bits = 2048, error = 600;

    PPDatasetReader *datasetReader;
    PPNode * Tree;
    PPVFDT_Classifier * classifier;

public:
    BENCHppVFDT();
    void read_dataset();
    void generate_newkeys();
    void create_classifier();
    long bench_training();

};

#endif //DAPPLE_BENCHPPVFDT_H
