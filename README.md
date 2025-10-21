# Analizor Lexical pentru un Sub-set C/C++

Acest proiect reprezinta un analizor lexical (scanner) simplu, scris in C++, pentru un limbaj de programare similar cu C/C++. Programul citeste un fisier sursa (`input.txt`), il descompune in unitati logice numite "tokeni" si afiseaza informatii despre fiecare token identificat.

Proiectul a fost dezvoltat ca parte a cursului de Compilatoare.

---

## Caracteristici (Features)

Analizorul lexical implementeaza urmatoarele functionalitati:

* **Identificarea Tokenilor**: Recunoaste corect principalele tipuri de tokeni:
    * **Cuvinte Cheie**: `void`, `main`, `int`, `if`, `else`, `while`, `float`
    * **Identificatori**: Nume de variabile si functii (ex: `beta123_c`)
    * **Constante**: Numere intregi (`10`) si reale (`13.78`, `.34`)
    * **Operatori**: Operatori simpli (`+`, `=`) si compusi (`++`, `==`)
    * **Delimitatori**: `(`, `)`, `{`, `}`, `;`
    * **Comentarii**: Comentarii pe o singura linie (`//...`) si pe mai multe linii (`/*...*/`)

* **Regula "Maximal Munch"**: Implementeaza corect principiul celui mai lung sir posibil, asigurand ca secvente precum `++` sau `==` sunt tratate ca un singur token.

* **Gestionarea Liniilor**: Contorizeaza corect liniile si raporteaza locatia fiecarui token, inclusiv intervalul de linii pentru comentariile multi-linie.

* **Gestionarea Erorilor**: Ofera un sistem robust de recuperare din erori. La intalnirea unei secvente de caractere invalide (ex: `$22A`), programul raporteaza o eroare si continua analiza de la urmatorul token valid, fara a se opri.

---

## Structura Codului

Intreaga logica este implementata intr-un singur fisier, `main.cpp`, si este structurata astfel:

* **`enum class TokenType`**: O enumerare care defineste toate tipurile posibile de tokeni pe care analizorul ii poate recunoaste.

* **`struct Token`**: O structura simpla care stocheaza informatiile despre un token identificat:
    * `lexeme`: Sirul de caractere efectiv (ex: "void", "123").
    * `type`: Tipul tokenului (o valoare din `TokenType`).
    * `start_line` / `end_line`: Linia (sau intervalul de linii) unde a fost gasit tokenul.

* **Functii Ajutatoare (`scan...`)**:
    * **`scanIdentifierOrKeyword()`**: Scaneaza un sir alfanumeric si determina daca este un cuvant cheie sau un identificator.
    * **`scanNumber()`**: Scaneaza o constanta numerica si determina daca este intreaga sau reala.
    * **`scanOperator()`**: Scaneaza un operator, implementand logica "maximal munch".
    * **`scanComment()`**: Scaneaza comentarii (`//` sau `/* */`) si gestioneaza corect contorizarea liniilor.

* **`getNextToken()`**: Functia "dispecer". Aceasta este inima analizorului. Ruleaza intr-o bucla, sare peste spatii, analizeaza caracterul curent si deleaga sarcina de scanare completa catre una dintre functiile ajutatoare. De asemenea, implementeaza logica de recuperare din erori.

* **`main()`**: Functia principala a programului. Responsabila cu:
    1.  Citirea fisierului `input.txt` intr-un `std::string`.
    2.  Initializarea variabilelor de stare (pozitie, linie curenta).
    3.  Apelarea `getNextToken()` intr-o bucla `while` pana la epuizarea fisierului.
    4.  Formatarea si afisarea informatiilor pentru fiecare token valid.

---

## Exemplu de Utilizare

Pentru un fisier `input.txt` cu urmatorul continut:
```cpp
void main(){ /* bbb*b
bb */
int beta123_c;
beta+++beta;
$22A==b+13.78.34$