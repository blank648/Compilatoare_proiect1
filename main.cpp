#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

//folosim o enumerare pentru a defini tipurile posibile de tokeni
enum class TokenType {
    KEYWORD,//cuvant cheie: int, void
    IDENTIFIER,//identificator ( variabila, main )
    INT_CONSTANT,//constanta intreaga
    FLOAT_CONSTANT,//constanta reala
    OPERATOR,//+,++,==
    COMMENT,//comentariu: /*...*/
    ERROR,//token individual sau eroare lexicala
    END_OF_FILE//token pentru sf fisierului
};

//structura de date pentru analizorul lexical
//fiecare bucata de cod pe care o idnetificam este stocata aici

struct Token {
    std::string lexme;//sir de caractere efectiv
    TokenType type;//tip token
    int start_line;//linia din fisier la care incepe tokenul
    int end_line;
};

//functie helper. transforma o valoare de tip TokenType intr-un string ( TokenType::KEYWORD -> key_word )
//util pentru formatarea finala
std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::KEYWORD:
            return "keyword";
        case TokenType::IDENTIFIER:
            return "identifier";
        case TokenType::INT_CONSTANT:
            return "int_constant";
        case TokenType::FLOAT_CONSTANT:
            return "float_constant";
        case TokenType::OPERATOR:
            return "operator";
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

Token getNextToken(const std::string& source, int& position, int& current_line);

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

    //1.citirea fisierului de intrare
    std::ifstream inputFile("input.txt");
    //verificam daca se deschide fisierul
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
        //afisam informatiile despre token
        std::cout << "'" << token.lexme << "'"
        << tokenTypeToString(token.type) << "'"
        << token.lexme.length() << "'";

        if (token.start_line == token.end_line) {
            std::cout << "linia " << token.start_line << std::endl;
        }else {
            std::cout << "linia " << token.start_line << token.end_line << std::endl;
        }

    }

    std::cout << "\n Scanare Finalizata." << std::endl;

    return 0;
}

Token getNextToken(const std::string& source, int& position, int& current_line) {
    if(position >= source.length()) {
        return {"", TokenType::END_OF_FILE, current_line, current_line};
    }

    position = source.length();

    return {"placeholder", TokenType::IDENTIFIER, 1, 1};
}