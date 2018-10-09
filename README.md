# Multi-Threading Decoder Using pthreads
Reads in user-entered ciphertext string consisting of three parts: `Part1`, `Part2`, and `Part3` and prints out the decoded plaintext.

Each parts is each fed into the `Fence`, `Hill`, and `Valley` threads respectively, where they are concurrently decoded.

The specific decoding scheme for each of these three methods can be found in the [Assignment Description](./assignment_description.pdf).

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

## Custom Sample Inputs
For testing invalid inputs caught by the initial thread
```
**DNE*DNE**DNE*DNE
***DNE* **DNE
***DNE*DNE t3w **
```
For testing `Part1`, the `Fence` Decoder Thread
```
*43125678812ttnaAptMTSUOaodwcoIXknLypETZ**DNE***DNE
*93456312ttnaAptMTSUOaodwcoIXknLypETZ**DNE***DNE
```
For testing `Part2`, the `Hill` Decoder Thread
```
*43125678812ttnaAptMTSUOaodwcoIXknLypETZ**3 paymoremoney 17 17 5 21 18 21 2 2 19 0 15***DNE
*43125678812ttnaAptMTSUOaodwcoIXknLypETZ**3  p ay more money  17 17 5 21   18 21 2 2   19 0 15  ***DNE
```
For testing `Part3`, the `Valley` Encoder Thread
```
*43125678812ttnaAptMTSUOaodwcoIXknLypETZ**3 paymoremoney 17 17 5 21 18 21 2 2 19 0 15***3 RRLMWBKASPDH 17 17 5 21 18 21 2 2 19 3 8
*43125678812ttnaAptMTSUOaodwcoIXknLypETZ**3 paymoremoney 17 17 5 21 18 21 2 2 19 0 15***3 R RLMWBK ASP  DH  17 17 5 21 18   21 2 2 19 3  8
***3 WKAZAPBPPPCMADD  3 2 1 20 15 4 10 22 3**3 GoodMorningJohn 3 2 1 20 15 4 10 22 3*43125678812ttnaAptMTSUOaodwcoIXknLypETZ
**3 paymoremoney 17 17 5 21 18 21 2 2 19 0 15*** 2 RFRWYQ 5 8 17 3 4 9*31427781OAOSRSDKYPWIKSRO
```