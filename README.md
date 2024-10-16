The program calculates sizes of texts compressed using different codes:
- Fibonacci code Fib3
- Fibonacci Higher Order Codes (Shmuel T. Klein and Dana Shapira. Denser high-order Fibonacci codes. In Data Compression Conference, DCC 2024, Snowbird, UT, USA, March 20-22, 2024, pages 263–272. IEEE, 2024.)
- Binary Coded Mixed Digit Codes (Igor O. Zavadskyi and Maksym Kovalchuk. Binary mixed-digit data compression codes. In String Processing and Information Retrieval SPIRE 2023, Pisa, Proceedings, volume 14240 of LNCS, pages 381–392. Springer, 2023.)
- Mixed d-ary Fibonacci codes (paper by I. Zavadskyi, S.T. Klein, and D. Shapira submitted to DCC'25).

It can be compiled with g++.exe -Wall -fexceptions -w -std=gnu++17 -c options.

The program has 1 command line argument: name of the text file.

Only the alphanumeric stream is encoded on a word-based alphabet, i.e. punctuation signs are eliminated, and in what remains, a sequence of characters between two consecutive spaces is considered a word, i.e. an alphabet symbol.
