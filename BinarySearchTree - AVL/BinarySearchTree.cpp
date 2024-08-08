//============================================================================
// Name        : BinarySearchTree.cpp
// Author      : Brian Engel
// Version     : 1.0
// Copyright   : Copyright © 2017 SNHU COCE
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <time.h>
#include <vector>
#include <functional>
#include <chrono>

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
    int height;

    // default constructor
    Node() {
        left = nullptr;
        right = nullptr;
        height = 1;
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
 * implement a binary search tree with AVL balancing
 */
class AVLBinarySearchTree {

private:
    Node* root;

    int height(Node* node);
    int getBalance(Node* node);
    Node* rotateRight(Node* y);
    Node* rotateLeft(Node* x);
    Node* insert(Node* node, Bid bid);
    Node* minValueNode(Node* node);
    Node* deleteNode(Node* root, std::string bidId);
    void inOrder(Node* node, std::function<void(Bid)> func);
    Node* search(Node* node, std::string bidId);
    void traverseDeleteTree(Node* node);

public:
    AVLBinarySearchTree();
    virtual ~AVLBinarySearchTree();
    void InOrder(std::function<void(Bid)> func);
    void Insert(Bid bid);
    void Remove(std::string bidId);
    Bid Search(std::string bidId);
};

/**
 * Default constructor
 */
AVLBinarySearchTree::AVLBinarySearchTree() {
    this->root = nullptr;
}

/**
 * Destructor
 */
AVLBinarySearchTree::~AVLBinarySearchTree() {

    traverseDeleteTree(this->root);
}

/*
 * Helper function for the destructor to traverse the tree
 * recursively and remove all nodes
 */
void AVLBinarySearchTree::traverseDeleteTree(Node* node) {
    if (node != nullptr) {
        // delete the left child first
        traverseDeleteTree(node->left);
        // delete the right child second
        traverseDeleteTree(node->right);
        // then delete the node itself
        delete node;
    }
}

/*
 * Private function to return height of a node
 */
int AVLBinarySearchTree::height(Node* node) {
    return node ? node->height : 0;
}

/*
 * Private function to return the balance factor of a node
 */
int AVLBinarySearchTree::getBalance(Node* node) {
    return node ? height(node->left) - height(node->right) : 0;
}

/*
 * Private function to perform a right rotation
 */
Node* AVLBinarySearchTree::rotateRight(Node* y) {
    // x becomes new root
    Node* x = y->left;
    // T2 becomes left child of y
    Node* T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = std::max(height(y->left), height(y->right)) + 1;
    x->height = std::max(height(x->left), height(x->right)) + 1;

    // Return new root
    return x;
}

/*
 * Private function to perform a left rotation
 */
Node* AVLBinarySearchTree::rotateLeft(Node* x) {
    // y becomes new root
    Node* y = x->right;
    // T2 becomes right child of x
    Node* T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update heights
    x->height = std::max(height(x->left), height(x->right)) + 1;
    y->height = std::max(height(y->left), height(y->right)) + 1;

    // Return new root
    return y;
}

/*
 * Public function to insert a bid into the tree
 */
void AVLBinarySearchTree::Insert(Bid bid) {
    root = insert(root, bid);
}

/*
 * Private function that inserts a bid into the tree.
 * Will return the root of the tree
 */
Node* AVLBinarySearchTree::insert(Node* node, Bid bid) {
    // Base case: If node is nullptr, create a new node
    if (!node) {
        return new Node(bid);
    }

    // Recursive case: Traverse the tree
    if (bid.bidId < node->bid.bidId) {
        // Insert in the left subtree
        node->left = insert(node->left, bid);
    }
    else if (bid.bidId > node->bid.bidId) {
        // Insert in the right subtree
        node->right = insert(node->right, bid);
    }
    else {
        // Duplicate bidId, do nothing
        return node;
    }

    // Update height of this ancestor node
    node->height = 1 + std::max(height(node->left), height(node->right));

    // Get the balance factor
    int balance = getBalance(node);

    // If node is unbalanced, there are 4 cases

    // Left Left Case
    if (balance > 1 && bid.bidId < node->left->bid.bidId) {
        return rotateRight(node);
    }

    // Right Right Case
    if (balance < -1 && bid.bidId > node->right->bid.bidId) {
        return rotateLeft(node);
    }

    // Left Right Case
    if (balance > 1 && bid.bidId > node->left->bid.bidId) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    // Right Left Case
    if (balance < -1 && bid.bidId < node->right->bid.bidId) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    // Return the unchanged node pointer
    return node;
}

/*
 * Public function to remove a bid from the tree
 */
void AVLBinarySearchTree::Remove(std::string bidId) {
    root = deleteNode(root, bidId);
}

/*
 * Private function to delete a node from the tree.
 * Will return the root of the tree
 */
Node* AVLBinarySearchTree::deleteNode(Node* root, std::string bidId) {
    // Base case: If root is nullptr
    if (!root) {
        return root;
    }

    // Recursive case: Traverse the tree
    if (bidId < root->bid.bidId) {
        // Traverse left subtree
        root->left = deleteNode(root->left, bidId);
    }
    else if (bidId > root->bid.bidId) {
        // Traverse right subtree
        root->right = deleteNode(root->right, bidId);
    }
    else {
        // Node with only one child or no child
        if (!root->left || !root->right) {
            Node* temp = root->left ? root->left : root->right;
            // No child case
            if (!temp) {
                temp = root;
                root = nullptr;
            }
            // One child case
            else {
                // Copy the contents of the non-empty child
                *root = *temp;
            }
            // Delete the old root
            delete temp;
        }
        else {
            // Node with two children: Get the inorder successor (smallest in the right subtree)
            Node* temp = minValueNode(root->right);
            // Copy the inorder successor's data to this node
            root->bid = temp->bid;
            // Delete the inorder successor
            root->right = deleteNode(root->right, temp->bid.bidId);
        }
    }

    // If the tree had only one node then return
    if (!root) {
        return root;
    }

    // Update height of this ancestor node
    root->height = std::max(height(root->left), height(root->right)) + 1;

    // Get the balance factor
    int balance = getBalance(root);

    // If node is unbalanced, there are 4 cases

    // Left Left Case
    if (balance > 1 && getBalance(root->left) >= 0) {
        return rotateRight(root);
    }

    // Left Right Case
    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }

    // Right Right Case
    if (balance < -1 && getBalance(root->right) <= 0) {
        return rotateLeft(root);
    }

    // Right Left Case
    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    // Return the (possibly) updated root
    return root;
}

/*
 * Private function to get the node with the minimum value
 * (used for deletion)
 */
Node* AVLBinarySearchTree::minValueNode(Node* node) {
    Node* current = node;
    // Loop down to find the leftmost leaf
    while (current->left != nullptr) {
        current = current->left;
    }
    return current;
}

/*
 * Public function to perform an in-order traversal
 */
void AVLBinarySearchTree::InOrder(std::function<void(Bid)> func) {
    inOrder(root, func);
}

/*
 * Private function to perform an in-order traversal
 */
void AVLBinarySearchTree::inOrder(Node* node, std::function<void(Bid)> func) {
    if (node != nullptr) {
        // Traverse left subtree
        inOrder(node->left, func);
        // Process node
        func(node->bid);
        // Traverse right subtree
        inOrder(node->right, func);
    }
}

/*
 * Public function to search for a bid by its ID
 */
Bid AVLBinarySearchTree::Search(std::string bidId) {
    Node* result = search(root, bidId);
    if (result != nullptr) {
        return result->bid;
    }
    else {
        return Bid(); // Return an empty bid or handle not found case
    }
}

/*
 * Private function to search for a bid in the tree
 */
Node* AVLBinarySearchTree::search(Node* node, std::string bidId) {
    // Base case: root is nullptr or bid is present at root
    if (node == nullptr || node->bid.bidId == bidId) {
        return node;
    }

    // Value is greater than root's value
    if (node->bid.bidId < bidId) {
        return search(node->right, bidId);
    }

    // Value is smaller than root's value
    return search(node->left, bidId);
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
void loadBids(string csvPath, AVLBinarySearchTree* bst) {
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

/*
 * Function for the main menu loop to get information
 * to insert a new bid, insert it, and display the time 
 * it took.
 */
void insertBid(AVLBinarySearchTree* bst) {
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
        bidKey = "98911";
    }

    // Define a timer variable
    clock_t ticks;

    // Define a binary search tree to hold all bids
    AVLBinarySearchTree* bst;
    bst = new AVLBinarySearchTree();
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
            bst->InOrder(displayBid);
            break;

        case 3:
            cout << "Enter Bid ID to find: ";
            getline(cin, bidKey);

            ticks = clock();

            bid = bst->Search(bidKey);

            ticks = clock() - ticks; // current clock ticks minus starting clock ticks

            if (!bid.bidId.empty()) {
                displayBid(bid);
            } else {
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
