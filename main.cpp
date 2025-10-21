#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <cstring>

//folosim o enumerare pentru a defini tipurile posibile de tokeni
enum class TokenType {
    KEYWORD,//cuvant cheie: int, void
    IDENTIFIER,//identificator ( variabila, main )
    INT_CONSTANT,//constanta intreaga
    FLOAT_CONSTANT,//constanta reala
    OPERATOR,//+,++,==
    DELIMITATOR,//() {} ;
    COMMENT,//comentariu: /*...*/
    ERROR,//token individual sau eroare lexicala
    END_OF_FILE//token pentru sf fisierului
};

//structura de date pentru analizorul lexical
struct Token {
    std::string lexeme;//sir de caractere efectiv
    TokenType type;//tip token
    int start_line;//linia din fisier la care incepe/se termina tokenul
    int end_line;
};

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::KEYWORD:
            return "key_word";
        case TokenType::IDENTIFIER:
            return "identifier";
        case TokenType::INT_CONSTANT:
            return "int_constant";
        case TokenType::FLOAT_CONSTANT:
            return "float_constant";
        case TokenType::OPERATOR:
            return "operator";
        case TokenType::DELIMITATOR:
            return "delimitator";
        case TokenType::COMMENT:
            return "comment";
        case TokenType::ERROR:
            return "error";
        case TokenType::END_OF_FILE:
            return "end_of_file";
        default:
            return "unknown";
    }
}
//det daca este un cuvant sau identificator
Token scanIdentifierOrKeyword( const std::string&source, int& position, int line ) {
    int start_pos = position;

    //consuma caracterele
    while( position < source.length() && (isalnum(source[position]) || source[position] == '_') ) {
        position++;
    }
    //extrage lexema
    std::string lexeme = source.substr(start_pos, position - start_pos);

    const std::vector<std::string> keywords = {"void", "main", "int", "if", "else", "while", "float"};
    //verific daca este cuvant cheie
    if (std::find(keywords.begin(), keywords.end(), lexeme) != keywords.end()) {
        return {lexeme, TokenType::KEYWORD, line, line};
    }

    //caz negativ ( nu este identificator )
    return {lexeme, TokenType::IDENTIFIER, line, line};
}
//det intreaga sau reala
Token scanNumber( const std::string&source, int& position, int line ) {
    int start_pos = position;
    bool is_float = false;
    //daca incepe cu .
    if (source[position] == '.' && position + 1 < source.length() && isdigit(source[position + 1])) {
        is_float = true;
        position++;
        while (position < source.length() && isdigit(source[position])) {
            position++;
        }
    } else { // numele normale
        while (position < source.length() && isdigit(source[position])) {
            position++;
        }
        //daca are parte zecimala
        if (position < source.length() && source[position] == '.') {
            if (position + 1 < source.length() && isdigit(source[position + 1])) {
                is_float = true;
                position++;
                //consum partea fractionara
                while (position < source.length() && isdigit(source[position])) {
                    position++;
                }
            }
        }
    }

    std::string lexeme = source.substr(start_pos, position - start_pos);
    //setam tipul tokenului
    TokenType type = is_float ? TokenType::FLOAT_CONSTANT : TokenType::INT_CONSTANT;
    return {lexeme, type, line, line};
}

//scanez operator pe logica maximal munch
Token scanOperator(const std::string& source, int& position, int line) {
    int start_pos = position;
    char first_char = source[start_pos];
    //verificam daca au unul sau mai multe caractere
    if (first_char == '=' || first_char == '!') {
        if (position + 1 < source.length() && source[position + 1] == '=') {
            position += 2;
        } else {
            position += 1;
        }
    } else if (first_char == '+') {
        if (position + 1 < source.length() && source[position + 1] == '+') {
            position += 2;
        } else {
            position += 1;
        }
    } else if (first_char == '-') {
        if (position + 1 < source.length() && source[position + 1] == '-') {
            position += 2;
        } else {
            position += 1;
        }
    } else {
        //operatori cu un singur caracter
        position++;
    }

    std::string lexeme = source.substr(start_pos, position - start_pos);
    return {lexeme, TokenType::OPERATOR, line, line};
}
//comentarii de tip // sau /*....*/
Token scanComment(const std::string& source, int& position, int& currentLine) {
    int start_pos = position;
    int start_line = currentLine;

    //comentariu single line
    if (source[position + 1] == '/') {
        position += 2;
        while (position < source.length() && source[position] != '\n') {
            position++;
        }
        std::string lexeme = source.substr(start_pos, position - start_pos);
        return {lexeme, TokenType::COMMENT, start_line, currentLine};
    } else if (source[position + 1] == '*') {
        //comentariu multiline
        position += 2;

        while (position + 1 < source.length() && !(source[position] == '*' && source[position + 1] == '/')) {
            if (source[position] == '\n') {
                currentLine++;
            }
            position++;
        }
        if (position + 1 >= source.length()) {
            std::cerr << "Eroare la linia " << start_line << ": Comentariu multi-linie neterminat." << std::endl;
            return {"Comentariu neterminat", TokenType::ERROR, start_line, currentLine};
        }
        //division operator
        position += 2;
        std::string lexeme = source.substr(start_pos, position - start_pos);
        return {lexeme, TokenType::COMMENT, start_line, currentLine};
    }

    position++;
    return {"/", TokenType::OPERATOR, start_line, start_line};
}
//functie dispencer.
//ruleaza bucla, sare printre spatii, analizeaza caracterul cerut si deleaga sarvina de scanare completa catre funtiile ajutatoare
//de asemenea, implementeaza logica de recuperare in erori
Token getNextToken(const std::string& source, int& position, int& current_line) {
    //sar peste spatii, taburi si linii noi
    while (position < source.length() && isspace(source[position])) {
        if (source[position] == '\n') {
            current_line++;
        }
        position++;
    }
    //verificam sfarsitul de fisier
    if (position >= source.length()) {
        return {"", TokenType::END_OF_FILE, current_line, current_line};
    }
    //trimitem catre get_uri
    char current_char = source[position];

    if (isalpha(current_char) || current_char == '_') {
        return scanIdentifierOrKeyword(source, position, current_line);
    }
    if (isdigit(current_char) || (current_char == '.' && position + 1 < source.length() && isdigit(source[position + 1]))) {
        return scanNumber(source, position, current_line);
    }
    if (current_char == '{' || current_char == '}' || current_char == '(' || current_char == ')' || current_char == ';') {
        position++;
        return {std::string(1, current_char), TokenType::DELIMITATOR, current_line, current_line};
    }
    if (current_char == '/') {
        return scanComment(source, position, current_line);
    }
    if (current_char == '+' || current_char == '-' || current_char == '=' || current_char == '*' || current_char == '!') {
        return scanOperator(source, position, current_line);
    }

    //gestionarea erorilor: avem eroare la $, #, &
    int error_start_pos = position;
    while (position < source.length() && !isspace(source[position]) &&
           !strchr("(){};=+-*/", source[position])) {
        position++;
    }
    if (position == error_start_pos) {
        position++;
    }

    std::string errorLexeme = source.substr(error_start_pos, position - error_start_pos);
    std::cerr << "Eroare Lexicala la linia " << current_line << ": Secventa necunoscuta '" << errorLexeme << "'" << std::endl;
    return {errorLexeme, TokenType::ERROR, current_line, current_line};
}

int main() {
    std::ifstream inputFile("input.txt");
    if (!inputFile.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return -1;
    }

    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    std::string sourceCode = buffer.str();
    inputFile.close();

    std::cout << "Fisierul input.txt a fost deschis cu succes. Incepe canarea... \n\n";

    int position = 0;
    int currentLine = 1;
    Token token;
    while ((token = getNextToken(sourceCode, position, currentLine)).type != TokenType::END_OF_FILE) {
        if (token.type != TokenType::ERROR) {
            std::cout << "'" << token.lexeme << "', "
                      << tokenTypeToString(token.type) << "; "
                      << token.lexeme.length() << "; ";
            if (token.start_line == token.end_line) {
                std::cout << "linia " << token.start_line << std::endl;
            } else {
                std::cout << "liniile " << token.start_line << "-" << token.end_line << std::endl;
            }
        }
    }

    std::cout << "\nScanare Finalizata." << std::endl;
    return 0;
}