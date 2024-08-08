//============================================================================
// Name        : BinarySearchTree.cpp
// Author      : Brian Engel
// Version     : 1.0
// Copyright   : Copyright © 2017 SNHU COCE
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <time.h>

#include "CSVparser.hpp"

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

// forward declarations
double strToDouble(string str, char ch);


// define a structure to hold bid information
struct Bid {
    string bidId; // unique identifier
    string title;
    string fund;
    double amount;
    Bid() {
        amount = 0.0;
    }
};
void displayBid(Bid bid);
// Internal structure for tree node
struct Node {
    Bid bid;
    Node *left;
    Node *right;

    // default constructor
    Node() {
        left = nullptr;
        right = nullptr;
    }

    // initialize with a bid
    Node(Bid aBid) :
            Node() {
        bid = aBid;
    }
};

//============================================================================
// Binary Search Tree class definition
//============================================================================

/**
 * Define a class containing data members and methods to
 * implement a binary search tree
 */
class BinarySearchTree {

private:
    Node* root;

    void addNode(Node* node, Bid bid);
    void inOrder(Node* node);
    void postOrder(Node* node);
    void preOrder(Node* node);
    //Node* removeNode(Node* node, string bidId);
    void traverseDeleteTree(Node* node);

public:
    BinarySearchTree();
    virtual ~BinarySearchTree();
    void InOrder();
    void PostOrder();
    void PreOrder();
    void Insert(Bid bid);
    void Remove(string bidId);
    Bid Search(string bidId);
};

/**
 * Default constructor
 */
BinarySearchTree::BinarySearchTree() {
    // FixMe (1): initialize housekeeping variables
    //root is equal to nullptr
    this->root = nullptr;
}

/**
 * Destructor
 */
BinarySearchTree::~BinarySearchTree() {

    // recurse from root deleting every node
    // I couldn't figure out how to recurse this actual function so I created a secondary one with a parameter to pass
    traverseDeleteTree(this->root);
}
void BinarySearchTree::traverseDeleteTree(Node* node) {
    if (node != nullptr) {
        // delete the left child first
        traverseDeleteTree(node->left);
        // delete the right child second
        traverseDeleteTree(node->right);
        // then delete the node itself
        delete node;
    }
}

/**
 * Traverse the tree in order
 */
void BinarySearchTree::InOrder() {
    // FixMe (2): In order root
    // call inOrder fuction and pass root
    inOrder(this->root);
}

/**
 * Traverse the tree in post-order
 */
void BinarySearchTree::PostOrder() {
    // FixMe (3): Post order root
    // postOrder root
    postOrder(this->root);
}

/**
 * Traverse the tree in pre-order
 */
void BinarySearchTree::PreOrder() {
    // FixMe (4): Pre order root
    // preOrder root
    preOrder(this->root);
}

/**
 * Insert a bid
 */
void BinarySearchTree::Insert(Bid bid) {
    // FIXME (5) Implement inserting a bid into the tree
    // if root equarl to null ptr
    Node* newNode = new Node(bid);
    if (this->root == nullptr) {
        // root is equal to new node bid
        this->root = newNode;
    }
    // else
    else {
        // add Node root and bid
        addNode(newNode, bid);
    }
}

/**
 * Remove a bid
 */
void BinarySearchTree::Remove(string bidId) {
    // FIXME (6) Implement removing a bid from the tree
    // remove node root bidID
    Node* parent = nullptr;
    Node* current = this->root;
    // Search for node
    while (current != nullptr) {
        // Node found 
        if (current->bid.bidId == bidId) {
            // Node found and no children
            if (current->left == nullptr && current->right == nullptr) {
                // Node is root
                if (parent == nullptr) {
                    this->root = nullptr;
                }
                else if (parent->left == current) {
                    parent->left = nullptr;
                }
                else {
                    parent->right = nullptr;
                }
            }
            // Remove node with only left child
            else if (current->right == nullptr) {
                // Node is root
                if (parent == nullptr) {
                    this->root = current->left;
                }
                else if (parent->left == current) {
                    parent->left = current->left;
                }
                else {
                    parent->right = current->left;
                }
            }
            // Remove node with only right child
            else if (current->left == nullptr) {
                // Node is root
                if (parent == nullptr) {
                    this->root = current->right;
                }
                else if (parent->left == current) {
                    parent->left = current->right;
                }
                else {
                    parent->right = current->right;
                }
            }
            // Remove node with two children
            else {
                // Find successor (leftmost child of right subtree)
                Node* successor = current->right;
                while (successor->left != nullptr) {
                    successor = successor->left;
                }
                // copy successor bid
                Bid temp = successor->bid;
                // Remove successor
                Remove(successor->bid.bidId);
                // replace current with successor
                current->bid = temp;
            }
        // Node found and removed
        return;
        }
        // Search right
        else if (current->bid.bidId < bidId) {
            parent = current;
            current = current->right;
        }
        // Search left
        else {
            parent = current;
            current = current->left;
        }
    }
    // Node not found
    return;
}

/**
 * Search for a bid
 */
Bid BinarySearchTree::Search(string bidId) {
    // FIXME (7) Implement searching the tree for a bid
    // set current node equal to root
    Node* current = this->root;
    // keep looping downwards until bottom reached or matching bidId found
    while (current != nullptr) {
        // if match found, return current bid
        if (bidId == current->bid.bidId) {
            return current->bid;
        }
        // if bid is smaller than current node then traverse left
        else if (bidId < current->bid.bidId) {
            current = current->left;
        }
        // else larger so traverse right
        else {
            current = current->right;
        }
    }
    // not found return empty bid
    Bid bid;
    return bid;
}

/**
 * Add a bid to some node (recursive)
 *
 * @param node Current node in tree
 * @param bid Bid to be added
 */
void BinarySearchTree::addNode(Node* node, Bid bid) {
    // FIXME (8) Implement inserting a bid into the tree
    Node* current = this->root;
    while (current != nullptr) {
        // if node is larger then add to left
        if (node->bid.bidId < current->bid.bidId) {
            // if no left node
            if (current->left == nullptr) {
                // this node becomes left
                current->left = node;
                current = nullptr;
            }
            else {
                // else recurse down the left node
                current = current->left;
            }
        }
        // else
        else {
            // if no right node
            if (current->right == nullptr) {
                // this node becomes right
                current->right = node;
                current = nullptr;
            }
            //else
            else {
                // recurse down the left node
                current = current->right;
            }
        }
    }
}
void BinarySearchTree::inOrder(Node* node) {
    // FixMe (9): Pre order root
    //if node is not equal to null ptr
    if (node != nullptr) {
        //InOrder not left
        inOrder(node->left);
        //output bidID, title, amount, fund
        displayBid(node->bid);
        //InOder right
        inOrder(node->right);
    }
}
void BinarySearchTree::postOrder(Node* node) {
      // FixMe (10): Pre order root
      //if node is not equal to null ptr
      //postOrder left
      //postOrder right
      //output bidID, title, amount, fund

}

void BinarySearchTree::preOrder(Node* node) {
      // FixMe (11): Pre order root
      //if node is not equal to null ptr
      //output bidID, title, amount, fund
      //postOrder left
      //postOrder right      
}

//============================================================================
// Static methods used for testing
//============================================================================

/**
 * Display the bid information to the console (std::out)
 *
 * @param bid struct containing the bid info
 */
void displayBid(Bid bid) {
    cout << bid.bidId << ": " << bid.title << " | " << bid.amount << " | "
            << bid.fund << endl;
    return;
}

/**
 * Load a CSV file containing bids into a container
 *
 * @param csvPath the path to the CSV file to load
 * @return a container holding all the bids read
 */
void loadBids(string csvPath, BinarySearchTree* bst) {
    cout << "Loading CSV file " << csvPath << endl;

    // initialize the CSV Parser using the given path
    csv::Parser file = csv::Parser(csvPath);

    // read and display header row - optional
    vector<string> header = file.getHeader();
    for (auto const& c : header) {
        cout << c << " | ";
    }
    cout << "" << endl;

    try {
        // loop to read rows of a CSV file
        for (unsigned int i = 0; i < file.rowCount(); i++) {

            // Create a data structure and add to the collection of bids
            Bid bid;
            bid.bidId = file[i][1];
            bid.title = file[i][0];
            bid.fund = file[i][8];
            bid.amount = strToDouble(file[i][4], '$');

            //cout << "Item: " << bid.title << ", Fund: " << bid.fund << ", Amount: " << bid.amount << endl;

            // push this bid to the end
            bst->Insert(bid);
        }
    } catch (csv::Error &e) {
        std::cerr << e.what() << std::endl;
    }
}

/**
 * Simple C function to convert a string to a double
 * after stripping out unwanted char
 *
 * credit: http://stackoverflow.com/a/24875936
 *
 * @param ch The character to strip out
 */
double strToDouble(string str, char ch) {
    str.erase(remove(str.begin(), str.end(), ch), str.end());
    return atof(str.c_str());
}

void insertBid(BinarySearchTree* bst) {
    Bid bid;
    cout << "Enter Bid ID: ";
    getline(cin, bid.bidId);
    cout << "Enter Title: ";
    getline(cin, bid.title);
    cout << "Enter Fund: ";
    getline(cin, bid.fund);
    cout << "Enter Amount: ";
    cin >> bid.amount;
    cin.ignore();
    // Define a timer variable
    clock_t ticks;
    ticks = clock();
    bst->Insert(bid);
    cout << "Bid inserted successfully." << endl;
    // Calculate elapsed time and display result
    ticks = clock() - ticks; // current clock ticks minus starting clock ticks
    cout << "time: " << ticks << " clock ticks" << endl;
    cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
}

/**
 * The one and only main() method
 */
int main(int argc, char* argv[]) {

    // process command line arguments
    string csvPath, bidKey;
    switch (argc) {
    case 2:
        csvPath = argv[1];
        bidKey = "98109";
        break;
    case 3:
        csvPath = argv[1];
        bidKey = argv[2];
        break;
    default:
        csvPath = "eBid_Monthly_Sales.csv";
        //bidKey = "98109";
        bidKey = "98911";
    }

    // Define a timer variable
    clock_t ticks;

    // Define a binary search tree to hold all bids
    BinarySearchTree* bst;
    bst = new BinarySearchTree();
    Bid bid;

    int choice = 0;
    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Load Bids" << endl;
        cout << "  2. Display All Bids" << endl;
        cout << "  3. Find Bid" << endl;
        cout << "  4. Remove Bid" << endl;
        cout << "  5. Insert Bid" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {

        case 1:
            // Initialize a timer variable before loading bids
            ticks = clock();

            // Complete the method call to load the bids
            loadBids(csvPath, bst);

            // Calculate elapsed time and display result
            ticks = clock() - ticks; // current clock ticks minus starting clock ticks
            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
            break;

        case 2:
            bst->InOrder();
            break;

        case 3:
            cout << "Enter Bid ID to find: ";
            getline(cin, bidKey);

            ticks = clock();

            bid = bst->Search(bidKey);

            ticks = clock() - ticks; // current clock ticks minus starting clock ticks

            if (!bid.bidId.empty()) {
                displayBid(bid);
            }
            else {
                cout << "Bid Id " << bidKey << " not found." << endl;
            }

            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;

            break;

        case 4:
            cout << "Enter Bid ID to remove: ";
            getline(cin, bidKey);

            // Initialize a timer variable before loading bids
            ticks = clock();

            bst->Remove(bidKey);

            // Calculate elapsed time and display result
            ticks = clock() - ticks; // current clock ticks minus starting clock ticks
            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
            break;

        case 5:
            insertBid(bst);
            break;

        default:
            if (choice != 9) {
                cout << "Invalid choice. Please try again." << endl;
            }
            break;
        }
    }

    cout << "Good bye." << endl;

    return 0;
}

/**
 * The one and only main() method
 */
//int main(int argc, char* argv[]) {
//
//    // process command line arguments
//    string csvPath, bidKey;
//    switch (argc) {
//    case 2:
//        csvPath = argv[1];
//        bidKey = "98109";
//        break;
//    case 3:
//        csvPath = argv[1];
//        bidKey = argv[2];
//        break;
//    default:
//        csvPath = "eBid_Monthly_Sales_Dec_2016.csv";
//        //bidKey = "98109";
//        bidKey = "98104";
//    }
//
//    // Define a timer variable
//    clock_t ticks;
//
//    // Define a binary search tree to hold all bids
//    BinarySearchTree* bst;
//    bst = new BinarySearchTree();
//    Bid bid;
//
//    int choice = 0;
//    while (choice != 9) {
//        cout << "Menu:" << endl;
//        cout << "  1. Load Bids" << endl;
//        cout << "  2. Display All Bids" << endl;
//        cout << "  3. Find Bid" << endl;
//        cout << "  4. Remove Bid" << endl;
//        cout << "  9. Exit" << endl;
//        cout << "Enter choice: ";
//        cin >> choice;
//
//        switch (choice) {
//
//        case 1:
//            
//            // Initialize a timer variable before loading bids
//            ticks = clock();
//
//            // Complete the method call to load the bids
//            loadBids(csvPath, bst);
//
//            //cout << bst->Size() << " bids read" << endl;
//
//            // Calculate elapsed time and display result
//            ticks = clock() - ticks; // current clock ticks minus starting clock ticks
//            cout << "time: " << ticks << " clock ticks" << endl;
//            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
//            break;
//
//        case 2:
//            bst->InOrder();
//            break;
//
//        case 3:
//            ticks = clock();
//
//            bid = bst->Search(bidKey);
//
//            ticks = clock() - ticks; // current clock ticks minus starting clock ticks
//
//            if (!bid.bidId.empty()) {
//                displayBid(bid);
//            } else {
//            	cout << "Bid Id " << bidKey << " not found." << endl;
//            }
//
//            cout << "time: " << ticks << " clock ticks" << endl;
//            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
//
//            break;
//
//        case 4:
//            bst->Remove(bidKey);
//            break;
//        }
//    }
//
//    cout << "Good bye." << endl;
//
//	return 0;
//}
