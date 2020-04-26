#include <conio.h>
#include <iostream>
#include <regex>
#include <windows.h>
#include "Database.h"
#include "ConsoleMenu.h"
#include "ConsoleTable.h"
using namespace std;
using namespace Extensions;

/* Public variable */
char xorKey = 'V';
regex allowEx{ "[a-zA-Z0-9]+" };
regex nisEx{ "\\b(202110)([^ ]*)" };
Admin adm;
Siswa sis;
ConsoleMenu mQuestion{
	"[Tidak]",
	"[Ya]"
};
ConsoleMenu mJurusan{
	{
		"Animasi dan multimedia",
		"Farmasi",
		"Akuntansi",
		"Pelayaran",
		"Pariwisata",
		"Tata boga",
		"Tata busana",
		"Teknik otomotif",
		"Administrasi perkantoran",
		"Pemasaran"
	},
	true
};
ConsoleMenu mAgama{
	{
		"Islam",
		"Protestan",
		"Katolik",
		"Hindu",
		"Buddha",
		"Khonghucu"
	},
	true
};
ConsoleMenu mJk{
	"Laki-laki",
	"Perempuan"
};

/* Prototype section */
void SignUp();
tuple<bool, string> SignIn();
void Recovery(const string username);
void Administrator(const string);

/* Main function */
int main() {
	/* Configuring console layout */ {
		try {
			SetConsoleTitle(L"SMK Localhost");
			Console::SetConsoleFont(L"Consolas", 20);
			Window::SetConsoleSize(120, 30);
			Sleep(100);
			Window::SetConsolePositionToCenter();
		}
		catch (exception& e) {
			Console::SetMessage(2, 2, ConsoleMessage::Danger, e.what());
			exit(1);
		}
	}

	/* Initializing database */ {
		adm.InitComponent();
		sis.InitComponent();
	}

	/* Dashboard Section */ {
		ConsoleMenu mDashboard{
			"1. Sign Up",
			"2. Sign in",
			"[Keluar]"
		};
		mDashboard.SetCursorColor(ConsoleColor::Green, ConsoleColor::Black);

		while (true) {
			Console::Clear();
			Console::SetCursorPosition(2, 2); Console::Print("=======================");
			Console::SetCursorPosition(2, 3); Console::Print("     SMK LOCALHOST");
			Console::SetCursorPosition(2, 4); Console::Print("=======================");

			switch (mDashboard.Print(2, 6)) {
			case 0: SignUp(); break;
			case 1: {
				auto result = SignIn();

				if (get<0>(result))
					Administrator(get<1>(result));
			}
				break;
			case 2: return 0;
			}
		}
	}
}
void SignUp() {
	bool back = true;
	short
		cPos = 6,
		index = 0;
	string
		username,
		password,
		password2;
	ConsoleMenu mSignUp{
		"Username            :",
		"Password            :",
		"Konfirmasi password :",
		"[Sign Up]",
		"[Kembali]"
	};
	mSignUp.SetCursorColor(ConsoleColor::Green, ConsoleColor::Black);

	Console::Clear();
	Console::SetCursorPosition(2, 2); Console::Print("=================");
	Console::SetCursorPosition(2, 3); Console::Print("     Sign Up");
	Console::SetCursorPosition(2, 4); Console::Print("=================");

	while (back) {
		switch (mSignUp.Print(2, 6, cPos, index)) {
		case 0:
			Console::Print(' ');
			Console::ClearField(username);
			username = Console::InputLine();
			break;
		case 1:
			Console::Print(' ');
			Console::ClearField(password);
			password = Console::InputPassword();
			break;
		case 2:
			Console::Print(' ');
			Console::ClearField(password2);
			password2 = Console::InputPassword();
			break;
		case 3:
			try {
				if (username == "" || password == "" || password2 == "")
					Console::SetMessage(2, 12, ConsoleMessage::Warning, "Silakan lengkapi data");
				else if (!regex_match(username, allowEx) || !regex_match(password, allowEx))
					Console::SetMessage(2, 12, ConsoleMessage::Warning, "Username dan password tidak boleh menggunakan karakter spesial");
				else if (username.length() < 7 || password.length() < 7 || username.length() > 15 || password.length() > 15)
					Console::SetMessage(2, 12, ConsoleMessage::Information, "Panjang username dan password 7-15 karakter");
				else if (adm.ReadData(username.c_str()))
					Console::SetMessage(2, 12, ConsoleMessage::Information, "Username sudah digunakan");
				else if (password != password2)
					Console::SetMessage(2, 12, ConsoleMessage::Information, "Password tidak sama");
				else {
					adm.AddData(username.c_str(), Console::XORCipher(password, xorKey).c_str());
					back = false;
					Console::SetMessage(2, 12, ConsoleMessage::Information, "Akun berhasil ditambahkan");
				}
			}
			catch (exception& e) {
				Console::SetMessage(2, 12, ConsoleMessage::Danger, e.what());
				exit(1);
			}
			break;
		case 4: back = false; break;
		}
	}
}
tuple<bool, string> SignIn() {
	bool
		accepted = false,
		back = true;
	short
		failed = 0,
		cPos = 6,
		index = 0;
	string
		username,
		password,
		userTemp = "";
	ConsoleMenu mSignIn{
		"Username :",
		"Password :",
		"[Sign In]",
		"[Kembali]"
	};
	mSignIn.SetCursorColor(ConsoleColor::Green, ConsoleColor::Black);

	Console::Clear();
	Console::SetCursorPosition(2, 2); Console::Print("=================");
	Console::SetCursorPosition(2, 3); Console::Print("     SIGN IN");
	Console::SetCursorPosition(2, 4); Console::Print("=================");

	while (back) {
		switch (mSignIn.Print(2, 6, cPos, index)) {
		case 0:
			Console::Print(' ');
			Console::ClearField(username);
			username = Console::InputLine();
			break;
		case 1:
			Console::Print(' ');
			Console::ClearField(password);
			password = Console::InputPassword();
			break;
		case 2:
			try {
				if (userTemp != username) {
					userTemp = username;
					failed = 0;
				}

				if (username == "" || password == "")
					Console::SetMessage(2, 11, ConsoleMessage::Warning, "Silakan lengkapi data");
				else {
					auto result = adm.ReadData(username.c_str(), Console::XORCipher(password, xorKey).c_str());

					if (!get<0>(result))
						Console::SetMessage(2, 11, ConsoleMessage::Information, "Akun tidak ditemukan");
					else if (get<2>(result)) {
						Recovery(username);
						Console::Clear();
						Console::SetCursorPosition(2, 2); Console::Print("=================");
						Console::SetCursorPosition(2, 3); Console::Print("     SIGN IN");
						Console::SetCursorPosition(2, 4); Console::Print("=================");
					}
					else if (!get<1>(result)) {
						failed++;
						Console::SetMessage(2, 11, ConsoleMessage::Information, "Password salah");
					}
					else {
						accepted = get<0>(result) && get<1>(result);
						back = false;
					}
				}

				if (failed == 3) {
					adm.UpdateData(username.c_str(), "", true);
					back = false;
					Console::SetMessage(2, 11, ConsoleMessage::Information, "Anda 3 kali salah password, akun diblokir");
				}
			}
			catch (exception& e) {
				Console::SetMessage(2, 11, ConsoleMessage::Danger, e.what());
				exit(1);
			}
			break;
		case 3: back = false; break;
		}
	}
	return tuple<bool, string>(accepted, username);
}
void Recovery(const string username) {
	string
		key,
		inKey,
		newPassword,
		newPassword2;

	/* Memasukkan kode verifikasi */
	while (true) {
		Console::Clear();
		Console::SetCursorPosition(2, 2); Console::Print("==================");
		Console::SetCursorPosition(2, 3); Console::Print("     RECOVERY");
		Console::SetCursorPosition(2, 4); Console::Print("==================");
		Console::SetCursorPosition(2, 5); Console::Print("Masukkan kode untuk memulihkan akun");

		Console::SetCursorPosition(2, 7); Console::Print("Kode : " + (key = Console::GenerateRandomKey(20)));
		Console::SetCursorPosition(2, 8); Console::Print("     $ "); inKey = Console::InputLine();

		if (inKey == "")
			Console::SetMessage(2, 10, ConsoleMessage::Warning, "Silakan masukkan kode");
		else if (inKey != key)
			Console::SetMessage(2, 10, ConsoleMessage::Information, "Kode tidak sama");
		else break;
	}

	/* Memasukkan password baru */
	while (true) {
		Console::Clear();
		Console::SetCursorPosition(2, 2); Console::Print("==================");
		Console::SetCursorPosition(2, 3); Console::Print("     RECOVERY");
		Console::SetCursorPosition(2, 4); Console::Print("==================");
		Console::SetCursorPosition(2, 5); Console::Print("Masukkan password baru");

		Console::SetCursorPosition(2, 7); Console::Print("Password            :");
		Console::SetCursorPosition(2, 8); Console::Print("Konfirmasi password :");
		Console::SetCursorPosition(24, 7); newPassword = Console::InputPassword();
		Console::SetCursorPosition(24, 8); newPassword2 = Console::InputPassword();

		if (newPassword == "" || newPassword2 == "")
			Console::SetMessage(2, 10, ConsoleMessage::Warning, "Silakan masukkan password baru");
		else if (!regex_match(newPassword, allowEx))
			Console::SetMessage(2, 10, ConsoleMessage::Warning, "Password tidak boleh menggunakan karakter spesial");
		else if (newPassword.length() < 7 || newPassword.length() > 15)
			Console::SetMessage(2, 10, ConsoleMessage::Information, "Panjang password 7-15 karakter");
		else if (newPassword2 != newPassword)
			Console::SetMessage(2, 10, ConsoleMessage::Information, "Password tidak sama");
		else break;
	}

	/* Final section */
	try {
		adm.UpdateData(username.c_str(), Console::XORCipher(newPassword, xorKey).c_str(), false);
		Console::SetMessage(2, 10, ConsoleMessage::Information, "Akun berhasil dipulihkan");
	}
	catch (exception& e) {
		Console::SetMessage(2, 10, ConsoleMessage::Danger, e.what());
		exit(1);
	}
}

/* Prototype section */
void AddSiswa();
void ShowSiswa();
void SearchSiswa();
void UpdateSiswa();
void DeleteSiswa();
void Setting(const string username);

/* Admin function */
void Administrator(const string username) {
	bool back = true;
	ConsoleMenu mAdministrator{
		"1. Pendaftaran siswa",
		"2. Tampilkan data siswa",
		"3. Cari data siswa",
		"4. Update data siswa",
		"5. Hapus data siswa",
		"6. Pengaturan",
		"[Keluar]"
	};
	mAdministrator.SetCursorColor(ConsoleColor::Green, ConsoleColor::Black);
	mQuestion.SetCursorColor(ConsoleColor::Green, ConsoleColor::Black);
	mJurusan.SetCursorColor(ConsoleColor::Green, ConsoleColor::Black);
	mAgama.SetCursorColor(ConsoleColor::Green, ConsoleColor::Black);
	mJk.SetCursorColor(ConsoleColor::Green, ConsoleColor::Black);

	while (back) {
		Console::Clear();
		Console::SetCursorPosition(2, 2); Console::Print("=======================");
		Console::SetCursorPosition(2, 3); Console::Print("     ADMINISTRATOR");
		Console::SetCursorPosition(2, 4); Console::Print("=======================");
		Console::SetCursorPosition(2, 5); Console::Print("Anda masuk sebagai: " + username);

		switch (mAdministrator.Print(2, 7)) {
		case 0: AddSiswa(); break;
		case 1: ShowSiswa(); break;
		case 2: SearchSiswa(); break;
		case 3: UpdateSiswa(); break;
		case 4: DeleteSiswa(); break;
		case 5: Setting(username); break;
		case 6:
			Console::SetCursorPosition(2, 15); Console::Print("Apakah anda yakin?");
			switch (mQuestion.Print(2, 16)) {
			case 1: back = false; break;
			}
			break;

		}
	}
}
void AddSiswa() {
	bool back = true;
	short
		cPos = 6,
		index = 0;
	string
		nama,
		alamat,
		ttl,
		agama,
		jk,
		jurusan,
		sa;
	ConsoleMenu mAddSiswa{
		"Nama                  :",
		"Alamat                :",
		"Tempat, tanggal lahir :",
		"Agama                 :",
		"Jenis kelamin         :",
		"Jurusan               :",
		"Sekolah asal          :",
		"[Daftar]",
		"[Kembali]"
	};
	mAddSiswa.SetCursorColor(ConsoleColor::Green, ConsoleColor::Black);

	while (back) {
		Console::Clear();
		Console::SetCursorPosition(2, 2);		Console::Print("===========================");
		Console::SetCursorPosition(2, 3);		Console::Print("     PENDAFTARAN SISWA");
		Console::SetCursorPosition(2, 4);		Console::Print("===========================");
		Console::SetCursorPosition(26, 6);	Console::Print(nama);
		Console::SetCursorPosition(26, 7);	Console::Print(alamat);
		Console::SetCursorPosition(26, 8);	Console::Print(ttl);
		Console::SetCursorPosition(26, 9);	Console::Print(agama);
		Console::SetCursorPosition(26, 10);	Console::Print(jk);
		Console::SetCursorPosition(26, 11);	Console::Print(jurusan);
		Console::SetCursorPosition(26, 12);	Console::Print(sa);

		switch (mAddSiswa.Print(2, 6, cPos, index)) {
		case 0:
			Console::Print(' ');
			Console::ClearField(nama);
			nama = Console::InputLine();
			break;
		case 1:
			Console::Print(' ');
			Console::ClearField(alamat);
			alamat = Console::InputLine();
			break;
		case 2:
			Console::Print(' ');
			Console::ClearField(ttl);
			ttl = Console::InputLine();
			break;
		case 3:
			Console::ClearField(agama);
			switch (mAgama.Print(26, 9)) {
			case 0: agama = "Islam"; break;
			case 1: agama = "Protestan"; break;
			case 2: agama = "Katolik"; break;
			case 3: agama = "Hindu"; break;
			case 4: agama = "Buddha"; break;
			case 5: agama = "Khonghucu"; break;
			}
			break;
		case 4:
			Console::ClearField(jk);
			switch (mJk.Print(26, 10)) {
			case 0: jk = "Laki-laki"; break;
			case 1: jk = "Perempuan"; break;
			}
			break;
		case 5:
			Console::ClearField(jurusan);
			switch (mJurusan.Print(26, 11)) {
			case 0: jurusan = "Animasi dan multimedia"; break;
			case 1: jurusan = "Farmasi"; break;
			case 2: jurusan = "Akuntansi"; break;
			case 3: jurusan = "Pelayaran"; break;
			case 4: jurusan = "Pariwisata"; break;
			case 5: jurusan = "Tata boga"; break;
			case 6: jurusan = "Tata busana"; break;
			case 7: jurusan = "Teknik otomotif"; break;
			case 8: jurusan = "Administrasi perkantoran"; break;
			case 9: jurusan = "Pemasaran"; break;
			}
			break;
		case 6:
			Console::Print(' ');
			Console::ClearField(sa);
			sa = Console::InputLine();
			break;
		case 7:
			try {
				if (nama == "" || alamat == "" || ttl == "" || agama == "" || jk == "" || jurusan == "" || sa == "")
					Console::SetMessage(2, 16, ConsoleMessage::Warning, "Silakan lengkapi data");
				else {
					sis.AddData(
						Console::FirstLetterToUppercase(nama).c_str(),
						alamat.c_str(),
						ttl.c_str(),
						agama.c_str(),
						jk.c_str(),
						jurusan.c_str(),
						sa.c_str()
					);
					back = false;
					Console::SetMessage(2, 16, ConsoleMessage::Information, "Pendaftaran berhasil");
				}
			}
			catch (exception& e) {
				Console::SetMessage(2, 16, ConsoleMessage::Danger, e.what());
				exit(1);
			}
			break;
		case 8: back = false; break;
		}
	}
}
void ShowSiswa() {
	short	no = 1;
	ConsoleTable tShow{
		"No",
		"Nomor Induk Siswa",
		"Nama",
		"Alamat",
		"Tempat, tanggal lahir",
		"Agama",
		"Jenis Kelamin",
		"Jurusan",
		"Sekolah asal"
	};

	Console::Clear();
	Console::SetCursorPosition(2, 2); Console::Print("====================");
	Console::SetCursorPosition(2, 3); Console::Print("     LIST SISWA");
	Console::SetCursorPosition(2, 4); Console::Print("====================");
	Console::Print("\n\n");
	try {
		auto result = sis.ReadData();
		for (auto& data : result) {
			tShow += {
				to_string(no),
				to_string(data.nis),
				data.nama,
				data.alamat,
				data.ttl,
				data.agama,
				data.jk,
				data.jurusan,
				data.sa
			};
			no++;
		}
		int max = tShow.GetMaxData();
		Console::SetBufferSize(
			(max > 120) ? max : 120,
			(no > 20) ? no + 10 : 30
		);
		Console::Print(tShow);
		cin.get();
		Console::SetBufferSize(120, 30);
	}
	catch (exception& e) {
		Console::SetMessage(2, 6, ConsoleMessage::Danger, e.what());
		exit(1);
	}
}
void SearchSiswa() {
	short no = 1;
	string userInput;

	while (true) {
		Console::Clear();
		Console::SetCursorPosition(2, 2); Console::Print("=========================");
		Console::SetCursorPosition(2, 3); Console::Print("     CARI DATA SISWA");
		Console::SetCursorPosition(2, 4); Console::Print("=========================");

		Console::SetCursorPosition(2, 6); Console::Print("Masukkan NIS atau Nama siswa: "); userInput = Console::InputLine();
		if (userInput == "")
			Console::SetMessage(2, 8, ConsoleMessage::Information, "Silakan masukkan NIS atau Nama Siswa");
		else break;
	}

	try {
		if (isdigit(userInput[0])) {
			if (!regex_search(userInput, nisEx))
				Console::SetMessage(2, 8, ConsoleMessage::Information, "Nomor Induk Siswa salah");
			else {
				auto result = sis.ReadData(Console::IntParse(userInput));

				if (!get<0>(result))
					Console::SetMessage(2, 8, ConsoleMessage::Information, "Data Siswa tidak ditemukan");
				else {
					Console::SetCursorPosition(2, 8);		cout << "Nomor Induk Siswa     : " << get<1>(result).nis;
					Console::SetCursorPosition(2, 9);		cout << "Nama                  : " << get<1>(result).nama;
					Console::SetCursorPosition(2, 10);	cout << "Alamat                : " << get<1>(result).alamat;
					Console::SetCursorPosition(2, 11);	cout << "Tempat, tanggal lahir : " << get<1>(result).ttl;
					Console::SetCursorPosition(2, 12);	cout << "Agama                 : " << get<1>(result).agama;
					Console::SetCursorPosition(2, 13);	cout << "Jenis kelamin         : " << get<1>(result).jk;
					Console::SetCursorPosition(2, 14);	cout << "Jurusan               : " << get<1>(result).jurusan;
					Console::SetCursorPosition(2, 15);	cout << "Sekolah asal          : " << get<1>(result).sa;
					cin.get();
				}
			}
		}
		else {
			auto result = sis.ReadData(Console::FirstLetterToUppercase(userInput).c_str());

			if (!get<0>(result))
				Console::SetMessage(2, 8, ConsoleMessage::Information, "Data Siswa tidak ditemukan");
			else {
				ConsoleTable tShow{
					"No",
					"Nomor Induk Siswa",
					"Nama",
					"Alamat",
					"Tempat, tanggal lahir",
					"Agama",
					"Jenis Kelamin",
					"Jurusan",
					"Sekolah asal"
				};
				for (auto& data : get<1>(result)) {
					tShow += {
						to_string(no),
						to_string(data.nis),
						data.nama,
						data.alamat,
						data.ttl,
						data.agama,
						data.jk,
						data.jurusan,
						data.sa
					};
					no++;
				}
				int max = tShow.GetMaxData();
				Console::SetBufferSize(
					(max > 120) ? max : 120,
					(no > 20) ? no + 10 : 30
				);
				Console::Print(tShow);
				cin.get();
				Console::SetBufferSize(120, 30);
			}
		}
	}
	catch (exception& e) {
		Console::SetMessage(2, 8, ConsoleMessage::Danger, e.what());
		exit(1);
	}
}
void UpdateSiswa() {
	bool back = true;
	int nis;
	short
		cPos = 8,
		index = 0;
	string
		userInput,
		nama,
		alamat,
		ttl,
		agama,
		jk,
		jurusan,
		sa;
	ConsoleMenu mUpdateSiswa{
		"Nama                  :",
		"Alamat                :",
		"Tempat, tanggal lahir :",
		"Agama                 :",
		"Jenis kelamin         :",
		"Jurusan               :",
		"Sekolah asal          :",
		"[Update]",
		"[Kembali]"
	};
	mUpdateSiswa.SetCursorColor(ConsoleColor::Green, ConsoleColor::Black);

	while (true) {
		Console::Clear();
		Console::SetCursorPosition(2, 2); Console::Print("===========================");
		Console::SetCursorPosition(2, 3); Console::Print("     UPDATE DATA SISWA");
		Console::SetCursorPosition(2, 4); Console::Print("===========================");

		Console::SetCursorPosition(2, 6); Console::Print("Masukkan Nomor Induk siswa: "); userInput = Console::InputLine();
		if (userInput == "")
			Console::SetMessage(2, 8, ConsoleMessage::Information, "Silakan masukkan Nomor Induk Siswa");
		else break;
	}

	try {
		if (!regex_search(userInput, nisEx))
			Console::SetMessage(2, 8, ConsoleMessage::Information, "Nomor Induk Siswa salah");
		else {
			auto result = sis.ReadData(Console::IntParse(userInput));

			if (!get<0>(result))
				Console::SetMessage(2, 8, ConsoleMessage::Information, "Data Siswa tidak ditemukan");
			else {
				nis = get<1>(result).nis;
				nama = get<1>(result).nama;
				alamat = get<1>(result).alamat;
				ttl = get<1>(result).ttl;
				agama = get<1>(result).agama;
				jk = get<1>(result).jk;
				jurusan = get<1>(result).jurusan;
				sa = get<1>(result).sa;

				while (back) {
					Console::Clear();
					Console::SetCursorPosition(2, 2); Console::Print("===========================");
					Console::SetCursorPosition(2, 3); Console::Print("     UPDATE DATA SISWA");
					Console::SetCursorPosition(2, 4); Console::Print("===========================");
					Console::SetCursorPosition(2, 5); Console::Print("Masukkan data baru");

					Console::SetCursorPosition(2, 7);		cout << "Nomor Induk Siswa     :";
					Console::SetCursorPosition(26, 7);	cout << nis;
					Console::SetCursorPosition(26, 8);	cout << nama;
					Console::SetCursorPosition(26, 9);	cout << alamat;
					Console::SetCursorPosition(26, 10);	cout << ttl;
					Console::SetCursorPosition(26, 11);	cout << agama;
					Console::SetCursorPosition(26, 12);	cout << jk;
					Console::SetCursorPosition(26, 13);	cout << jurusan;
					Console::SetCursorPosition(26, 14);	cout << sa;

					switch (mUpdateSiswa.Print(2, 8, cPos, index)) {
					case 0:
						Console::Print(' ');
						Console::ClearField(nama);
						nama = Console::InputLine();
						break;
					case 1:
						Console::Print(' ');
						Console::ClearField(alamat);
						alamat = Console::InputLine();
						break;
					case 2:
						Console::Print(' ');
						Console::ClearField(ttl);
						ttl = Console::InputLine();
						break;
					case 3:
						Console::ClearField(agama);
						switch (mAgama.Print(26, 11)) {
						case 0: agama = "Islam"; break;
						case 1: agama = "Protestan"; break;
						case 2: agama = "Katolik"; break;
						case 3: agama = "Hindu"; break;
						case 4: agama = "Buddha"; break;
						case 5: agama = "Khonghucu"; break;
						}
						break;
					case 4:
						Console::ClearField(jk);
						switch (mJk.Print(26, 12)) {
						case 0: jk = "Laki-laki"; break;
						case 1: jk = "Perempuan"; break;
						}
						break;
					case 5:
						Console::ClearField(jurusan);
						switch (mJurusan.Print(26, 13)) {
						case 0: jurusan = "Animasi dan multimedia"; break;
						case 1: jurusan = "Farmasi"; break;
						case 2: jurusan = "Akuntansi"; break;
						case 3: jurusan = "Pelayaran"; break;
						case 4: jurusan = "Pariwisata"; break;
						case 5: jurusan = "Tata boga"; break;
						case 6: jurusan = "Tata busana"; break;
						case 7: jurusan = "Teknik otomotif"; break;
						case 8: jurusan = "Administrasi perkantoran"; break;
						case 9: jurusan = "Pemasaran"; break;
						}
						break;
					case 6:
						Console::Print(' ');
						Console::ClearField(sa);
						sa = Console::InputLine();
						break;
					case 7:
						Console::SetCursorPosition(2, 18); Console::Print("Apakah anda yakin?");
						switch (mQuestion.Print(2, 19)) {
						case 1:
							sis.UpdateData(
								nis,
								Console::FirstLetterToUppercase(nama).c_str(),
								alamat.c_str(),
								ttl.c_str(),
								agama.c_str(),
								jk.c_str(),
								jurusan.c_str(),
								sa.c_str()
							);
							back = false;
							Console::SetMessage(2, 22, ConsoleMessage::Information, "Data Berhasil Diupdate");
							break;
						}
						break;
					case 8: back = false; break;
					}
				}
			}
		}
	}
	catch (exception& e) {
		Console::SetMessage(2, 8, ConsoleMessage::Information, e.what());
		exit(1);
	}
}
void DeleteSiswa() {
	string userInput;

	while (true) {
		Console::Clear();
		Console::SetCursorPosition(2, 2); Console::Print("==========================");
		Console::SetCursorPosition(2, 3); Console::Print("     HAPUS DATA SISWA");
		Console::SetCursorPosition(2, 4); Console::Print("==========================");

		Console::SetCursorPosition(2, 6); Console::Print("Masukkan Nomor Induk siswa: "); userInput = Console::InputLine();
		if (userInput == "")
			Console::SetMessage(2, 8, ConsoleMessage::Information, "Silakan masukkan Nomor Induk Siswa");
		else break;
	}

	try {
		if (!regex_search(userInput, nisEx))
			Console::SetMessage(2, 8, ConsoleMessage::Information, "Nomor Induk Siswa salah");
		else {
			auto result = sis.ReadData(Console::IntParse(userInput));

			if (!get<0>(result))
				Console::SetMessage(2, 8, ConsoleMessage::Information, "Data Siswa tidak ditemukan");
			else {
				Console::SetCursorPosition(2, 8);		cout << "Nomor Induk Siswa     : " << get<1>(result).nis;
				Console::SetCursorPosition(2, 9);		cout << "Nama                  : " << get<1>(result).nama;
				Console::SetCursorPosition(2, 10);	cout << "Alamat                : " << get<1>(result).alamat;
				Console::SetCursorPosition(2, 11);	cout << "Tempat, tanggal lahir : " << get<1>(result).ttl;
				Console::SetCursorPosition(2, 12);	cout << "Agama                 : " << get<1>(result).agama;
				Console::SetCursorPosition(2, 13);	cout << "Jenis kelamin         : " << get<1>(result).jk;
				Console::SetCursorPosition(2, 14);	cout << "Jurusan               : " << get<1>(result).jurusan;
				Console::SetCursorPosition(2, 15);	cout << "Sekolah asal          : " << get<1>(result).sa;
				
				Console::SetCursorPosition(2, 17); Console::Print("Apakah anda yakin?");
				switch (mQuestion.Print(2, 18)) {
				case 1:
					sis.DeleteData(Console::IntParse(userInput));
					Console::SetMessage(2, 21, ConsoleMessage::Information, "Data Siswa berhasil dihapus");
					break;
				}
			}
		}
	}
	catch (exception& e) {
		Console::SetMessage(2, 8, ConsoleMessage::Danger, e.what());
		exit(1);
	}
}

/* Prototype section */
void ChangePassword(const string username);
void DeleteAccount(const string username);

/* Setting Function */
void Setting(const string username) {
	bool back = true;
	ConsoleMenu mSetting{
		"1. Ganti password",
		"2. Hapus akun",
		"[Kembali]"
	};
	mSetting.SetCursorColor(ConsoleColor::Green, ConsoleColor::Black);

	while (back) {
		Console::Clear();
		Console::SetCursorPosition(2, 2); Console::Print("====================");
		Console::SetCursorPosition(2, 3); Console::Print("     PENGATURAN");
		Console::SetCursorPosition(2, 4); Console::Print("====================");

		switch (mSetting.Print(2, 6)) {
		case 0: ChangePassword(username); break;
		case 1: DeleteAccount(username); break;
		case 2: back = false; break;
		}
	}
}
void ChangePassword(const string username) {
	bool password = false;
	string
		oldPassword,
		newPassword,
		newPassword2;

	while (true) {
		Console::Clear();
		Console::SetCursorPosition(2, 2); Console::Print("========================");
		Console::SetCursorPosition(2, 3); Console::Print("     GANTI PASSWORD");
		Console::SetCursorPosition(2, 4); Console::Print("========================");

		Console::SetCursorPosition(2, 6); Console::Print("Masukkan password lama : "); oldPassword = Console::InputPassword();
		if (oldPassword == "")
			Console::SetMessage(2, 8, ConsoleMessage::Information, "Silakan masukkan password lama");
		else break;
	}
	
	try {
		auto result = adm.ReadData(username.c_str(), Console::XORCipher(oldPassword, xorKey).c_str());

		if (!get<1>(result))
			Console::SetMessage(2, 8, ConsoleMessage::Information, "Password salah");
		else password = true;
	}
	catch (exception& e) {
		Console::SetMessage(2, 8, ConsoleMessage::Danger, e.what());
		exit(1);
	}

	if (password) {
		while (true) {
			Console::Clear();
			Console::SetCursorPosition(2, 2); Console::Print("========================");
			Console::SetCursorPosition(2, 3); Console::Print("     GANTI PASSWORD");
			Console::SetCursorPosition(2, 4); Console::Print("========================");
			Console::SetCursorPosition(2, 5); Console::Print("Masukkan password baru");

			Console::SetCursorPosition(2, 7); Console::Print("Password            :");
			Console::SetCursorPosition(2, 8); Console::Print("Konfirmasi password :");
			Console::SetCursorPosition(24, 7); newPassword = Console::InputPassword();
			Console::SetCursorPosition(24, 8); newPassword2 = Console::InputPassword();

			if (newPassword == "" || newPassword2 == "")
				Console::SetMessage(2, 10, ConsoleMessage::Warning, "Silakan masukkan password baru");
			else if (!regex_match(newPassword, allowEx))
				Console::SetMessage(2, 10, ConsoleMessage::Warning, "Password tidak boleh menggunakan karakter spesial");
			else if (newPassword.length() < 7 || newPassword.length() > 15)
				Console::SetMessage(2, 10, ConsoleMessage::Information, "Panjang password 7-15 karakter");
			else if (newPassword2 != newPassword)
				Console::SetMessage(2, 10, ConsoleMessage::Information, "Password tidak sama");
			else break;
		}

		/* Final section */
		try {
			adm.UpdateData(username.c_str(), Console::XORCipher(newPassword, xorKey).c_str(), false);
			Console::SetMessage(2, 10, ConsoleMessage::Information, "Password berhasil diganti");
		}
		catch (exception& e) {
			Console::SetMessage(2, 10, ConsoleMessage::Danger, e.what());
			exit(1);
		}
	}
}
void DeleteAccount(const string username) {
	string password;
	ConsoleMenu mQuestion{
		"[Tidak]",
		"[Tidak]",
		"[Tidak]",
		"[Tidak]",
		"[Tidak]",
		"[Tidak]",
		"[Tidak]",
		"[Ya]",
		"[Tidak]",
		"[Tidak]",
		"[Tidak]"
	};
	mQuestion.SetCursorColor(ConsoleColor::Red, ConsoleColor::Black);

	Console::Clear();
	Console::SetCursorPosition(2, 2); Console::Print("====================");
	Console::SetCursorPosition(2, 3); Console::Print("     HAPUS AKUN");
	Console::SetCursorPosition(2, 4); Console::Print("====================");
	Console::SetCursorPosition(2, 5); Console::Print("Akun anda akan dihapus secara permanen");

	Console::SetCursorPosition(2, 7); Console::Print("Apakah anda yakin?");
	switch (mQuestion.Print(2, 8)) {
	case 7:
		Console::Print(" Masukkan password : "); password = Console::InputPassword();

		try {
			auto result = adm.ReadData(username.c_str(), Console::XORCipher(password, xorKey).c_str());

			if (!get<1>(result))
				Console::SetMessage(2, 20, ConsoleMessage::Information, "Password salah");
			else {
				adm.DeleteData(username.c_str());
				Console::SetMessage(2, 20, ConsoleMessage::Information, "Akun anda telah dihapus");
				exit(0);
			}
		}
		catch (exception& e) {
			Console::SetMessage(2, 20, ConsoleMessage::Danger, e.what());
			exit(1);
		}
		break;
	}
}