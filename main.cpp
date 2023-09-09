/*
Project Name: 235 Lab 11 - AVL
Description: Make and Operate an AVL Binary Search Tree
*/

#ifdef _MSC_VER
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define VS_MEM_CHECK _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#else
#define VS_MEM_CHECK
#endif


#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "AVL.h"
using namespace std;

int main(int argc, const char * argv[]) {
    
    VS_MEM_CHECK; //Checks for Memory Leaks
    
    //Opens Files
    if (argc < 3) {
       cerr << "Please provide name of input and output files" << endl;
       return 1;
    }
    cout << "Input file: " << argv[1] << endl;
    ifstream in(argv[1]);
    if (!in) {
       cerr << "Unable to open " << argv[1] << " for input" << endl;
       return 2;
    }
    cout << "Output file: " << argv[2] << endl;
    
    ofstream out(argv[2]);
    if (!out) {
       in.close();
       cerr << "Unable to open " << argv[2] << " for output" << endl;
       return 3;
    }
    
    string line, command, data;
    AVL<int>* intAVL = NULL;
    AVL<string>* strAVL = NULL;
    bool prevDeclaredInt = false;
    bool prevDeclaredStr = false;
    bool usingInt = true;
    
    while (getline(in, line)) {
        if (line == "") continue;
        istringstream iss(line);
        iss >> command;
        if (command == "print" || command == "tree") {
            line.push_back(':');
        }
        out << line << " ";
        
        //Instantiates an AVL<int>
        if (command == "INT") {
            if (prevDeclaredInt) {
                delete intAVL;
            }
            intAVL = new AVL<int>;
            usingInt = true;
            prevDeclaredInt = true;
            out << "true";
        }
        
        //Instantiates an AVL<string>
        else if (command == "STRING") {
            if (prevDeclaredStr) {
                delete strAVL;
            }
            strAVL = new AVL<string>;
            usingInt = false;
            prevDeclaredStr = true;
            out << "true";
        }
        
        //Adds an item to AVL
        else if (command == "add") {
            iss >> data;
            if (usingInt) {
                out << boolalpha << intAVL->addNode(stoi(data));
            }
            else {
                out << boolalpha << strAVL->addNode(data);
            }
        }
        
        //Removes an item from AVL
        else if (command == "remove") {
            iss >> data;
            if (usingInt) {
                out << boolalpha << intAVL->removeNode(stoi(data));
            }
            else {
                out << boolalpha << strAVL->removeNode(data);
            }
        }
        
        //Clears the AVL
        else if (command == "clear") {
            if (usingInt) {
                out << boolalpha << intAVL->clearTree();
            }
            else {
                out << boolalpha << strAVL->clearTree();
            }
        }
        
        //Returns the number of nodes in AVL
        else if (command == "size") {
            if (usingInt) {
                out << boolalpha << intAVL->sizeTree();
            }
            else {
                out << boolalpha << strAVL->sizeTree();
            }
        }
        
        //Prints a level-order view of the AVL
        else if (command == "print") {
            if (usingInt) {
                out << boolalpha << intAVL;
            }
            else {
                out << boolalpha << strAVL;
            }
        }
        
        //Finds an item in AVL
        else if (command == "find") {
            iss >> data;
            if (usingInt) {
                out << intAVL->find(stoi(data));
            }
            else {
                out << strAVL->find(data);
            }
        }
        
        //Prints an in-order view of the AVL
        else if (command == "tree") {
            if (usingInt) {
                out << intAVL->treeInOrder();
            }
            else {
                out << strAVL->treeInOrder();
            }
        }
        
        out << endl;
    }
    
    delete intAVL;
    delete strAVL;
    
    return 0;
}
