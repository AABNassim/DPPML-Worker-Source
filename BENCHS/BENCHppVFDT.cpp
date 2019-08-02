//
// Created by nassimaab on 1/13/19.
//


#include "BENCHppVFDT.h"
#include "../PPML/PPVFDT/PPVFDT_Classifier.h"
#include "../LIB/config4cpp/include/config4cpp/Configuration.h"
#include <chrono>

using namespace std;
using namespace std::chrono;

BENCHppVFDT::BENCHppVFDT() {

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

    try {
        cfg->parse(configFile);
        path        = cfg->lookupString(scope, "path");
        full        = cfg->lookupBoolean(scope, "full");
        chunk_prefix=  cfg->lookupString(scope, "chunk_prefix");
        classNumber = cfg->lookupInt(scope,"class_number");
        dimension = cfg->lookupInt(scope,"dimension");

        scope = "vfdt";
        delta = cfg->lookupFloat(scope,"delta");
        tau = cfg->lookupFloat(scope,"tau");
        grace = cfg->lookupInt(scope, "grace");
        max_depth = cfg->lookupInt(scope, "max_depth");
        thresholdNumber = cfg->lookupInt(scope, "threshold_number");
        numbTh =  cfg->lookupInt(scope, "thread_number");

    } catch(const config4cpp::ConfigurationException & ex) {
        cerr << ex.c_str() << endl;
        cfg->destroy();
    }

    cfg->destroy();


    gmp_randstate_t randstate;
    DTPKC dtpkc;

    // Generate public parameters of DTPKC
    gmp_randinit_default(randstate);
    gmp_randseed_ui(randstate,time(NULL));

    dtpkc.deserializeDtpkc();

    cout << "Config file read " << endl;
    PPDatasetReader *datasetReader = new PPDatasetReader (path, !full, chunk_prefix, classNumber, dimension, dtpkc);
    dtpkc.pkw = datasetReader -> trainKey;

    cout << "Dataset read " << endl;
    PPNode * tree = new PPNode(datasetReader -> dimension - 1, datasetReader -> classNumber);
    const clock_t begin_time = clock();
    PPVFDT_Classifier * classifier = new PPVFDT_Classifier(tree, datasetReader, delta, tau, grace, max_depth, thresholdNumber, dtpkc, numbTh);
    cout << "Classification started  " << endl;
    err = classifier -> train();

    cout << "Classification ended : err = " << err << endl;
    std::cout << "Time : " << float(clock() - begin_time) /  CLOCKS_PER_SEC;
    classifier -> Test();
    classifier -> printStats();
    cout << "Classification eval ended" << endl;
}

void BENCHppVFDT::generate_newkeys() {
    gmp_randinit_default(randstate);
    gmp_randseed_ui(randstate,time(NULL));
    dtpkc.keygen(randstate, nb_bits, error);
}

void BENCHppVFDT::read_dataset() {
    PPDatasetReader *datasetReader = new PPDatasetReader(path, !full, chunk_prefix, classNumber, dimension, dtpkc);
}

void BENCHppVFDT::create_classifier() {
    Tree = new PPNode(datasetReader -> dimension - 1, datasetReader -> classNumber);
    classifier = new PPVFDT_Classifier(Tree, datasetReader, delta, tau, grace, max_depth, thresholdNumber, dtpkc, numbTh);
}

long BENCHppVFDT::bench_training() {
    high_resolution_clock::time_point start = high_resolution_clock::now();
    err = classifier -> train();
    high_resolution_clock::time_point end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();
    return duration;
}
