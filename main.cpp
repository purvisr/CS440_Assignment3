/*
Skeleton code for linear hash indexing
*/

#include <string>
#include <ios>
#include <fstream>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include "classes.h"
using namespace std;


int main(int argc, char* const argv[]) { 
    // Create the index
    LinearHashIndex emp_index("EmployeeIndex");
    emp_index.createFromFile("Employee.csv");

    int idToFind=0;
    string idString;
    Record foundRecord;
    cout << "Enter IDs to search for. \nWhen you wish to exit the loop, type '-1 or any other negative number'\n";

    // Loop to lookup IDs until user is ready to quit
    while(1){
        cout << "Input: ";
        scanf("%d",&idToFind);
        if (idToFind < 0)
        {
            break;
        }
        foundRecord = emp_index.findRecordById(idToFind);
        cout << "Record found:\n";
        foundRecord.print();

    }
    return 0;
}
