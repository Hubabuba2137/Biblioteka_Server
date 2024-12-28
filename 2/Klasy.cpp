#include <iostream>
#include <string>
#include <vector>
#include "Klasy.h"
#include <fstream>
#include <algorithm>
#include <sstream>

using namespace std;



//Ksia¿ki
Ksiazka::Ksiazka() : id(0), ISBN(0), tytul(""), autor(""), okladka("") {}

Ksiazka::Ksiazka(string t, string au, int isbn, string okl)
	: tytul(std::move(t)), autor(std::move(au)), ISBN(isbn), okladka(std::move(okl)) {}


std::ostream& operator<<(std::ostream& os, const Ksiazka& k) {
	os << "[" << k.id << ", " << k.tytul << ", " << k.autor << ", " << k.ISBN << "]\n";
	return os;
}




//Baza Ksi¹¿ek

BazaKsiazek::BazaKsiazek() {
	ifstream plik(nazwa_pliku);
	if (!plik) {
		ofstream nowy_plik(nazwa_pliku); // Tworzymy plik, jeœli nie istnieje
	}
	else {
		loadFromFile();
	}
}

void BazaKsiazek::loadFromFile() {
	lista_ksiazek.clear();

	ifstream plik(nazwa_pliku);
	if (!plik) {
		std::cerr << "Nie mo¿na otworzyæ pliku: " << nazwa_pliku << std::endl;
		return;
	}

	string linia;
	while (std::getline(plik, linia)) {
		std::stringstream ss(linia);
		string id_str, tytul, autor, isbn_str, okladka;

		// Rozdziel linie na poszczególne czêœci, oddzielone przecinkami
		if (std::getline(ss, id_str, ',') &&
			std::getline(ss, tytul, ',') &&
			std::getline(ss, autor, ',') &&
			std::getline(ss, isbn_str, ',') &&
			std::getline(ss, okladka)) {

			// Usuñ ewentualne spacje na pocz¹tku i koñcu
			tytul = trim(tytul);
			autor = trim(autor);
			okladka = trim(okladka);

			int id = std::stoi(trim(id_str));
			int isbn = std::stoi(trim(isbn_str));

			// Utwórz obiekt Ksiazka i dodaj go do wektora
			Ksiazka ksiazka(tytul, autor, isbn, okladka);
			ksiazka.id = id;
			lista_ksiazek.push_back(ksiazka);
		}
	}

	plik.close();
}

void BazaKsiazek::saveToFile() {
	ofstream plik(nazwa_pliku, ios::trunc);
	if (!plik) {
		std::cerr << "Nie mo¿na otworzyæ pliku do zapisu: " << nazwa_pliku << std::endl;
		return;
	}

	for (const auto& ksiazka : lista_ksiazek) {
		plik << ksiazka.id << ", " << ksiazka.tytul << ", " << ksiazka.autor << ", "
			<< ksiazka.ISBN << ", " << ksiazka.okladka << "\n";
	}

	plik.close();
}

// Funkcja pomocnicza do usuwania spacji z pocz¹tku i koñca stringa
static std::string trim(const std::string& str) {
	const auto strBegin = str.find_first_not_of(" \t");
	const auto strEnd = str.find_last_not_of(" \t");
	const auto strRange = strEnd - strBegin + 1;

	return strBegin == std::string::npos ? "" : str.substr(strBegin, strRange);
}

void BazaKsiazek::add(string t, string au, int isbn, string okl) {
	if (wyszukaj_czy_jest(isbn)) {
		throw runtime_error("Ksi¹¿ka o podanym ISBN ju¿ istnieje.");
	}
	else {
		if (wyszukaj_czy_jest(t)) {
			throw runtime_error("Ksi¹¿ka o podanym tytule ju¿ istnieje.");
		}
		else {
			Ksiazka nowa(t, au, isbn, okl);
			nowa.id = int(lista_ksiazek.size() + 1);
			lista_ksiazek.push_back(nowa);

			// Dodanie do pliku
			ofstream plik(nazwa_pliku, ios::app);
			plik << nowa.id << ", " << nowa.tytul << ", " << nowa.autor << ", "
				<< nowa.ISBN << ", " << nowa.okladka << "\n";
			plik.close();

			this->loadFromFile(); // Aktualizacja listy ksi¹¿ek
		}
	}
}
void BazaKsiazek::usun_ksiazke(int isbn) {
	auto it = std::find_if(lista_ksiazek.begin(), lista_ksiazek.end(), [&](const Ksiazka& k) {
		return k.ISBN == isbn;
		});

	if (it != lista_ksiazek.end()) {
		lista_ksiazek.erase(it);
	}
}


const vector<Ksiazka>& BazaKsiazek::getBooks() const {
	return lista_ksiazek;
}

void BazaKsiazek::setBooks() {
	loadFromFile();
}

bool BazaKsiazek::wyszukaj_czy_jest(int isbn) const {
	for (const auto& ksiazka : lista_ksiazek) {
		if (ksiazka.ISBN == isbn) {
			return true;
		}
	}
	return false;
}

bool BazaKsiazek::wyszukaj_czy_jest(string tyt) const {
	for (const auto& ksiazka : lista_ksiazek) {
		if (ksiazka.tytul == tyt) {
			return true;
		}
	}
	return false;
}

Ksiazka BazaKsiazek::wyszukaj(const std::string& tyt) const {
	for (const auto& ksiazka : lista_ksiazek) {
		if (ksiazka.tytul == tyt) {
			return ksiazka;
		}
	}
	throw std::runtime_error("Nie znaleziono ksi¹¿ki o podanym tytule.");
}

Ksiazka BazaKsiazek::wyszukaj(int isbn) const {
	for (const auto& ksiazka : lista_ksiazek) {
		if (ksiazka.ISBN == isbn) {
			return ksiazka;
		}
	}
	throw runtime_error("Nie znaleziono ksi¹¿ki o podanym ISBN.");
}




//Dane osobowe
DaneOsobowe::DaneOsobowe() : numer_tel(0) {}
DaneOsobowe::DaneOsobowe(string i, string n, string e, string a, int tel) : imie(i), nazwisko(n), email(e), adres(a), numer_tel(tel) {}

string DaneOsobowe::getImie() {
	return this->imie;
}

string DaneOsobowe::getNazwisko() {
	return this->nazwisko;
}

string DaneOsobowe::getAdres() {
	return this->adres;
}

string DaneOsobowe::getEmail() {
	return this->email;
}

int DaneOsobowe::getNumer() {
	return this->numer_tel;
}




//Czytelnik
Czytelnik::Czytelnik(string i, string n, string e, string a, int tel, string l, string h){
	DaneOsobowe Dane_osobowe(i, n, e, a, tel);
	login = l;
	haslo = h;
}

Czytelnik::Czytelnik(){
	DaneOsobowe dane_osobowe;
	login = "";
	haslo = "";
}

bool Czytelnik::checkPass(const string& pass) const {
	return haslo == pass;
}




//Baza u¿ytkowników
BazaUzytkownikow::BazaUzytkownikow() {
	ifstream plik(nazwa_pliku);
	if (!plik) {
		ofstream nowy_plik(nazwa_pliku); // Tworzymy plik, jeœli nie istnieje
	}
	else {
		loadFromFile();
	}
}

void BazaUzytkownikow::add(string i, string n, string e, string a, int tel, string l, string h) {
	Czytelnik nowy(i, n, e, a, tel, l, h);
	nowy.id = int(lista_czytelnikow.size() + 1);
	lista_czytelnikow.push_back(nowy);

	// Dodanie do pliku
	ofstream plik(nazwa_pliku, ios::app);
	plik << nowy.id << ", " << nowy.dane_osobowe.getImie() << ", " << nowy.dane_osobowe.getNazwisko() << ", "
		<< nowy.dane_osobowe.getEmail() << ", " << nowy.dane_osobowe.getAdres() << ", " << nowy.dane_osobowe.getNumer() << ", "
		<< nowy.login << ", " << nowy.haslo << "\n";
	plik.close();
}

void BazaUzytkownikow::loadFromFile() {
	lista_czytelnikow.clear();

	ifstream plik(nazwa_pliku);
	if (!plik) return;

	string imie, nazwisko, email, adres, login, haslo;
	int numer_tel, id;

	while (plik >> id >> imie >> nazwisko >> email >> adres >> numer_tel >> login >> haslo) {
		Czytelnik czytelnik(imie, nazwisko, email, adres, numer_tel, login, haslo);
		czytelnik.id = id; // Przypisujemy ID
		lista_czytelnikow.push_back(czytelnik);
	}
	plik.close();
}

void BazaUzytkownikow::saveToFile() {
	ofstream plik(nazwa_pliku, ios::trunc);
	for (auto czytelnik : lista_czytelnikow) {
		plik << czytelnik.id << " " << czytelnik.dane_osobowe.getImie() << " " << czytelnik.dane_osobowe.getNazwisko() << " "
			<< czytelnik.dane_osobowe.getEmail() << " " << czytelnik.dane_osobowe.getAdres() << " " << czytelnik.dane_osobowe.getNumer() << " "
			<< czytelnik.login << " " << czytelnik.haslo << "\n";
	}
	plik.close();
}

bool BazaUzytkownikow::wyszukaj_czy_jest(const string& login) const {
	for (const auto& czytelnik : lista_czytelnikow) {
		if (czytelnik.login == login) {
			return true;
		}
	}
	return false;
}

Czytelnik BazaUzytkownikow::wyszukaj(const string& login) const {
	for (const auto& czytelnik : lista_czytelnikow) {
		if (czytelnik.login == login) {
			return czytelnik;
		}
	}

	throw runtime_error("Nie znaleziono u¿ytkownika o podanym loginie.");
}




//Wypo¿yczenie
Wypozyczenie::Wypozyczenie(string data_wyp, string data_odd, bool zak, int numer_skr, Ksiazka  wyp_k, Czytelnik wyp_cz) {
	data_wypozyczenia = data_wyp;
	data_oddania = data_odd;
	zakonczone = zak;
	numer_skrytki = numer_skr;
	wyp_ksiazka = wyp_k;
	wyp_czyt = wyp_cz;
}

Wypozyczenie::Wypozyczenie() {
	data_wypozyczenia = "";
	data_oddania = "";
	zakonczone = false;
	numer_skrytki = 0;
}

void Wypozyczenie::end_wyp() {
	zakonczone = true;
}




//Bibliotekarz
void Bibliotekarz::dodanie_ksiazki(string t, string au, int isbn,string okl, BazaKsiazek baza) {
	baza.add(t, au, isbn, okl);
}

/*
void Bibliotekarz::usuniecie_ksiazki(Ksiazka k, BazaKsiazek baza) {
	baza.remove_ksiazke(k);
}*/




//Skrytka
Skrytka::Skrytka() : id(0), wolna(true), numer_wypozyczenia(-1) {}

Skrytka::Skrytka(int id, bool wolna, int numer_wypozyczenia)
	: id(id), wolna(wolna), numer_wypozyczenia(numer_wypozyczenia) {}




//Baza skrytek
BazaSkrytek::BazaSkrytek() {
	ifstream plik(nazwa_pliku);
	if (!plik) {
		ofstream nowy_plik(nazwa_pliku); 
	}
	else {
		loadFromFile();
	}
}

void BazaSkrytek::loadFromFile() {
	lista_skrytek.clear();

	ifstream plik(nazwa_pliku);
	if (!plik) return;

	int id, numer_wypozyczenia;
	bool wolna;

	while (plik >> id >> wolna >> numer_wypozyczenia) {
		Skrytka skrytka(id, wolna, numer_wypozyczenia);
		lista_skrytek.push_back(skrytka);
	}
	plik.close();
}

void BazaSkrytek::saveToFile() {
	ofstream plik(nazwa_pliku, ios::trunc);
	for (const auto& skrytka : lista_skrytek) {
		plik << skrytka.id << " " << skrytka.wolna << " " << skrytka.numer_wypozyczenia << "\n";
	}
	plik.close();
}

void BazaSkrytek::add(int id, bool wolna, int numer_wypozyczenia) {
	Skrytka nowa(id, wolna, numer_wypozyczenia);
	lista_skrytek.push_back(nowa);

	// Dodanie do pliku
	ofstream plik(nazwa_pliku, ios::app);
	plik << nowa.id << " " << nowa.wolna << " " << nowa.numer_wypozyczenia << "\n";
	plik.close();
}

void BazaSkrytek::zajmij(int id, int id_wyp) {
	if (id <= this->lista_skrytek.size()) {
		if (this->lista_skrytek[id].wolna = false) {
			this->lista_skrytek[id].numer_wypozyczenia = id_wyp;
			this->lista_skrytek[id].wolna = false;
		}
		else {
			cout << "Skrytka zajêta!\n";
		}
	}
	else {
		cout << "Nie ma takiej skrytki!\n";
	}
}

void BazaSkrytek::zwolnij(int id) {
	if (id <= this->lista_skrytek.size()) {
		if (this->lista_skrytek[id].wolna == false) {
			this->lista_skrytek[id].numer_wypozyczenia = 0;
			this->lista_skrytek[id].wolna = true;
		}
		else{
			cout << "Skrytka ju¿ jest wolna!\n";
		}
	}
	else {
		cout << "Nie ma takiej skrytki!\n";
	}
}

bool BazaSkrytek::wyszukaj_czy_jest(int id) const {
	for (const auto& skrytka : lista_skrytek) {
		if (skrytka.id == id) {
			return true;
		}
	}
	return false;
}

Skrytka BazaSkrytek::wyszukaj(int id) const {
	for (const auto& skrytka : lista_skrytek) {
		if (skrytka.id == id) {
			return skrytka;
		}
	}
	throw runtime_error("Nie znaleziono skrytki o podanym ID.");
}

void BazaSkrytek::usun_skrytke(int id) {
	for (size_t i = 0; i < lista_skrytek.size(); ++i) {
		if (lista_skrytek[i].id == id) {
			lista_skrytek.erase(lista_skrytek.begin() + i);
			saveToFile(); // Aktualizacja pliku
			return;
		}
	}
	throw runtime_error("Nie znaleziono skrytki o podanym ID do usuniêcia.");
}

ostream& operator<<(ostream& os, const Skrytka& skrytka) {
	os << "[" << skrytka.id << ", wolna: " << (skrytka.wolna ? "tak" : "nie")
		<< ", numer wypo¿yczenia: " << skrytka.numer_wypozyczenia << "]\n";
	return os;
}

