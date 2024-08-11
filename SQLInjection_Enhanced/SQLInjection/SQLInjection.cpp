// SQLInjection.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <algorithm>
#include <iostream>
#include <locale>
#include <tuple>
#include <vector>
#include <string>

#include "sqlite3.h"

// DO NOT CHANGE
// Type alias for user records consisting of ID, NAME, and PASSWORD
typedef std::tuple<std::string, std::string, std::string> user_record;
const std::string str_where = " where ";

// Forward Declarations
bool query_user_by_name(sqlite3* db, const std::string& name, std::vector<user_record>& records);
bool insert_user(sqlite3* db, const std::string& name, const std::string& password);

// DO NOT CHANGE
// Callback function for SQLite queries
static int callback(void* possible_vector, int argc, char** argv, char** azColName)
{
    if (possible_vector == NULL)
    { // no vector passed in, so just display the results
        for (int i = 0; i < argc; i++)
        {
            std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << std::endl;
        }
        std::cout << std::endl;
    }
    else
    {
        std::vector< user_record >* rows =
            static_cast<std::vector< user_record > *>(possible_vector);

        rows->push_back(std::make_tuple(argv[0], argv[1], argv[2]));
    }
    return 0;
}

// Function to create the USERS and TRANSACTION_LOG tables in the database
// Returns true if successful, false otherwise
bool initialize_database(sqlite3* db)
{
    char* error_message = NULL;

    // SQL statement to create USERS table
    std::string sql = "CREATE TABLE USERS(" \
        "ID INTEGER PRIMARY KEY AUTOINCREMENT," \
        "NAME           TEXT    NOT NULL," \
        "PASSWORD       TEXT    NOT NULL);";

    int result = sqlite3_exec(db, sql.c_str(), callback, NULL, &error_message);
    if (result != SQLITE_OK)
    {
        std::cout << "Failed to create USERS table. ERROR = " << error_message << std::endl;
        sqlite3_free(error_message);
        return false;
    }
    std::cout << "USERS table created." << std::endl;

    // SQL to create TRANSACTION_LOG table
    std::string sql_log = "CREATE TABLE TRANSACTION_LOG (" \
        "ID INTEGER PRIMARY KEY AUTOINCREMENT, " \
        "TYPE TEXT NOT NULL, " \
        "TIMESTAMP DATETIME DEFAULT CURRENT_TIMESTAMP, " \
        "SUCCESS BOOLEAN NOT NULL, " \
        "USER_INPUT TEXT, "\
        "ERROR_MESSAGE TEXT); ";

    result = sqlite3_exec(db, sql_log.c_str(), callback, NULL, &error_message);
    if (result != SQLITE_OK)
    {
        std::cout << "Failed to create TRANSACTION_LOG table. ERROR = " << error_message << std::endl;
        sqlite3_free(error_message);
        return false;
    }
    std::cout << "TRANSACTION_LOG table created." << std::endl;

    return true;
}

// DO NOT CHANGE
// Function to simulate SQL injection by modifying the input query
bool run_query_injection(sqlite3* db, const std::string& sql, std::vector< user_record >& records)
{
    std::string injectedSQL(sql);
    std::string localCopy(sql);

    // we work on the local copy because of the const
    std::transform(localCopy.begin(), localCopy.end(), localCopy.begin(), ::tolower);
    if (localCopy.find_last_of(str_where) >= 0)
    { // this sql has a where clause
        if (localCopy.back() == ';')
        { // smart SQL developer terminated with a semicolon - we can fix that!
            injectedSQL.pop_back();
        }

        switch (rand() % 4)
        {
        case 1:
            injectedSQL.append("");
            break;
        case 2:
            injectedSQL.append("extraletters");
            break;
        case 3:
            injectedSQL.append(" or 'hack'='hack';");
            break;
        case 0:
        default:
            injectedSQL.append(" or 1=1;");
            break;
        }
    }

    return query_user_by_name(db, injectedSQL, records);
}

// Function to print query results
void dump_results(const std::vector< user_record >& records)
{
    for (auto record : records)
    {
        std::cout << "User: " << std::get<1>(record) << " [UID=" << std::get<0>(record) << " PWD=" << std::get<2>(record) << "]" << std::endl;
    }
}

// Function to log transactions in the TRANSACTION_LOG table
bool log_transaction(sqlite3* db, const std::string& type, bool success, const std::string& user_input, const std::string& error_message = "") {
    std::string sql = "INSERT INTO TRANSACTION_LOG (TYPE, SUCCESS, USER_INPUT, ERROR_MESSAGE) VALUES (?, ?, ?, ?)";

    // Prepare the SQL statement
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cout << "Failed to prepare statement for logging. ERROR = " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Bind parameters
    if (sqlite3_bind_text(stmt, 1, type.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, success ? 1 : 0) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 3, user_input.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 4, error_message.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
        std::cout << "Failed to bind parameters for logging. ERROR = " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    // Execute the statement
    int result = sqlite3_step(stmt);
    if (result != SQLITE_DONE) {
        std::cout << "Failed to execute log statement. ERROR = " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    // Finalize the statement to release resources
    if (sqlite3_finalize(stmt) != SQLITE_OK) {
        std::cout << "Failed to finalize log statement. ERROR = " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    return true;
}

// Function to query all users from the USERS table
void query_all_users(sqlite3* db, std::vector<user_record>& records) {

    std::string sql = "SELECT * FROM USERS";
    std::cout << std::endl << std::endl << sql << std::endl;

    // Prepare the SQL statement
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::string error_message = "Failed to prepare statement. ERROR = " + std::string(sqlite3_errmsg(db));
        std::cout << error_message << std::endl;
        log_transaction(db, "QUERY_ALL_USERS", false, "", error_message);
        return;
    }

    // Execute the statement
    int result;
    while ((result = sqlite3_step(stmt)) == SQLITE_ROW) {
        std::string id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

        records.push_back(std::make_tuple(id, name, password));
    }

    // Check for execution errors
    if (result != SQLITE_DONE) {
        std::string error_message = "Failed to execute statement. ERROR = " + std::string(sqlite3_errmsg(db));
        std::cout << error_message << std::endl;
        log_transaction(db, "QUERY_ALL_USERS", false, "null", error_message);
    }
    else {
        log_transaction(db, "QUERY_ALL_USERS", true, "null", "Query executed successfully.");
    }

    // Finalize the statement to release resources
    if (sqlite3_finalize(stmt) != SQLITE_OK) {
        std::string error_message = "Failed to finalize statement. ERROR = " + std::string(sqlite3_errmsg(db));
        std::cout << error_message << std::endl;
        log_transaction(db, "QUERY_ALL_USERS", false, "null", error_message);
    }

    // Dump results
    dump_results(records);
    records.clear();
}

// Function to query users by name from the USERS table
bool query_user_by_name(sqlite3* db, const std::string& name, std::vector<user_record>& records) {
    // Prepare the SQL statement with the given name parameter
    std::string sql = "SELECT ID, NAME, PASSWORD FROM USERS WHERE NAME = ?";
    std::cout << std::endl << std::endl << "SELECT ID, NAME, PASSWORD FROM USERS WHERE NAME = " + name << std::endl;

    // Prepare the SQL statement
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::string error_msg = sqlite3_errmsg(db);
        std::cout << "Failed to prepare statement. ERROR = " << error_msg << std::endl;
        log_transaction(db, "QUERY_USER_BY_NAME", false, "Name: " + name, error_msg);
        return false;
    }

    // Bind the parameter (name) to the placeholder
    if (sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
        std::string error_msg = sqlite3_errmsg(db);
        std::cout << "Failed to bind parameter. ERROR = " << error_msg << std::endl;
        sqlite3_finalize(stmt);
        log_transaction(db, "QUERY_USER_BY_NAME", false, "Name: " + name, error_msg);
        return false;
    }

    // Execute the prepared statement
    int result;
    while ((result = sqlite3_step(stmt)) == SQLITE_ROW) {
        std::string id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        records.push_back(std::make_tuple(id, name, password));
    }

    // Check for execution errors
    if (result != SQLITE_DONE) {
        std::string error_msg = sqlite3_errmsg(db);
        std::cout << "Failed to execute statement. ERROR = " << error_msg << std::endl;
        log_transaction(db, "QUERY_USER_BY_NAME", false, "Name: " + name, error_msg);
    }

    // Finalize the statement to release resources
    if (sqlite3_finalize(stmt) != SQLITE_OK) {
        std::string error_msg = sqlite3_errmsg(db);
        std::cout << "Failed to finalize statement. ERROR = " << error_msg << std::endl;
        log_transaction(db, "QUERY_USER_BY_NAME", false, "Name: " + name, error_msg);
    }

    // Dump results
    // If no results
    if (records.empty()) {
        // create a vector of strings for suspicious words or symbols
        std::vector<std::string> suspicious_keywords = { " or ", " and ", " drop "};

        // copy the sql string and make it lower case to account for different capitalizations
        std::string lowerCaseSQL = name;
        std::transform(lowerCaseSQL.begin(), lowerCaseSQL.end(), lowerCaseSQL.begin(), ::tolower);
        std::vector<std::string> foundWords = {};
        // search for suspicious words and symbols in the copied string and if found output and return false
        for (const std::string& keyword : suspicious_keywords) {
            if (lowerCaseSQL.find(keyword) != std::string::npos) {
                foundWords.push_back(keyword);
            }
        }
        // If no keywords where found in input
        if (foundWords.empty()) {
            std::cout << "No records found." << std::endl;
            log_transaction(db, "QUERY_USER_BY_NAME", false, "Name: " + name, "No records found.");
        }
        // If keywords where found in input signifying a possible attempted attack
        else {
            std::string foundString = "";
            for (auto word : foundWords) {
                foundString += word;
            }
            std::cout << "No records found." << std::endl;
            log_transaction(db, "QUERY_USER_BY_NAME", false, "Name: " + name, "Suspicious keywords: " + foundString);
        }
    }
    // Query returned results
    else {
        dump_results(records);
        log_transaction(db, "QUERY_USER_BY_NAME", true, "Name: " + name, "Records found.");
    }

    records.clear();
    return true;
}

// Function to insert a new user into the USERS table
bool insert_user(sqlite3* db, const std::string& name, const std::string& password) {
    // SQL insert statement with placeholders
    std::string sql = "INSERT INTO USERS (NAME, PASSWORD) VALUES (?, ?)";
    std::cout << std::endl << std::endl << "Inserting record: NAME=" << name << ", PASSWORD=" << password << std::endl;
    bool success = false;

    // Prepare the SQL statement
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cout << "Failed to prepare statement. ERROR = " << sqlite3_errmsg(db) << std::endl;
        log_transaction(db, "INSERT_USER", success, "{Name: " + name + ", Password: " + password + "}", sqlite3_errmsg(db));
        return false;
    }

    // Bind parameters
    if (sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
        std::cout << "Failed to bind NAME parameter. ERROR = " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        log_transaction(db, "INSERT_USER", success, "{Name: " + name + ", Password: " + password + "}", sqlite3_errmsg(db));
        return false;
    }
    if (sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
        std::cout << "Failed to bind PASSWORD parameter. ERROR = " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        log_transaction(db, "INSERT_USER", success, "{Name: " + name + ", Password: " + password + "}", sqlite3_errmsg(db));
        return false;
    }

    // Execute the statement
    int result = sqlite3_step(stmt);
    if (result != SQLITE_DONE) {
        std::string error_message = "Failed to execute statement. ERROR = " + std::string(sqlite3_errmsg(db));
        std::cout << error_message << std::endl;
        log_transaction(db, "INSERT_USER", false, name, error_message);
        return false;
    }

    // Finalize the statement to release resources
    if (sqlite3_finalize(stmt) != SQLITE_OK) {
        std::string error_message = "Failed to finalize statement. ERROR = " + std::string(sqlite3_errmsg(db));
        std::cout << error_message << std::endl;
        log_transaction(db, "INSERT_USER", false, name, error_message);
        return false;
    }

    log_transaction(db, "INSERT_USER", true, name, "User inserted successfully.");
    return true;
}

// Function to run a series of database operations and simulate SQL injection attacks
void run_queries(sqlite3* db)
{
    char* error_message = NULL;
    std::vector< user_record > records;

    // Insert several users into the USERS table
    insert_user(db, "Fred", "Flinstone");
    insert_user(db, "Barney", "Rubble");
    insert_user(db, "Wilma", "Flinstone");
    insert_user(db, "Betty", "Rubble");

    // Query all users
    query_all_users(db, records);

    // Query user by name 'Fred'
    query_user_by_name(db, "Fred", records);

    //  run query with injection 5 times
    for (auto i = 0; i < 5; ++i)
    {
        if (!run_query_injection(db, "Fred", records)) continue;
    }

    // Insert an additional user
    insert_user(db, "BamBam", "12345");
    
    // Query all users again to include the newly inserted user
    query_all_users(db, records);
}

// Function to print all records from the TRANSACTION_LOG table
void print_all_logs(sqlite3* db) {
    const char* query = "SELECT * FROM transaction_log;";
    sqlite3_stmt* stmt;

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement. ERROR = " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    std::cout << "All Transaction Log Records:" << std::endl;

    // Execute the statement and process results
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);  
        const unsigned char* transaction_type = sqlite3_column_text(stmt, 1); 
        const unsigned char* timestamp = sqlite3_column_text(stmt, 2); 
        const unsigned char* success_status = sqlite3_column_text(stmt, 3); 
        const unsigned char* user_input = sqlite3_column_text(stmt, 4); 
        const unsigned char* error_message = sqlite3_column_text(stmt, 5); 

        std::cout << "ID: " << id
            << ", Type: " << (transaction_type ? reinterpret_cast<const char*>(transaction_type) : "NULL")
            << ", Timestamp: " << (timestamp ? reinterpret_cast<const char*>(timestamp) : "NULL")
            << ", Success: " << (success_status ? reinterpret_cast<const char*>(success_status) : "NULL")
            << ", User Input: " << (user_input ? reinterpret_cast<const char*>(user_input) : "NULL")
            << ", Message: " << (error_message ? reinterpret_cast<const char*>(error_message) : "NULL")
            << std::endl;
    }

    // Finalize the statement
    sqlite3_finalize(stmt);
}

// Function to print all failed logs (transactions with success = 0) from the TRANSACTION_LOG table
void print_failed_logs(sqlite3* db) {
    const char* query = "SELECT * FROM transaction_log WHERE success = '0';";
    sqlite3_stmt* stmt;

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement. ERROR = " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    std::cout << "All Transaction Log Records:" << std::endl;

    // Execute the statement and process results
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);  
        const unsigned char* transaction_type = sqlite3_column_text(stmt, 1); 
        const unsigned char* timestamp = sqlite3_column_text(stmt, 2); 
        const unsigned char* success_status = sqlite3_column_text(stmt, 3); 
        const unsigned char* user_input = sqlite3_column_text(stmt, 4); 
        const unsigned char* error_message = sqlite3_column_text(stmt, 5); 

        std::cout << "ID: " << id
            << ", Type: " << (transaction_type ? reinterpret_cast<const char*>(transaction_type) : "NULL")
            << ", Timestamp: " << (timestamp ? reinterpret_cast<const char*>(timestamp) : "NULL")
            << ", Success: " << (success_status ? reinterpret_cast<const char*>(success_status) : "NULL")
            << ", User Input: " << (user_input ? reinterpret_cast<const char*>(user_input) : "NULL")
            << ", Message: " << (error_message ? reinterpret_cast<const char*>(error_message) : "NULL")
            << std::endl;
    }

    // Finalize the statement
    sqlite3_finalize(stmt);
}

// Main function
int main()
{
    // initialize random seed:
    srand(time(nullptr));

    int return_code = 0;
    std::cout << "SQL Injection Example" << std::endl;

    // the database handle
    sqlite3* db = NULL;
    char* error_message = NULL;

    // open the database connection
    int result = sqlite3_open(":memory:", &db);

    if (result != SQLITE_OK)
    {
        std::cout << "Failed to connect to the database and terminating. ERROR=" << sqlite3_errmsg(db) << std::endl;
        return -1;
    }

    std::cout << "Connected to the database." << std::endl;

    // initialize our database
    if (!initialize_database(db))
    {
        std::cout << "Database Initialization Failed. Terminating." << std::endl;
        return_code = -1;
    }
    else
    {
        run_queries(db);
    }
    print_all_logs(db);

    bool running = true;
    // loop for testing
    while (running) {
        std::cout << "\nSelect an option:\n";
        std::cout << "1. Query all users\n";
        std::cout << "2. Query user by name\n";
        std::cout << "3. Insert a new user\n";
        std::cout << "4. Print all Transactions\n";
        std::cout << "5. Print failed Transactions\n";
        std::cout << "6. Exit\n";
        std::cout << "Enter your choice (1-6): ";

        char choice;
        std::cin >> choice;
        std::cin.ignore(); // Ignore the newline character left in the buffer

        std::vector<user_record> records;

        switch (choice) {
            // Query and display all users
            case '1': {
                query_all_users(db, records);
                break;
            }
            // Query and display user by name
            case '2': {
                std::string name;
                std::cout << "Enter the name of the user to query: ";
                std::getline(std::cin, name);
                query_user_by_name(db, name, records);
                break;
            }
            // Insert new user
            case '3': {
                std::string name, password;
                std::cout << "Enter the name of the new user: ";
                std::getline(std::cin, name);
                std::cout << "Enter the password for the new user: ";
                std::getline(std::cin, password);
                insert_user(db, name, password);
                break;
            }
            // Print all transaction logs
            case '4': {
                print_all_logs(db);
                break;
            }
            // Print failed transaction logs
            case '5': {
                print_failed_logs(db);
                break;
            }
            // Exit
            case '6': {
                running = false;
                break;
            }
            default:
                std::cout << "Invalid choice. Please enter a number between 1 and 6." << std::endl;
        }
    }

    // close the connection if opened
    if (db != NULL)
    {
        sqlite3_close(db);
    }

    return return_code;
}
