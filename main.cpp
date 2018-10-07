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
// Method Signatures and Globals
// ==============================
// Thread Methods
void *sifter(void *);
void *decoder(void *);
void* fence(void*);
void* hill(void*);
void* valley(void*);
// Validation Methods
bool isValidCipher();
// Helper/Utility Methods
void splitIntoParts();
ulong findPartLength(ulong desired_index, ulong other_index_1, ulong other_index_2);
ulong maximum(ulong a, ulong b, ulong c);
ulong median(ulong a, ulong b, ulong c);
ulong minimum(ulong a, ulong b, ulong c);

// Globals
bool is_logging = true; // True to turn on console logging for debugging purposes
string cipher_string, part_one, part_two, part_three;

// ==============================
// Main Method
// ==============================
/* Entry point. Spawns sifter thread */
int main() {
    pthread_t sifter_thread;
    void *sifter_retval;

    // Booleans (represented as ints) returned from pthread methods
    int sifter_created;
    int sifter_joined;

    // Returns 0 if sifter thread has been created successfully, otherwise 1 is returned upon creation failure
    sifter_created = pthread_create(&sifter_thread, nullptr, sifter, nullptr);

    // If sifter thread is created successfully
    if (sifter_created == 0) {
        if (is_logging) {
            printf("Sifter thread spawned successfully.\n");
        }
    } else if (sifter_created == 1) {
        printf("Error spawning sifter thread from main module thread.\n");
        std::exit(1); // Exit with unsuccessful error code
    }

    // Join sifter thread with parent thread. Waits for results of sifter thread before continuing sequential execution
    sifter_joined = pthread_join(sifter_thread, &sifter_retval);

    // If sifter join is successful
    if (sifter_joined == 0) {
        if (is_logging) {
            printf("Sifter joined successfully.\n");
        }

        // Handle sifter thread return (0 = exit successfully)
        if ((long)sifter_retval == 0) {
            printf("Program quiting...");
            exit(0); // Exit with success code 0
        // 1 = uncaught error
        } else if ((long)sifter_retval == 1) {
            printf("Something went wrong during program execution. "
                   "Please contact an admin with details of what you were doing at the time of crash.");
            exit(1); // Exit with error code 1
        // 2 = user exceed max input attempts
        } else if ((long)sifter_retval == 2) {
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
 * Thread returns a uint code with the following meanings:
 *   0 = successful exit (user enters quit command)
 *   1 = uncaught error
 *   2 = user ran out of input attempts
 * */
void* sifter(void *) {
    uint test_index = 0;

    while (true) {
        cipher_string = ""; // Reset cipher_string to an empty string (for multiple runs of program)
        // ------------------------------
        // Get a Valid Cipher String From User
        // ------------------------------
        bool is_valid_cipher;
        int attempts = 0;
        // TODO: Change back into 3 before deploying
        int max_attempts = 13;

        // TEMP
        uint total_tests = 16;
        string* test_inputs = new string[total_tests];
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
        test_inputs[13] = "**ABC 5 6 7 8 10 1 4 8 11*K SAAPXGOW**MKPW 10 2 12 1*J RZZOWFNV"; // Invalid
        test_inputs[14] = "***ABC 5 6 7 8 10 1 4 8 11* **J RZZOWFNV"; // Invalid?
        test_inputs[15] = "***ABC 5 6 7 8 10 1 4 8 11* t3w **"; // Invalid?




        // Prompt user to enter input cipher string until a valid string is entered or attempts run out
        while (cipher_string.empty() && attempts < max_attempts) {
            printf("Enter your cipher string of three parts, or enter 'quit' to exit program:\n");
            attempts++;
            try {
                if (test_index < total_tests) {
                    cipher_string = test_inputs[test_index++];
                } else {
                    getline(cin, cipher_string);
                }
                if (is_logging) {
                    printf("Entered String: \n%s\n", cipher_string.c_str());
                }
            } catch (...) {
                pthread_exit((void *) 1); // Exit with unsuccessful 1 code on reading input error
            }

            if (cipher_string == "quit" || cipher_string == "exit") {
                pthread_exit((void *) 0);  // Exit thread and return successful 0
            }

            is_valid_cipher = isValidCipher();

            // Give user another attempt to enter input, unless they exceed attempts
            if (!is_valid_cipher) {
                // If user ran out of input attempts, notify user and exit program
                if (attempts >= max_attempts) {
                    printf("\nYou've run out of the maximum allowed attempts: %d"
                           "\nPlease restart the program if you wish to try again.",
                           max_attempts);
                    pthread_exit((void *) 2); // Exit with unsuccessful 2 code
                } else {
                    printf("\nInvalid input. "
                           "Please enter a string made up of the three parts that make up the encoded message.\n");
                    cipher_string = ""; // Empty user input in preparation for next iteration
                }
            }
        }

        if (is_logging) {
            printf("\nUser entered a valid cipher of: \"%s\" after %d attempts\n", cipher_string.c_str(), attempts);
        }

        // ------------------------------
        // Spawn Decoder Thread
        // ------------------------------
        pthread_t decoder_thread;

        char *decoder_retval;

        // Booleans (represented as ints) returned from pthread methods
        int decoder_created;
        int decoder_joined;

        // Create decoder thread and pass the input cipher as an argument
        decoder_created = pthread_create(&decoder_thread, nullptr, decoder, nullptr);

        if (decoder_created == 0) {
            if (is_logging) {
                printf("\nDecoder thread spawned successfully.\n");
            }
        } else {
            printf("Error spawning decoder thread from shifter thread.\n");
            std::exit(1); // Exit with unsuccessful error code
        }

        // Join decoder thread with sifter thread. Waits for results of decoder thread
        decoder_joined = pthread_join(decoder_thread, (void **) &decoder_retval);

        if (decoder_joined == 0) {
            if (is_logging) {
                printf("Decoder thread joined successfully.\n");
                printf("%s\n", decoder_retval); // Print return value from decoder thread
            }
        } else {
            printf("Error joining decoder thread with sifter thread.\n");
            std::exit(1);  // Exit with unsuccessful error code
        }
    }
}

/* Passed Part1, Part2, and Part3 into 3 separate threads named: fence, hill, and valley respectively.
 * The encoding schemes that each thread follows are detailed in assignment_description.pdf, a copy of
 * which can be found in the parent directory of the repository that this program belongs to. */
void* decoder(void*) {
    /* Note to Dr. Hashemi: It made more sense to already split the thread into three parts during validation,
     * so rather than splitting again, this method will use the global variables containing each part to deliver them
     * to each thread */


    pthread_exit((void *)"Decoder Completed.");
}

/* Takes part_one (stored as global) of the ciphertext input and decodes it using the Fence Thread scheme defined in
 * ./assignment_description.pdf */
void* fence(void*) {

}

/* Takes part_two (stored as global) of the ciphertext input and decodes it using the Hill Thread scheme defined in
 * ./assignment_description.pdf */
void* hill(void*) {

}

/* Takes part_two (stored as global) of the ciphertext input and decodes it using the Valley Thread scheme defined in
 * ./assignment_description.pdf */
void* valley(void*) {

}


// ==============================
// Validation Methods
// ==============================
/* Returns true if passed in cipher_string is in three parts specified by a number of asterisk symbols (6 total)
 *
 * There are three phases of validation:
 *  First Validation: There are at least one of each *, **, and *** in the cipher string
 *  Second Validation: There are only one of each *, **, and *** in the cipher string
 *  Third Validation: Each part is non-null
 * */
bool isValidCipher() {
    // Use regex to check that the cipher string contains at least 1 of each "*", "**", and "***".
    regex asterisk_regex("(?=.*?(^|[^\\*])\\*($|[^\\*]))" // Look ahead for '*' not followed/preceded by '*'
                         "(?=.*?(^|[^\\*])\\*\\*($|[^\\*]))" // Look ahead for '**' not followed/preceded by '**'
                         "(?=.*?(^|[^\\*])\\*\\*\\*($|[^\\*]))"  // Look ahead for '***' not followed/preceded by '***'
                         ".*"); // All look-ahead's also account for start (^) or end ($) of string

    // First Validation: If string does contain one of each section
    if (regex_match(cipher_string, asterisk_regex)) {
        if (is_logging) {
            printf("First validation test passed: There are at least one of each: *, **, and *** in cipher string.\n");
        }
    } else {
        if (is_logging) {
            printf("First validation test failed: There are not at least one of each: *, **, and *** in cipher string.\n");
        }
        return false;
    }

    // Second Validation: Verify that there is only one occurrence of each *, **, and *** (for a total of 6)
    if (std::count(cipher_string.begin(), cipher_string.end(), '*') != 6) {
        if (is_logging) {
            printf("Second validation Failed: There are not only one of each: *, **, and ***.\n");
        }
        return false;
    } else {
        if (is_logging) {
            printf("Second validation test passed: There are only one of each: *, **, and *** in cipher string.\n");
        }
    }

    // Verify that splitting the string into three parts gives 3 non-null parts
    splitIntoParts();

    if (is_logging) {
        printf("Part1: %s\n", part_one.c_str());
        printf("Part2: %s\n", part_two.c_str());
        printf("Part3: %s\n", part_three.c_str());
    }

    // Third Validation: Verify that each part is non-null
    if (part_one.length() <= 0 || part_two.length() <= 0 || part_three.length() <= 0) {
        if (is_logging) {
            printf("Third validation test failed: There are null values for at least one part.\n");
            return false;
        }
    } else {
        if (is_logging) {
            printf("Third validation test passed: Each part is non-null.\n");
        }
    }

    // If all validation tests pass, return true
    return true;
}


// ==============================
// Helper/Utility Methods
// ==============================
/* Splits the ciphertext into three parts where Part1, Part2, and Part3 are started by *, **, and *** respectively
 * The results of the split are saved in the global variables part_one, part_two, and part_three. */
void splitIntoParts() {
    ulong part_one_length, part_two_length, part_three_length = 0;

    // Find the index of the only occurrence of part 3's *** asterisks
    ulong three_asterisk_index = cipher_string.find("***");

    // Attempt to find the index of part 2's two asterisks
    ulong two_asterisk_index = cipher_string.find("**");
    // If the ** asterisks are registered as the first 2 asterisks of ***, search from an index after ***
    if (two_asterisk_index == three_asterisk_index) {
        two_asterisk_index = cipher_string.find("**", three_asterisk_index + 2);
    }

    // Attempt to find the index of part 3's single, * asterisks
    ulong one_asterisk_index = cipher_string.find('*');
    ulong temp_index = one_asterisk_index;
    // While the index of the single * asterisk is picking up the index of ** or *** asterisks, continue searching
    while (one_asterisk_index == two_asterisk_index ||
           one_asterisk_index == two_asterisk_index + 1 ||
           one_asterisk_index == three_asterisk_index ||
           one_asterisk_index == three_asterisk_index + 1 ||
           one_asterisk_index == three_asterisk_index + 2) {
        one_asterisk_index = cipher_string.find('*', temp_index++);
    }

    // Get the length of each part using the indexes of each asterisk
    part_one_length = findPartLength(one_asterisk_index, two_asterisk_index, three_asterisk_index);
    // Extract the part substring from after each part-identifying asterisk to the adjusted length of the part
    part_one = cipher_string.substr(one_asterisk_index + 1, part_one_length - 1);

    // Repeat the same procedures for getting the part_one split for part 2 and 3
    part_two_length = findPartLength(two_asterisk_index, one_asterisk_index, three_asterisk_index);
    part_two = cipher_string.substr(two_asterisk_index + 2, part_two_length - 2);

    part_three_length = findPartLength(three_asterisk_index, one_asterisk_index, two_asterisk_index);
    part_three = cipher_string.substr(three_asterisk_index + 3, part_three_length - 3);
}

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

/* Takes in the indexes of the three asterisks, with the first argument "desired_index" being the asterisks for whose
 * part length we wish to determine and return. Returns the part length from the identifying-asterisk to the beginning
 * of the next asterisk or the end of the ciphertext */
ulong findPartLength(ulong desired_index, ulong other_index_1, ulong other_index_2) {
    ulong part_length = 0;

    // If the '*', '**', or '***' in question is the last of the three parts
    if (maximum(desired_index, other_index_1, other_index_2) == desired_index) {
        part_length = cipher_string.length() - desired_index;

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