#include "Database.h"
#include "ConsoleTable.h"
using std::to_string;
using std::cout;

/* Admin section */
void Admin::InitComponent() {
	ofs.open(fileBase, ios::app);
	ofs.close();
}

void Admin::AddData(
	const char* username,
	const char* password
	) {
	database db;

	strcpy_s(db.username, username);
	strcpy_s(db.password, password);
	db.blocked = false;

	ofs.open(fileBase, ios::binary | ios::app);
	if (!ofs.is_open())
		throw invalid_argument{ "Database tidak ditemukan" };
	ofs.write(reinterpret_cast<char*>(&db), sizeof database);
	ofs.close();
}

bool Admin::ReadData(
	const char* username
	) {
	bool bUsername = false;
	database db;

	ifs.open(fileBase, ios::binary);
	if (!ifs.is_open())
		throw invalid_argument{ "Database tidak ditemukan" };
	while (ifs.read(reinterpret_cast<char*>(&db), sizeof database)) {
		if (strcmp(username, db.username) == 0) {
			bUsername = true;
			break;
		}
	}
	ifs.close();
	return bUsername;
}

tuple<bool, bool, bool> Admin::ReadData(
	const char* username,
	const char* password
	) {
	bool
		bUsername = false,
		bPassword = false,
		bBlocked = false;
	database db;

	ifs.open(fileBase, ios::binary);
	if (!ifs.is_open())
		throw invalid_argument{ "Database tidak ditemukan" };
	while (ifs.read(reinterpret_cast<char*>(&db), sizeof database)) {
		if (strcmp(username, db.username) == 0) {
			bUsername = true;
			if (strcmp(password, db.password) == 0)
				bPassword = true;
			bBlocked = db.blocked;
			break;
		}
	}
	ifs.close();
	return tuple<bool, bool, bool>(bUsername, bPassword, bBlocked);
}

void Admin::UpdateData(
	const char* username,
	const char* newPassword,
	const bool newBlocked
	) {
	database db;

	fs.open(fileBase, ios::in | ios::out | ios::binary);
	if (!fs.is_open())
		throw invalid_argument{ "Database tidak ditemukan" };
	while (fs.read(reinterpret_cast<char*>(&db), sizeof database)) {
		int pos = -static_cast<int>(sizeof database);

		memset(db.password, 0, sizeof db.password);
		strcpy_s(db.password, newPassword);
		db.blocked = newBlocked;
		fs.seekp(pos, ios::cur);
		fs.write(reinterpret_cast<char*>(&db), sizeof database);
		break;
	}
	fs.close();
}

void Admin::DeleteData(
	const char* username
	) {
	database db;

	ifs.open(fileBase, ios::binary);
	if (!ifs.is_open())
		throw invalid_argument{ "Database tidak ditemukan" };
	ofs.open(fileTemp, ios::binary);
	while (ifs.read(reinterpret_cast<char*>(&db), sizeof database))
		if (strcmp(username, db.username) != 0)
			ofs.write(reinterpret_cast<char*>(&db), sizeof database);
	ifs.close();
	ofs.close();
	remove(fileBase.c_str());
	rename(fileTemp.c_str(), fileBase.c_str());
}

/* Siswa section */
int Siswa::GetLastNIS() {
	short pos = -static_cast<short>(sizeof database);
	database db;

	ifs.open(fileBase, ios::binary);
	if (!ifs.is_open())
		throw invalid_argument{ "Database tidak ditemukan" };
	ifs.seekg(pos, ios::end);
	ifs.read(reinterpret_cast<char*>(&db), sizeof database);
	ifs.close();
	return db.nis;
}

void Siswa::InitComponent() {
	ofs.open(fileBase, ios::app);
	ofs.close();
}

void Siswa::AddData(
	const char* nama,
	const char* alamat,
	const char* ttl,
	const char* agama,
	const char* jk,
	const char* jurusan,
	const char* sa
	) {
	int iLastNIS = GetLastNIS();
	string sLastNIS = to_string(iLastNIS);
	database db;

	db.nis = iLastNIS < 0 ?
		2021101 :
		Console::IntParse(
			sLastNIS.replace(
				6,
				string::npos,
				to_string(Console::IntParse(sLastNIS.substr(6)) + 1)
			)
		);
	strcpy_s(db.nama, nama);
	strcpy_s(db.alamat, alamat);
	strcpy_s(db.ttl, ttl);
	strcpy_s(db.agama, agama);
	strcpy_s(db.jk, jk);
	strcpy_s(db.jurusan, jurusan);
	strcpy_s(db.sa, sa);

	ofs.open(fileBase, ios::binary | ios::app);
	ofs.write(reinterpret_cast<char*>(&db), sizeof database);
	ofs.close();
}

vector<Siswa::database> Siswa::ReadData() {
	vector<Siswa::database> storedData;
	database db;

	ifs.open(fileBase, ios::binary);
	if (!ifs.is_open())
		throw invalid_argument{ "Database tidak ditemukan" };
	while (ifs.read(reinterpret_cast<char*>(&db), sizeof database))
		storedData.push_back(db);
	ifs.close();
	return storedData;
}

tuple<bool, Siswa::database> Siswa::ReadData(
	const int nis
	) {
	bool dataExist = false;
	database db;

	ifs.open(fileBase, ios::binary);
	while (ifs.read(reinterpret_cast<char*>(&db), sizeof database)) {
		if (nis == db.nis) {
			dataExist = true;
			break;
		}
	}
	ifs.close();
	return tuple<bool, database>(dataExist, db);
}

tuple<bool, vector<Siswa::database>> Siswa::ReadData(
	const char* nama
	) {
	bool dataExist = false;
	vector<Siswa::database> storedData;
	database db;

	ifs.open(fileBase, ios::binary);
	if (!ifs.is_open())
		throw invalid_argument{ "Database tidak ditemukan" };
	while (ifs.read(reinterpret_cast<char*>(&db), sizeof database)) {
		string sNama = db.nama;

		if (sNama.find(nama) != string::npos) {
			dataExist = true;
			storedData.push_back(db);
		}
	}
	ifs.close();
	return tuple<bool, vector<database>>(dataExist, storedData);
}

void Siswa::UpdateData(
	const int nis,
	const char* newNama,
	const char* newAlamat,
	const char* newTtl,
	const char* newAgama,
	const char* newJk,
	const char* newJurusan,
	const char* newSa
	) {
	database db;

	fs.open(fileBase, ios::in | ios::out | ios::binary);
	if (!fs.is_open())
		throw invalid_argument{ "Database tidak ditemukan" };
	while (fs.read(reinterpret_cast<char*>(&db), sizeof database)) {
		if (nis == db.nis) {
			int pos = -static_cast<int>(sizeof database);

			/* Erasing database */
			memset(db.nama, NULL, sizeof db.nama);
			memset(db.alamat, NULL, sizeof db.alamat);
			memset(db.ttl, NULL, sizeof db.ttl);
			memset(db.agama, NULL, sizeof db.agama);
			memset(db.jk, NULL, sizeof db.jk);
			memset(db.jurusan, NULL, sizeof db.jurusan);
			memset(db.sa, NULL, sizeof db.sa);

			/* Copying new data to database */
			strcpy_s(db.nama, newNama);
			strcpy_s(db.alamat, newAlamat);
			strcpy_s(db.ttl, newTtl);
			strcpy_s(db.agama, newAgama);
			strcpy_s(db.jk, newJk);
			strcpy_s(db.jurusan, newJurusan);
			strcpy_s(db.sa, newSa);

			/* Writing new data */
			fs.seekp(pos, ios::cur);
			fs.write(reinterpret_cast<char*>(&db), sizeof database);
			break;
		}
	}
	fs.close();
}

void Siswa::DeleteData(
	const int nis
	) {
	database db;

	ifs.open(fileBase, ios::binary);
	if (!ifs.is_open())
		throw invalid_argument{ "Database tidak ditemukan" };
	ofs.open(fileTemp, ios::binary);
	while (ifs.read(reinterpret_cast<char*>(&db), sizeof database))
		if (nis != db.nis) ofs.write(reinterpret_cast<char*>(&db), sizeof database);
	ifs.close();
	ofs.close();
	remove(fileBase.c_str());
	rename(fileTemp.c_str(), fileBase.c_str());
}