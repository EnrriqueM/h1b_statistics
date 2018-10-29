//
//  main.cpp
//  h1b_statistics
//
//  Created by Enrique Mendoza on 10/27/18.
//  INSIGHT FELLOWSHIP PROGRAM
//

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <iomanip>

void parse(std::ifstream&);
void calcStats(std::ifstream&, int, int, int, int);
std::string getStat(std::ifstream&, int);
void checkFile(std::string);
void updateList(std::unordered_map<std::string, int>&, std::string);
void findTopTen(std::vector<std::string>&, std::unordered_map<std::string, int>&);
void writeFile(std::vector<std::string>&, std::unordered_map<std::string, int>&, std::string, std::string, long int);

int main(int argc, const char * argv[]) {
    std::string filename;
    
    if (argc <= 1)
    {
        std::cout << "No filename given\n";
        return 0;
    }
    
    for (int i = 1; i < argc; i++)
    {
        filename = argv[i];
        checkFile(filename);
    }
    
    std::cout<< "End of program\n";
    return 0;
}

//Parse the data file
void parse(std::ifstream& dataFile)
{
    std::string temp;
    int statusColumn = -1, stateColumn = -1, jobColumn = -1, totalCol = 41;
    
    for (int i = 0; i < totalCol && getline(dataFile, temp, ';'); i++)
    {
        //Check if the current column is the status
        std::size_t found = temp.find("STATUS");
        if (found!=std::string::npos){
            //std::cout << temp << ": " << i << std::endl;
            statusColumn = i;
        }
        
        //Check if the currrent column is the job title column
        found = temp.find("TITLE");
        if (found!=std::string::npos){
            //std::cout << temp << ": " << i << std::endl;
            jobColumn = i;
        }
        
        //Check is the current column is the state column
        found = temp.find("STATE");
        if (found!=std::string::npos){
            //std::cout << temp << ": " << i << std::endl;
            stateColumn = i;
        }
    }
    
    //If the columns need have been found, start calulating its statistics
    if (statusColumn != -1 && stateColumn != -1 && jobColumn != -1)
    {
        calcStats(dataFile, stateColumn, statusColumn, jobColumn, totalCol);
    }
    else
    {
        std::cout << "Failed to find columns" << std::endl;
    }
    
    return;
}

//This function writes a vector into a text file
void writeFile(std::vector<std::string>& topTen, std::unordered_map<std::string, int>& list, std::string loc, std::string msg, long int tot)
{
    const int maxTen = 10;
    double percentage;
    std::ofstream output(loc);
    
    output << msg;
    
    std::cout << "Writing to " << loc << std::endl;
    
    for (int i = 0; i < maxTen; i++)
    {
        percentage = (static_cast<double>(list[topTen[i]]) / tot) * 100;
        output << std::setprecision(1) << std::fixed << topTen[i] << ';' << list[topTen[i]] << ';' << percentage << '%' << std::endl;
    }
    
    output.close();
}

void findTopTen(std::vector<std::string>& topTen, std::unordered_map<std::string, int>& list)
{
    const int maxTen = 10;
    //Go through every item in the list and find the most frequency
    std::unordered_map<std::string, int>::iterator it = list.begin();
    while (it != list.end())
    {
        //If list is empty, add the first item found
        if(topTen.empty())
            topTen.push_back(it->first);
        else
        {
            //Loop through current top ten and compare to current item
            for (int j = 0; j < maxTen; j++)
            {
                //if current iteration is less than current item, add to that position and break out of loop
                if (it->second > list[topTen[j]])
                {
                    topTen.insert(topTen.begin()+j, it->first);
                    break;
                }
                //Check if you hit the end of topTen vector
                else if (j+1 >= maxTen)
                {
                    if(topTen.size() == maxTen)
                        topTen[maxTen] = it->first;
                    else
                        topTen.push_back(it->first);
                }
                //If topTen vector isnt fully filled, add current item to end
                else
                    topTen.push_back(it->first);
            }
        }
        it++;
    }
}

//Calcualte stats
void calcStats(std::ifstream& dataFile, int stateCol, int statusCol, int jobCol, int totCol)
{
    std::string temp, result;
    bool certified = false;
    long int totalApplication = 0;
    std::vector<std::string> topStates, topJobs;
    //hash map<name, frequncy>
    std::unordered_map<std::string, int> states, jobs;
    
    for (int i = 0; getline(dataFile, temp, ';'); i++)
    {
        if (temp == "CERTIFIED")
        {
            totalApplication++;
            if (stateCol < jobCol)
            {
                //Find the states and add to list if neccesary
                result = getStat(dataFile, stateCol - statusCol);
                updateList(states, result);
                
                //Find the job and add to the list if necessary
                result = getStat(dataFile, jobCol - stateCol - statusCol);
                updateList(jobs, result);
            }
            else
            {
                //Find the job and add to the list if necessary
                result = getStat(dataFile, jobCol - statusCol);
                updateList(jobs, result);
                
                //Find the states and add to list if neccesary
                result = getStat(dataFile, stateCol - jobCol - statusCol);
                updateList(states, result);
            }
        }
    }
    
    //Find top tens
    findTopTen(topJobs, jobs);
    //Write to file
    writeFile(topJobs, jobs, "./output/top_10_occupations.txt", "TOP_OCCUPATIONS;NUMBER_CERTIFIED_APPLICATIONS;PERCENTAGE\n", totalApplication);
    
    findTopTen(topStates, states);
    //Write to file
    writeFile(topStates, states, "./output/top_10_states.txt", "TOP_STATES;NUMBER_CERTIFIED_APPLICATIONS;PERCENTAGE\n", totalApplication);
    
    return;
}

//Updates the states or jobs found lists
void updateList(std::unordered_map<std::string, int>& list, std::string result)
{
    //Find where the result is in the list
    std::unordered_map<std::string, int>::const_iterator found = list.find(result);
    
    //If it exist update its count, if not add to list
    if (found == list.end())
    {
        list[result] = 1;
    }
    else
    {
        const int freq = list[result];
        list[result] = freq+1;
        //std::cout << list[result] << std::endl;
    }
}

std::string getStat(std::ifstream& df, int col)
{
    std::string result;
    
    //Move over to the desired column
    for (int i = 0; i < col && getline(df, result, ';'); i++);
    
    //std::cout << result << std::endl;
    return result;
}

//Check if the data file exists
void checkFile(std::string fn)
{
    std::ifstream dataFile;
    dataFile.open(fn);
    
    if(!dataFile)
    {
        std::cout << "Failed to open " << fn << std::endl;
    }
    else
    {
        std::cout << "File found. Starting to parse... \n";
        parse(dataFile);
    }
    
    dataFile.close();
    return;
}
