
/**
**
 * Class :  Record
 * Authors : Talbi Rania
** Creation date : 13/11/2018
** Last Update : 14/11/2018 : Added the print method for tests
** Purpose : Represents a record of a given dataset
 *
**/

#ifndef DAPPLE_RECORD_H
#define DAPPLE_RECORD_H
#include<vector>
#include <iostream>

using namespace std;

class Record {


public :
    int id;
    std::vector <int> values;
    int label;

    /******* Functions **********/

    // Constructor of the record
public :
    Record(int id, std::vector<int> values) {
        this->id = id;
        this->values = values;
    }

public :
    Record(int id, std::vector<int> values, int label) {
        this->id = id;
        this->values = values;
        this->label = label;
    }


public:
    int dim() {
        return this->values.size();
    }


public :

    void print() {
        int i = 0;
        cout << "ID = " << id << " record = ( ";
        for (i = 0; i < values.size() - 1; i++)
            cout << values[i] << ", ";
            cout << values[i] <<" ) " << endl;
    }

};

#endif //DAPPLE_RECORD_H
