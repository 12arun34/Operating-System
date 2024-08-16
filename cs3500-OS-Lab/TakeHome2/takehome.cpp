#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>
#include <algorithm>
#include <unordered_map>
#include<string>

using namespace std;

// Constants for default values
 int MainMemorySize = 32 * 1024; // 32 KB
 int VirtualMemorySize = 32 * 1024; // 32 KB
 int PageSize = 512; // 512 bytes

 int CurMemorySize=0;
 int CurVirtualSize=0;

struct PageTableEntry {
    int logicalPageNumber;
    int physicalPageNumber;
};

struct MemoryPage {
    int pid; // Process ID
    int pageId; // Page ID
    vector<int> data;
    time_t lastAccessTime; // For LRU page replacement
};

struct Process {
    int pid;
    string processFilename;
    int size; // Size in KB
    vector<PageTableEntry> pageTable;
    bool inMainMemory;
};

vector<MemoryPage> mainMemory;
vector<MemoryPage> virtualMemory;
vector<Process> processes;
int pidCounter = 1;

// Helper function for LRU page replacement
int findLRUPage() {
    time_t oldestTime = time(nullptr);
    int lruPage = -1;

    for (int i = 0; i < mainMemory.size(); ++i) {
        if (difftime(time(nullptr), oldestTime) > difftime(time(nullptr), mainMemory[i].lastAccessTime)) {
            oldestTime = mainMemory[i].lastAccessTime;
            lruPage = i;
        }
    }

    return lruPage;
}

// Function to swap out a process to virtual memory using LRU algorithm
bool swapOutCommand(int pid) {
    //cout<<"swapOutCommand function entered"<<endl;
    for (Process& process : processes) {
        if (process.pid == pid && process.inMainMemory) {
            int lruPageIndex = findLRUPage();

            if (lruPageIndex != -1) {
                MemoryPage& lruPage = mainMemory[lruPageIndex];
                int virtualPageIndex = lruPage.pageId;

                // Update page table entry to indicate the page is in virtual memory
                process.pageTable[virtualPageIndex].physicalPageNumber = -1;

                // Swap the page to virtual memory
                virtualMemory[virtualPageIndex] = lruPage;

                // Remove the page from main memory
                mainMemory[lruPageIndex].pid = -1;
                mainMemory[lruPageIndex].pageId = -1;
                mainMemory[lruPageIndex].data.clear();
                mainMemory[lruPageIndex].lastAccessTime = time(nullptr);

                cout << "Process " << process.pid << " swapped out page " << virtualPageIndex << " from main memory" << endl;

                return true;
            } else {
                cout << "Error: Unable to find an LRU page for swapping out" << endl;
                return false;
            }
        }
    }

    cout << "Error: Process with PID " << pid << " not found in main memory" << endl;
    return false;
}

// Function to swap in a process from virtual memory to main memory using LRU algorithm
bool swapInCommand(int pid) {
    //cout<<"swapCommand function entered"<<endl;
    for (Process& process : processes) {
        if (process.pid == pid && !process.inMainMemory) {
            int emptyPageIndex = -1;

            // Find an empty slot in main memory
            for (int i = 0; i < mainMemory.size(); ++i) {
                if (mainMemory[i].pid == -1) {
                    emptyPageIndex = i;
                    break;
                }
            }

            if (emptyPageIndex != -1) {
                // Find the page to be swapped in from virtual memory using LRU
                int lruPageIndex = findLRUPage();

                if (lruPageIndex != -1) {
                    MemoryPage& lruPage = virtualMemory[lruPageIndex];

                    // Update page table entry to indicate the page is in main memory
                    process.pageTable[lruPage.pageId].physicalPageNumber = emptyPageIndex;

                    // Swap the page to main memory
                    mainMemory[emptyPageIndex] = lruPage;

                    // Remove the page from virtual memory
                    virtualMemory[lruPageIndex].pid = -1;
                    virtualMemory[lruPageIndex].pageId = -1;
                    virtualMemory[lruPageIndex].data.clear();
                    virtualMemory[lruPageIndex].lastAccessTime = time(nullptr);

                    cout << "Process " << process.pid << " swapped in page " << lruPage.pageId << " to main memory" << endl;
                    return true;
                } else {
                    cout << "Error: Unable to find an LRU page for swapping in" << endl;
                    return false;
                }
            } else {
                cout << "Error: Main memory is full, cannot swap in process " << process.pid << endl;
                return false;
            }
        }
    }

    cout << "Error: Process with PID " << pid << " not found in virtual memory" << endl;
    return false;
}

bool loadExecutable(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cout << "Error: Unable to open file " << filename << endl;
        return false;
    }

    int size;
    file >> size;
    Process process;
    
    process.processFilename = filename;
    process.size = size;

    int numLogicalPages = (size * 1024) / PageSize;

    if (numLogicalPages > mainMemory.size() - CurMemorySize && numLogicalPages > virtualMemory.size() - CurVirtualSize) {
        cout <<filename<< " could not be loaded - memory is full "<< endl;
        return false;
    }
    process.pid = pidCounter++;
    process.pageTable.resize(numLogicalPages);

    for (int i = 0; i < numLogicalPages; ++i) {
        MemoryPage page;
        page.pid = process.pid;
        page.pageId = i;
        page.lastAccessTime = time(nullptr); // Initialize last access time

        for (int j = 0; j < PageSize; ++j) {
            int value;
            file >> value;
            page.data.push_back(value);
        }
        //int k = CurMemorySize + i;
        if (numLogicalPages <= mainMemory.size() - CurMemorySize) {
            process.inMainMemory = true;
            CurMemorySize++;
            mainMemory[CurMemorySize-1] = page;
            process.pageTable[i].logicalPageNumber = i;
            process.pageTable[i].physicalPageNumber = CurMemorySize -1;
        } else {
            process.inMainMemory = false;
            CurVirtualSize++;
            virtualMemory.push_back(page);
            process.pageTable[i].logicalPageNumber = i;
            process.pageTable[i].physicalPageNumber = CurVirtualSize - mainMemory.size();
        }
    }

    
    processes.push_back(process);

    cout << "Process " << process.pid << " is loaded in " << (process.inMainMemory ? "main" : "virtual") << " memory" << endl;
    return true;
}

bool executeProcess(int pid, ifstream& infile) {
    //cout<<"executedProcess function entered"<<endl;
    for (Process& process : processes) {
        if (process.pid == pid) {
            if (!process.inMainMemory) {
                cout << "Process " << process.pid << " is not in main memory. Swapping in..." << endl;
                if (!swapInCommand(pid)) {
                    cout << "Error: Failed to swap in process " << process.pid << endl;
                    return false;
                }
            }
            int siz;
            infile>>siz;
            int limit = siz;
            string command;
            while (infile >> command) {
                cout << "Command: " << command << endl;
                if (command == "add") {
                    int x, y, z;
                    infile >> x >> y >> z;
                    if(x<=(1024*limit) && y<=(1024*limit) && z<=(1024*limit)){
                    cout << "Result: Value in addr " << x << " = " << mainMemory[process.pageTable[x / PageSize].physicalPageNumber].data[x % PageSize] << ", addr " << y << " = " << mainMemory[process.pageTable[y / PageSize].physicalPageNumber].data[y % PageSize] << ", addr " << z << " = " << mainMemory[process.pageTable[z / PageSize].physicalPageNumber].data[z % PageSize] << endl;
                    int a = mainMemory[process.pageTable[x / PageSize].physicalPageNumber].data[x % PageSize];
                    int b = mainMemory[process.pageTable[y / PageSize].physicalPageNumber].data[y % PageSize];
                    mainMemory[process.pageTable[z / PageSize].physicalPageNumber].data[z % PageSize] = a+b ;
                    }
                    else{
                        if(x>(1024*limit)){
                        cout<<"Invalid Memory Address "<<x<<" specified for process id "<<pid<<endl;
                        }
                        else if(y>(1024*limit)){
                        cout<<"Invalid Memory Address "<<y<<" specified for process id "<<pid<<endl;
                        }
                        else{
                        cout<<"Invalid Memory Address "<<z<<" specified for process id "<<pid<<endl;
                        }
                          
                    }
                } else if (command == "sub") {
                    int x, y, z;
                    infile >> x >> y >> z;
                    if(x<=(1024*limit) && y<=(1024*limit) && z<=(1024*limit)){
                    cout << "Result: Value in addr " << x << " = " << mainMemory[process.pageTable[x / PageSize].physicalPageNumber].data[x % PageSize] << ", addr " << y << " = " << mainMemory[process.pageTable[y / PageSize].physicalPageNumber].data[y % PageSize] << ", addr " << z << " = " << mainMemory[process.pageTable[z / PageSize].physicalPageNumber].data[z % PageSize] << endl;
                    int a = mainMemory[process.pageTable[x / PageSize].physicalPageNumber].data[x % PageSize];
                    int b = mainMemory[process.pageTable[y / PageSize].physicalPageNumber].data[y % PageSize];
                    mainMemory[process.pageTable[z / PageSize].physicalPageNumber].data[z % PageSize] = a-b ;
                    }
                    else{
                        if(x>(1024*limit)){
                        cout<<"Invalid Memory Address "<<x<<" specified for process id "<<pid<<endl;
                        }
                        else if(y>(1024*limit)){
                        cout<<"Invalid Memory Address "<<y<<" specified for process id "<<pid<<endl;
                        }
                        else{
                        cout<<"Invalid Memory Address "<<z<<" specified for process id "<<pid<<endl;
                        }
                          
                    }
                } else if (command == "print") {
                    int x;
                    infile >> x;
                    if(x<=(1024*limit)){
                    cout << "Result: Value in addr " << x << " = " << mainMemory[process.pageTable[x / PageSize].physicalPageNumber].data[x % PageSize] << endl;
                    }
                    else{
                        cout<<"Invalid Memory Address "<<x<<" specified for process id "<<pid<<endl;
                    }
                } else if (command == "load") {
                    int a, y;
                    infile >> a >> y;
                     if(y<=(1024*limit)){
                    cout << "Result: Value of " << a << " is now stored in addr " << y << endl;
                    mainMemory[process.pageTable[y / PageSize].physicalPageNumber].data[y % PageSize] = a;
                     }
                     else{
                         cout<<"Invalid Memory Address "<<y<<" specified for process id "<<pid<<endl;
                     }
                } else {
                    cout << "Unknown command: " << command << endl;
                }
            }

            return true;
        }
    }

    cout << "Error: Process with PID " << pid << " not found." << endl;
    return false;
}

bool runCommand(int pid) {
    //cout<<"runcommand function entered"<<endl;
   // cout<<"pid:"<<pid<<endl;
    string filename ;
    for (const Process& process : processes) {
        if (process.pid == pid) {
            //cout<<"pid found in processes"<<process.processFilename<<endl;
            filename = process.processFilename;
        }
    }
    //cout<<"procesfile to run : "<<filename<<endl;
    ifstream infile(filename);
    if (!infile) {
        cout << "Error: Unable to open file " << filename << endl;
        return false;
    }

    return executeProcess(pid, infile);
}

void pteCommand(int pid, const string& outfile) {
    ofstream output(outfile, ios::app);
    if (!output) {
        cout << "Error: Unable to open file " << outfile << endl;
        return;
    }

    for (const Process& process : processes) {
        if (process.pid == pid) {
            output << "Page Table Entries for Process " << pid << ":" << endl;
            for (const PageTableEntry& entry : process.pageTable) {
                output << "Logical Page " << entry.logicalPageNumber << " -> Physical Page " << entry.physicalPageNumber << endl;
            }
            output << endl;
            output.close();
            return;
        }
    }

    cout << "Error: Process with PID " << pid << " not found." << endl;
    output.close();
}

void pteAllCommand(const string& outfile) {
    //cout<<"pteAllCommand function entered"<<endl;
    ofstream output(outfile, ios::app);
    if (!output) {
        cout << "Error: Unable to open file " << outfile << endl;
        return;
    }

    output << "Page Table Entries for All Processes:" << endl;
    for (const Process& process : processes) {
        output << "Process " << process.pid << ":" << endl;
        for (const PageTableEntry& entry : process.pageTable) {
            output << "Logical Page " << entry.logicalPageNumber << " -> Physical Page " << entry.physicalPageNumber << endl;
        }
        output << endl;
    }
    output.close();
}



// Function to print memory values
void printMemory(int memloc, int length) {
    for (const MemoryPage& page : mainMemory) {
        if (page.pid != -1 && memloc >= page.pageId * PageSize && memloc < (page.pageId + 1) * PageSize) {
            int offset = memloc - page.pageId * PageSize;
            int pageOffset = memloc % PageSize;

            if (pageOffset + length <= PageSize) {
                cout << "Memory Values:" << endl;
                for (int i = 0; i < length; ++i) {
                    cout << "Value at memory location " << memloc + i << ": " << page.data[pageOffset + i] << endl;
                }
                return;
            } else {
                cout << "Error: Memory location " << memloc + length - 1 << " is out of bounds" << endl;
                return;
            }
        }
    }

    cout << "Error: Memory location " << memloc << " not found in main memory" << endl;
}

// Function to kill a process and release its memory
void killCommand(int pid) {
    auto processIter = processes.begin();
    while (processIter != processes.end()) {
        if (processIter->pid == pid) {
            for (PageTableEntry& entry : processIter->pageTable) {
                if (entry.physicalPageNumber != -1) {
                    MemoryPage& page = mainMemory[entry.physicalPageNumber];
                    page.pid = -1;
                    page.pageId = -1;
                    page.data.clear();
                    page.lastAccessTime = time(nullptr);
                }
            }

            processIter = processes.erase(processIter);
            cout << "Process " << pid << " is killed, and its memory is released" << endl;
            return;
        } else {
            ++processIter;
        }
    }

    cout << "Error: Process with PID " << pid << " not found" << endl;
}

// Function to list processes in main memory and virtual memory
void listProcesses() {
    cout << "Processes in Main Memory:" << endl;
    for (const MemoryPage& page : mainMemory) {
        if (page.pid != -1 && page.pid != 0) {
            cout << "Process " << page.pid << " (Page " << page.pageId << ")" << endl;
        }
    }

    cout << "Processes in Virtual Memory:" << endl;
    for (const MemoryPage& page : virtualMemory) {
        if (page.pid != -1 && page.pid != 0) {
            cout << "Process " << page.pid << " (Page " << page.pageId << ")" << endl;
        }
    }
}

// Rest of the code remains unchanged



int main(int argc , char* argv[]) {
    // Variables for user-defined memory sizes and page size
    //int MainMemorySize, VirtualMemorySize, PageSize;
    
    // cout << "Enter Main Memory Size (in KB): ";
    // cin >> MainMemorySize;

    // cout << "Enter Virtual Memory Size (in KB): ";
    // cin >> VirtualMemorySize;

    // cout << "Enter Page Size (in bytes): ";
    // cin >> PageSize;

    MainMemorySize = stoi(argv[2]);
    VirtualMemorySize = stoi(argv[4]);
    PageSize = stoi(argv[6]);

    string input_file = argv[8];
   // string output_file = argv[10];

    // Calculate the number of pages in main memory and virtual memory
    int numMainMemoryPages = MainMemorySize * 1024 / PageSize;
    int numVirtualMemoryPages = VirtualMemorySize * 1024 / PageSize;

    // Resize mainMemory and virtualMemory based on the calculated number of pages
    mainMemory.resize(numMainMemoryPages);
    virtualMemory.resize(numVirtualMemoryPages);

    ifstream infile(input_file); // Change to your input file name
    //ofstream cout(output_file);
    string command;
    while (infile >> command) {
        label:
        if (command == "load") {
            string filename;
            vector<string> filenames; // Store filenames for this load command
            while (infile >> filename && filename != "load" && filename != "run" && filename != "kill" &&
                   filename != "listpr" && filename != "pte" && filename != "pteall" && filename != "swapout" &&
                   filename != "swapin" && filename != "print" && filename != "exit") {
                filenames.push_back(filename);
            }
            
            // Load executables with the collected filenames
            for (const string& fname : filenames) {
                loadExecutable(fname);
            }
            command = filename;
            cout<<endl;
            goto label;
        } else if (command == "run") {
            //cout<<"run command detected"<<endl;
            int pid;
            infile >> pid;
             runCommand(pid);
             cout<<endl;
        } else if (command == "kill") {
            int pid;
            infile >> pid;
            killCommand(pid);
            cout<<endl;
        } else if (command == "listpr") {
            listProcesses();
            cout<<endl;
        } else if (command == "pte") {
            int pid;
            string outfile;
            infile >> pid >> outfile;
            pteCommand(pid, outfile);
            cout<<endl;
        } else if (command == "pteall") {
            string outfile;
            infile >> outfile;
            pteAllCommand(outfile);
            cout<<endl;
        } else if (command == "swapout") {
            int pid;
            infile >> pid;
            swapOutCommand(pid);
            cout<<endl;
        } else if (command == "swapin") {
            int pid;
            infile >> pid;
            swapInCommand(pid);
            cout<<endl;
        } else if (command == "print") {
            int memloc, length;
            infile >> memloc >> length;
            printMemory(memloc, length);
            cout<<endl;
        } else if (command == "exit") {
            break;
        } else {
            cout << "Error: Invalid command in infile: " << command << endl;
            cout<<endl;
        }
    }

    return 0;
}
