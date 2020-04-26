#ifndef DATABASE_H
#define DATABASE_H
#include <fstream>
#include <string>
#include <tuple>
#include <vector>
#include "Extensions.h"
using std::fstream;
using std::ifstream;
using std::ofstream;
using std::ios;
using std::string;
using std::invalid_argument;
using std::tuple;
using std::make_tuple;
using std::vector;
class Admin {
private:
	struct database : private Extensions::Console {
		char
			username[16],
			password[16];
		bool blocked;
	};
	const string fileBase = "admin.bin";

protected:
	fstream fs;
	ifstream ifs;
	ofstream ofs;
	const string fileTemp = "temp.dat";

public:
	void InitComponent();

	void AddData(
		const char* username,
		const char* password
		);

	bool ReadData(
		const char* username
		);

	tuple<bool, bool, bool> ReadData(
		const char* username,
		const char* password
		);

	void UpdateData(
		const char* username,
		const char* newPassword,
		const bool newBlocked
		);

	void DeleteData(
		const char* username
		);
};

class Siswa : private Admin {
private:
	struct database {
		int nis;
		char
			nama[128],
			alamat[128],
			ttl[64],
			agama[16],
			jk[16],
			jurusan[64],
			sa[64];
	};
	const string fileBase = "siswa.bin";

	int GetLastNIS();

public:
	void InitComponent();

	void AddData(
		const char* nama,
		const char* alamat,
		const char* ttl,
		const char* agama,
		const char* jk,
		const char* jurusan,
		const char* sa
		);

	vector<database> ReadData();

	tuple<bool, database> ReadData(
		const int nis
		);

	tuple<bool, vector<database>> ReadData(
		const char* nama
		);

	void UpdateData(
		const int nis,
		const char* newNama,
		const char* newAlamat,
		const char* newTtl,
		const char* newAgama,
		const char* newJk,
		const char* newJurusan,
		const char* newSa
		);

	void DeleteData(
		const int nis
		);
};
#endif // !DATABASE_H