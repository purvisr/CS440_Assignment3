#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <bitset>
#include <fstream>
using namespace std;

class Record {
public:
    int id, manager_id;
    std::string bio, name;

    Record() : id(0), manager_id(0), bio(""), name("") {}

    Record(vector<std::string> fields) {
        id = stoi(fields[0]);
        name = fields[1];
        bio = fields[2];
        manager_id = stoi(fields[3]);
    }

    void print() {
        cout << "\tID: " << id << "\n";
        cout << "\tNAME: " << name << "\n";
        cout << "\tBIO: " << bio << "\n";
        cout << "\tMANAGER_ID: " << manager_id << "\n";
    }
};


class LinearHashIndex {

private:
    const int BLOCK_SIZE = 4096;
    int numOverflows = 0;
    int numSplits = 0;
    int hasOverflow[256];
    int overflowPointer[256];
    int isSplit[256];
    int offset[256];
    char pageBuffer[4096];
    char overflowPageBuffer[4096];
    //vector<vector<int>> recordOffsets;
    vector<int> blockDirectory; // Map the least-significant-bits of h(id) to a bucket location in EmployeeIndex (e.g., the jth bucket)
                                // can scan to correct bucket using j*BLOCK_SIZE as offset (using seek function)
								// can initialize to a size of 256 (assume that we will never have more than 256 regular (i.e., non-overflow) buckets)
    int n;  // The number of indexes in blockDirectory currently being used
    int bits;	// The number of least-significant-bits of h(id) to check. Will need to increase i once n > 2^i
    int numRecords;    // Records currently in index. Used to test whether to increase n
    int nextFreeBlock; // Next place to write a bucket. Should increment it by BLOCK_SIZE whenever a bucket is written to EmployeeIndex
    string fName;      // Name of index file

    // Insert new record into index
    void insertRecord(Record record) {
        int remainder = record.id % 216;
        string binary_remainder = bitset<8>(remainder).to_string();
        char binary_remainder_str[9] = {0};
        copy(binary_remainder.begin(),binary_remainder.end(),binary_remainder_str);
        string least_significant_bits = binary_remainder.substr(binary_remainder.length()-bits);
        string least_significant_bits_unsplit = binary_remainder.substr(binary_remainder.length()-bits+1);
        const char* least_significant_bits_str = least_significant_bits.c_str();
        const char* least_significant_bits_unsplit_str = least_significant_bits_unsplit.c_str();


        fstream file(fName, fstream::in | fstream::out | ios::binary);
        file.close();
        // Add record to the index in the correct block, creating a overflow block if necessary
        for (int i = 0; i < n; ++i) {
            int index = i;
            if(bits>=3){
                if(isSplit[i]==1){
                    if (blockDirectory[i] == atoi(least_significant_bits_str)){
                         int startOfPage = i * BLOCK_SIZE + offset[i];
                        //file.read(pageBuffer,sizeof(pageBuffer));
                        char id[9];
                        char mid[9];
                        itoa(record.id, id, 10);
                        char tempid= id[0];
                        itoa(record.manager_id, mid, 10);
                        id[0] = tempid;

                        fstream file(fName, fstream::in | fstream::out | ios::binary);

                        int recordSize = 2*sizeof(id) + record.name.size()+record.bio.size();
                        int check = offset[i] + recordSize;
                        if(check > BLOCK_SIZE){
                            int temp = i;
                            while(1){
                                if ((hasOverflow[temp]==1))
                                {
                                    startOfPage = overflowPointer[i] * BLOCK_SIZE + offset[overflowPointer[i]];
                                    if(offset[overflowPointer[i]] += recordSize > BLOCK_SIZE){
                                        temp = overflowPointer[i];
                                    }
                                    else{
                                        index = overflowPointer[i];
                                        break;
                                    }
                                    
                                }
                                else{
                                    blockDirectory[n+numOverflows] = blockDirectory[i];
                                    overflowPointer[i] = n+numOverflows;
                                    startOfPage = overflowPointer[i] * BLOCK_SIZE + offset[overflowPointer[i]];
                                    index = overflowPointer[i];
                                    break;
                                }
                            }
                        }

                        file.seekg(startOfPage);
                        file.write(id, sizeof(id));
                        file.write(record.name.c_str(), record.name.size()+1);
                        file.write(record.bio.c_str(), record.bio.size()+1);
                        file.write(mid, sizeof(mid));
                        file.seekg(startOfPage);

                       
                        offset[index]+= recordSize+2;
                        file.clear();
                        file.close();
                        break;
                    }
                }
                else{
                    if (blockDirectory[i] == atoi(least_significant_bits_unsplit_str)){
                         int startOfPage = i * BLOCK_SIZE + offset[i];
                        //file.read(pageBuffer,sizeof(pageBuffer));
                        char id[9];
                        char mid[9];
                        itoa(record.id, id, 10);
                        char tempid= id[0];
                        itoa(record.manager_id, mid, 10);
                        id[0] = tempid;

                        fstream file(fName, fstream::in | fstream::out | ios::binary);
                        
                        int recordSize = 2*sizeof(id) + record.name.size()+record.bio.size();
                        int check = offset[i] + recordSize;
                        if(check > BLOCK_SIZE){
                            int temp = i;
                            while(1){
                                if ((hasOverflow[temp]==1))
                                {
                                    startOfPage = overflowPointer[i] * BLOCK_SIZE + offset[overflowPointer[i]];
                                    if(offset[overflowPointer[i]] += recordSize > BLOCK_SIZE){
                                        temp = overflowPointer[i];
                                    }
                                    else{
                                        index = overflowPointer[i];
                                        break;
                                    }
                                    
                                }
                                else{
                                    blockDirectory[n+numOverflows] = blockDirectory[i];
                                    overflowPointer[i] = n+numOverflows;
                                    startOfPage = overflowPointer[i] * BLOCK_SIZE + offset[overflowPointer[i]];
                                    index = overflowPointer[i];
                                    break;
                                }
                            }
                        }

                        file.seekg(startOfPage);
                        file.write(id, sizeof(id));
                        file.write(record.name.c_str(), record.name.size()+1);
                        file.write(record.bio.c_str(), record.bio.size()+1);
                        file.write(mid, sizeof(mid));
                        file.seekg(startOfPage);

                       
                        offset[index]+= recordSize+2;
                        file.clear();
                        file.close();
                        break;
                    }
                }
            }
            else{
                if (blockDirectory[i] == atoi(least_significant_bits_str)){
                        int startOfPage = i * BLOCK_SIZE + offset[i];
                        //file.read(pageBuffer,sizeof(pageBuffer));
                        char id[9];
                        char mid[9];
                        itoa(record.id, id, 10);
                        char tempid= id[0];
                        itoa(record.manager_id, mid, 10);
                        id[0] = tempid;

                        fstream file(fName, fstream::in | fstream::out | ios::binary);

                        int recordSize = 2*sizeof(id) + record.name.size()+record.bio.size();
                        int check = offset[i] + recordSize;
                        if(check > BLOCK_SIZE){
                            int temp = i;
                            while(1){
                                if ((hasOverflow[temp]==1))
                                {
                                    startOfPage = overflowPointer[i] * BLOCK_SIZE + offset[overflowPointer[i]];
                                    if(offset[overflowPointer[i]] += recordSize > BLOCK_SIZE){
                                        temp = overflowPointer[i];
                                    }
                                    else{
                                        index = overflowPointer[i];
                                        break;
                                    }
                                    
                                }
                                else{
                                    blockDirectory[n+numOverflows] = blockDirectory[i];
                                    overflowPointer[i] = n+numOverflows;
                                    startOfPage = overflowPointer[i] * BLOCK_SIZE + offset[overflowPointer[i]];
                                    index = overflowPointer[i];
                                    break;
                                }
                            }
                        }

                        file.seekg(startOfPage);
                        file.write(id, sizeof(id));
                        file.write(record.name.c_str(), record.name.size()+1);
                        file.write(record.bio.c_str(), record.bio.size()+1);
                        file.write(mid, sizeof(mid));
                        file.seekg(startOfPage);

                        offset[index]+= recordSize + 2;
                        
                        
                        file.clear();
                        file.close();
                        break;
                    }
            }
        }

        // Take neccessary steps if capacity is reached:
        float average = 0;
        for (int i=0;i<n;i++){
            average += offset[i];
        }

        if(average/(n*BLOCK_SIZE) >= 0.7){
            char idToHash[9] = {0};
            int compare = 0;
            n+=1;
            isSplit[numSplits] = 1;
            blockDirectory[n-1+numOverflows] = atoi(bitset<8>(n-1).to_string().c_str());
            fstream file(fName, fstream::in | fstream::out | ios::binary);
            file.seekg(numSplits * BLOCK_SIZE);
            file.read(pageBuffer,sizeof(pageBuffer));
            file.seekg((n-1+numOverflows)*BLOCK_SIZE + offset[n-1+numOverflows]);
            //file.read(overflowPageBuffer,sizeof(overflowPageBuffer));


            int oscillate = 0;
            cout << pageBuffer <<endl;
            for(int i =0;i<BLOCK_SIZE;i++){
                if(isdigit(pageBuffer[i])){
                    if(oscillate == 0){
                        memcpy(idToHash, pageBuffer+i, sizeof(idToHash)-sizeof(char));
                        idToHash[8] = '\0';
                        cout << idToHash << endl;
                        oscillate = 1;
                        i+=7;
                        int remainder = atoi(idToHash) % 216;
                        string binary_remainder = bitset<8>(remainder).to_string();
                        char binary_remainder_str[9] = {0};
                        copy(binary_remainder.begin(),binary_remainder.end(),binary_remainder_str);
                        string least_significant_bits = binary_remainder.substr(binary_remainder.length()-bits);
                        const char* least_significant_bits_str = least_significant_bits.c_str();
                        if (blockDirectory[n-1] == atoi(least_significant_bits_str)){
                            int tally = 8;
                            int start = i;
                            cout << "test\n";
                            while(1){
                                i+=1;
                                if(isdigit(pageBuffer[i])){
                                    memcpy(overflowPageBuffer,pageBuffer+start-7,tally);
                                    file.write(overflowPageBuffer,tally);
                                    break;
                                }
                                
                                tally+=1;
                            }
                        }
                    }
                    else{
                        oscillate = 0;
                        i+=7;
                    }
                }
            }
            

            
            
            numSplits+=1;

            //split page
        }

        if(n >= pow(2,bits)+1){
            bits+=1;
            //increment bits
        }

        return;
    }

public:
    LinearHashIndex(string indexFileName) {
        n = 4; // Start with 4 buckets in index
        bits = 2; // Need 2 bits to address 4 buckets
        numRecords = 0;
        nextFreeBlock = 0;
        fName = indexFileName;

        blockDirectory.resize(256);
        blockDirectory[0] = 00;
        blockDirectory[1] = 01;
        blockDirectory[2] = 10;
        blockDirectory[3] = 11;
        for(int i=4;i<blockDirectory.size();i++)
        {
            blockDirectory[i]=2;
        }
        for(int i=0;i<256;i++)
        {
            overflowPointer[i]=0;
            hasOverflow[i]=0;
            isSplit[i]=0;
            offset[i]=0;
            pageBuffer[i]='x';
        }

        // Create your EmployeeIndex file and write out the initial 4 buckets
        // make sure to account for the created buckets by incrementing nextFreeBlock appropriately
        ofstream file(fName, ios::app);
        char char1[2] = "&";
        char1[1] = '&';
        for(int i=0;i<n/2*BLOCK_SIZE;i++){
            file.write(char1,sizeof(char1));
        }
        nextFreeBlock+= 4 * BLOCK_SIZE;

        file.close();
    }

    // Read csv file and add records to the index
    void createFromFile(string csvFName) {
        ifstream file;
        file.open("Employee.csv");
        if(file.is_open()){
            string line;
            // Read each line from the CSV file.
            while (getline(file, line)) {
                vector<string> fields;
                stringstream ss(line);

                // Split each line into fields using ',' as the delimiter.
                string field;
                while (getline(ss, field, ',')) {
                    fields.push_back(field);
                }

                // Create a Record from the fields and insert it.
                Record record(fields);
                insertRecord(record);
        }

        file.close(); // Close the CSV file after processing.
        }
    }

    // Given an ID, find the relevant record and print it
    Record findRecordById(int id) {
        fstream file(fName, ios::in |ios::binary);
        string eid;
        string mid;
        if (!file.is_open()) {
            cerr << "Error opening file: " << fName << endl;
            exit(EXIT_FAILURE);
        }

        int remainder = id % 216;
        string binary_remainder = bitset<8>(remainder).to_string();
        char binary_remainder_str[9] = {0};
        copy(binary_remainder.begin(),binary_remainder.end(),binary_remainder_str);
        string least_significant_bits = binary_remainder.substr(binary_remainder.length()-2);


        //copy(binary_remainder.begin(),binary_remainder.end(),binary_remainder_str);
        //string least_significant_bits = binary_remainder.substr(binary_remainder.length()-bits);


        const char* least_significant_bits_str = least_significant_bits.c_str();
        for (int i=0; i<n+numOverflows; i++){
            if(blockDirectory[i] == atoi(least_significant_bits_str)){
                file.seekg(i*BLOCK_SIZE);
            }
        }
        Record record;
        //file.seekg(0,ios::beg);

        while (getline(file,eid,'\0')) {
            getline(file, record.name, '\0');
            getline(file, record.bio, '\0');
            getline(file, mid,'\0');

            
            // If the ID matches, close the file and return the found record.
            if (atoi(eid.c_str()) == id) {
                record.id = atoi(eid.c_str());
                record.manager_id = atoi(mid.c_str());
                file.close();
                cout << "SUCCESS\n";
                return record;
            }
        }
        file.close();

        return Record({ "0", "", "", "0" });


        
    }
};
