#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <ctime>

using namespace std;

std::string trim(const std::string& str);

class Bibliotekarz;
class Ksiazka;
class BazaKsiazek;
class DaneOsobowe;
class Czytelnik;
class BazaUzytkownikow;
class Skrytka;
class BazaSkrytek;
class Wypozyczenie;
class BazaWypozyczen;



class Ksiazka {
    friend class BazaKsiazek;
    friend ostream& operator<<(ostream& os, const Ksiazka& ksiazka);
	int id;

public:
	string tytul;
	string autor;
	string okladka;
	int ISBN;
    Ksiazka();
    Ksiazka(string t, string au, int isbn, string okl);
};

class BazaKsiazek {
	vector<Ksiazka> lista_ksiazek;
	string nazwa_pliku = "baza_ksiazek.txt";

	void loadFromFile(); 
	void saveToFile(); 
public:
	BazaKsiazek();
	void setBooks();
	const vector<Ksiazka>& getBooks() const;
	void add(string t, string au, int isbn, string okl);
	void usun_ksiazke(int isbn);
	bool wyszukaj_czy_jest(int isbn) const;
	bool wyszukaj_czy_jest(string tyt) const;
	Ksiazka wyszukaj(int isbn) const;
	Ksiazka wyszukaj(const std::string& tyt) const;
};

class DaneOsobowe {
	friend class Czytelnik;
	friend class BazaUzytkownikow;

public:
	string imie;
	string nazwisko;
	string email;
	string adres;
	int numer_tel;


	DaneOsobowe();
	DaneOsobowe(string i, string n, string e, string a, int tel);
};

class Czytelnik {
	friend class BazaUzytkownikow;
	friend class BazaWypozyczen;
	friend ostream& operator<<(ostream& os, const Czytelnik& czytelnik);

	int id;
	DaneOsobowe dane_osobowe; // Pole klasy Czytelnik
	string login;
	string haslo;

public:
	Czytelnik(string i, string n, string e, string a, int tel, string l, string h);
	Czytelnik();
	void wypozycz(int isbn, BazaKsiazek baza_ksiazek, BazaWypozyczen baza_wyp, BazaSkrytek baza_skrytek);
	bool checkPass(const string& pass) const;
};

class BazaUzytkownikow {
	vector<Czytelnik> lista_czytelnikow;
	string nazwa_pliku = "baza_czytelnikow.txt";

	void loadFromFile();
	void saveToFile();  

public:
	BazaUzytkownikow();
	void setUsers();
	void add(string i, string n, string e, string a, int tel, string l, string h);
	bool wyszukaj_czy_jest(const string& login) const;
	Czytelnik wyszukaj(const string& login) const;
};

class Bibliotekarz {
	string login;
	string haslo;
public:
	Bibliotekarz();
	void dodanie_ksiazki(string t, string au, int isbn, string okl, BazaKsiazek baza);
	void usuniecie_ksiazki(Ksiazka k, BazaKsiazek baza);
	void akceptacja_wyp(BazaWypozyczen baza_wyp, int num);
	//void przyjecie_zwr();
};

class Skrytka {
	friend class BazaSkrytek;
	friend ostream& operator<<(ostream& os, const Skrytka& skrytka);

	int id;
	bool wolna;
	int numer_wypozyczenia;

public:
	void setID(int id);
	Skrytka();
	Skrytka(int id, bool wolna, int numer_wypozyczenia);
};

class BazaSkrytek {
	vector<Skrytka> lista_skrytek;
	string nazwa_pliku = "baza_skrytek.txt";

	void loadFromFile(); // Wczytanie bazy z pliku
	void saveToFile();   // Zapis ca³ej bazy do pliku

public:
	BazaSkrytek(int num);
	void setSkrytki();
	void add();
	void zajmij(int id, int id_wyp);
	void zwolnij(int id);
	void usun_skrytke(int id);
	bool wyszukaj_czy_jest(int id) const;
	Skrytka wyszukaj(int id) const;
	int getFirstFree();
};

class Wypozyczenie {
	friend ostream& operator<<(ostream& os, const Wypozyczenie& wypozyczenie);
	friend class BazaWypozyczen;
	friend class BazaKsiazek;
	friend class BazaUzytkownikow;
	friend class BazaSkrytek;
	friend class Bibliotekarz;

	time_t data_wypozyczenia;
	time_t data_oddania;
	int numer_skrytki;
	int isbn_ksiazki;
	string login_czytelnika;
public:
	string getLogin();
	Wypozyczenie(time_t dat_wyp, time_t dat_od, int skr, int num_ks, string log);
	void end_wyp();
};

class BazaWypozyczen {
	//mamy dwie listy wypozyczen, pierwsza to lista wypozyczen, druga to lista wypozyczen w poczekalni (do akceptacji przez bibliotekarza
	//po akceptacji przenosimy wypozyczenie do listy wypozyczen

	friend class Czytelnik;
	friend class Bibliotekarz;

	string nazwa_pliku = "baza_wypozyczen.txt";
	string nazwa_pliku_pocz = "baza_wypozyczen_poczekalnia.txt";

	void loadFromFile();
	void saveToFile();

	void loadFromFile_pocz();
	void saveToFile_pocz();

	void add(time_t dat_wyp, time_t dat_od, int skr, int num_ks, string log);
public:
	BazaWypozyczen();

	vector<Wypozyczenie> lista_wypozyczen;
	vector<Wypozyczenie> lista_wypozyczen_pocz;

	vector<Wypozyczenie> getlista_wypozyczen_pocz();

	void setWyp();

	void przyznanieWyp(string login, int isbn, BazaSkrytek baza_skrytek);

	void end_wyp(int num_ks);

};
