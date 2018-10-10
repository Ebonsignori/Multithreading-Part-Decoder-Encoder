# Multi-Threading Decoder Using pthreads
Reads in user-entered ciphertext string consisting of three parts: `Part1`, `Part2`, and `Part3` and prints out the decoded plaintext.

Each parts is each fed into the `Fence`, `Hill`, and `Valley` threads respectively, where they are concurrently decoded.

The specific decoding scheme for each of these three methods can be found in the [Assignment Description](./assignment_description.pdf).

Though the readability of this program would benefit if it were modularized across files, 
the program is instead in one large file: [main.cpp](./main.cpp), as instructed. 

## Sample Inputs
### Hashemi Assigned Inputs
```
***3 rrlmwbkaspdh 17 17 5 21 18 21 2 2 19 12 *123555eu5eotsya**3 GoodMorningJohn 3 2 1 20 15 4 10 22 3

**2 Global Warming 12 4 5 23 18 13 4 *** 2 RFRWYQ 5 8 17 3 4 9*31427781OAOSRSDKYPWIKSRO

**ACDUJF 1 4 6 12***MNKLHY 1 2 3 4 5 6 7 8 9*M FGCV

** KMLN 12 3 4 17 *** GHL 9 9 8 8 7 7 6 a6 5 * P TRBKHYT

*B PHHW PH DIVHU WKH WRJD SDUWB

*2 MNKK LLSGQW

**GG CI MOQN 9 2 1 15 ***ST TNJVQLAIVDIG 21 20 24 21 24 9 -5 21 21

** XNPD 5 8 17 3 ***KL LNSHDLEWMTRW 4 9 15 15 17 6 24 0 17*D GJFZNKZQDITSJ

**GGMLN 12 3 4 b7 *** GHL 9 9 8 8 7 7 6  16  5 * P GRBKHYT

** BBLN 12 3 4 17 *** GHL 9 9 8 8 7 7 6  6  5 * CRKLMNBKHYT

** MWUULNGXAP 12 15 7 6*** GHYTRKL MNP 1 0 0 -1 2 3 12*CDM UYKOOKPIRQQN

*zg ZHCHUUHOPXKPYAF***GHEDQHQQ 9 11 4 5 ** ABHJUTVOP 5 -2 3 4 12 32 1 0 3

***ABC 5 6 7 8 10 1 4 8 11*K SAAPXGOW**MKPW 10 2 12 1*J RZZOWFNV
```

### C++ Hardcoded String Array of Some Sample Inputs
```cpp
// TEMP
uint total_tests = 27;
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

// Custom Tests
// Invalid Input Tests
test_inputs[13] = "**DNE*DNE**DNE*DNE"; // Too many * - Invalid
test_inputs[14] = "***DNE* **DNE"; // Null part 1 - Invalid
test_inputs[15] = "***DNE*DNE t3w **"; // Null part 2 - Invalid

// Part 1 - Fence
test_inputs[16] = "*43125678812ttnaAptMTSUOaodwcoIXknLypETZ**DNE***DNE"; // Valid
test_inputs[17] = "*93456312ttnaAptMTSUOaodwcoIXknLypETZ**DNE***DNE"; // Invalid

// Part 2 - Hill
test_inputs[18] = "*43125678812ttnaAptMTSUOaodwcoIXknLypETZ**3 paymoremoney 17 17 5 21 18 21 2 2 19 0 15***DNE";
test_inputs[19] = "*43125678812ttnaAptMTSUOaodwcoIXknLypETZ**3  p ay more money  17 17 5 21   18 21 2 2   19 0 15  ***DNE";
test_inputs[20] = "*DNE**2 poopityscoop 6 7 21 1 6 6 6 1337 999 12345678910 ***DNE";

// Part 3 - Valley
test_inputs[21] = "*43125678812ttnaAptMTSUOaodwcoIXknLypETZ**3 paymoremoney 17 17 5 21 18 21 2 2 19 0 15***3 RRLMWBKASPDH 17 17 5 21 18 21 2 2 19 3 8";
test_inputs[22] = "*43125678812ttnaAptMTSUOaodwcoIXknLypETZ**3 paymoremoney 17 17 5 21 18 21 2 2 19 0 15***3 R RLMWBK ASP  DH  17 17 5 21 18   21 2 2 19 3  8 ";
test_inputs[23] = "***3 WKAZAPBPPPCMADD  3 2 1 20 15 4 10 22 3**3 GoodMorningJohn 3 2 1 20 15 4 10 22 3*43125678812ttnaAptMTSUOaodwcoIXknLypETZ";
test_inputs[24] = "**3 paymoremoney 17 17 5 21 18 21 2 2 19 0 15*** 2 RFRWYQ 5 8 17 3 4 9*31427781OAOSRSDKYPWIKSRO";
test_inputs[25] = "*DNE**DNE***2 NDBHXPEGAQBH 6 7 21 1 6 6 6 1337 999 12345678910 test"; // Should not work because part3 has alphabetic characters
test_inputs[26] = "*DNE**2 poopityscoop 6 7 21 1 6 6 6 1337 999 12345678910***2 UPJJFXCEUCJJ 6 7 21 1 6 6 6 1337 999 12345678910"; // Should decode to poopityscoop
```
