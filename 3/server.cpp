#include <crow.h>
#include <iostream>
#include <string>
#include "Klasy.h"
#include <unordered_map>
#include <random>

using namespace std;

std::string generateSessionID() {
	static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	std::string sessionID = "";
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
	BazaSkrytek baza_skrytek(10);
	Bibliotekarz bib;
	BazaWypozyczen baza_wypozyczen;

	baza_ksiazek.setBooks();
	baza_uzytkownikow.setUsers();
	baza_skrytek.setSkrytki();
	baza_wypozyczen.setWyp();


	//sesje logowania
	std::unordered_map<std::string, std::string> sessions;

	CROW_ROUTE(app, "/")([&sessions, &baza_ksiazek](const crow::request& req) {
		std::string sessionID = req.get_header_value("Cookie");
		std::string login = "";

		if (!sessionID.empty() && sessions.count(sessionID)) {
			login = sessions[sessionID];
		}

		auto page = crow::mustache::load("main.html");
		crow::mustache::context ctx;

		ctx["login"] = login;

		// Tworzenie listy książek
		std::vector<Ksiazka> ksiazki = baza_ksiazek.getBooks();
		crow::json::wvalue::list ksiazki_list;

		for (const auto& ksiazka : ksiazki) {
			crow::json::wvalue book_ctx;
			book_ctx["tytul"] = ksiazka.tytul;
			book_ctx["autor"] = ksiazka.autor;
			book_ctx["okladka"] = ksiazka.okladka;
			ksiazki_list.push_back(std::move(book_ctx));
		}

		ctx["ksiazki"] = std::move(ksiazki_list);

		return page.render(ctx);
		});

	CROW_ROUTE(app, "/main.html")([&sessions, &baza_ksiazek](const crow::request& req) {
		std::string sessionID = req.get_header_value("Cookie");
		std::string login = "";

		if (!sessionID.empty() && sessions.count(sessionID)) {
			login = sessions[sessionID];
		}

		auto page = crow::mustache::load("main.html");
		crow::mustache::context ctx;

		ctx["login"] = login;

		// Tworzenie listy książek
		baza_ksiazek.setBooks();
		std::vector<Ksiazka> ksiazki = baza_ksiazek.getBooks();
		crow::json::wvalue::list ksiazki_list;

		for (const auto& ksiazka : ksiazki) {
			crow::json::wvalue book_ctx;
			book_ctx["tytul"] = ksiazka.tytul;
			book_ctx["autor"] = ksiazka.autor;
			book_ctx["okladka"] = ksiazka.okladka;
			ksiazki_list.push_back(std::move(book_ctx));
		}

		ctx["ksiazki"] = std::move(ksiazki_list); // Przypisanie listy książek do kontekstu

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

	CROW_ROUTE(app, "/main_log.html")([&sessions, &baza_ksiazek](const crow::request& req) {
		std::string sessionID = req.get_header_value("Cookie");
		sessionID.erase(0, 10);//usuwamy prefiks sessionID=
		std::string login;

		if (!sessionID.empty() && sessions.count(sessionID)) {
			login = sessions[sessionID];
		}

		//std::cout << "Login: " << login << std::endl;

		auto page = crow::mustache::load("main_log.html");
		crow::mustache::context ctx;

		ctx["login"] = login; // Wstaw login do szablonu

		// Tworzenie listy książek
		std::vector<Ksiazka> ksiazki = baza_ksiazek.getBooks();
		crow::json::wvalue::list ksiazki_list;

		for (const auto& ksiazka : ksiazki) {
			crow::json::wvalue book_ctx;
			book_ctx["tytul"] = ksiazka.tytul;
			book_ctx["autor"] = ksiazka.autor;
			book_ctx["okladka"] = ksiazka.okladka;
			ksiazki_list.push_back(std::move(book_ctx));
		}

		ctx["ksiazki"] = std::move(ksiazki_list); // Przypisanie listy książek do kontekstu

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
		if (login == "admin") {
			if (haslo == "admin") {
				std::string sessionID = generateSessionID();
				sessions[sessionID] = login;

				crow::response res(200, "admin");
				res.add_header("Set-Cookie", "sessionID=" + sessionID + "; Path=/; HttpOnly");

				return res;
			}
			else {
				return crow::response(401, "Bledne haslo");
			}
		}
		else if (baza_uzytkownikow.wyszukaj_czy_jest(login)) {
			auto user = baza_uzytkownikow.wyszukaj(login);
			if (user.checkPass(haslo)) {
				std::string sessionID = generateSessionID();
				sessions[sessionID] = login; // Zapisanie sesji w mapie
				//cout << "Zalogowano: " << login << " w sesji: " << sessionID << endl;
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
					result["okladka"] = ksiazka.okladka;
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
					result["okladka"] = ksiazka.okladka;
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
					result["okladka"] = ksiazka.okladka;
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
					result["okladka"] = ksiazka.okladka;
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

	CROW_ROUTE(app, "/static/<string>")
		([](const std::string& filename) {
		std::ifstream file("static/" + filename, std::ios::binary);
		if (!file.is_open()) {
			return crow::response(404);
		}
		std::ostringstream content;
		content << file.rdbuf();
		return crow::response(content.str());
			});

	CROW_ROUTE(app, "/bib_main.html")
		([](const crow::request& req) {
		auto page = crow::mustache::load_text("bib_main.html");
		return page;
			});

	CROW_ROUTE(app, "/bib_book.html")
		([&baza_ksiazek](const crow::request& req) {


		//Pobieranie i wyświetlanie listy książek
		std::vector<Ksiazka> ksiazki = baza_ksiazek.getBooks();
		crow::json::wvalue::list ksiazki_list;

		auto page = crow::mustache::load("bib_book.html");
		crow::mustache::context ctx;

		for (const auto& ksiazka : ksiazki) {
			crow::json::wvalue book_ctx;
			book_ctx["tytul"] = ksiazka.tytul;
			book_ctx["autor"] = ksiazka.autor;
			book_ctx["isbn"] = ksiazka.ISBN;
			book_ctx["okladka"] = ksiazka.okladka;
			ksiazki_list.push_back(std::move(book_ctx));
		}

		ctx["ksiazki"] = std::move(ksiazki_list); // Przypisanie listy książek do kontekstu

		return page.render(ctx);
			});

	CROW_ROUTE(app, "/api/bib_book").methods(crow::HTTPMethod::POST)([&bib, &baza_ksiazek](const crow::request& req) {
		try {

			//Ze strony otrzymujemy JSON wyglądający tak: {"tytul":"Fenomenologia","autor":"Hegel","isbn":"123","okladka":"okladka.jpg"}
			//musimy go poprzycinać

			std::string body = req.body;

			//jako że początek body to {"tytul":" to musimy wyciąć pierwsze 10 znaków

			string tytul = body.substr(10, body.length());

			//wyszukujemy " w stringu, bo to znacznik końca tytułu
			int i = tytul.find_first_of('"');

			//kopiujemy zawartość tytułu po " i 11 innych znakach, gdzie zacznie się autor
			string autor = tytul.substr(i + 11, tytul.length());

			//docinamy żeby był sam tytuł
			tytul = tytul.substr(0, i);
			//cout << "Tytul: " << tytul << endl;

			//powtarzamy to dla wszystkich danych
			i = autor.find_first_of('"');
			string ISBN = autor.substr(i + 10, autor.length());
			autor = autor.substr(0, i);
			//cout << "Autor: " << autor << endl;

			i = ISBN.find_first_of('"');
			string okladka = ISBN.substr(i + 13, ISBN.length());
			ISBN = ISBN.substr(0, i);
			//cout << "ISBN: " << ISBN << endl;
			okladka = okladka.substr(0, okladka.length() - 2);
			//cout << "Okladka: " << okladka << endl;


			//tworzymy obiekt książki
			if (baza_ksiazek.wyszukaj_czy_jest(std::stoi(ISBN))) {
				return crow::response(400, R"({"error": "Książka o podanym ISBN już istnieje"})");
			}
			else {
				bib.dodanie_ksiazki(tytul, autor, std::stoi(ISBN), okladka, baza_ksiazek);

				return crow::response(200);
			}
		}
		catch (const std::exception& e) {
			// Obsługa błędów
			std::cerr << "Błąd: " << e.what() << std::endl;
			return crow::response(400, R"({"error": "Wystąpił błąd podczas przetwarzania danych"})");
		}
		});


	CROW_ROUTE(app, "/api/bib_book/<int>")
		.methods("DELETE"_method)([&baza_ksiazek](int isbn) {
		if (baza_ksiazek.wyszukaj_czy_jest(isbn)) {
			baza_ksiazek.usun_ksiazke(isbn);
			cout << "Książka: "<< isbn <<" została usunięta" << endl;
			return crow::response(200, "Książka została usunięta");
		}
		return crow::response(404, "Nie znaleziono książki o podanym ISBN");
			});



	CROW_ROUTE(app, "/bib_wyp.html")
		([](const crow::request& req) {
		auto page = crow::mustache::load_text("bib_wyp.html");
		return page;
			});


	CROW_ROUTE(app, "/api/borrow").methods("POST"_method)([&sessions](const crow::request& req) {
		try {
			auto body = crow::json::load(req.body);

			if (!body) {
				return crow::response(400, "Invalid JSON");
			}

			std::string isbn = body["isbn"].s();

			// Pobranie loginu użytkownika z ciasteczka
			std::string sessionID = req.get_header_value("Cookie");
			sessionID.erase(0, 10); // Usunięcie prefiksu "sessionID="
			std::string login = sessions.count(sessionID) ? sessions[sessionID] : "";

			if (login.empty()) {
				return crow::response(401, "Nie jesteś zalogowany.");
			}

			// Wypisanie danych na ekranie
			std::cout << "Wypożyczanie książki:" << std::endl;
			std::cout << "ISBN: " << isbn << std::endl;
			std::cout << "Login użytkownika: " << login << std::endl;

			return crow::response(200, "Książka została wypożyczona.");
		}
		catch (const std::exception& e) {
			return crow::response(500, e.what());
		}
		});


	app.port(8080).multithreaded().run();
}