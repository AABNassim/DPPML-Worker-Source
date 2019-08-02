//
// Created by nassimaab on 5/13/19.
//

#ifndef DPPML_DATASETREADER_H
#define DPPML_DATASETREADER_H

#include "Record.h"

#include <fstream> // to manage file streams
#include <stdio.h> //
#include <string.h>// needed for strtok
#include <vector>
#include <cstring>
#include "Record.h"
#include <time.h>
#include <string>


using  namespace std;

class DatasetReader {

public :
    static std::string path;
    static int test_size, train_size, current_train_record, current_test_record, dimension, classNumber;
    static std::ifstream *train, *test, *result;

    /******* Functions **********/

public :
    DatasetReader(string path, int classNumber, int dimension);

public:
    Record * getTrainRecord();

public:
    Record * getTestRecord();

public:
    void closeTraining();

public:
    void closeTesting();

public:
    void reinit();

public:
    void reloadTest();

public:
    void reloadTrain();

public:
    vector <Record*> read_all_train_records();

public:
    vector <Record*> read_all_test_records();
};

#endif //DPPML_DATASETREADER_H
