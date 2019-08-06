//
// Created by nassimaab on 5/13/19.
//

#include "DatasetReader.h"

using namespace std;

std::string DatasetReader::path;
int DatasetReader::test_size, DatasetReader::classNumber, DatasetReader::train_size, DatasetReader::current_train_record, DatasetReader::current_test_record, DatasetReader::dimension;
std::ifstream *DatasetReader::train, *DatasetReader::test, *DatasetReader::result;

DatasetReader:: DatasetReader(string path, int classNumber, int dimension)
{
    std::string line;

    this->classNumber = classNumber;
    this->path = path;
    this->dimension = dimension;
    this->test = new std::ifstream(path + "testing.data");
    current_train_record = current_test_record = 0;

    this->train = new ifstream(path + "training.data");
    this->result = new ifstream(path + "result.data");

    while (getline(*this->train, line)) {
        ++this->train_size;
    }
    this->train->clear();
    this->train->seekg(0);

    while (getline(*this->test, line)) {
        ++this->test_size;
    }

    this->test->clear();
    this->test->seekg(0);
}



void DatasetReader::reinit() {

    std::string line;
    delete this->test;
    this->test= new std::ifstream(path+"testing.data");
    current_train_record=0;
    current_test_record=0;
    delete this->train;

    this->train=new ifstream(path+"training.data");

    this->train_size = 0;
    while (getline(*this->train, line)) {
        ++this->train_size;
    }
    this->train->clear();
    this->train->seekg(0);

    this->test_size=0;
    while (getline(*this->test, line)) {
        ++this->test_size;
    }
    this->test->clear();
    this->test->seekg(0);
}

void DatasetReader::reloadTest()
{
    string line;
    test->clear();
    test->seekg(0);

    result->clear();
    result->seekg(0);

    current_test_record = 0;
}

void DatasetReader::reloadTrain()
{
    string line;
    test->clear();
    test->seekg(0);
    current_test_record = 0;
}

Record *
DatasetReader::getTrainRecord()
{
    string line;
    char *v;
    std::vector<double> values;
    Record * r;

    if (train->is_open()) {

        std::getline(*train, line);
        if (train->eof())
            train->close();

        char * dup =  new char[line.length() + 1];
        std::strcpy(dup, line.c_str());
        v=strtok (dup,",");
        while (v != NULL)
        {
            values.push_back(std::atof(v));
            v = strtok (NULL, ",");
        }

        int label = values.back();
        values.pop_back(); // Added by Nassim
        delete [] dup;
        this->current_train_record++;
        r = new  Record (this->current_train_record, values, label);

        return r;
    }
    else {
        throw std::runtime_error("There is no next record");
    }
}

Record *
DatasetReader::getTestRecord()
{
    string line;
    string label_line;
    char *v;
    std::vector<double> values;
    Record * r;

    if (test->is_open()) {
        std::getline(*test, line);
        std::getline(*result, label_line);

        if (test->eof())
        {
            test->clear();
            test->seekg(0);
        }

        if (result->eof())
        {
            result->clear();
            result->seekg(0);
        }


        char * dup =  new char[line.length() + 1];
        std::strcpy(dup, line.c_str());
        int label = std::atoi(label_line.c_str());

        v=strtok (dup, ",");
        while (v != NULL)
        {
            values.push_back(std::atof(v));
            v=strtok (NULL, ",");
        }
        delete [] dup;
        free (v);
        this->current_test_record++;
        r = new Record (this->current_test_record, values, label);

        return r;

    }
    else {
        throw std::runtime_error("There is no next record");
    }
}


void
DatasetReader::closeTraining()
{
    this->train->close();
}


void
DatasetReader::closeTesting()
{
    this->test->close();
}

vector <Record*> DatasetReader::read_all_train_records() {
    string line;
    char *v;
    //Record * r;
    int cpt = 0;


    vector <Record*> records;
    if (train->is_open()) {
        reloadTrain();
        while (1) {
            ++cpt;
            std::vector<double> values;
            std::getline(*train, line);
            values.push_back(1);
            //cout << "LA CHANKLA " << line << endl;
            char *dup = new char[line.length() + 1];
            std::strcpy(dup, line.c_str());
            v = strtok(dup, ",");
            while (v != NULL) {
                //cout << v << " ";
                values.push_back(std::atof(v));
                v = strtok(NULL, ",");
            }
            //cout << endl;
            int label = values.back();
            //cout << "LOL" << endl;

            values.pop_back();

            delete[] dup;
            this->current_train_record++;
            //cout << "GNEUGNEU" << endl;

            Record* r = new Record(this->current_train_record, values, label);
            //cout << r->values.size() << endl;
            //r->print();
            //cout << "qssou" << endl;

            records.push_back(r);
            //cout << "klikli" << endl;
            if (train->eof()) break;


        }
    }
    else {
        throw std::runtime_error("There is no next record");
    }

    /*for (int i = 0; i < 50; i++) {
        cout << "Record num " << i << ". Values: ";
        Record* rcd = records[i];
        for (int j = 0; j < rcd->values.size(); j++) {
            cout << rcd->values[j] << " ";
        }
        cout << " Label: " << rcd->label << endl;
    }*/


    return records;
}

vector <Record*> DatasetReader::read_all_test_records() {
    string line;
    string label_line;
    char *v;


    vector <Record*> records;
    if (test->is_open() && result->is_open()) {
        reloadTest();

        while (1) {
            std::vector<double> values;
            std::getline(*test, line);
            std::getline(*result, label_line);
            values.push_back(1);

            char *dup = new char[line.length() + 1];
            std::strcpy(dup, line.c_str());
            v = strtok(dup, ",");
            while (v != NULL) {
                values.push_back(std::atof(v));
                v = strtok(NULL, ",");
            }
            delete[] dup;
            int label = std::atoi(label_line.c_str());
            this->current_train_record++;
            Record* r = new Record(this->current_train_record, values, label);
            //r->print();
            records.push_back(r);
            if (test->eof()) break;

        }
    }
    else {
        throw std::runtime_error("There is no next record");
    }

    return records;
}
