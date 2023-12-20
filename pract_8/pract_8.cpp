#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <fstream>
#include <bitset>
#include <Windows.h>

using namespace std;

// структура для элемента массива символов
struct symbol {
    char sym;

    int frequency;
    string code;
    double prob;

    symbol(char sym) {
        this->sym = sym;
        this->prob = 0;
        this->frequency = frequency = 1;
    }

    static bool reverseCompareFreq(symbol s1, symbol s2) {
        if (s1.frequency != s2.frequency) return s1.frequency > s2.frequency;
        return s1.sym < s2.sym;
    }

    static bool compareFreq(symbol s1, symbol s2) {
        if (s1.frequency != s2.frequency) return s1.frequency < s2.frequency;
        return s1.sym > s2.sym;
    }

    static bool CompareCodes(symbol s1, symbol s2) {
        if (s1.code.length() != s2.code.length()) return s1.code.length() < s2.code.length();
        for (int i = 0; i < s1.code.length(); ++i) {
            if (s1.code[i] == s2.code[i]) continue;
            return s1.code[i] < s2.code[i];
        }
    }

};

// поиск символа в массиве символов по полю sym
int findSymbol(vector<symbol>& symbols, char sym) {
    for (int i = 0; i < symbols.size(); ++i) {
        if (symbols[i].sym == sym) return i;
    }
    return -1;
}

// поиск символа в массиве символов по его коду
int findSymbol(vector<symbol>& symbols, string code) {
    for (int i = 0; i < symbols.size(); ++i) {
        if (symbols[i].code == code) return i;
    }
    return -1;
}

// метод получения массива символов по тексту
vector<symbol>& getSymbols(string text) {
    vector<symbol>* symbols = new vector<symbol>;

    int allFreq = text.length();

    for (char el : text) {
        int index = findSymbol(*symbols, el);
        if (index == -1) symbols->push_back(symbol(el));
        else (*symbols)[index].frequency++;
    }

    for (int i = 0; i < symbols->size(); ++i) {
        (*symbols)[i].prob = (double)(*symbols)[i].frequency / allFreq;
    }

    return *symbols;
}

// класс для построения дерева Шеннона-Фано
class TreeShenon {
    // массив символов
    vector<symbol> symbols;

    // метод построения дерева Шеннона-Фано
    void buildTree(string mark, int start, int end) {
        if (start == end) { 
            if (start != symbols.size()) symbols[start].code = mark;
            return;
        }
        if (start + 1 == symbols.size()) {
            symbols[start].code = mark;
            return;
        }

        int allFreq = 0;
        for (int i = start; i < end; ++i) {
            allFreq += symbols[i].frequency;
        }

        int new_end = start, leftFreq = 0;
        
        for (; new_end < end; ++new_end) {
            leftFreq += symbols[new_end].frequency;
            if (leftFreq >= allFreq / 2) break;
        }

        buildTree(mark + "0", start, new_end);
        buildTree(mark + "1", new_end + 1, end);
    }

public:
    // конструктор
    TreeShenon(vector<symbol> symbols) {
        this->symbols = symbols;
        sort(this->symbols.begin(), this->symbols.end(), symbol::reverseCompareFreq);
        buildTree("", 0, symbols.size());
    }

    // метод вывода символов и их кодов
    void showSymbolsWithCode() {
        int bits = 0;
        int count = 0;
        cout << "\n№\tsymbol\tfreq\tprob\tcode\tbits\n";
        sort(symbols.begin(), symbols.end(), symbol::CompareCodes);
        for (symbol el : symbols) {
            string sym;
            if (el.sym == ' ') sym = "пробел";
            else sym = el.sym;

            printf("%d\t%s\t%d\t%.2f\t%s\t%d\n", count++, sym.c_str(), el.frequency,
                el.prob, el.code.c_str(), el.frequency * el.code.length());

            bits += el.code.length() * el.frequency;
        }

        cout << "\nИтого: " << bits << endl << endl;
    }

    // метод получения массива символов
    vector<symbol> getSymbols() {
        return symbols;
    }
};

// класс для построения дерева Хаффмана
class TreeHaffman {
    // узел дерева Хаффмана
    struct node {
        int parent, left, right;

        node() {
            parent = left = right = 0;
        }
    };

    // массив символов
    vector<symbol> symbols;

    // указатель на корневой узел дерева Хаффмана
    int head;
    // массив для табличного представления дерва Хаффмана
    vector<node> tree;

    // метод построения дерева Хаффмана
    void buildTree() {
        sort(symbols.begin(), symbols.end(), symbol::compareFreq);

        set<pair<int, int>> nodes;
        tree = vector<node>(symbols.size());

        for (int i = 0; i < symbols.size(); ++i) {
            nodes.insert( { symbols[i].frequency, i } );
        }

        while (nodes.size() != 1) {
            pair<int, int> left = *nodes.begin();
            nodes.erase(nodes.begin());

            pair<int, int> right = *nodes.begin();
            nodes.erase(nodes.begin());

            node new_node;
            pair<int, int> parent;
             parent.first = left.first + right.first;
            new_node.left = left.second;
            new_node.right = right.second;

            tree[left.second].parent = tree[right.second].parent = parent.second = tree.size();

            tree.push_back(new_node);
            nodes.insert(parent);
        }

        head = (*nodes.begin()).second;

    }

    // метод построения кодов для символов
    void buildCodes(string mark, int list) {
        if (list < symbols.size()) {
            symbols[list].code = mark;
            return;
        }

        buildCodes(mark + "0", tree[list].left);
        buildCodes(mark + "1", tree[list].right);
    }


public:
    // конструктор
    TreeHaffman(vector<symbol> symbols) {
        head = 0;
        this->symbols = symbols;

        buildTree();
        buildCodes("", head);
    }

    // метод вывода символов и их кодов
    void showSymbolsWithCode() {
        int bits = 0;
        int count = 0;
        cout << "\n№\tsymbol\tfreq\tprob\tcode\tbits\n";
        sort(symbols.begin(), symbols.end(), symbol::CompareCodes);
        for (symbol el : symbols) {
            string sym;
            if (el.sym == ' ') sym = "пробел";
            else sym = el.sym;
            
            printf("%d\t%s\t%d\t%.2f\t%s\t%d\n", count++, sym.c_str(), el.frequency,
                el.prob, el.code.c_str(), el.frequency * el.code.length());

            bits += el.code.length() * el.frequency;
        }

        cout << "\nИтого: " << bits << endl << endl;
    }

    // метод получения массива символов
    vector<symbol> getSymbols() {
        return symbols;
    }
};

// функция кодирования текста с помощью массива символов
string CodeText(vector<symbol> symbols, string text) {
    string codedStr = "";
    for (int i = 0; i < text.length(); ++i) {
        int index = findSymbol(symbols, text[i]);
        codedStr += symbols[index].code;
        if (i < text.length() - 1) codedStr += " ";
    }

    return codedStr;
}

// функция декодирования текста с помощью массива символов
string DecodeText(vector<symbol> symbols, string text) {
    string decodedStr = "", code = "";
    for (int i = 0; i < text.length(); ++i) {
        if (text[i] != ' ') code += text[i];
        else {
            decodedStr += symbols[findSymbol(symbols, code)].sym;
            code = "";
        }
    }
    decodedStr += symbols[findSymbol(symbols, code)].sym;

    return decodedStr;
}

// функция архивирования файла с помощью метода Хаффмана
bool archivate(string name_input, string name_output) {
    fstream in(name_input, ios::in);

    if (!in) {
        cout << "Файла " << name_input << " не существует\n";
        return false;
    }

    string input_symbols = "";
    while (!in.eof()) {
        string temp = "";
        getline(in, temp);
        if (!input_symbols.empty()) temp = "\n" + temp;
        input_symbols += temp;
    }
    in.close();

    vector<symbol> symbols = getSymbols(input_symbols);

    TreeHaffman tree(symbols);

    symbols = tree.getSymbols();

    fstream out(name_output, ios::out | ios::binary);
    int max_len = 0, len_code = 0;
    char sym; const char* code;
    for (int i = 0; i < symbols.size(); ++i) {
        sym = symbols[i].sym;
        len_code = symbols[i].code.length();
        code = symbols[i].code.c_str();
        out.write( (char*)&sym, sizeof(sym) );
        out.write( (char*)&len_code,  sizeof(int) );
        out.write( code, len_code);

        if (len_code > max_len) {
            max_len = len_code;
        }

    }
    string coded = CodeText(symbols, input_symbols);
    while (coded.find(" ") != string::npos) coded.erase(coded.find(" "), 1);

    sym = 0;
    out.write((char*)&sym, sizeof(char));
    len_code = 8 - coded.length() % 8;
    out.write((char*)&len_code, sizeof(len_code));

    string temp = ""; int i = 0; unsigned char c = 0;
    while (coded.length() / 8) {
        temp += coded[i++];
        if (temp.length() == 8) {
            c = 0;
            for (int j = 0; j < temp.length(); ++j)
            if (temp[j] == '1') {
                c = c | (1 << (temp.length() - 1 - j) % 8);
            }
            out.write( (char*)&c, sizeof(unsigned char) );
            coded.erase(0, i);
            i = 0;
            temp = "";
        }
    }
    if (!coded.empty()) {
        c = 0;
        for (int i = 0; i < coded.length(); ++i) {
            if (coded[i] == '1') {
                c = c | (1 << (7 - i));
            }
        }
        out.write( (char*)&c, sizeof(unsigned char) );
    }
    
    out.close();

    return true;
}


// функция разархивирования файла архивированного с помощью метода Хаффмана
void dearchivate(string name_input) {
    cout << "Разархивирование\n";

    fstream in(name_input, ios::in | ios::binary);

    vector<symbol> symbols;
    char s = 128; char* code;
    int min_len = INT_MAX, max_len = 0, len_code = 0, len_trash = 0;

    while (true) {
        in.read( (char*)&s, sizeof(s) );
        if (s == 0) {
            in.read( (char*)&len_trash, sizeof(int) );
            break;
        }
        in.read( (char*)&len_code, sizeof(len_code) );
        code = new char[len_code + 1];
        in.read( code, len_code );
        code[len_code] = '\0';
        
        symbol new_s(s);
        new_s.code = code;

        symbols.push_back(new_s);

        delete[] code;

        if (len_code > max_len) max_len = len_code;
        if (len_code < min_len) min_len = len_code;
    }


    char c;
    string coded = "";
    while (true) {
        in.read( (char*)&c, sizeof(unsigned char) );
        if (!in.eof()) coded += bitset<8>(c).to_string();
        else break;
    }

    in.close();

    sort(symbols.begin(), symbols.end(), symbol::CompareCodes);
    string decoded;
    while (coded.length() != len_trash) {
        for (int i = max_len; i >= min_len; --i) {
            string sub = coded.substr(0, i);
            int index = findSymbol(symbols, sub);
            if (index != -1) {
                decoded += symbols[index].sym;
                coded.erase(0, i);
                break;
            }
        }
    }

    string name_out = "de" + name_input.substr(0, name_input.find(".")) + ".txt";

    fstream out(name_out, ios::out);
    out << decoded;
    out.close();

    cout << "Успешно\nРезультат декодирования в файле " << name_out << "\n\n";
}

void HaffmanTxt(string name_input, string name_arch) {
    cout << "\nАрхивирование" << endl;
    if (!archivate(name_input, name_arch)) {
        cout << "Файла " << name_input << " не существует\n";
    }
    else {
        cout << "Успешно\n\n";
        dearchivate(name_arch);
    }
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    string vvod = "Для завершения работы прогаммы введите 0\n";
    vvod += "Для работы с деревом Шеннона-Фано введите 1\n";
    vvod += "Для работы с деревом Хаффмана введите 2\n";
    vvod += "Для архивирования файла методом Хаффмана введите 3\n";

    string vvodShen = "Для кодирования фразы \"Ана, дэус, рики, паки, Дормы кормы констунтаки, Дэус Дэус канадэус - бац!\" введите 1\n";
    vvodShen += "Для вывода таблицы символов с кодами введите 2\n";
    vvodShen += "Для вывода закодированной фразы введите 3\n";
    vvodShen += "Для вывода декодированной фразы введите 4\n";
    vvodShen += "Для кодирования любой фразы введите 5\n";
    vvodShen += "Для выхода из данной подпрограммы введите -1\n";

    string vvodHaff = "Для кодирования фразы \"Смирнов  Даниил  Анатольевич\" введите 1\n";
    vvodHaff += "Для вывода таблицы символов с кодами введите 2\n";
    vvodHaff += "Для вывода закодированной фразы введите 3\n";
    vvodHaff += "Для вывода декодированной фразы введите 4\n";
    vvodHaff += "Для кодирования любой фразы введите 5\n";
    vvodHaff += "Для выхода из данной подпрограммы введите -1\n";

    int vib = 10, vibTree = 10;
    TreeShenon* treeS = NULL; TreeHaffman* treeH = NULL;
    string coded = "", decoded = "", text = "";
    while (true) {
        if (vibTree == 0) break;

        if (vibTree == 10) {
            system("cls");
            coded = decoded = text = "";
            cout << vvod;
            cin >> vibTree;
            cin.ignore();
            vib = 10;

            if (treeS) delete treeS;
            if (treeH) delete treeH;
            treeS = NULL; treeH = NULL;
        }
        else if (vibTree == 1) {
            if (vib == 10) {
                system("cls");
                cout << vvodShen;
            }
            cin >> vib;
            cin.ignore();

            if (vib == 1) {
                text = "Ана, дэус, рики, паки, Дормы кормы констунтаки, Дэус Дэус канадэус - бац!";
                if (treeS) { delete treeS; treeS = NULL; }
                treeS = new TreeShenon(getSymbols(text));
            }
            else if (vib == 2) {
                if (treeS) treeS->showSymbolsWithCode();
                else cout << "Никакая фраза ещё не была закодирована\n";
            }
            else if (vib == 3) {
                if (!treeS) cout << "Никакая фраза ещё не была закодирована\n";
                else {
                    coded = CodeText(treeS->getSymbols(), text);
                    cout << "\nЗакодированная фраза\n" << coded << "\n\n";
                }
            }
            else if (vib == 4) {
                if (!treeS || coded.empty()) cout << "Никакая фраза ещё не была закодирована\n";
                else {
                    decoded = DecodeText(treeS->getSymbols(), coded);
                    cout << "\nДекодированная фраза\n" << decoded << "\n\n";
                }
            }
            else if (vib == 5) {
                cout << "Введите фразу для кодирования:  ";
                getline(cin, text);
                if (treeS) { delete treeS; treeS = NULL; }
                treeS = new TreeShenon(getSymbols(text));
            }
            else if (vib == -1) vibTree = 10;
            else vib = 10;

        }
        else if (vibTree == 2) {
            if (vib == 10) {
                system("cls");
                cout << vvodHaff;
            }
            cin >> vib;
            cin.ignore();

            if (vib == 1) {
                text = "Смирнов  Даниил  Анатольевич";
                if (treeH) { delete treeH; treeH = NULL; }
                treeH = new TreeHaffman(getSymbols(text));
            }
            else if (vib == 2) {
                if (treeH) treeH->showSymbolsWithCode();
                else cout << "Никакая фраза ещё не была закодирована\n";
            }
            else if (vib == 3) {
                if (!treeH) cout << "Никакая фраза ещё не была закодирована\n";
                else {
                    coded = CodeText(treeH->getSymbols(), text);
                    cout << "\nЗакодированная фраза\n" << coded << "\n\n";
                }
            }
            else if (vib == 4) {
                if (!treeH || coded.empty()) cout << "Никакая фраза ещё не была закодирована\n";
                else {
                    decoded = DecodeText(treeH->getSymbols(), coded);
                    cout << "\nДекодированная фраза\n" << decoded << "\n\n";
                }
            }
            else if (vib == 5) {
                cout << "Введите фразу для кодирования:  ";
                getline(cin, text);
                if (treeH) { delete treeH; treeH = NULL; }
                treeH = new TreeHaffman(getSymbols(text));
            }
            else if (vib == -1) vibTree = 10;
            else vib = 10;
        }
        else if (vibTree == 3) {
            cout << "Введите название файла с данными: ";
            getline(cin, coded);
            cout << "Введите название файла-архива: ";
            getline(cin, decoded);

            HaffmanTxt(coded, decoded);

            cout << "Для продолжения работы введите любое число\t";
            cin >> vibTree;
            cin.ignore();

            vibTree = 10;

        }
        else system("cls");
    }
}