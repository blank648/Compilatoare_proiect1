#include <iostream>
#include <string.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>

//folosim o enumerare pentru a defini tipurile posibile de tokeni
enum class TokenType {
    KEYWORD,//cuvant cheie: int, void
    IDENTIFIER,//identificator ( variabila, main )
    INT_CONSTANT,//constanta intreaga
    FLOAT_CONSTANT,//constanta reala
    OPERATOR,//+,++,==
    DELIMITATOR,
    COMMENT,//comentariu: /*...*/
    ERROR,//token individual sau eroare lexicala
    END_OF_FILE//token pentru sf fisierului
};

//structura de date pentru analizorul lexical
//fiecare bucata de cod pe care o idnetificam este stocata aici
struct Token {
    std::string lexeme;//sir de caractere efectiv
    TokenType type;//tip token
    int start_line;//linia din fisier la care incepe tokenul
    int end_line;
};

//functie helper. transforma o valoare de tip TokenType intr-un string ( TokenType::KEYWORD -> key_word )
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

Token scanIdentifierOrKeyword( const std::string&source, int& position, int line ) {
    int start_pos = position;
    while( position < source.length() && isalnum(source[position]) || source[position] == '_' ) {
        position++;
    }
    //extragem lexema
    std::string lexeme = source.substr(start_pos, position - start_pos);
    //cuvinte cheie cunoscute
    const std::vector<std::string> keywords = {"void", "main", "int", "if", "else", "while"};

    if (std::find(keywords.begin(), keywords.end(), lexeme) != keywords.end()) {
        return {lexeme, TokenType::KEYWORD, line, line};
    }

    return {lexeme, TokenType::IDENTIFIER, line, line};
}

Token scanNumber( const std::string&source, int& position, int line ) {
    int start_pos = position;
    bool is_float = false;

    if (source[position] == '.' && position + 1 < source.length() && isdigit(source[position + 1])) {
        is_float = true;
        position++; // consum punctul
        while (position < source.length() && isdigit(source[position])) {
            position++;
        }
    } else { // numere normale
        while (position < source.length() && isdigit(source[position])) {
            position++;
        }
        if (position < source.length() && source[position] == '.') {
            if (position + 1 < source.length() && isdigit(source[position + 1])) {
                is_float = true;
                position++;
                while (position < source.length() && isdigit(source[position])) {
                    position++;
                }
            }
        }
    }

    std::string lexeme = source.substr(start_pos, position - start_pos);
    TokenType type = is_float ? TokenType::FLOAT_CONSTANT : TokenType::INT_CONSTANT;
    return {lexeme, type, line, line};
}

Token scanOperator(const std::string& source, int& position, int line) {
    int start_pos = position;
    char first_char = source[start_pos];

    if (first_char == '=' || first_char == '!') {
        if (position + 1 < source.length() && source[position + 1] == '=') {
            position += 2; // '==' sau '!='
        } else {
            position += 1; // '='
        }
    } else if (first_char == '+') {
        if (position + 1 < source.length() && source[position + 1] == '+') {
            position += 2; // '++'
        } else {
            position += 1; // '+'
        }
    } else if (first_char == '-') {
        if (position + 1 < source.length() && source[position + 1] == '-') {
            position += 2; // '--'
        } else {
            position += 1; // '-'
        }
    } else {
        position++;
    }
    std::string lexeme = source.substr(start_pos, position - start_pos);
    return {lexeme, TokenType::OPERATOR, line, line};
}

Token scanComment(const std::string& source, int& position, int& currentLine) {
    int start_pos = position;
    int start_line = currentLine;
    if (source[position + 1] == '/') {
        position += 2;
        while (position < source.length() && source[position] != '\n') {
            position++;
        }
        std::string lexeme = source.substr(start_pos, position - start_pos);
        return {lexeme, TokenType::COMMENT, start_line, currentLine};
    } else if (source[position + 1] == '*') {
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

        position += 2;
        std::string lexeme = source.substr(start_pos, position - start_pos);
        return {lexeme, TokenType::COMMENT, start_line, currentLine};
    }
    // doar operatorul /
    position++;
    return {"/", TokenType::OPERATOR, start_line, start_line};
}


Token getNextToken(const std::string& source, int& position, int& current_line) {
    //1.sar peste spatii, tab-uri si linii noi
    while( position < source.length() && isspace(source[position]) ) {
        if( source[position] == '\n') {
            current_line++;
        }
        position++;
    }

    if( position >= source.length() ) {
        return {"", TokenType::END_OF_FILE, current_line, current_line};
    }

    char current_char = source[position];

    //verifica daca e un identificator sau cuvant cheie ( incepe cu litera sau _ )
    if(isalpha(current_char) || current_char == '_') {
        return scanIdentifierOrKeyword(source, position, current_line);
    }

    if(isdigit(current_char)) {
        return scanNumber(source, position, current_line);
    }

    if(current_char == '{' || current_char == '}' || current_char == '[' || current_char == ']' || current_char == '(' || current_char == ')' || current_char == ';') {
        position++;
        return {std::string(1, current_char), TokenType::DELIMITATOR, current_line, current_line};
    }

    if(current_char == '/') {
        if(position + 1 < source.length() && source[position + 1] == '/' || source[position + 1] == '*') {
            return scanComment(source, position, current_line);
        }
    }

    if(current_char == '+' || current_char == '-' || current_char == '=' || current_char == '/' || current_char == '*') {
        return scanOperator(source, position, current_line);
    }

    // --- gestionarea avansata a erorilor ---
    int error_start_pos = position;
    // consumam caractere până la următorul spațiu, delimitator sau operator
    while (position < source.length() && !isspace(source[position]) &&
           !strchr("(){};=+-*/", source[position])) {
        position++;
           }
    // Dacă nu am avansat, forțăm avansarea cu un caracter pentru a evita bucle infinite
    if (position == error_start_pos) {
        position++;
    }
    std::string errorLexeme = source.substr(error_start_pos, position - error_start_pos);
    std::cerr << "Eroare Lexicala la linia " << current_line << ": Secventa necunoscuta '" << errorLexeme << "'" << std::endl;
    return {errorLexeme, TokenType::ERROR, current_line, current_line};
}

int main() {
    //test pentru functionare token
    /*Token exemplu_token;
    exemplu_token.lexme = "void";
    exemplu_token.type = TokenType::KEYWORD;
    exemplu_token.start_line = 3;
    exemplu_token.end_line = 3;

    std::cout << "\n Exemplu de utilizare a structurii Token:" << std::endl;
    std::cout << exemplu_token.lexme << std::endl;
    std::cout << tokenTypeToString(exemplu_token.type) << std::endl;
    std::cout << exemplu_token.start_line << std::endl;*/

    std::ifstream inputFile("input.txt");

    if(!inputFile.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return -1;
    }

    std::stringstream buffer;
    buffer << inputFile.rdbuf();//citesc continutul fisierului in buffer
    std::string sourceCode = buffer.str(); //fac conv la string
    inputFile.close();

    std::cout << "Fisierul input.txt a fost deschis cu succes. Incepe canarea... \n\n";

    //2.initializam variabilele de stare
    int position = 0; //pozitia caracterului in source code
    int currentLine = 1; //pt erori si tokeni

    //3.bucla principala de scanare
    //bucla ruleaza in continuu, solicitand urmatorul token de la functia de scanare
    //se opreste doaratunciu cand functia returneaza un token special END_OF_FILE
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

    std::cout << "\n Scanare Finalizata." << std::endl;

    return 0;
}
