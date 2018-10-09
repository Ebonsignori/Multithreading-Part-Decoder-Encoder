#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <algorithm>

using namespace std;


// ==============================
// Method Signatures
// ==============================
// Thread Methods
void *sifter(void *);
void *decoder(void *);
typedef void* (*decoder_function)(void*); // Generic for tread functions
void* fence(void*);
void* hill(void*);
void* valley(void*);
// Share hill and valley logic methods
bool hillValleyGetTokenOne(string &part, string &calling_thread, int &current_index, int &section_one_int);
bool hillValleyGetTokenTwo(string &part, string &calling_thread, int &current_index, int section_one_int, string &section_two);
bool hillValleyGenerateMatrixK(string &section_three, string &calling_thread, int section_one_int, long** K);
// Validation Methods
bool isValidInputString();
// Matrix Manipulation Methods
void oneByNTimesNbyNMatrix(long size, long one_by_n[], long **n_by_n, long result_matrix[]);
void scalarModOnebyNMatrix(long size, long matrix[], long mod_factor);
long threeByThreeDeterminant(long **matrix);
long twoByTwoDeterminant(long **matrix);
void findSubMatrix(long size, long row_to_ignore, long col_to_ignore, long** matrix,  long** sub_matrix);
// Input Part Handling Methods
void splitIntoParts();
ulong findPartLength(ulong desired_index, ulong other_index_1, ulong other_index_2);
// Helper/Utility Methods
ulong maximum(ulong a, ulong b, ulong c);
ulong median(ulong a, ulong b, ulong c);
ulong minimum(ulong a, ulong b, ulong c);
void removeAllWhiteSpace(string &input_str);
void removeLeadingWhitespace(string &input_str);
void removeTrailingWhitespace(string &input_str);
bool isOnlyWhitespace(string input_str);
long convertToAlphabetPosition(char character);
char convertFromAlphabetPosition(long position_number);
long findModularMultiplicativeInverse(long b, long n);
long gcd(long a, long b);
// Debug Methods
void print1dMatrix(long size, long *matrix);
void print2dMatrix(long size, long **matrix);

// ==============================
// Globals
// ==============================
const bool IS_LOGGING = true; // True to turn on console logging for debugging purposes
const int ALPHABET_SIZE = 26;
string user_input_string, part_one, part_two, part_three;
string fence_thread_result, hill_thread_result, valley_thread_result;

// ==============================
// Regex Globals
// ==============================
// (Defined once so they aren't reallocated in memory on each function call)
regex numeric_start_regex("(^(\\d|" // Find the first X numeric digits from a string if they exist
                            "\\s)*)"); // Include whitespace

regex asterisk_regex("(?=.*?(^|[^\\*])\\*($|[^\\*]))" // Look ahead for '*' not followed/preceded by '*'
                     "(?=.*?(^|[^\\*])\\*\\*($|[^\\*]))" // Look ahead for '**' not followed/preceded by '**'
                     "(?=.*?(^|[^\\*])\\*\\*\\*($|[^\\*]))"  // Look ahead for '***' not followed/preceded by '***'
                     ".*"); // All look-ahead's also account for start (^) or end ($) of string

// ==============================
// Main Method
// ==============================
/* Entry point. Spawns sifter thread */
int main() {
    pthread_t sifter_thread;
    ulong sifter_retval;

    // Booleans (represented as ints) returned from pthread methods
    int sifter_created;
    int sifter_joined;

    // Returns 0 if sifter thread has been created successfully, otherwise 1 is returned upon creation failure
    sifter_created = pthread_create(&sifter_thread, nullptr, sifter, nullptr);

    // If sifter thread is created successfully
    if (sifter_created == 0) {
        if (IS_LOGGING) {
            printf("Sifter thread spawned successfully.\n");
        }
    } else if (sifter_created == 1) {
        printf("Error spawning sifter thread from main module thread.\n");
        std::exit(1); // Exit with unsuccessful error code
    }

    // Join sifter thread with parent thread. Waits for results of sifter thread before continuing sequential execution
    sifter_joined = pthread_join(sifter_thread, (void **)&sifter_retval);

    // ------------------------------
    // Handle Results Passed Through From Sifter Thread
    // ------------------------------
    // If sifter join is successful
    if (sifter_joined == 0) {
        if (IS_LOGGING) {
            printf("Sifter joined successfully.\n");
        }
        // Handle sifter thread return (0 = exit successfully)
        if (sifter_retval == 0) {
            printf("Program quiting...");
            exit(0); // Exit with success code 0
        // 1 = uncaught error
        } else if (sifter_retval == 1) {
            printf("Something went wrong during program execution. "
                   "Please contact an admin with details of what you were doing at the time of crash.");
            exit(1); // Exit with error code 1
        // 2 = user exceed max input attempts
        } else if (sifter_retval == 2) {
            exit(1); // Exit with error code 1
        }

    // If error joining sifter thread, exit with error
    } else {
        printf("Error joining sifter thread with main module thread.\n");
        std::exit(1); // Exit with unsuccessful error code
    }

    return 0;
}


// ==============================
// Thread Methods
// ==============================
/* Asks user for message (a character string made up of three parts). If the message is invalid, then the program gives
 * the user only three chances to enter a valid message (with three no-null parts) The thread sifter then spawns the
 * decoder thread that divides the message into three parts.
 *
 * Thread returns a uint code that is handled by the main method, with the following meanings:
 *   0 = successful exit (user enters quit command)
 *   1 = uncaught error
 *   2 = user ran out of input attempts
 * */
void* sifter(void *) {
    uint test_index = 0;

    while (true) {
        user_input_string = ""; // Reset user_input_string to an empty string (for multiple runs of program)
        // ------------------------------
        // Get a Valid Input String From User
        // ------------------------------
        bool is_valid_user_input;
        int attempts = 0;
        // TODO: Change back into 3 before deploying
        int max_attempts = 13;

        // TEMP
        uint total_tests = 25;
        auto test_inputs = new string[total_tests];
        // Hashemi Tests
        test_inputs[0] =  "***3 rrlmwbkaspdh 17 17 5 21 18 21 2 2 19 12 *123555eu5eotsya**3 GoodMorningJohn 3 2 1 20 15 4 10 22 3";
        test_inputs[1] =  "**2 Global Warming 12 4 5 23 18 13 4 *** 2 RFRWYQ 5 8 17 3 4 9*31427781OAOSRSDKYPWIKSRO";
        test_inputs[2] =  "**ACDUJF 1 4 6 12***MNKLHY 1 2 3 4 5 6 7 8 9*M FGCV";
        test_inputs[3] =  "** KMLN 12 3 4 17 *** GHL 9 9 8 8 7 7 6 a6 5 * P TRBKHYT";
        test_inputs[4] =  "*B PHHW PH DIVHU WKH WRJD SDUWB";
        test_inputs[5] =  "*2 MNKK LLSGQW";
        test_inputs[6] =  "**GG CI MOQN 9 2 1 15 ***ST TNJVQLAIVDIG 21 20 24 21 24 9 -5 21 21";
        test_inputs[7] =  "** XNPD 5 8 17 3 ***KL LNSHDLEWMTRW 4 9 15 15 17 6 24 0 17*D GJFZNKZQDITSJ";
        test_inputs[8] =  "**GGMLN 12 3 4 b7 *** GHL 9 9 8 8 7 7 6  16  5 * P GRBKHYT";
        test_inputs[9] =  "** BBLN 12 3 4 17 *** GHL 9 9 8 8 7 7 6  6  5 * CRKLMNBKHYT";
        test_inputs[10] = "** MWUULNGXAP 12 15 7 6*** GHYTRKL MNP 1 0 0 -1 2 3 12*CDM UYKOOKPIRQQN";
        test_inputs[11] = "*zg ZHCHUUHOPXKPYAF***GHEDQHQQ 9 11 4 5 ** ABHJUTVOP 5 -2 3 4 12 32 1 0 3";
        test_inputs[12] = "***ABC 5 6 7 8 10 1 4 8 11*K SAAPXGOW**MKPW 10 2 12 1*J RZZOWFNV";
        // Evan Tests
        test_inputs[13] = "**DNE*DNE**DNE*DNE"; // Too many * - Invalid
        test_inputs[14] = "***DNE* **DNE"; // Null part 1 - Invalid
        test_inputs[15] = "***DNE*DNE t3w **"; // Null part 2 - Invalid
        // Part 1 - Fence
        test_inputs[16] = "*43125678812ttnaAptMTSUOaodwcoIXknLypETZ**DNE***DNE"; // Valid
        test_inputs[17] = "*93456312ttnaAptMTSUOaodwcoIXknLypETZ**DNE***DNE"; // Invalid
        // Part 2 - Hill
        test_inputs[18] = "*43125678812ttnaAptMTSUOaodwcoIXknLypETZ**3 paymoremoney 17 17 5 21 18 21 2 2 19 0 15***DNE";
        test_inputs[19] = "*43125678812ttnaAptMTSUOaodwcoIXknLypETZ**3  p ay more money  17 17 5 21   18 21 2 2   19 0 15  ***DNE";
        // Part 3 - Valley
        test_inputs[20] = "*43125678812ttnaAptMTSUOaodwcoIXknLypETZ**3 paymoremoney 17 17 5 21 18 21 2 2 19 0 15***3 RRLMWBKASPDH 17 17 5 21 18 21 2 2 19 3 8";
        test_inputs[21] = "*43125678812ttnaAptMTSUOaodwcoIXknLypETZ**3 paymoremoney 17 17 5 21 18 21 2 2 19 0 15***3 R RLMWBK ASP  DH  17 17 5 21 18   21 2 2 19 3  8 ";
        test_inputs[22] = "***3 WKAZAPBPPPCMADD  3 2 1 20 15 4 10 22 3**3 GoodMorningJohn 3 2 1 20 15 4 10 22 3*43125678812ttnaAptMTSUOaodwcoIXknLypETZ";
        test_inputs[23] = "**3 paymoremoney 17 17 5 21 18 21 2 2 19 0 15*** 2 RFRWYQ 5 8 17 3 4 9*31427781OAOSRSDKYPWIKSRO";

        // Prompt user to enter input string until a valid string is entered or attempts run out
        while (user_input_string.empty() && attempts < max_attempts) {
            printf("\n\nEnter your input string of three parts, or enter 'quit' to exit program:\n");
            attempts++;
            try {
                if (test_index < total_tests) {
                    user_input_string = test_inputs[test_index++];
                } else {
                    getline(cin, user_input_string);
                }
                if (IS_LOGGING) {
                    printf("Entered String: %s\n", user_input_string.c_str());
                }
            } catch (...) {
                pthread_exit((void *) 1); // Exit thread with unsuccessful 1 code on reading input error
            }

            if (user_input_string == "quit" || user_input_string == "exit") {
                pthread_exit((void *) nullptr); // Exit thread and return successful 0
            }

            is_valid_user_input = isValidInputString();

            // Give user another attempt to enter input, unless they exceed attempts
            if (!is_valid_user_input) {
                // If user ran out of input attempts, notify user and exit program
                if (attempts >= max_attempts) {
                    printf("You've run out of the maximum allowed attempts: %d"
                           "Please restart the program if you wish to try again.",
                           max_attempts);
                    pthread_exit((void *) 2); // Exit thread with unsuccessful 2 code
                } else {
                    printf("Invalid input. "
                           "Please enter a string made up of the three parts that make up the encoded message.\n");
                    user_input_string = ""; // Empty user input in preparation for next iteration
                }
            }
        }

        if (IS_LOGGING) {
            printf("User entered a valid input of: \"%s\" after %d attempts\n", user_input_string.c_str(), attempts);
        }

        // ------------------------------
        // Spawn Decoder Thread
        // ------------------------------
        pthread_t decoder_thread;

        ulong decoder_retval;

        // Booleans (represented as ints) returned from pthread methods
        int decoder_created;
        int decoder_joined;

        // Create decoder thread and pass the user input as an argument
        decoder_created = pthread_create(&decoder_thread, nullptr, decoder, nullptr);

        if (decoder_created == 0) {
            if (IS_LOGGING) {
                printf("Decoder thread spawned successfully.\n");
            }
        } else {
            printf("Error spawning decoder thread from shifter thread.\n");
            pthread_exit((void *) 1); // Exit thread with unsuccessful 1 code
        }

        // Join decoder thread with sifter thread. Waits for results of decoder thread
        decoder_joined = pthread_join(decoder_thread, (void **)&decoder_retval);

        // ------------------------------
        // Handle Results Passed Through From Decoder Thread
        // ------------------------------
        if (decoder_joined == 0) {
            // On successful join, log if logging is turned on
            if (IS_LOGGING) {
                printf("Decoder thread joined successfully.\n");
            }
            // If decoder is successful, or the user input is invalid, continue with next iteration
            else if (decoder_retval == 0 || decoder_retval == 2) {
                continue;
            }
            // If error occurred in join or in a decoder thread, exit thread and stop execution
            else if (decoder_retval == 1) {
                printf("Error occurred within decoder thread, or with joining decoder with sifter thread.\n");
                pthread_exit((void *) 1); // Exit thread with unsuccessful 1 code
            }
        // If error joining decoder thread, exit with error
        } else {
            printf("Error joining decoder thread with main module thread.\n");
            pthread_exit((void *) 1); // Exit thread with unsuccessful 1 code
        }
    }
}

/* Passed Part1, Part2, and Part3 into 3 separate threads named: fence, hill, and valley respectively.
 * The encoding schemes that each thread follows are detailed in assignment_description.pdf, a copy of
 * which can be found in the parent directory of the repository that this program belongs to.
 *
 * All Decoder threads, including this thread pass through their return value upon exit. Values returned from the
 * Decoder (this), Fence, Hill, and Valley threads are passed through and handled by the sifter thread. Before passing
 * the value through, each method handles its own output message, detailing the return if necessary (on input error).
 * Each code has the following return codes:
 *  0 = successful exit (all 3 thread decoded)
 *  1 = Generic error
 *  2 = Invalid user input string found by one of the decoder threads
 * */
void* decoder(void*) {
    /* Note to Dr. Hashemi: It made more sense to already split the thread into three parts during validation,
     * so rather than splitting again, this method will use the global variables containing each part to deliver them
     * to each thread */
    // Decoder threads are the fence, hill, and valley threads
    pthread_t decoder_threads[3];
    decoder_function threads_funcs[3] = {&fence, &hill, &valley};
    ulong decoder_retvals[3];
    int decoder_thread_created[3];
    int decoder_thread_joined[3];

    // For each decoder method, create a thread
    for (uint i = 0; i < 3; i++) {
        decoder_thread_created[i] = pthread_create(&decoder_threads[i], nullptr, threads_funcs[i], nullptr);
    }
    // After each thread has begun, wait for each to be completed with pthread_join
    for (uint i = 0; i < 3; i++) {
        if (decoder_thread_created[i] == 0) {
            decoder_thread_joined[i] = pthread_join(decoder_threads[i], (void **)&decoder_retvals[i]);

            // Split user input sent to a part was found to be invalid
            if (decoder_retvals[i] == 2) {
                if (i == 0) {
                    printf("Part1 of your string was found to be invalid by the Fence thread.\nDecoding stopped.\n");
                } else if (i == 1) {
                    printf("Part2 of your string was found to be invalid by the Hill thread.\nDecoding stopped.\n");
                } else if (i == 2) {
                    printf("Part3 of your string was found to be invalid by the Valley thread.\nDecoding stopped.\n");
                }
                pthread_exit((void *) 2); // Exit thread with unsuccessful 2 code: Invalid input
            }

            // If something went wrong with join, or with one of the decoder threads, exit thread
            if (decoder_thread_joined[i] != 0 || decoder_retvals[i] != 0) {
                pthread_exit((void *) 1); // Exit thread with unsuccessful 1 code
            }
        }
    }

    pthread_exit((void *) nullptr); // Exit thread with successful code
}

/* Takes part_one (stored as global) of the user input and decodes it using the Fence Thread scheme defined in
 * ./assignment_description.pdf
 *
 * Thread returns a uint code with the following meanings:
 *  0 = successful exit (thread decoded)
 *  1 = Generic error
 *  2 = Invalid user input
 * */
void* fence(void*) {
    if (IS_LOGGING) {
        printf("Fence(): Part1: %s\n", part_one.c_str());
    }

    string section_one; // Numeric Part
    string section_two; // Remaining characters after numeric part

    // Extract numeric part of part_one (all leading digits)
    cmatch numeric_part_match;
    regex_search(part_one.c_str(), numeric_part_match, numeric_start_regex);
    // Extract match
    section_one = numeric_part_match.str();
    // Remove whitespaces from match
    removeAllWhiteSpace(section_one);
    // Extract contents after match
    section_two = numeric_part_match.suffix();
    // Remove whitespaces from suffix
    removeAllWhiteSpace(section_two);

    // If there is no numeric part, exit thread
    if (section_one.length() <= 0) {
        printf("Fence(): Section1 is empty: %s\n", section_one.c_str());
        pthread_exit((void *) 2); // Exit thread with unsuccessful code, 2: Invalid user input
    } else {
        if (IS_LOGGING) {
            printf("Fence(): Part1: Section1: %s\n", section_one.c_str());
        }
    }

    // If there is no character part, exit thread
    if (section_two.length() <= 0) {
        printf("Fence(): Section2 is empty: %s\n", section_one.c_str());
        pthread_exit((void *) 2); // Exit thread with unsuccessful code, 2: Invalid user input
    } else {
        if (IS_LOGGING) {
            printf("Fence(): Part1: Section2: %s\n", section_two.c_str());
        }
    }

    // --------------------------------
    // Find Q and P
    // --------------------------------
    map<char, int> digitPositions; // Key value pair for each digit char mapped to its position in section_one
    char key;
    // Init digitPositions with -1 for each digit position
    for (char& digit : section_one) {
        digitPositions[digit] = -1;
    }
    // Find repeated digit
    int q = -1, p = -1;
    for (int i = 0; i < section_one.size(); ++i) {
        key = (char)section_one[i];
        // If digit has already been found
        if (digitPositions[key] != -1) {
            p = digitPositions[key]; // First occurrence of repeated digit
            q = i; // Second occurrence of repeated digit
            break;
        } else {
            digitPositions[key] = i; // Set position of digit
        }
    }
    // If no repeating digits, exit thread with input error code
    if (p == -1 || q == -1) {
        printf("Fence(): Section1 has no repeated digits: %s\n", section_one.c_str());
        pthread_exit((void *) 2); // Exit thread with unsuccessful code, 2: Invalid user input
    }

    // --------------------------------
    // Find and Validate k
    // --------------------------------
    // k will be all characters before consecutive duplicates or all characters before the 2nd nonconsecutive duplicate
    string k;
    if (q == p + 1) {
        k = section_one.substr(0, (ulong)p);
    } else {
        k = section_one.substr(0, (ulong)q);
    }
    long n = k.length();
    // Exit thread with input error if k is empty
    if (n <= 0) {
        printf("Fence(): k is empty.\n");
        pthread_exit((void *) 2); // Exit thread with unsuccessful code, 2: Invalid user input
    }
    // In order for k to be valid, it must consume all digits from 1 to N where N = length(k);
    for (int i = 1; i < n + 1; i++) {
        key = '0' + (char)i;
        if (k.find(key) == string::npos) {
            printf("Fence(): k doesn't have 1 to Length(k) digits: %s\n", k.c_str());
            pthread_exit((void *) 2); // Exit thread with unsuccessful code, 2: Invalid user input
        }
    }

    // --------------------------------
    // Find j and partition section_two
    // --------------------------------
    if (section_two.length() % n != 0) {
        printf("Fence(): length(section_two) is not divisible by length(k) (N): %s\n", k.c_str());
        pthread_exit((void *) 2); // Exit thread with unsuccessful code, 2: Invalid user input
    }
    long j = section_two.length() / n;
    // Partition section_two into n columns of j characters in each column (j rows)
    char section_two_matrix[j][n];
    int section_two_index = 0;
    for (int col = 0; col < n; col++) {
        for (int row = 0; row < j; row++) {
            section_two_matrix[row][col] = (char)section_two[section_two_index++];
        }
    }

    // --------------------------------
    // Rearrange Matrix and find plaintext
    // --------------------------------
    int new_col;
    // The new matrix will be the columns of the old section_two matrix arranged by the digits in k
    char rearranged_matrix[j][n];
    for (int old_col = 0; old_col < n; old_col++) {
        // Adjust for c indexing starting at 0 by - 1
        new_col = k[old_col] - '0' - 1;
        for (int row = 0; row < j; row++) {
            rearranged_matrix[row][old_col] = section_two_matrix[row][new_col];
        }
    }

    // Build plaintext string from matrix and store it in fence_thread_result global
    fence_thread_result = "";
    for (int row = 0; row < j; row++) {
        for (int col = 0; col < n; col++) {
            fence_thread_result += rearranged_matrix[row][col];
        }
    }

    if (IS_LOGGING) {
        printf("Fence(): Decoded Part 1: %s\n", fence_thread_result.c_str());
    }

    pthread_exit((void *) nullptr); // Successful exit
}

/* Takes part_two (stored as global) of the user input and decodes it using the Hill Thread scheme defined in
 * ./assignment_description.pdf
 *
 * Much of this method shares logic with the Valley thread and call a series of hillValley...() functions to parse
 * three sections from their respective part (part 2 for hill, part 3 for valley)
 * */
void* hill(void*) {
    string current_thread = "Hill";
    if (IS_LOGGING) {
        printf("Hill(): Part2: %s\n", part_two.c_str());
    }

    // --------------------------------
    // Extract Section 1 From part_two Using the Logic Shared With Valley Thread
    // --------------------------------
    int section_one_int, current_index; // Will be set in hillValleyGetTokenOne()
    // If the shared logic found that there was an error in the user input, exit the thread
    if (!hillValleyGetTokenOne(part_two, current_thread, current_index, section_one_int)) {
        pthread_exit((void *) 2); // Exit thread with unsuccessful code, 2: Invalid user input
    }

    // --------------------------------
    // Extract Section 2 From part_two
    // --------------------------------
    string section_two; // Will be set in hillValleyGetTokenTwo()
    if (!hillValleyGetTokenTwo(part_two, current_thread, current_index, section_one_int, section_two)) {
        pthread_exit((void *) 2);
    }

    // --------------------------------
    // Extract Section 3 From part_two
    // --------------------------------
    string section_three = part_two.substr((ulong)current_index, part_two.length() - (ulong)current_index);

    // --------------------------------
    // Generate Matrix K From Section 3
    // --------------------------------
    // Initialize the K Matrix of size nxn (square), where n is token 1 of Part 2
    auto ** K = (long**)malloc(sizeof(long*)*section_one_int);
    for (int i = 0 ; i < section_one_int; i++) {
        *(K + i) = (long *) malloc(sizeof(long) * section_one_int);
    }

    if (!hillValleyGenerateMatrixK(section_three, current_thread, section_one_int, K)) {
        pthread_exit((void *) 2);
    }

    // --------------------------------
    // Calculate C =(PK) mod (26) Where P = every set of 2 or 3 characters from section 2
    // --------------------------------
    long P[section_one_int]; // Matrix that holds 2 or 3 plaintext character number positions
    long C[section_one_int]; // Resultant matrix that holds 2 or 3 encoded characters at a time
    hill_thread_result = ""; // Will hold the ciphertext
    for (int i = 0; i < section_two.length(); i++) {
        P[i % section_one_int] = convertToAlphabetPosition(section_two[i]);
        // If three characters of section two have been extracted, calculate (PK) mod 26
        if ((i + 1) % section_one_int == 0) {
            // C = P * K
            oneByNTimesNbyNMatrix(section_one_int, P, K, C);
            // C = (P* K) mod (26)
            scalarModOnebyNMatrix(section_one_int, C, ALPHABET_SIZE);
            // Convert C back into characters and build the cipher string stored in global hill_thread_result variable
            for (long &alphabet_position : C) {
                hill_thread_result += convertFromAlphabetPosition(alphabet_position);
            }
        }
    }

    if (IS_LOGGING) {
        printf("Hill(): Encoded Part 2: %s\n", hill_thread_result.c_str());
    }

    pthread_exit((void *) nullptr); // Successful exit
}

/* Takes part_three (stored as global) of the user input and decodes it using the Valley Thread scheme defined in
 * ./assignment_description.pdf
 *
 * Much of this method shares logic with the Hill thread and call a series of hillValley...() functions to parse
 * three sections from their respective part (part 2 for hill, part 3 for valley)
 * */
void* valley(void*) {
    string current_thread = "Valley";
    if (IS_LOGGING) {
        printf("Valley(): Part3: %s\n", part_three.c_str());
    }

    // --------------------------------
    // Extract Section 1 From part_three Using the Logic Shared With Hill Thread
    // --------------------------------
    int section_one_int, current_index; // Will be set in hillValleyGetTokenOne()
    // If the shared logic found that there was an error in the user input, exit the thread
    if (!hillValleyGetTokenOne(part_three, current_thread, current_index, section_one_int)) {
        pthread_exit((void *) 2); // Exit thread with unsuccessful code, 2: Invalid user input
    }

    // --------------------------------
    // Extract Section 2 From part_three
    // --------------------------------
    string section_two; // Will be set in hillValleyGetTokenTwo()
    if (!hillValleyGetTokenTwo(part_three, current_thread, current_index, section_one_int, section_two)) {
        pthread_exit((void *) 2);
    }

    // --------------------------------
    // Extract Section 3 From part_three
    // --------------------------------
    string section_three = part_three.substr((ulong)current_index, part_three.length() - (ulong)current_index);

    // --------------------------------
    // Generate Matrix K^-1 From Section 3
    // --------------------------------
    // Initialize the K Matrix of size nxn (square), where n is token 1 of Part 2
    auto ** K = (long**)malloc(sizeof(long*)*section_one_int);
    for (int i = 0 ; i < section_one_int; i++) {
        *(K + i) = (long *) malloc(sizeof(long) * section_one_int);
    }

    if (!hillValleyGenerateMatrixK(section_three, current_thread, section_one_int, K)) {
        pthread_exit((void *) 2);
    }

    // --------------------------------
    // Invert matrix K if it is invertible
    // --------------------------------
    // Initialize K_inverse
    auto ** K_inverse = (long**)malloc(sizeof(long*)*section_one_int);
    for (int i = 0 ; i < section_one_int; i++) {
        *(K_inverse + i) = (long *) malloc(sizeof(long) * section_one_int);
    }

    long n; // The determinant of the original K matrix
    long z; // (det K) mod 26
    long h; // (det K)^-1 mod 26
    if (section_one_int == 3) {
        // n = Determinant of matrix K
        n = threeByThreeDeterminant(K);

        // Find n mod 26 (add 26 if negative)
        z = n % ALPHABET_SIZE;
        if (z < 0) {
            z += ALPHABET_SIZE;
        }

        // If relatively prime, find the multiplicative inverse of (z)^-1 mod 26. Returns false if not relatively prime
        h = findModularMultiplicativeInverse(z, ALPHABET_SIZE);

        // If not relatively prime, K^-1 mod 26 cannot be found, so exit thread
        if (h == -1) {
            printf("Valley(): Unable to find multiplicative inverse of K matrix.\n");
            pthread_exit((void *) 2);
        }

        // Initialize temp 2x2 sub_matrix holder
        auto ** sub_matrix = (long**)malloc(sizeof(long*)*2);
        for (int i = 0 ; i < 2; i++) {
            *(sub_matrix + i) = (long *) malloc(sizeof(long) * 2);
        }

        // Calculate K^-1 using [K^-1]sub_i,j = h(-1)^(i+j)(Dsub_j,i) mod m for i,j = 0 to token 1 (3 in this case)
        long h_sign;
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) {
                // Calculate the sign of h: (-1) ^ (i+j)
                if ((row + col) % 2 == 0) {
                    h_sign = 1;
                } else {
                    h_sign = -1;
                }

                // Get the 2x2 submatrix by ignoring the opposite of the current K_inverse row and col (swap them)
                findSubMatrix(3, col, row, K, sub_matrix);

                // Calculate h(-1)^(i+j)(Dsub_j,i) mod m and save the result in K_inverse matrix
                K_inverse[row][col] = (h_sign * h) * twoByTwoDeterminant(sub_matrix) % ALPHABET_SIZE;
                // If mod result is negative, add 26
                if (K_inverse[row][col] < 0) {
                    K_inverse[row][col] += ALPHABET_SIZE;
                }
            }
        }

    // When token 1 is 2, and matrix k is a 2x2
    } else {
        // n = Determinant of matrix K
        n = twoByTwoDeterminant(K);

        // Find n mod 26 (add 26 if negative)
        z = n % ALPHABET_SIZE;
        if (z < 0) {
            z += ALPHABET_SIZE;
        }

        // If relatively prime, find the multiplicative inverse of (z)^-1 mod 26. Returns false if not relatively prime
        h = findModularMultiplicativeInverse(z, ALPHABET_SIZE);

        // If not relatively prime, K^-1 mod 26 cannot be found, so exit thread
        if (h == -1) {
            printf("Valley(): Unable to find multiplicative inverse of K matrix.\n");
            pthread_exit((void *) 2);
        }

        // Calculate K^-1 using [K^-1]sub_i,j = h(-1)^(i+j)(Dsub_j,i) mod m for i,j = 0 to token 1 (3 in this case)
        long h_sign;
        for (int row = 0; row < 2; row++) {
            for (int col = 0; col < 2; col++) {
                // Calculate the sign of h: (-1) ^ (i+j)
                if ((row + col) % 2 == 0) {
                    h_sign = 1;
                } else {
                    h_sign = -1;
                }

                // Calculate h(-1)^(i+j)(det(K)) mod m and save the result in K_inverse matrix
                K_inverse[row][col] = ((h_sign * h) * n) % ALPHABET_SIZE;
                // If mod result is negative, add 26
                if (K_inverse[row][col] < 0) {
                    K_inverse[row][col] += ALPHABET_SIZE;
                }
            }
        }
    }

    // --------------------------------
    // Calculate P =(C(K^-1)) mod (26) Where C = every set of 2 or 3 characters from section 2
    // --------------------------------
    long P[section_one_int]; // Matrix that holds 2 or 3 plaintext character number positions
    long C[section_one_int]; // Resultant matrix that holds 2 or 3 encoded characters at a time
    valley_thread_result = ""; // Will hold the ciphertext
    for (int i = 0; i < section_two.length(); i++) {
        P[i % section_one_int] = convertToAlphabetPosition(section_two[i]);
        // If three characters of section two have been extracted, calculate (P(K^-1)) mod 26
        if ((i + 1) % section_one_int == 0) {
            // C = P * K
            oneByNTimesNbyNMatrix(section_one_int, P, K_inverse, C);

            // C = (P* K^-1) mod (26)
            scalarModOnebyNMatrix(section_one_int, C, ALPHABET_SIZE);

            // Convert C back into characters and build the cipher string stored in global hill_thread_result variable
            for (long &alphabet_position : C) {
                valley_thread_result += convertFromAlphabetPosition(alphabet_position);
            }
        }
    }

    if (IS_LOGGING) {
        printf("Valley(): Decoded Part 3: %s\n", valley_thread_result.c_str());
    }

    pthread_exit((void *) nullptr); // Successful exit
}

// ==============================
// Share Hill and Valley Logic Methods
// ==============================
/* Extracts section 1 of part_X for both hill and valley thread. */
bool hillValleyGetTokenOne(string &part, string &calling_thread, int &current_index, int &section_one_int) {
    string section_one = "0";
    // Find the first token (after whitespace) that is either a 2 or a 3
    for (int i = 0; i < part.length(); i++) {
        if (!isspace(part[i])) {
            // If first token isn't a 2 or 3, exit thread
            if (part[i] == '2' || part[i] == '3') {
                section_one = part[i];
                current_index = i + 1;
                break;
            } else {
                printf("%s(): The first token is not a 2 or a 3: %c\n", calling_thread.c_str(), part[i]);
                return false; // Return false to exit thread with invalid user input code
            }
        }
    }

    // Failsafe: Should never be reached with new validation checks
    // If input was all whitespace, exit thread with input error code
    if (section_one == "0") {
        printf("%s(): Current part is all whitespace. %s\n", calling_thread.c_str(), part.c_str());
        return false; // Return false to exit thread with invalid user input code
    }

    // Set character of digit as integer
    section_one_int = section_one[0] - '0';

    return true; // If section 1 is valid, returns true with values for current_index and section_one_int set correctly
}

/* Extracts section 2 of part_X for both hill and valley thread. */
bool hillValleyGetTokenTwo(string &part, string &calling_thread, int &current_index, int section_one_int, string &section_two) {
    // Check that there is a character after the first token, and it isn't another digit
    if (current_index < part.length() && !isdigit(part[current_index])) {
        // Continue iteration from where token 1 ended to find the second token
        for (current_index; current_index < part.length(); current_index++) {
            // Continue until a non-alpha or non-space character is found
            if (isalpha(part[current_index]) || isspace(part[current_index])) {
                // Extract token 2
                section_two += part[current_index];
            } else {
                break;
            }
        }
    } else {
        printf("%s(): current part doesn't have a second token or token 1 is more than 1 char. %s\n", calling_thread.c_str(), part.c_str());
        return false; // Return false to exit thread with invalid user input code
    }

    // Remove whitespace from section 2
    removeAllWhiteSpace(section_two);

    // If no token 2 (alphabetic chars) were found, exit thread with invalid user input code
    if (section_two.empty()) {
        printf("%s(): current part doesn't have only alphabetic chars for section 2. %s\n",
               calling_thread.c_str(), part.c_str());
        return false; // Return false to exit thread with invalid user input code
    }
    // If the length of token 2 is not a multiple of token 1, exit thread with invalid user input code
    if (section_two.length() % section_one_int != 0) {
        printf("%s(): length(section 2): %lu is not a multiple of token 1: %d.\n",
               calling_thread.c_str(), section_two.length(), section_one_int);
        return false; // Return false to exit thread with invalid user input code
    }
    // If section 2 is valid, returns true with value of token 2 stored in section_two, and current_index is updated
    return true;
}

/* Generates matrix K given section_three of part_X for both hill and valley thread. */
bool hillValleyGenerateMatrixK(string &section_three, string &calling_thread, int section_one_int, long** K) {
    regex e("\\s+");
    regex_token_iterator<string::iterator> token_iterator(section_three.begin(), section_three.end(), e , -1);
    regex_token_iterator<string::iterator> end;
    int row = 0, col = 0, number_of_tokens = 0;

    while (token_iterator != end) {
        // Get current token string from iterator
        string current_token = (string)*token_iterator++;

        // Check that the token is a digit (only numerical characters) and not alphabetic
        for (char& character : current_token) {
            if (!isdigit(character)) {
                printf("%s(): invalid section 3: All tokens must be numeric characters: %s.\n",
                       calling_thread.c_str(), section_three.c_str());
                return false; // Return false to exit thread with invalid user input code
            }
        }

        // Keep track of current row and column index of the K matrix
        if (col > section_one_int - 1) {
            col = 0;
            row++;
        }

        // If matrix is filled even if there are more tokens, exit loop
        if (row >= section_one_int) {
            break;
        }

        // Convert each digit token in section3 into a number and place it into the K matrix
        try {
            K[row][col++] = stoi(current_token);
        } catch (...) {
            printf("%s(): invalid section 3: Non-digit characters in third section: %s.\n",
                   calling_thread.c_str(), section_three.c_str());
            return false; // Return false to exit thread with invalid user input code
        }
        // Keep track of the number of tokens added to section 3
        number_of_tokens++;
    }

    // Check that the number of tokens is >= 4 or >= 9 for when token1 is 2 and 3, respectively
    if (section_one_int == 2 && number_of_tokens < 4) {
        printf("%s(): invalid section 3: Token 1 is 2, but the number of characters is less than 4: %s.\n",
               calling_thread.c_str(), section_three.c_str());
        return false; // Return false to exit thread with invalid user input code
    }
    if (section_one_int == 3 && number_of_tokens < 9) {
        printf("%s(): invalid section 3: Token 1 is 3, but the number of characters is less than 9: %s.\n",
               calling_thread.c_str(), section_three.c_str());
        return false; // Return false to exit thread with invalid user input code
    }

    return true;
}

// ==============================
// Validation Methods
// ==============================
/* Returns true if passed in user_input_string is in three parts specified by a number of asterisk symbols (6 total)
 *
 * There are three phases of validation:
 *  First Validation: There are at least one of each *, **, and *** in the user input string
 *  Second Validation: There are only one of each *, **, and *** in the use input string
 *  Third Validation: Each part is non-null
 * */
bool isValidInputString() {
    // ------------------------
    // Use regex to check that the user input string contains at least 1 of each "*", "**", and "***".
    // ------------------------
    // First Validation: If string does contain one of each section
    if (regex_match(user_input_string, asterisk_regex)) {
        if (IS_LOGGING) {
            printf("First validation test passed: There are at least one of each: *, **, and *** in input string.\n");
        }
    } else {
        if (IS_LOGGING) {
            printf("First validation test failed: There are not at least one of each: *, **, and *** in input string.\n");
        }
        return false;
    }

    // Second Validation: Verify that there is only one occurrence of each *, **, and *** (for a total of 6)
    if (count(user_input_string.begin(), user_input_string.end(), '*') != 6) {
        if (IS_LOGGING) {
            printf("Second validation Failed: There are not only one of each: *, **, and ***.\n");
        }
        return false;
    } else {
        if (IS_LOGGING) {
            printf("Second validation test passed: There are only one of each: *, **, and *** in input string.\n");
        }
    }

    // Verify that splitting the string into three parts gives 3 non-null parts
    splitIntoParts();

    // Third Validation: Verify that each part is non-null (has elements and those elements aren't just whitespace)
    if (part_one.length() <= 0 || isOnlyWhitespace(part_one) ||
        part_two.length() <= 0 || isOnlyWhitespace(part_two) ||
        part_three.length() <= 0 || isOnlyWhitespace(part_three)) {
        if (IS_LOGGING) {
            printf("Third validation test failed: There are null values for at least one part.\n");
            return false;
        }
    } else {
        if (IS_LOGGING) {
            printf("Third validation test passed: Each part is non-null.\n");
        }
    }

    // If all validation tests pass, return true
    return true;
}

// ==============================
// Matrix Manipulation Methods
// ==============================
/* Multiples matrices with dimensions: 1xn * nxn and stores the 1xn results in result_matrix[] */
void oneByNTimesNbyNMatrix(long size, long one_by_n[], long **n_by_n, long result_matrix[]) {
    long sum;
    // Loop through columns of nxn matrix (i)
    for (int i = 0; i < size; i++) {
        sum = 0;
        // Loop through columns of 1xn matrix (j)
        for (int j = 0; j < size; j++) {
            // Sum the product of each column in the 1xn matrix with the current column of the nxn matrix
            sum += one_by_n[j] * n_by_n[j][i];
        }
        result_matrix[i] = sum;
    }
}
/* Performs the operation matrix mod (mod_factor), where the matrix is 1xsize */
void scalarModOnebyNMatrix(long size, long matrix[], long mod_factor) {
    for (int col = 0; col < size; col++) {
        matrix[col] = matrix[col] % mod_factor;
        if (matrix[col] < 0) {
            matrix[col] =+ mod_factor;
        }
    }
}

/* Returns the 3x3 determinant of the passed in matrix */
long threeByThreeDeterminant(long **matrix) {
    // Initialize temp 2x2 sub_matrix holder
    auto ** sub_matrix = (long**)malloc(sizeof(long*)*2);
    for (int i = 0 ; i < 2; i++) {
        *(sub_matrix + i) = (long *) malloc(sizeof(long) * 2);
    }

    long sub_determinant[3];
    long current_coefficient;
    for (int col = 0; col < 3; col++) {
        current_coefficient = matrix[0][col]; // Get each element on first row to scalar multiply out 2x2 sub matrices
        findSubMatrix(3, 0, col, matrix, sub_matrix); // Get the 2x2 submatrix by ignoring current column
        sub_determinant[col] = twoByTwoDeterminant(sub_matrix) * current_coefficient; // Find sub determinant * coeff
    }

    // Finish calculations of 3x3 determinant by combining sub determinants
    return sub_determinant[0] - sub_determinant[1] + sub_determinant[2];
}

/* Returns the integer 2x2 determinant of a matrix */
long twoByTwoDeterminant(long **matrix) {
    return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
}

/* Accepts a sizexsize matrix and builds size-1xsize-1 sub_matrix from passed in larger matrix.
 * Buuild sub-matrix as if we were to cross out a row and col (rows/cols_to_ignore) from the larger matrix */
void findSubMatrix(long size, long row_to_ignore, long col_to_ignore, long** matrix,  long** sub_matrix) {
    // Copy non-ignored rows and cols of matrix into sub_matrix
    int sub_row = 0;
    int sub_col = 0;
    for (int row = 0; row < size; row++) {
        // Skip copying over row_to_ignore when building matrix
        if (row == row_to_ignore) {
            continue;
        }

        for (int col = 0; col < size; col++) {
            // Skip col_to_ignore when building matrix
            if (col == col_to_ignore) {
                continue;
            }
            sub_matrix[sub_row][sub_col++] = matrix[row][col];
        }
        // Reset sub matrix cols, go to next sub matrix row
        sub_col = 0;
        sub_row++;
    }
}


// ==============================
// Input Part Handling Methods
// ==============================
/* Splits the input string into three parts where Part1, Part2, and Part3 are started by *, **, and *** respectively
 * The results of the split are saved in the global variables part_one, part_two, and part_three. */
void splitIntoParts() {
    ulong part_one_length, part_two_length, part_three_length = 0;

    // Find the index of the only occurrence of part 3's *** asterisks
    ulong three_asterisk_index = user_input_string.find("***");

    // Attempt to find the index of part 2's two asterisks
    ulong two_asterisk_index = user_input_string.find("**");
    // If the ** asterisks are registered as the first 2 asterisks of ***, search from an index after ***
    if (two_asterisk_index == three_asterisk_index) {
        two_asterisk_index = user_input_string.find("**", three_asterisk_index + 2);
    }

    // Attempt to find the index of part 3's single, * asterisks
    ulong one_asterisk_index = user_input_string.find('*');
    ulong temp_index = one_asterisk_index;
    // While the index of the single * asterisk is picking up the index of ** or *** asterisks, continue searching
    while (one_asterisk_index == two_asterisk_index ||
           one_asterisk_index == two_asterisk_index + 1 ||
           one_asterisk_index == three_asterisk_index ||
           one_asterisk_index == three_asterisk_index + 1 ||
           one_asterisk_index == three_asterisk_index + 2) {
        one_asterisk_index = user_input_string.find('*', temp_index++);
    }

    // Get the length of each part using the indexes of each asterisk
    part_one_length = findPartLength(one_asterisk_index, two_asterisk_index, three_asterisk_index);
    // Extract the part substring from after each part-identifying asterisk to the adjusted length of the part
    part_one = user_input_string.substr(one_asterisk_index + 1, part_one_length - 1);

    // Repeat the same procedures for getting the part_one split for part 2 and 3
    part_two_length = findPartLength(two_asterisk_index, one_asterisk_index, three_asterisk_index);
    part_two = user_input_string.substr(two_asterisk_index + 2, part_two_length - 2);

    part_three_length = findPartLength(three_asterisk_index, one_asterisk_index, two_asterisk_index);
    part_three = user_input_string.substr(three_asterisk_index + 3, part_three_length - 3);
}

/* Takes in the indexes of the three asterisks, with the first argument "desired_index" being the asterisks for whose
 * part length we wish to determine and return. Returns the part length from the identifying-asterisk to the beginning
 * of the next asterisk or the end of the input string */
ulong findPartLength(ulong desired_index, ulong other_index_1, ulong other_index_2) {
    ulong part_length = 0;

    // If the '*', '**', or '***' in question is the last of the three parts
    if (maximum(desired_index, other_index_1, other_index_2) == desired_index) {
        part_length = user_input_string.length() - desired_index;

        // If the '*', '**', or '***' in question is the second of the three parts
    } else if (median(desired_index, other_index_1, other_index_2) == desired_index) {
        part_length = max(other_index_1, other_index_2) - desired_index;
    }

        // If the '*', '**', or '***' in question is the first of the three parts
    else if (minimum(desired_index, other_index_1, other_index_2) == desired_index) {
        part_length = min(other_index_1, other_index_2) - desired_index;
    }

        // Failsafe that should never be reached
    else {
        printf("Something went terribly wrong in findPartLength(). Please contact admin.");
        exit(1);
    }

    return part_length;
}



// ==============================
// Helper/Utility Methods
// ==============================
/* Returns the maximum value of 3 values */
ulong maximum(ulong a, ulong b, ulong c) {
    return max(max(a, b), max(b, c));
}

/* Returns the middle value of 3 values */
ulong median(ulong a, ulong b, ulong c) {
    return max(min(a, b), min(max(a, b), c));
}

/* Returns the minimum value of 3 values */
ulong minimum(ulong a, ulong b, ulong c) {
    return min(min(a, b), min(b, c));
}

/* Removes all whitespace from a pass-by-reference string */
void removeAllWhiteSpace(string &input_str) {
    input_str = regex_replace(input_str, regex("\\s+"), "");
}

/* Removes all left (leading) whitespace */
void removeLeadingWhitespace(string &input_str) {
    while(!input_str.empty() && isspace(*input_str.begin()))
        input_str.erase(input_str.begin());
}


/* Removes all right (trailing) whitespace */
void removeTrailingWhitespace(string &input_str) {
    while(!input_str.empty() && isspace(*input_str.rbegin()))
        input_str.erase(input_str.length() - 1);
}

/* Returns true if a string only consists of whitespace, false otherwise */
bool isOnlyWhitespace(string input_str) {
    for (char& c : input_str) {
        if (!isspace(c)) {
            return false;
        }
    }

    return true;
}

/* Converts a character into its alphabetic position number. Where A = 0, B = 1, ... Z = 25. Case insensitive. */
long convertToAlphabetPosition(char character) {
    if (islower(character)) {
        return character - 'a';
    }
    return character - 'A';
}


/* Converts a character from its alphabetic position number to the character that position.
 * Where 0 = A, 1 = B, ... 25 = Z. Case insensitive. */
char convertFromAlphabetPosition(long position_number) {
    // If lower case, start with ASCII character lowercase a (97)
    if (position_number >= 97 && position_number <= 122) {
        return char(97 + position_number);
    }
    return (char)(65 + position_number);
}

/* Returns the modular multiplicative inverse of b^-1 mod (n).
 * Loops through all possible x < n, until (b * x) mod n = 1 is found */
long findModularMultiplicativeInverse(long b, long n) {
    if (gcd(b, n) != 1) {
        return -1;
    }

    b = b % n;

    // Try every x < n until one is found
    for (int x = 1; x < n; x++) {
        // If (b * x) mod n = 1, then x is the multiplicative inverse
        if ((b * x) % n == 1) {
            return x;
        }
    }

    // If no multiplicative inverse is found, return -1
    return -1;
}

/* Returns the gcd of two integer numbers */
long gcd(long a, long b) {
    return b == 0 ? a : gcd(b, a % b);
}

// ==================
// Debug Methods
// ==================
void print2dMatrix(long size, long **matrix) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%ld ", matrix[i][j]);
        }
        printf("\n");
    }
}

void print1dMatrix(long size, long *matrix) {
    printf("[");
    for (int i = 0; i < size; i++) {
        printf("%ld ", matrix[i]);
    }
    printf("]\n");
}



