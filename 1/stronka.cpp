#include <crow.h>
#include <iostream>
#include <string>
#include "Klasy.h"
#include <unordered_map>
#include <random>

using namespace std;

std::string generateSessionID() {
	static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	std::string sessionID="";
	for (int i = 0; i < 16; ++i) {
		sessionID += alphanum[rand() % (sizeof(alphanum) - 1)];
	}
	return sessionID;
}

std::string serve_file(const std::string& file_path) {
	std::ifstream file(file_path, std::ios::binary);
	if (!file.is_open()) {
		throw std::runtime_error("Nie można otworzyć pliku: " + file_path);
	}

	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return content;
}

int main()
{
	srand(time(nullptr));
    crow::SimpleApp app;

	BazaKsiazek baza_ksiazek;
	BazaUzytkownikow baza_uzytkownikow;
	BazaSkrytek baza_skrytek;

	std::unordered_map<std::string, std::string> sessions;

    CROW_ROUTE(app, "/")([&sessions](const crow::request& req) {
		std::string sessionID = req.get_header_value("Cookie");
		std::string login = "";

		if (!sessionID.empty() && sessions.count(sessionID)) {
			login = sessions[sessionID];
		}

		auto page = crow::mustache::load("main.html");
		crow::mustache::context ctx;
		ctx["login"] = login; // Wstaw login do strony
		return page.render(ctx); // Użyj renderowania strony
		});

    CROW_ROUTE(app, "/main.html")([&sessions](const crow::request& req) {
		std::string sessionID = req.get_header_value("Cookie");
		std::string login = "";

		if (!sessionID.empty() && sessions.count(sessionID)) {
			login = sessions[sessionID];
		}

		auto page = crow::mustache::load("main.html");
		crow::mustache::context ctx;
		ctx["login"] = login; 
		return page.render(ctx);
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

	CROW_ROUTE(app, "/main_log.html")([&sessions](const crow::request& req) {
		std::string sessionID = req.get_header_value("Cookie");
		sessionID.erase(0, 10);//usuwamy prefiks sessionID=
		std::string login;
		
		if (!sessionID.empty() && sessions.count(sessionID)) {
			login = sessions[sessionID];
		}

		std::cout << "Login: " << login << std::endl;

		auto page = crow::mustache::load("main_log.html");
		crow::mustache::context ctx;
		ctx["login"] = login; // Wstaw login do szablonu
		return page.render(ctx); // Renderuj stronę z loginem
		});

	CROW_ROUTE(app, "/wyszukanie_log.html")
		([&sessions](const crow::request& req) {
		std::string sessionID = req.get_header_value("Cookie");
		sessionID.erase(0, 10);//usuwamy prefiks sessionID=
		std::string login;

		if (!sessionID.empty() && sessions.count(sessionID)) {
			login = sessions[sessionID];
		}

		auto page = crow::mustache::load("wyszukanie_log.html");
		crow::mustache::context ctx;
		ctx["login"] = login;

		return page.render(ctx);
			});

	CROW_ROUTE(app, "/api/login").methods(crow::HTTPMethod::POST)([&baza_uzytkownikow, &sessions](const crow::request& req) {\
		//cout << req.body << endl;
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
		
		try {
			if (baza_uzytkownikow.wyszukaj_czy_jest(login)) {
				auto user = baza_uzytkownikow.wyszukaj(login);
				if (user.checkPass(haslo)) {
					std::string sessionID = generateSessionID();
					sessions[sessionID] = login; // Zapisanie sesji w mapie
					cout << "Zalogowano: " << login <<" w sesji: "<<sessionID<< endl;
					crow::response res(200, "Poprawne logowanie");
					res.add_header("Set-Cookie", "sessionID=" + sessionID + "; Path=/; HttpOnly");

					return res;
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

		//std::cout << imie << ", " << nazwisko << ", " << email << ", " << adres << ", " << tel << ", " << login << ", " << haslo << std::endl;
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

	CROW_ROUTE(app, "/api/wyszukaj_log").methods(crow::HTTPMethod::POST)([&baza_ksiazek](const crow::request& req) {
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

	CROW_ROUTE(app, "/static/<string>")([](const std::string& file_name) {
		try {
			std::string file_path = "./static/" + file_name; // Katalog "static"
			std::string content = serve_file(file_path);
			return crow::response(200, content);
		}
		catch (const std::exception& e) {
			return crow::response(404, "Nie znaleziono pliku.");
		}
		});

    app.port(8080).multithreaded().run();
}
