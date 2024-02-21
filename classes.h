// *** I didn't have time to test my work. I got some of the basic stuff done. I'm available Tuesday evening to help with what you need 
// This is what's left to do (I think this is it): 
// Handle overflow pages (line 94)
// Rehash when average page capacity is more than 70% (line 97)
// findRecordById()  (line 166)
// Make sure a max of 3 pages are in main memory at all times 

#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <istream>
#include <fstream>
#include <sstream>
#include <bitset>
#include <ctime>

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

    vector<int> blockDirectory; // Map the least-significant-bits of h(id) to a bucket location in EmployeeIndex (e.g., the jth bucket)
                                // can scan to correct bucket using j*BLOCK_SIZE as offset (using seek function)
								// can initialize to a size of 256 (assume that we will never have more than 256 regular (i.e., non-overflow) buckets)
    vector<int> average;
    int n;  // The number of indexes in blockDirectory currently being used
    int bits;	// The number of least-significant-bits of h(id) to check. Will need to increase i once n > 2^i
    int numRecords;    // Records currently in index. Used to test whether to increase n
    int nextFreeBlock; // Next place to write a bucket. Should increment it by BLOCK_SIZE whenever a bucket is written to EmployeeIndex
    string fName;      // Name of index file

    // Insert new record into index
    void insertRecord(Record record) {
        int remainder = record.id % 216;

        string binary_remainder = bitset<8>(remainder).to_string();
        string least_significant_bits = binary_remainder.substr(binary_remainder.length()-bits);

        fstream file(fName, fstream::in | fstream::out | ios::binary);

        // Add record to the index in the correct block, creating a overflow block if necessary
        for (int i = 0; i < blockDirectory.size(); ++i) {
            if (blockDirectory[i] == atoi(least_significant_bits.c_str())){
                // Seek in file to beginning of hashed page
                int startOfPage = i * BLOCK_SIZE + 1;
                file.seekp(startOfPage);
                int char1 = file.peek();
                // See if page is empty
                bool isEmpty = ((char1 != 48  && char1 != 49));

                // If page is empty, write record to the beginging of the page
                if (isEmpty){
                    file.close();
                    fstream file(fName, fstream::in | fstream::out | ios::binary);
                    file.seekp(startOfPage);
                    file.write(binary_remainder.c_str(), sizeof(binary_remainder));
                    if(startOfPage == 1)
                    {
                        average[0] += sizeof(binary_remainder);
                    }
                    else
                    {
                        average[(startOfPage-1)/BLOCK_SIZE] += sizeof(binary_remainder);
                    }
                }
                // If page is not empty, loop to the next available space in memory
                else{
                    int tally = 0;
                    bool isEmpty2 = false;
                    while (!isEmpty2) {
                        tally++;
                        // Read data at current position
                        char char2 = file.peek();
                        // See if page is empty
                        isEmpty2 = (char2 == '\000');
        
                        // Check if read was successful and if data is empty
                        //isEmpty = file && file.gcount() == 0;
        
                        // If not empty, seek to next location
                        if (!isEmpty2) {
                            startOfPage += sizeof(char2);
                            file.seekp(startOfPage);
                        }
                    }

                    // If there's no room on page, create overflow page
                    if (4*tally+sizeof(binary_remainder)>>BLOCK_SIZE){
                        // increase n; increase i (if necessary); place records in the new bucket that may have been originally misplaced due to a bit flip
                        // Will need to increase i once n > 2^
                        int average_capacity=0;
                        for (int j=0; j<n;j++){
                            average_capacity += average[j]/BLOCK_SIZE;
                        }
                        average_capacity = average_capacity / n;
                        if(average_capacity >= 0.7)
                        {
                            n+=1;
                            average[n-1] = 0;
                        }
                        
                        if (n >= pow(2,i)+1)
                        {
                            bits+=1;
                            for (int i=0; i< pow(2,bits); i++){
                                blockDirectory[i] = atoi(bitset<8>(i).to_string().c_str());
                            }
                            //TODO: rearrange after i increases
                        }
                        
                    }
                file.close();
                fstream file(fName, fstream::in | fstream::out | ios::binary);
                file.seekp(startOfPage+tally);
                file.write(binary_remainder.c_str(), sizeof(binary_remainder));
                average[(startOfPage-1)/BLOCK_SIZE]+=sizeof(binary_remainder);
                numRecords++;
            }
        }

    }
    file.close();
    }

public:
    LinearHashIndex(string indexFileName) {
        n = 4; // Start with 4 buckets in index
        bits = 2; // Need 2 bits to address 4 buckets
        numRecords = 0;
        nextFreeBlock = 0;
        fName = indexFileName;

        // Create your EmployeeIndex file and write out the initial 4 buckets
        fstream file(fName, ios::app);
        if (!file.is_open()) {
            cerr << "Error creating file: " << fName << endl;
            exit(EXIT_FAILURE);
        }

        blockDirectory.resize(256);
        blockDirectory[0] = 00;
        blockDirectory[1] = 01;
        blockDirectory[2] = 10;
        blockDirectory[3] = 11;
        for(int i=4;i<blockDirectory.size();i++)
        {
            blockDirectory[i]=2;
        }

        average.resize(256);
        average[0] = 0;
        average[1] = 0;
        average[2] = 0;
        average[3] = 0;

        // make sure to account for the created buckets by incrementing nextFreeBlock appropriately
        nextFreeBlock = 4 * BLOCK_SIZE; 

        //file.write(reinterpret_cast<const char*>(&blockDirectory[0]), blockDirectory.size() * BLOCK_SIZE);
        
        file.close(); 
    }

    // Read csv file and add records to the index
    void createFromFile(string csvFName) {
        ifstream file(csvFName);
        if (!file.is_open()) {
            cerr << "Error opening file: " << csvFName << endl;
            exit(EXIT_FAILURE);
        }

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

    // Given an ID, find the relevant record and print it
    Record findRecordById(int id) {
        
    }
};
