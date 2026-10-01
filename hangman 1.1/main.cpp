#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <cstdlib>
using namespace std;

// Struktura za igralce
struct Igralec {
    char ime[50];
    int vpisna_stevilka;
    char trenutno_geslo[100];
    char zgodovina_ugibanj[26];
    int st_ugibanj;
    double potrebni_cas;
    Igralec* naslednji;
};

// Funkcija za dodajanje igralca na urejen seznam
void dodajIgralca(Igralec*& seznam, Igralec* novIgralec) {
    if (seznam == nullptr || novIgralec->potrebni_cas < seznam->potrebni_cas) {
        novIgralec->naslednji = seznam;
        seznam = novIgralec;
    } else {
        Igralec* trenutni = seznam;
        while (trenutni->naslednji != nullptr && trenutni->naslednji->potrebni_cas < novIgralec->potrebni_cas) {
            trenutni = trenutni->naslednji;
        }
        novIgralec->naslednji = trenutni->naslednji;
        trenutni->naslednji = novIgralec;
    }

    // Brisanje presežnih igralcev (več kot 5)
    int st_igralcev = 0;
    Igralec* trenutni = seznam;
    while (trenutni != nullptr) {
        st_igralcev++;
        if (st_igralcev == 5 && trenutni->naslednji != nullptr) {
            Igralec* zaIzbris = trenutni->naslednji;
            trenutni->naslednji = nullptr;
            delete zaIzbris;
        }
        trenutni = trenutni->naslednji;
    }
}

// Funkcija za branje scenarijev iz datoteke
void preberiScenarije(const char* Scenariji, char scenariji[11][500]) {
    ifstream datoteka(Scenariji);
    if (!datoteka) {
        cerr << "Napaka pri odpiranju datoteke scenarijev!" << endl;
        exit(1);
    }

    char vrstica[500];
    int indeks = 0;
    char trenutniScenarij[500] = "";

    while (datoteka.getline(vrstica, 500)) {
        if (strcmp(vrstica, "---") == 0) {
            strcpy(scenariji[indeks++], trenutniScenarij);
            strcpy(trenutniScenarij, "");
        } else {
            strcat(trenutniScenarij, vrstica);
            strcat(trenutniScenarij, "\n");
        }
    }
    if (strlen(trenutniScenarij) > 0) {
        strcpy(scenariji[indeks], trenutniScenarij);
    }

    datoteka.close();
}

// Funkcija za branje gesel iz datoteke
void preberiGesla(const char* Gesla, char gesla[][100], int& stGesel, int dolzina) {
    ifstream datoteka(Gesla);
    if (!datoteka) {
        cerr << "Napaka pri odpiranju datoteke gesel!" << endl;
        exit(1);
    }

    char vrstica[100];
    stGesel = 0;

    while (datoteka.getline(vrstica, 100)) {
        if (strlen(vrstica) == dolzina) {
            strcpy(gesla[stGesel++], vrstica);
        }
    }

    datoteka.close();
}

// Funkcija za preverjanje črke
bool preveriCrko(char crka, const char* geslo, char* trenutno) {
    bool uganjeno = false;
    for (size_t i = 0; i < strlen(geslo); ++i) {
        if (geslo[i] == crka && trenutno[i] == '_') {
            trenutno[i] = crka;
            uganjeno = true;
        }
    }
    return uganjeno;
}

// Funkcija za igro vislic
void igrajVislice(Igralec* igralec, const char* geslo, char scenariji[10][500]) {
    char trenutno[100];
    for (size_t i = 0; i < strlen(geslo); ++i) trenutno[i] = '_';
    trenutno[strlen(geslo)] = '\0';

    int poskusi = 10;
    char ugibana_crka;
    igralec->st_ugibanj = 0;
    memset(igralec->zgodovina_ugibanj, 0, sizeof(igralec->zgodovina_ugibanj));

    time_t start_time = time(nullptr);

    while (poskusi > 0 && strcmp(trenutno, geslo) != 0) {
        cout << "\nGeslo: " << trenutno << "\n";
        cout << scenariji[10 - poskusi] << endl;
        cout << "Preostali poskusi: " << poskusi << "\n";
        cout << "Vnesite črko: ";
        cin >> ugibana_crka;

        ugibana_crka = tolower(ugibana_crka);

        if (!isalpha(ugibana_crka)) {
            cout << "Prosimo, vnesite veljavno črko.\n";
            continue;
        }

        bool ze_ugibano = false;
        for (int i = 0; i < igralec->st_ugibanj; i++) {
            if (igralec->zgodovina_ugibanj[i] == ugibana_crka) {
                ze_ugibano = true;
                break;
            }
        }

        if (ze_ugibano) {
            cout << "To črko ste že ugibali!\n";
            continue;
        }

        igralec->zgodovina_ugibanj[igralec->st_ugibanj++] = ugibana_crka;

        if (preveriCrko(ugibana_crka, geslo, trenutno)) {
            cout << "Pravilno!\n";
        } else {
            cout << "Napačno!\n";
            --poskusi;
        }

        igralec->potrebni_cas = difftime(time(nullptr), start_time);

        if (strcmp(trenutno, geslo) == 0) {
            cout << "Čestitke, uganili ste geslo: " << geslo << "!\n";
            return;
        }
    }

    if (strcmp(trenutno, geslo) != 0) {
        cout << scenariji[10] << "\n";
        cout << "Izgubili ste. Geslo je bilo: " << geslo << ".\n";
    }
}

// Funkcija za izpis statistike v datoteko
void izpisiStatistikoVDatoteko(Igralec* seznam, const char* imeDatoteke) {
    ofstream datoteka(imeDatoteke);
    if (!datoteka) {
        cerr << "Napaka pri odpiranju datoteke za zapis statistike!" << endl;
        exit(1);
    }

    datoteka << "Ime\t,Vpisna številka\t,Geslo\t,Število ugibanj\t,Čas[s]\n";

    Igralec* trenutni = seznam;
    while (trenutni != nullptr) {
        datoteka << trenutni->ime << "," <<"\t\t\t" << trenutni->vpisna_stevilka << "," << "\t\t\t" << trenutni->trenutno_geslo << "," << "\t\t\t"
                 << trenutni->st_ugibanj << "," << "\t\t\t" << trenutni->potrebni_cas << "\n";
        trenutni = trenutni->naslednji;
    }

    datoteka.close();
}

int main() {
    char scenariji[11][500];
    char gesla[100][100];
    int stGesel;

    preberiScenarije("Hangman_scenariji.txt", scenariji);

    int dolzinaGesla;
    cout << "Vnesite dolžino gesel za igro: ";
    cin >> dolzinaGesla;
    preberiGesla("Hangman_gesla.txt", gesla, stGesel, dolzinaGesla);

    Igralec* seznam = nullptr;

    for (int i = 0; i < 10; ++i) {
        Igralec* novIgralec = new Igralec;

        cout << "Vnesite ime igralca: ";
        cin >> novIgralec->ime;
        while (true) {
            cout << "Vnesite vpisno številko: ";
            cin >> novIgralec->vpisna_stevilka;

            // Preverjanje za veljavno celo število
            if (cin.fail()) {
                cout << "Napaka: Prosimo, vnesite veljavno celo število za vpisno številko." << endl;
                cin.clear();             // Počisti napako
                cin.ignore(1000, '\n');  // Ignorira preostale napačne vnose
            } else {
                break; // Veljaven vnos
            }
        }

        srand(time(nullptr) + i);
        strcpy(novIgralec->trenutno_geslo, gesla[rand() % stGesel]);

        igrajVislice(novIgralec, novIgralec->trenutno_geslo, scenariji);

        dodajIgralca(seznam, novIgralec);
    }

    izpisiStatistikoVDatoteko(seznam, "statistika.csv");

    while (seznam != nullptr) {
        Igralec* zaIzbris = seznam;
        seznam = seznam->naslednji;
        delete zaIzbris;
    }

    return 0;
}
