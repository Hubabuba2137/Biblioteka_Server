#include <crow.h>
#include <iostream>
#include <string>
#include "Klasy.h"

vector<string> get_rejestracja(string body) {
	string imie;
	string nazwisko;
	string email;
	string adres;
	string tel;
	string login;
	string haslo;
	int pos = 0;

	imie = body.substr(9, body.length());
	char c = 34;// 34 to kod ASCII znaku "
	pos = login.find(c);
	nazwisko = imie.substr(pos + 14, imie.length());
	imie.substr(0, pos);
	pos = nazwisko.find(c);
	email = nazwisko.substr(pos + 14, nazwisko.length());
	nazwisko.substr(0, pos);
	pos = email.find(c);
	adres = email.substr(pos + 11, email.length());
	email.substr(0, pos);
	pos = adres.find(c);
	tel = stoi(adres.substr(pos + 13, adres.length()));
	adres.substr(0, pos);
	pos = tel.find(c);
	login = tel.substr(pos + 11, tel.length());
	tel.substr(0, pos);
	pos = login.find(c);
	haslo = login.substr(pos + 11, login.length());
	login.substr(0, pos);
	pos = haslo.find(c);
	haslo.substr(0, pos);

	vector<string> dane;
	dane.push_back(imie);
	dane.push_back(nazwisko);
	dane.push_back(email);
	dane.push_back(adres);
	dane.push_back(tel);
	dane.push_back(login);
	dane.push_back(haslo);

	return dane;
}

int main()
{
    crow::SimpleApp app;

	BazaKsiazek baza_ksiazek;
	BazaUzytkownikow baza_uzytkownikow;
	BazaSkrytek baza_skrytek;

	baza_ksiazek.add("Pan Tadeusz", "Adam Mickiewicz", 121);
	baza_ksiazek.add("Ogniem i mieczem", "Henryk Sienkiewicz", 122);
	baza_ksiazek.add("Lalka", "Bolesław Prus", 123);
	baza_ksiazek.add("Quo vadis", "Henryk Sienkiewicz", 124);

    CROW_ROUTE(app, "/")([]() {
        auto page = crow::mustache::load_text("main.html");
        return page;
		});

    CROW_ROUTE(app, "/main.html")([]() {
        auto page = crow::mustache::load_text("main.html");
        return page;
		});

	CROW_ROUTE(app, "/logowanie.html")
		([](const crow::request& req) {
		auto page = crow::mustache::load_text("logowanie.html");
		return page;
		});

	CROW_ROUTE(app, "/rejestracja.html")
		([](const crow::request& req) {
		auto page = crow::mustache::load_text("rejestracja.html");
		return page;
		});

	CROW_ROUTE(app, "/wyszukanie.html")
		([](const crow::request& req) {
		auto page = crow::mustache::load_text("wyszukanie.html");
		return page;
			});

	CROW_ROUTE(app, "/api/login").methods(crow::HTTPMethod::POST)([&baza_uzytkownikow](const crow::request& req) {
		auto body = req.body;
		string login = body;
		login.erase(0, 10);
		char c = 34;
		int pos = login.find(c);

		string haslo = body;
		haslo.erase(0, pos + 13);

		login.erase(pos, login.size());
		haslo.erase(0, 11);
		haslo.pop_back();
		haslo.pop_back();

		std::cout << login << std::endl;
		std::cout << haslo << std::endl; 
		
		try {
			if (baza_uzytkownikow.wyszukaj_czy_jest(login)) {
				auto user = baza_uzytkownikow.wyszukaj(login);
				if (user.checkPass(haslo)) {
					return crow::response(200, "Poprawne logowanie");
				}
				else {
					return crow::response(401, "Bledne haslo");
				}
			}
			else {
				return crow::response(404, "Nie znaleziono uzytkownika");
			}
		}
		catch (const std::exception& e) {
			return crow::response(500, e.what());
		}
	});

	CROW_ROUTE(app, "/api/rejestracja").methods(crow::HTTPMethod::POST)([&baza_uzytkownikow](const crow::request& req) {
		auto body = req.body;
		
		string imie;
		string nazwisko;
		string email;
		string adres;
		string tel;
		string login;
		string haslo;
		int pos = 0;

		imie = body.substr(9, body.length());
		char c = 34;// 34 to kod ASCII znaku "
		pos = imie.find(c);
		nazwisko = imie.substr(pos + 14, imie.length());
		imie = imie.substr(0, pos);
		pos = nazwisko.find(c);
		email = nazwisko.substr(pos + 11, nazwisko.length());
		nazwisko = nazwisko.substr(0, pos);
		pos = email.find(c);
		adres = email.substr(pos + 11, email.length());
		email = email.substr(0, pos);
		pos = adres.find(c);
		tel = adres.substr(pos + 13, adres.length());
		adres = adres.substr(0, pos);
		pos = tel.find(c);
		login = tel.substr(pos + 11, tel.length());
		tel = tel.substr(0, pos);
		pos = login.find(c);
		haslo = login.substr(pos + 11, login.length());
		login = login.substr(0, pos);
		pos = haslo.find(c);
		haslo = haslo.substr(0, pos);

		std::cout << imie << ", " << nazwisko << ", " << email << ", " << adres << ", " << tel << ", " << login << ", " << haslo << std::endl;
		int telefon = std::stoi(tel);

		try {
			if (baza_uzytkownikow.wyszukaj_czy_jest(login)) {
				return crow::response(409, "User already exists");
			}
			else {
				baza_uzytkownikow.add(imie, nazwisko, email, adres, telefon, login, haslo);
				return crow::response(200, "User registered successfully");
			}
		}
		catch (const std::exception& e) {
			return crow::response(500, e.what());
		}
		});

	CROW_ROUTE(app, "/api/wyszukaj").methods(crow::HTTPMethod::POST)([&baza_ksiazek](const crow::request& req) {
		auto body = crow::json::load(req.body);
		if (!body) {
			return crow::response(400, "Invalid JSON format");
		}

		std::string typ = body["typ"].s(); // "tytul" lub "isbn"
		std::string wartosc = body["wartosc"].s();

		try {
			if (typ == "tytul") {
				if (baza_ksiazek.wyszukaj_czy_jest(wartosc)) {
					auto ksiazka = baza_ksiazek.wyszukaj((wartosc));
					crow::json::wvalue result;
					result["tytul"] = ksiazka.tytul;
					result["autor"] = ksiazka.autor;
					result["ISBN"] = ksiazka.ISBN;
					return crow::response(result);
				}
				else {
					return crow::response(404, "Nie znaleziono książki o podanym tytule");
				}
			}
			else if (typ == "isbn") {
				int isbn = std::stoi(wartosc);
				if (baza_ksiazek.wyszukaj_czy_jest(isbn)) {
					auto ksiazka = baza_ksiazek.wyszukaj(isbn);
					crow::json::wvalue result;
					result["tytul"] = ksiazka.tytul;
					result["autor"] = ksiazka.autor;
					result["ISBN"] = ksiazka.ISBN;
					return crow::response(result);
				}
				else {
					return crow::response(404, "Nie znaleziono książki o podanym ISBN");
				}
			}
			else {
				return crow::response(400, "Nieprawidłowy typ wyszukiwania");
			}
		}
		catch (const std::exception& e) {
			return crow::response(500, e.what());
		}
		});


    app.port(8080).multithreaded().run();
}
