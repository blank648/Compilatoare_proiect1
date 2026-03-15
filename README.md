# Lexical Analyzer for a C/C++ Subset

This project is a simple lexical analyzer (scanner), written in C++, for a programming language similar to C/C++. The program reads a source file (`input.txt`), breaks it down into logical units called "tokens," and displays information about each identified token.

The project was developed as part of a Compilers course.

---

## Features

The lexical analyzer implements the following functionality:

* **Token Identification**: Correctly recognizes the main types of tokens:
    * **Keywords**: `void`, `main`, `int`, `if`, `else`, `while`, `float`
    * **Identifiers**: Variable and function names (e.g., `beta123_c`)
    * **Constants**: Integer (`10`) and real (`13.78`, `.34`) numbers
    * **Operators**: Simple (`+`, `=`) and compound (`++`, `==`) operators
    * **Delimiters**: `(`, `)`, `{`, `}`, `;`
    * **Comments**: Single-line (`//...`) and multi-line (`/*...*/`) comments

* **"Maximal Munch" Rule**: Correctly implements the longest possible match principle, ensuring that sequences such as `++` or `==` are treated as a single token.

* **Line Tracking**: Correctly counts lines and reports the location of each token, including the line range for multi-line comments.

* **Error Handling**: Provides a robust error recovery system. When an invalid character sequence is encountered (e.g., `$22A`), the program reports an error and continues the analysis from the next valid token without stopping.

---

## Code Structure

The entire logic is implemented in a single file, `main.cpp`, and is structured as follows:

* **`enum class TokenType`**: An enumeration that defines all possible token types that the analyzer can recognize.

* **`struct Token`**: A simple structure that stores information about an identified token:
    * `lexeme`: The actual character sequence (e.g., "void", "123").
    * `type`: The token type (a value from `TokenType`).
    * `start_line` / `end_line`: The line (or range of lines) where the token was found.

* **Helper Functions (`scan...`)**:
    * **`scanIdentifierOrKeyword()`**: Scans an alphanumeric sequence and determines whether it is a keyword or an identifier.
    * **`scanNumber()`**: Scans a numeric constant and determines whether it is an integer or a real number.
    * **`scanOperator()`**: Scans an operator, implementing the "maximal munch" logic.
    * **`scanComment()`**: Scans comments (`//` or `/* */`) and correctly handles line counting.

* **`getNextToken()`**: The "dispatcher" function. This is the heart of the analyzer. It runs in a loop, skips whitespace, analyzes the current character, and delegates the complete scanning task to one of the helper functions. It also implements the error recovery logic.

* **`main()`**: The main function of the program. Responsible for:
    1.  Reading the `input.txt` file into a `std::string`.
    2.  Initializing the state variables (position, current line).
    3.  Calling `getNextToken()` in a `while` loop until the end of the file is reached.
    4.  Formatting and displaying the information for each valid token.

---

## Usage Example

For an `input.txt` file with the following content:
```cpp
void main(){ /* bbb*b
bb */
int beta123_c;
beta+++beta;
$22A==b+13.78.34$
```
