#include <iostream>
#include <string>
#include <vector>
#include "Klasy.h"
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <ctime>

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
		saveToFile();
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





//Bibliotekarz

Bibliotekarz::Bibliotekarz() {}

void Bibliotekarz::dodanie_ksiazki(string t, string au, int isbn, string okl, BazaKsiazek baza) {
	baza.add(t, au, isbn, okl);
}


void Bibliotekarz::usuniecie_ksiazki(Ksiazka k, BazaKsiazek baza) {
	int isbn = k.ISBN;
	baza.usun_ksiazke(isbn);
}

void Bibliotekarz::akceptacja_wyp(BazaWypozyczen baza_wyp, int num) {
	//num to numer które wypo¿yczenie akceptujemy i przenosimy z poczekalni
	vector<Wypozyczenie> wypozyczenia = baza_wyp.getlista_wypozyczen_pocz();


	baza_wyp.add(wypozyczenia[num].data_wypozyczenia, wypozyczenia[num].data_oddania, wypozyczenia[num].numer_skrytki, wypozyczenia[num].isbn_ksiazki, wypozyczenia[num].login_czytelnika);
}




//Dane osobowe
DaneOsobowe::DaneOsobowe() : numer_tel(0) {}
DaneOsobowe::DaneOsobowe(string i, string n, string e, string a, int tel){

	imie = i;
	nazwisko = n;
	email = e;
	adres = a;
	numer_tel = tel;

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


void Czytelnik::wypozycz(int isbn, BazaKsiazek baza_ksiazek, BazaWypozyczen baza_wyp, BazaSkrytek baza_skrytek) {
	string login = this->login;

	if (baza_ksiazek.wyszukaj_czy_jest(isbn)) {
		baza_wyp.przyznanieWyp(login, isbn, baza_skrytek);
	}
	else {
		throw runtime_error("Nie znaleziono ksi¹¿ki o podanym ISBN.");
	}
	
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
	plik << nowy.id << " " << nowy.dane_osobowe.imie<< " " << nowy.dane_osobowe.nazwisko<< " "
		<< nowy.dane_osobowe.email << " " << nowy.dane_osobowe.adres<< " " << nowy.dane_osobowe.numer_tel << " "
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
		cout << imie << " " << nazwisko << " " << email << " " << adres << " " << numer_tel << " " << login << " " << haslo << endl;
		Czytelnik czytelnik(imie, nazwisko, email, adres, numer_tel, login, haslo);
		czytelnik.id = id; // Przypisujemy ID
		lista_czytelnikow.push_back(czytelnik);
	}

	plik.close();
}


void BazaUzytkownikow::saveToFile() {

	ofstream plik(nazwa_pliku, ios::trunc);
	for (Czytelnik czytelnik : lista_czytelnikow) {
		plik << czytelnik.id << " " << czytelnik.dane_osobowe.imie << " " << czytelnik.dane_osobowe.nazwisko << " "
			<< czytelnik.dane_osobowe.email << " " << czytelnik.dane_osobowe.adres << " " << czytelnik.dane_osobowe.numer_tel << " "
			<< czytelnik.login << " " << czytelnik.haslo << "\n";
	}
	plik.close();
}

void BazaUzytkownikow::setUsers() {
	loadFromFile();
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
Wypozyczenie::Wypozyczenie(time_t dat_wyp, time_t dat_od, int skr, int num_ks, string log) {
	data_wypozyczenia = dat_wyp;
	data_oddania = dat_od;
	numer_skrytki = skr;
	isbn_ksiazki = num_ks;
	login_czytelnika = log;
}

ostream& operator<<(ostream& os, const Wypozyczenie& w) {
	os << "Data wypo¿yczenia: " << w.data_wypozyczenia << "\nData oddania: " << w.data_oddania << "\nNumer skrytki: " << w.numer_skrytki << "\nISBN ksi¹¿ki: " << w.isbn_ksiazki << "\nLogin u¿ytkownika: "<<w.login_czytelnika<<"\n";
	return os;
}

string Wypozyczenie::getLogin() {
	return this->login_czytelnika;
}








//Skrytka
Skrytka::Skrytka() : id(0), wolna(true), numer_wypozyczenia(-1) {}

Skrytka::Skrytka(int id, bool wolna, int numer_wypozyczenia)
	: id(id), wolna(wolna), numer_wypozyczenia(numer_wypozyczenia) {}

void Skrytka::setID(int id) {
	this->id = id;
}




//Baza skrytek
BazaSkrytek::BazaSkrytek(int num) {
	ifstream plik(nazwa_pliku);
	if (!plik) {
		// Jeœli plik nie istnieje, tworzymy now¹ listê skrytek
		for (int i = 0; i < num; i++) {
			int id = i + 1;
			bool wolna = true;
			int numer_wypozyczenia = -1;
			Skrytka nowa(id, wolna, numer_wypozyczenia);
			lista_skrytek.push_back(nowa);
		}
		saveToFile(); // Zapisujemy nowo utworzon¹ listê do pliku
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

void BazaSkrytek::setSkrytki() {
	loadFromFile();
}

void BazaSkrytek::add() {
	int id = lista_skrytek.size() + 1;
	int numer_wypozyczenia = -1;
	bool wolna = true;

	Skrytka nowa(id, wolna, numer_wypozyczenia);
	lista_skrytek.push_back(nowa);

	saveToFile(); // Zapisujemy aktualizowan¹ listê do pliku
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

int BazaSkrytek::getFirstFree() {
	for (int j = 0; j < this->lista_skrytek.size(); j++) {
		if (this->lista_skrytek[j].wolna == true) {
			lista_skrytek[j].wolna = false;
			return j;
		}
	}
}

ostream& operator<<(ostream& os, const Skrytka& skrytka) {
	os << "[" << skrytka.id << ", wolna: " << (skrytka.wolna ? "tak" : "nie")
		<< ", numer wypo¿yczenia: " << skrytka.numer_wypozyczenia << "]\n";
	return os;
}





//Baza wypo¿yczeñ
BazaWypozyczen::BazaWypozyczen() {
	ifstream plik(nazwa_pliku);
	if (!plik) {
		ofstream nowy_plik(nazwa_pliku); // Tworzymy plik, jeœli nie istnieje
	}
	else {
		loadFromFile();
	}
}

void BazaWypozyczen::loadFromFile() {
	lista_wypozyczen.clear();
	ifstream plik(nazwa_pliku);
	if (!plik) return;
	time_t data_wyp, data_od;
	string log;
	int skr, num_ks;
	while (plik >> data_wyp >> data_od >> skr >> num_ks) {
		Wypozyczenie wypozyczenie(data_wyp, data_od, skr, num_ks, log);
		lista_wypozyczen.push_back(wypozyczenie);
	}
	plik.close();
}

void BazaWypozyczen::saveToFile() {
	ofstream plik(nazwa_pliku, ios::trunc);
	for (const auto& wypozyczenie : lista_wypozyczen) {
		plik << wypozyczenie.data_wypozyczenia << " " << wypozyczenie.data_oddania << " " << wypozyczenie.numer_skrytki << " " << wypozyczenie.isbn_ksiazki << " " << wypozyczenie.login_czytelnika << "\n";
	}
	plik.close();
}

void BazaUzytkownikow::add(string i, string n, string e, string a, int tel, string l, string h) {
	Czytelnik nowy(i, n, e, a, tel, l, h);
	nowy.id = int(lista_czytelnikow.size() + 1);
	lista_czytelnikow.push_back(nowy);

	ofstream plik(nazwa_pliku, ios::app);
	if (!plik) {
		cerr << "B³¹d przy otwieraniu pliku: " << nazwa_pliku << endl;
	}
	else {
		plik << nowy.id << " " << nowy.dane_osobowe.imie << " " << nowy.dane_osobowe.nazwisko
			<< " " << nowy.dane_osobowe.email << " " << nowy.dane_osobowe.adres
			<< " " << nowy.dane_osobowe.numer_tel << " " << nowy.login << " " << nowy.haslo << "\n";
		plik.close();
	}

	cout << "Dodano u¿ytkownika: " << nowy.dane_osobowe.imie << " " << nowy.dane_osobowe.nazwisko << endl;

	loadFromFile(); // Aktualizacja danych w pamiêci
}

void BazaWypozyczen::end_wyp(int num_ks) {
	auto it = std::find_if(lista_wypozyczen.begin(), lista_wypozyczen.end(), [&](const Wypozyczenie& w) {
		return w.isbn_ksiazki == num_ks;
		});
	if (it != lista_wypozyczen.end()) {
		lista_wypozyczen.erase(it);
		saveToFile();
	}
}

void BazaWypozyczen::setWyp() {
	loadFromFile();
}

void BazaUzytkownikow::loadFromFile() {
	lista_czytelnikow.clear();

	ifstream plik(nazwa_pliku);
	if (!plik) {
		cerr << "B³¹d przy otwieraniu pliku: " << nazwa_pliku << endl;
		return;
	}

	string linia;
	while (getline(plik, linia)) {
		stringstream ss(linia);
		string id_str, imie, nazwisko, email, adres, numer_tel_str, login, haslo;

		if (ss >> id_str >> imie >> nazwisko >> email >> ws) {
			getline(ss, adres, ' ');
			ss >> numer_tel_str >> login >> haslo;

			int id = stoi(id_str);
			int numer_tel = stoi(numer_tel_str);

			Czytelnik czytelnik(imie, nazwisko, email, adres, numer_tel, login, haslo);
			czytelnik.id = id;
			lista_czytelnikow.push_back(czytelnik);
		}
	}

	plik.close();
}

void BazaWypozyczen::saveToFile_pocz() {
	ofstream plik(nazwa_pliku_pocz, ios::trunc);
	for (const auto& wypozyczenie : lista_wypozyczen) {
		plik << wypozyczenie.isbn_ksiazki << " " << wypozyczenie.login_czytelnika << "\n";
	}
	plik.close();
}

void BazaWypozyczen::przyznanieWyp(string login, int isbn, BazaSkrytek baza_skrytek) {
	bool czy_mozna = false;

	//sprawdzenie czy czytelnik mo¿e wypo¿yczyæ ksi¹¿kê itd.

	czy_mozna = true;

	//przyznanie wypo¿yczenia
	if (czy_mozna == true) {
		int skr = baza_skrytek.getFirstFree();

		if (skr == -1) {
			cout << "Brak wolnych skrytek\n";
			return;
		}
		else{
			//dodanie wypo¿yczenia do poczekalni do akceptacji przez bibliotekarza
			lista_wypozyczen_pocz.push_back(Wypozyczenie(time(0), time(0), skr, isbn, login));
			saveToFile_pocz();
		}
	}
	else {
		cout << "Nie mo¿na wypo¿yczyæ ksi¹¿ki\n";
	}
}

vector<Wypozyczenie> BazaWypozyczen::getlista_wypozyczen_pocz() {
	loadFromFile_pocz();
	return lista_wypozyczen_pocz;
}