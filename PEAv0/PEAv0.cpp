// PEAv0.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.

#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <chrono>

using namespace std;

class TSP {
public:
	string TSPname; // nazwa pliku
	int d = 0;	// ilosc wierzcholkow grafu
	int** tab = NULL;	// macierz reprezentujaca graf
	int pathLength = 0;	//zmienna do przechowania biezacej dlugosci sciezki
	int minPathLength = 0;	//aktualna minimalna dlugosc sciezki
	int cityptr = 0, cityhptr = 0;	//poitery na stosy do przechowania sciezek
	int v0 = 0; //wierzcholek poczatkowy
	int curr_bound = 0; //bound do algorytmu Branch and Bound
	bool* visited = NULL;	//lista odwiedzonych miast
	int* cityList = NULL;	//tablica do przechowania sciezki	
	int* cityListh = NULL;	//pomocnicza tablica do przechowania sciezki
	int licznik = 0;

	~TSP() {
		if (tab != NULL) {
			for (int i = d; i < d; i++) {
				//delete[] tab[i];
			}

			//delete[] tab;
		}
		if (tab != NULL) {
			//delete[]visited;
		}
		if (tab != NULL) {
			//delete[]cityList;
		}
		if (tab != NULL) {
			//delete[]cityListh;
		}
	}

	bool loadTSP(string& name) {	//wczytanie grafu z pliku, inicjacja potrzebnych zmiennych, macierzy oraz tabel
		fstream file;
		string line;
		file.open(name, ios::in);
		if (!file.good()) {
			cout << "\nBlad wczytywania pliku.\n";
			file.close();
			file.clear();
			return false;
		}
		else {
			getline(file, line);
			TSPname = line;
			getline(file, line);
			d = stoi(line);
			cout << "Nazwa pliku: " << TSPname << "\nIlosc wierzcholkow: " << d << "\n";

			tab = new int* [d];
			for (int i = 0; i < d; i++) {
				tab[i] = new int[d];
			}
			for (int i = 0; i < d; i++) {
				for (int j = 0; j < d; j++) {
					file >> tab[i][j];
				}
			}
		}

		visited = new bool[d];
		cityList = new int[d];
		cityListh = new int[d];

		clearAll();

		file.close();
		file.clear();
		return true;
	}

	void clearAll() {	// Funkcja restartująca zmienne i tabele pomocnicze do domyślnych wartości
		pathLength = 0;
		minPathLength = 0;
		v0 = 0;
		cityhptr = cityptr = 0;
		curr_bound = 0;
		licznik = 0;

		for (int i = 0; i < d; i++) {
			visited[i] = false;
			cityList[i] = -1;
			cityListh[i] = -1;
		}
	}
	void showTSP() {	//	wyświetlenie macierzy wag
		for (int i = 0; i < d; i++) {
			for (int j = 0; j < d; j++) {
				cout << tab[i][j] << "    ";
			}
			cout << endl;
		}
	}

	void manualPath() {	// ręczne obiczanie długości ścieżki
		clearAll();
		int v0 = -1;
		cout << "Podaj miasto poczatkowe: ";
		cin >> v0;
		cout << "\n";
		if (v0 < 0 || v0 >= d) {
			cout << "Nie ma takiego miasta!";
		}
		int v = v0, nxt = -1, c = 0;
		visited[v0] = true;
		cityList[0] = v0;
		bool end = false;

		do {
			cout << "Jestes w miescie " << v << " Przebyta droga: " << pathLength << " Podaj kolejne miasto: " << endl;
			cin >> nxt;
			if (nxt < 0 || nxt >= d) {
				cout << "Nie ma takiego miasta!\n";
				nxt = -1;
			}
			else {
				if (visited[nxt] == true) {
					cout << "Juz byles w tym miescie!\n";
					nxt = -1;
				}
				else {
					pathLength += tab[v][nxt];
					v = nxt;
					nxt = -1;
					visited[v] = true;
					c++;
					cityList[c] = v;
					if (c == d - 1) end = true;
				}
			}


		} while (end == false);

		pathLength += tab[v][v0];

		cout << "Sciezka: ";
		for (int i = 0; i < d; i++) {
			cout << cityList[i] << " -> ";
		}
		cout << cityList[0] << endl;

		cout << "Dlugosc sciezki: " << pathLength << endl;

		clearAll();
	}

	void bruteForce() {	//	rozpoczęcie algorytmu brute force
		clearAll();

		v0 = 0;

		TSPbf(v0);

		cout << "Znaleziona sciezka: ";
		for (int i = 0; i < cityptr; i++) {
			cout << cityList[i] << " -> ";
		}
		cout << v0 << endl;
		cout << "Minimalna dlugosc sciezki: " << minPathLength << endl;

		clearAll();
	}

	void TSPbf(int v) {	// algorytm rekurencyjny do brute force'a
		int u;

		cityListh[cityhptr++] = v;

		if (cityhptr < d) {
			visited[v] = true;
			for (u = 0; u < d; u++) {
				if (visited[u] == false)
				{
					pathLength += tab[v][u];
					TSPbf(u);
					pathLength -= tab[v][u];
				}
			}
			visited[v] = false;
		}
		else {
			pathLength += tab[v][v0];
			if (pathLength < minPathLength || minPathLength < 1) {
				minPathLength = pathLength;
				for (u = 0; u < cityhptr; u++) {
					cityList[u] = cityListh[u];
				}
				cityptr = cityhptr;
			}
			pathLength -= tab[v][v0];
		}
		cityhptr--;
	}

	void bandb() {	//	rozpoczęcie algorytmub&b
		clearAll();

		setCurr_bound();

		TSPbandb(v0);

		cout << "Znaleziona sciezka: ";
		for (int i = 0; i < cityptr; i++) {
			cout << cityList[i] << " -> ";
		}
		cout << v0 << endl;
		cout << "Minimalna dlugosc sciezki: " << minPathLength << endl;
		cout << "wykonano " << licznik << "razy" << endl;
		clearAll();
	}

	void TSPbandb(int v) {	// algorytm rekurencyjny do b&b
		int u;

		cityListh[cityhptr++] = v;
		licznik++;
		if (cityhptr < d) {
			visited[v] = true;
			for (u = 0; u < d; u++) {
				if (visited[u] == false)
				{
					int temp = curr_bound;
					pathLength += tab[v][u];
					if (cityhptr == 1)
						curr_bound -= ((firstMin(cityListh[cityhptr - 1]) + firstMin(u)) / 2);
					else
						curr_bound -= ((secondMin(cityListh[cityhptr - 1]) + firstMin(u)) / 2);

					if(curr_bound+pathLength<minPathLength || minPathLength < 1)	TSPbandb(u);
					pathLength -= tab[v][u];
					curr_bound = temp;
				}
			}
			visited[v] = false;
		}
		else {
			pathLength += tab[v][v0];
			if (pathLength < minPathLength || minPathLength < 1) {
				minPathLength = pathLength;
				for (u = 0; u < cityhptr; u++) {
					cityList[u] = cityListh[u];
				}
				cityptr = cityhptr;
			}
			pathLength -= tab[v][v0];
		}
		cityhptr--;
	}

	int firstMin(int i)
	{
		int min = INT_MAX;
		for (int k = 0; k < d; k++)
			if (tab[i][k] < min && i != k)
				min = tab[i][k];
		return min;
	}

	int secondMin(int i)
	{
		int first = INT_MAX, second = INT_MAX;
		for (int j = 0; j < d; j++)
		{
			if (i == j)
				continue;

			if (tab[i][j] <= first)
			{
				second = first;
				first = tab[i][j];
			}
			else if (tab[i][j] <= second &&
				tab[i][j] != first)
				second = tab[i][j];
		}
		return second;
	}
	
	void setCurr_bound() {
		for (int i = 0; i < d; i++) curr_bound += (firstMin(i) + secondMin(i));

		curr_bound = (curr_bound & 1) ? curr_bound / 2 + 1 : curr_bound / 2;
	}
};

int main() {
	TSP TSPObj = TSP();
	int choice, choice2;
	auto start_time = chrono::high_resolution_clock::now();
	auto end_time = chrono::high_resolution_clock::now();
	auto time = end_time - start_time;
	string filename;
	do {
		cout << "PROJEKT\nProjektowanie Efektywnych Algorytmow\nETAP 1\nJakub Grzybek\nNr indeksu: xxxxxx\n===============================\n"
			<< "1. Wczytaj graf\n"
			<< "0. Zamknij program\n";
		cin >> choice;

		switch (choice) {
		case 1:
			cout << "Podaj nazwe pliku z grafem: ";
			cin >> filename;
			cout << "\n";
			if (TSPObj.loadTSP(filename)) {
				char yn;
				cout << "Wyswietlic macierz grafu? Y/N\n";
				cin >> yn;
				if (yn == 'y' || yn == 'Y') {
					TSPObj.showTSP();
				}
				else cout << "Koniec wczytywania/n";

				do {
					cout << "PROJEKT\nProjektowanie Efektywnych Algorytmów\nJakub Grzybek\n241365\n===============================\n"
						<< "1. Manualne obliczenie dlugosci sciezki\n"
						<< "2. Brute Force\n"
						<< "3. Branch and Bound \n"
						<< "0. Zamknij program\n";
					cin >> choice2;

					switch (choice2) {
					case 1:
						TSPObj.manualPath();
						break;

					case 2:
						start_time = chrono::high_resolution_clock::now();
						TSPObj.bruteForce();
						end_time = chrono::high_resolution_clock::now();
						time = end_time - start_time;

						cout << "\nOperacja zajela " << time / chrono::milliseconds(1) << " ms." << endl;
						break;
					case 3:
						start_time = chrono::high_resolution_clock::now();
						TSPObj.bandb();
						end_time = chrono::high_resolution_clock::now();
						time = end_time - start_time;

						cout << "\nOperacja zajela " << time / chrono::milliseconds(1) << " ms." << endl;
						break;
					case 0: 
						choice = 0;
						break;
					default: {
						cout << "Bledny wybor\n";
						break;
					}
					}
				} while (choice2 != 0);
			}
			break;
		case 0: 
			break;
		default: {
			cout << "Bledny wybor\n";
			break;
		}
		}
	} while (choice != 0);
	TSPObj.~TSP();
	return 0;
}
