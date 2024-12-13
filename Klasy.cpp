#include <iostream>
#include <string>
#include <vector>
#include "Klasy.h"
#include <fstream>
#include <algorithm>

using namespace std;

Ksiazka::Ksiazka() : id(0), ISBN(0) {}

Ksiazka::Ksiazka(string t, string au, int isbn)
	: tytul(t), autor(au), ISBN(isbn), id(0) {}

std::ostream& operator<<(std::ostream& os, const Ksiazka& k) {
	os << "[" << k.id << ", " << k.tytul << ", " << k.autor << ", " << k.ISBN << "]\n";
	return os;
}

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
	if (!plik) return;

	string tytul, autor;
	int ISBN, id;
	char format;

	while (plik >> id >> tytul >> autor >> ISBN) {
		Ksiazka ksiazka(tytul, autor, ISBN);
		ksiazka.id = id;
		lista_ksiazek.push_back(ksiazka);
	}
	plik.close();
}

void BazaKsiazek::saveToFile() {
	ofstream plik(nazwa_pliku, ios::trunc);
	for (const auto& ksiazka : lista_ksiazek) {
		plik << ksiazka.id << " " << ksiazka.tytul << " " << ksiazka.autor << " "
			<< ksiazka.ISBN << "\n";
	}
	plik.close();
}

void BazaKsiazek::add(string t, string au, int isbn) {
	Ksiazka nowa(t, au, isbn);
	nowa.id = lista_ksiazek.size() + 1;
	lista_ksiazek.push_back(nowa);

	// Dodanie do pliku
	ofstream plik(nazwa_pliku, ios::app);
	plik << nowa.id << " " << nowa.tytul << " " << nowa.autor << " "
		<< nowa.ISBN <<"\n";
	plik.close();
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

Czytelnik::Czytelnik(string i, string n, string e, string a, int tel, string l, string h)
	: imie(i), nazwisko(n), email(e), adres(a), numer_tel(tel), login(l), haslo(h), id(0) {}

Czytelnik::Czytelnik() : numer_tel(0), id(0) {}

bool Czytelnik::checkPass(const string& pass) const {
	return haslo == pass;
}

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
	nowy.id = lista_czytelnikow.size() + 1;
	lista_czytelnikow.push_back(nowy);

	// Dodanie do pliku
	ofstream plik(nazwa_pliku, ios::app);
	plik << nowy.id << " " << nowy.imie << " " << nowy.nazwisko << " "
		<< nowy.email << " " << nowy.adres << " " << nowy.numer_tel << " "
		<< nowy.login << " " << nowy.haslo << "\n";
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
	for (const auto& czytelnik : lista_czytelnikow) {
		plik << czytelnik.id << " " << czytelnik.imie << " " << czytelnik.nazwisko << " "
			<< czytelnik.email << " " << czytelnik.adres << " " << czytelnik.numer_tel << " "
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

	std::cout << "Nie znaleziono u¿ytkownika!" << std::endl;
}

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

void Bibliotekarz::dodanie_ksiazki(string t, string au, int isbn, BazaKsiazek baza) {
	baza.add(t, au, isbn);
}

/*
void Bibliotekarz::usuniecie_ksiazki(Ksiazka k, BazaKsiazek baza) {
	baza.remove_ksiazke(k);
}*/

Skrytka::Skrytka() : id(0), wolna(true), numer_wypozyczenia(-1) {}

Skrytka::Skrytka(int id, bool wolna, int numer_wypozyczenia)
	: id(id), wolna(wolna), numer_wypozyczenia(numer_wypozyczenia) {}

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

