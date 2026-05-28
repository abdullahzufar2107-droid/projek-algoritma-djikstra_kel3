#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <sstream>

using namespace std;

// KODE WARNA ANSI
#define RESET   "\033[0m"
#define MERAH   "\033[31m"
#define KUNING  "\033[33m"
#define CYAN    "\033[36m"
#define HIJAU   "\033[32m"

// ============================================================
//  STRUCT
// ============================================================

struct Edge {
    int   tujuan;
    float jarak;
    float waktu;
    bool  aktif = true;
};

struct Node {
    string nama;
    string kode;
    bool   terblokir = false;
};

// ============================================================
//  KONSTANTA & VARIABEL GLOBAL
// ============================================================

const float INF = numeric_limits<float>::infinity();

int                  jumlahNode;
int                  jumlahEdge;
vector<Node>         nodes;
vector<vector<Edge>> graf;

// FUNGSI INPUT
int inputInt(const string& pesan, int minVal, int maxVal) {
    int nilai;
    while (true) {
        cout << pesan;
        if (cin >> nilai) {
            if (nilai >= minVal && nilai <= maxVal) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return nilai;
            }
            cout << MERAH << "  [!] Input harus antara " << minVal
                 << " dan " << maxVal << ". Coba lagi." << RESET << "\n";
        } else {
            cout << MERAH << "  [!] Input tidak valid. Masukkan angka bulat. Coba lagi." << RESET << "\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

float inputFloat(const string& pesan, float minVal) {
    float nilai;
    while (true) {
        cout << pesan;
        if (cin >> nilai) {
            if (nilai >= minVal) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return nilai;
            }
            cout << MERAH << "  [!] Nilai harus >= " << minVal << ". Coba lagi." << RESET << "\n";
        } else {
            cout << MERAH << "  [!] Input tidak valid. Masukkan angka. Coba lagi." << RESET << "\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

// Input char pilihan y/n
char inputYN(const string& pesan) {
    char nilai;
    while (true) {
        cout << pesan;
        if (cin >> nilai) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            nilai = tolower(nilai);
            if (nilai == 'y' || nilai == 'n') return nilai;
        }
        cout << MERAH << "  [!] Input tidak valid. Masukkan y atau n. Coba lagi." << RESET << "\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string inputString(const string& pesan) {
    string nilai;
    while (true) {
        cout << pesan;
        getline(cin, nilai);
        if (!nilai.empty()) return nilai;
        cout << MERAH << "  [!] Input tidak boleh kosong. Coba lagi." << RESET << "\n";
    }
}

// UTILITAS TAMPILAN
void cetakGaris(char karakter, int panjang) {
    for (int i = 0; i < panjang; i++) cout << karakter;
    cout << "\n";
}

void cetakJudul() {
    cout << "\n";
    cetakGaris('=', 65);
    cout << "   SISTEM JALUR EVAKUASI TERCEPAT\n";
    cout << "   Penanggulangan Bencana Alam\n";
    cout << "   Implementasi Algoritma Dijkstra - C++\n";
    cetakGaris('=', 65);
    cout << "   Skenario : Bencana Banjir Bandang\n";
    cout << "   Wilayah  : Pegunungan, Pedesaan & Perkotaan Pinggiran\n";
    cetakGaris('=', 65);
    cout << "\n";
}

// ----- MODE MANUAL -----
void inputGrafManual() {
    cout << "\n";
    cetakGaris('-', 65);
    cout << "  MODE MANUAL: Anda menentukan sendiri jumlah edge\n";
    cout << "  dan memasukkan data setiap edge satu per satu.\n";
    cetakGaris('-', 65);

    cout << "\n  Referensi nomor node:\n";
    cetakGaris('-', 55);
    for (int i = 0; i < jumlahNode; i++) {
        cout << "  [" << i << "] " << nodes[i].kode
             << " - " << nodes[i].nama << "\n";
    }
    cetakGaris('-', 55);

    cout << "\n  Petunjuk pengisian edge:\n";
    cout << "  - Masukkan nomor node asal dan tujuan (lihat referensi di atas)\n";
    cout << "  - Masukkan jarak antar node dalam satuan km\n";
    cout << "  - Masukkan waktu tempuh dalam satuan menit\n";
    cout << "  - Setiap edge otomatis berlaku dua arah (A ke B = B ke A)\n";
    cout << "  - Edge duplikat akan dilewati otomatis\n";
    cout << "\n  Contoh: node asal=0, tujuan=1, jarak=2.5, waktu=7\n";
    cout << "          artinya jalur dari " << nodes[0].kode
         << " ke " << nodes[1].kode
         << " sejauh 2.5 km ditempuh dalam 7 menit\n\n";

    jumlahEdge = inputInt("  Masukkan jumlah edge yang ingin diisi : ", 1, 500);
    cout << "\n";

    int edgeBerhasil = 0;
    for (int i = 0; i < jumlahEdge; i++) {
        cout << "  -- Edge ke-" << (i + 1) << " dari " << jumlahEdge << " --\n";

        int asal, tujuan;
        float jarak, waktu;

        asal = inputInt(
            "  Node asal   [0-" + to_string(jumlahNode - 1) + "] : ",
            0, jumlahNode - 1);

        while (true) {
            tujuan = inputInt(
                "  Node tujuan [0-" + to_string(jumlahNode - 1) + "] : ",
                0, jumlahNode - 1);
            if (tujuan != asal) break;
            cout << MERAH << "  [!] Node tujuan tidak boleh sama dengan node asal. Coba lagi." << RESET << "\n";
        }

        // Cek duplikat
        bool duplikat = false;
        for (auto& e : graf[asal]) {
            if (e.tujuan == tujuan) { duplikat = true; break; }
        }
        if (duplikat) {
            cout << MERAH << "  [!] Edge " << nodes[asal].kode << " <-> "
                 << nodes[tujuan].kode
                 << " sudah ada sebelumnya. Edge ini dilewati." << RESET << "\n\n";
            continue;
        }

        jarak = inputFloat("  Jarak (km)    contoh: 2.5  -> ", 0.1f);
        waktu = inputFloat("  Waktu (menit) contoh: 7    -> ", 0.1f);
        cout << "\n";

        graf[asal].push_back({tujuan, jarak, waktu});
        graf[tujuan].push_back({asal,  jarak, waktu});
        edgeBerhasil++;
    }

    jumlahEdge = edgeBerhasil;
    cout << "  [OK] Total edge berhasil ditambahkan: " << jumlahEdge << "\n\n";
}

// ----- MODE OTOMATIS (DIPANDU) -----
void inputGrafOtomatis() {
    cout << "\n";
    cetakGaris('-', 65);
    cout << "  MODE DIPANDU: Program akan menanyakan satu per satu\n";
    cout << "  setiap kemungkinan pasangan node. Anda cukup menjawab\n";
    cout << "  y (ada jalur) atau n (tidak ada jalur).\n";
    cetakGaris('-', 65);

    cout << "\n  Petunjuk:\n";
    cout << "  - Program menampilkan pasangan node secara berurutan\n";
    cout << "  - Jawab 'y' jika ada jalur langsung antara dua node tersebut\n";
    cout << "  - Jawab 'n' jika tidak ada jalur langsung\n";
    cout << "  - Jika 'y', masukkan jarak (km) dan waktu (menit)\n";
    cout << "  - Setiap jalur otomatis berlaku dua arah\n";
    cout << "\n  Contoh: Ada jalur antara [A] Desa dan [B] Persimpangan?\n";
    cout << "          Jawab 'y', lalu isi jarak=2.0 dan waktu=5\n\n";

    jumlahEdge = 0;

    for (int i = 0; i < jumlahNode; i++) {
        for (int j = i + 1; j < jumlahNode; j++) {
            cout << "  Ada jalur antara [" << nodes[i].kode << "] "
                 << nodes[i].nama
                 << " <-> [" << nodes[j].kode << "] "
                 << nodes[j].nama << " ? (y/n) : ";

            char jawab = inputYN("");

            if (jawab == 'y') {
                float jarak = inputFloat(
                    "    Jarak (km)    contoh: 2.5  -> ", 0.1f);
                float waktu = inputFloat(
                    "    Waktu (menit) contoh: 7    -> ", 0.1f);
                cout << "\n";

                graf[i].push_back({j, jarak, waktu});
                graf[j].push_back({i, jarak, waktu});
                jumlahEdge++;
            } else {
                cout << "\n";
            }
        }
    }

    cout << "  [OK] Total edge berhasil ditambahkan: " << jumlahEdge << "\n\n";
}

// ----- FUNGSI INPUT GRAF UTAMA -----
void inputGraf() {
    cetakGaris('=', 65);
    cout << "  INPUT DATA GRAF\n";
    cetakGaris('=', 65);

    cout << "\n  Petunjuk:\n";
    cout << "  - Node adalah setiap lokasi dalam peta evakuasi\n";
    cout << "  - Contoh node: Desa Terdampak, Jembatan, Posko BPBD, dll\n";
    cout << "  - Minimal 2 node, maksimal 100 node\n\n";

    jumlahNode = inputInt("  Masukkan jumlah node (lokasi) : ", 2, 100);

    nodes.resize(jumlahNode);
    graf.resize(jumlahNode);

    cout << "\n  Masukkan data setiap node:\n";
    cetakGaris('-', 50);
    cout << "  Petunjuk pengisian node:\n";
    cout << "  - Kode : 1 huruf kapital sebagai singkatan lokasi\n";
    cout << "           Contoh: A, B, C, D ...\n";
    cout << "  - Nama : nama lengkap lokasi tersebut\n";
    cout << "           Contoh: Desa Terdampak Banjir\n";
    cetakGaris('-', 50);
    cout << "\n";

    for (int i = 0; i < jumlahNode; i++) {
        cout << "  -- Node ke-" << (i + 1) << " --\n";
        nodes[i].kode = inputString("  Kode (contoh: A) : ");
        nodes[i].nama = inputString("  Nama lokasi      : ");
        cout << "\n";
    }

    cout << "\n";
    cetakGaris('=', 65);
    cout << "  PILIH MODE INPUT EDGE (JALUR ANTAR NODE)\n";
    cetakGaris('=', 65);
    cout << "\n  [1] Mode Manual  - Anda tentukan sendiri jumlah edge\n";
    cout << "                     dan isi data setiap edge satu per satu\n";
    cout << "                     (cocok jika sudah tahu pasti edge mana saja)\n\n";
    cout << "  [2] Mode Dipandu - Program menanyakan satu per satu\n";
    cout << "                     semua kemungkinan pasangan node\n";
    cout << "                     (cocok jika belum hafal kombinasi edge)\n\n";

    int modeInput = inputInt("  Pilihan (1/2) : ", 1, 2);

    if (modeInput == 1) inputGrafManual();
    else                inputGrafOtomatis();
}

// KONFIRMASI DATA
void tampilKonfirmasiData() {
    cout << "\n";
    cetakGaris('=', 65);
    cout << "  KONFIRMASI DATA YANG DIINPUT\n";
    cetakGaris('=', 65);

    cout << "\n  DAFTAR NODE (" << jumlahNode << " lokasi):\n";
    cetakGaris('-', 55);
    cout << "  " << left << setw(6) << "No"
                          << setw(8) << "Kode"
                          << "Nama Lokasi\n";
    cetakGaris('-', 55);
    for (int i = 0; i < jumlahNode; i++) {
        cout << "  " << left << setw(6) << i
                              << setw(8) << nodes[i].kode
                              << nodes[i].nama << "\n";
    }

    cout << "\n  DAFTAR EDGE (" << jumlahEdge << " jalur):\n";
    cetakGaris('-', 65);
    cout << "  " << left << setw(22) << "Dari"
                          << setw(22) << "Ke"
                          << setw(12) << "Jarak(km)"
                          << "Waktu(mnt)\n";
    cetakGaris('-', 65);

    for (int i = 0; i < jumlahNode; i++) {
        for (auto& e : graf[i]) {
            if (i < e.tujuan) {
                string dari = nodes[i].kode + "-" + nodes[i].nama;
                string ke   = nodes[e.tujuan].kode + "-" + nodes[e.tujuan].nama;
                if (dari.length() > 20) dari = dari.substr(0, 19) + ".";
                if (ke.length()   > 20) ke   = ke.substr(0, 19)   + ".";
                cout << "  " << left
                     << setw(22) << dari
                     << setw(22) << ke
                     << setw(12) << fixed << setprecision(1) << e.jarak
                     << fixed    << setprecision(1) << e.waktu << "\n";
            }
        }
    }
    cout << "\n";
}

void tampilGrafASCII() {
    cout << "\n";
    cetakGaris('=', 65);
    cout << "  VISUALISASI GRAF JALUR EVAKUASI\n";
    cetakGaris('=', 65);
    cout << "\n";

    bool adaEdge = false;
    for (int i = 0; i < jumlahNode; i++) {
        for (auto& e : graf[i]) {
            if (i < e.tujuan) {
                adaEdge = true;
                cout << "  [" << nodes[i].kode << "] "
                     << nodes[i].nama << "\n";
                cout << "       |\n";
                cout << "       +---(";
                cout << fixed << setprecision(1) << e.jarak << " km / ";
                cout << fixed << setprecision(1) << e.waktu << " mnt";
                cout << ")---> [" << nodes[e.tujuan].kode << "] "
                     << nodes[e.tujuan].nama << "\n\n";
            }
        }
    }

    if (!adaEdge) cout << "  (Tidak ada edge untuk ditampilkan)\n\n";

    cetakGaris('-', 65);
    cout << "  Ket : [X]                   = Node / Lokasi\n";
    cout << "        +---(jarak/waktu)--->=  Jalur penghubung (berlaku dua arah)\n";
    cetakGaris('=', 65);
    cout << "\n";
}

// ============================================================
//  DIJKSTRA
// ============================================================

vector<float> dijkstra(int sumber, int mode, float alpha, float beta,
                        vector<int>& parent) {
    vector<float> dist(jumlahNode, INF);
    vector<bool>  visited(jumlahNode, false);
    parent.assign(jumlahNode, -1);
    dist[sumber] = 0;

    priority_queue<pair<float,int>,
                   vector<pair<float,int>>,
                   greater<pair<float,int>>> pq;
    pq.push({0.0f, sumber});

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        if (visited[u]) continue;
        visited[u] = true;

        for (auto& e : graf[u]) {
            int   v     = e.tujuan;
            float bobot = 0.0f;

            if      (mode == 1) bobot = e.jarak;
            else if (mode == 2) bobot = e.waktu;
            else                bobot = (alpha * e.jarak) + (beta * e.waktu);

            if (dist[u] + bobot < dist[v]) {
                dist[v]   = dist[u] + bobot;
                parent[v] = u;
                pq.push({dist[v], v});
            }
        }
    }
    return dist;
}

// ============================================================
//  REKONSTRUKSI JALUR
// ============================================================

vector<int> rekonstruksiJalur(int sumber, int tujuan,
                               const vector<int>& parent) {
    vector<int> jalur;
    if (tujuan != sumber && parent[tujuan] == -1) return jalur;
    int curr = tujuan;
    while (curr != -1) {
        jalur.push_back(curr);
        curr = parent[curr];
    }
    reverse(jalur.begin(), jalur.end());
    return jalur;
}

// ============================================================
//  HITUNG TOTAL JARAK & WAKTU
// ============================================================

void hitungTotal(const vector<int>& jalur,
                 float& totalJarak, float& totalWaktu) {
    totalJarak = 0.0f;
    totalWaktu = 0.0f;
    for (int i = 0; i < (int)jalur.size() - 1; i++) {
        int u = jalur[i], v = jalur[i + 1];
        for (auto& e : graf[u]) {
            if (e.tujuan == v) {
                totalJarak += e.jarak;
                totalWaktu += e.waktu;
                break;
            }
        }
    }
}

// ============================================================
//  VISUALISASI 2 — JALUR TERPILIH
// ============================================================

void tampilJalur(const vector<int>& jalur, int mode,
                 float alpha, float beta) {
    cout << "\n";
    cetakGaris('=', 60);
    cout << "  JALUR EVAKUASI DITEMUKAN\n";
    if      (mode == 1) cout << "  Mode : Jarak Terpendek\n";
    else if (mode == 2) cout << "  Mode : Waktu Tercepat\n";
    else {
        cout << "  Mode : Gabungan Optimal";
        cout << " (alpha=" << fixed << setprecision(1) << alpha;
        cout << ", beta="  << fixed << setprecision(1) << beta << ")\n";
    }
    cetakGaris('=', 60);

    if (jalur.empty()) {
        cout << "\n" << MERAH << "  [!] Tidak ada jalur yang dapat ditemukan.\n";
        cout << "      Periksa apakah node sumber dan tujuan terhubung." << RESET << "\n\n";
        cetakGaris('=', 60);
        return;
    }

    float totalJarak = 0.0f, totalWaktu = 0.0f;
    cout << "\n";

    for (int i = 0; i < (int)jalur.size() - 1; i++) {
        int   u = jalur[i], v = jalur[i + 1];
        float j = 0.0f,     w = 0.0f;
        for (auto& e : graf[u]) {
            if (e.tujuan == v) { j = e.jarak; w = e.waktu; break; }
        }
        totalJarak += j;
        totalWaktu += w;

        cout << "  [" << nodes[u].kode << "] "
             << "---(";
        cout << fixed << setprecision(1) << j << " km, ";
        cout << fixed << setprecision(1) << w << " mnt)";
        cout << "---> [" << nodes[v].kode << "]\n";
    }

    float bobotGab = (alpha * totalJarak) + (beta * totalWaktu);

    cout << "\n";
    cetakGaris('-', 60);
    cout << "  Total Jarak    : " << fixed << setprecision(1) << totalJarak << " km\n";
    cout << "  Total Waktu    : " << fixed << setprecision(1) << totalWaktu << " menit\n";
    cout << "  Bobot Gabungan : " << fixed << setprecision(2) << bobotGab << "\n";
    cetakGaris('=', 60);
    cout << "\n";
}

// ============================================================
//  VISUALISASI 3 — TABEL JARAK MINIMUM
// ============================================================

void tampilTabelMinimum(int sumber,
                         const vector<int>& p1,
                         const vector<int>& p2,
                         const vector<int>& p3) {
    cout << "\n";
    cetakGaris('=', 72);
    cout << "  TABEL JARAK MINIMUM DARI NODE ["
         << nodes[sumber].kode << "] "
         << nodes[sumber].nama << "\n";
    cetakGaris('=', 72);
    cout << "  " << left
         << setw(6)  << "Node"
         << setw(26) << "Nama Lokasi"
         << setw(13) << "Jarak(km)"
         << setw(13) << "Waktu(mnt)"
         << "Bobot Gab.\n";
    cetakGaris('-', 72);

    for (int i = 0; i < jumlahNode; i++) {
        vector<int> jalurG = rekonstruksiJalur(sumber, i, p3);

        float tJg = 0.0f, tWg = 0.0f;
        hitungTotal(jalurG, tJg, tWg);
        float bg = (0.5f * tJg) + (0.5f * tWg);

        string nama = nodes[i].nama;
        if (nama.length() > 24) nama = nama.substr(0, 22) + "..";

        cout << "  " << left << setw(6) << nodes[i].kode
                              << setw(26) << nama;

        if (jalurG.empty() && i != sumber) {
            cout << setw(13) << "N/A"
                 << setw(13) << "N/A"
                 << "N/A\n";
        } else {
            cout << setw(13) << fixed << setprecision(1) << tJg
                 << setw(13) << fixed << setprecision(1) << tWg
                 << fixed    << setprecision(2) << bg << "\n";
        }
    }
    cetakGaris('=', 72);
    cout << "\n";
}

// ============================================================
//  VISUALISASI 4 — PERBANDINGAN TIGA MODE
// ============================================================

void tampilPerbandinganTigaMode(int sumber, int tujuan,
                                 float alpha, float beta,
                                 const vector<int>& p1,
                                 const vector<int>& p2,
                                 const vector<int>& p3) {
    vector<int> j1 = rekonstruksiJalur(sumber, tujuan, p1);
    vector<int> j2 = rekonstruksiJalur(sumber, tujuan, p2);
    vector<int> j3 = rekonstruksiJalur(sumber, tujuan, p3);

    float jarak1=0,waktu1=0, jarak2=0,waktu2=0, jarak3=0,waktu3=0;
    hitungTotal(j1, jarak1, waktu1);
    hitungTotal(j2, jarak2, waktu2);
    hitungTotal(j3, jarak3, waktu3);

    float bg1 = (alpha * jarak1) + (beta * waktu1);
    float bg2 = (alpha * jarak2) + (beta * waktu2);
    float bg3 = (alpha * jarak3) + (beta * waktu3);

    auto buatStrJalur = [](const vector<int>& jl,
                            const vector<Node>& nd) -> string {
        if (jl.empty()) return "Tdk ada jalur";
        string s;
        for (int i = 0; i < (int)jl.size(); i++) {
            if (i > 0) s += "-";
            s += nd[jl[i]].kode;
        }
        return s;
    };

    auto potong = [](string s, int maks) -> string {
        if ((int)s.length() > maks) return s.substr(0, maks - 2) + "..";
        return s;
    };

    auto fmtFloat = [](float v, int presisi) -> string {
        ostringstream oss;
        oss << fixed << setprecision(presisi) << v;
        return oss.str();
    };

    string sj1 = potong(buatStrJalur(j1, nodes), 16);
    string sj2 = potong(buatStrJalur(j2, nodes), 16);
    string sj3 = potong(buatStrJalur(j3, nodes), 16);

    cout << "\n";
    cetakGaris('=', 74);
    cout << "  PERBANDINGAN HASIL TIGA MODE PENCARIAN\n";
    cout << "  Dari [" << nodes[sumber].kode << "] " << nodes[sumber].nama
         << "  -->  [" << nodes[tujuan].kode << "] " << nodes[tujuan].nama << "\n";
    cetakGaris('=', 74);
    cout << "  " << left
         << setw(18) << "Keterangan"
         << setw(18) << "MODE 1 (Jarak)"
         << setw(18) << "MODE 2 (Waktu)"
         << "MODE 3 (Gabungan)\n";
    cetakGaris('-', 74);
    cout << "  " << left << setw(18) << "Jalur"
         << setw(18) << sj1 << setw(18) << sj2 << sj3 << "\n";
    cout << "  " << left << setw(18) << "Total Jarak"
         << setw(18) << (fmtFloat(jarak1, 1) + " km")
         << setw(18) << (fmtFloat(jarak2, 1) + " km")
         << (fmtFloat(jarak3, 1) + " km") << "\n";
    cout << "  " << left << setw(18) << "Total Waktu"
         << setw(18) << (fmtFloat(waktu1, 1) + " mnt")
         << setw(18) << (fmtFloat(waktu2, 1) + " mnt")
         << (fmtFloat(waktu3, 1) + " mnt") << "\n";
    cout << "  " << left << setw(18) << "Bobot Gabungan"
         << setw(18) << fmtFloat(bg1, 2)
         << setw(18) << fmtFloat(bg2, 2)
         << fmtFloat(bg3, 2) << "\n";
    cetakGaris('-', 74);

    string rekomendasi = "Tidak ada jalur yang tersedia";
    float  minBG       = INF;
    if (!j1.empty() && bg1 < minBG) { minBG = bg1; rekomendasi = "MODE 1 - Jarak Terpendek"; }
    if (!j2.empty() && bg2 < minBG) { minBG = bg2; rekomendasi = "MODE 2 - Waktu Tercepat";  }
    if (!j3.empty() && bg3 < minBG) { minBG = bg3; rekomendasi = "MODE 3 - Gabungan Optimal";}

    cout << "\n  >> " << HIJAU << "Rekomendasi : " << rekomendasi << RESET << "\n";
    cetakGaris('=', 74);
    cout << "\n";
}

// ============================================================
//  INPUT PILIHAN PENCARIAN
// ============================================================

void inputPilihanPencarian(int& sumber, int& tujuan, int& mode,
                            float& alpha, float& beta) {
    cetakGaris('=', 65);
    cout << "  INPUT PILIHAN PENCARIAN JALUR EVAKUASI\n";
    cetakGaris('=', 65);

    cout << "\n  Daftar Lokasi:\n";
    cetakGaris('-', 55);
    for (int i = 0; i < jumlahNode; i++) {
        cout << "  [" << i << "] " << nodes[i].kode
             << " - " << nodes[i].nama << "\n";
    }
    cetakGaris('-', 55);
    cout << "\n  Petunjuk:\n";
    cout << "  - Node " << CYAN << "SUMBER" << RESET << " adalah lokasi awal titik evakuasi (lokasi bencana)\n";
    cout << "  - Node " << CYAN << "TUJUAN" << RESET << " adalah lokasi akhir / posko aman\n";
    cout << "  - Masukkan nomor sesuai daftar di atas\n\n";

    sumber = inputInt("  Nomor node " CYAN "SUMBER" RESET " (titik start evakuasi) : ",
                      0, jumlahNode - 1);

    while (true) {
        tujuan = inputInt("  Nomor node " CYAN "TUJUAN" RESET " (posko aman)          : ",
                          0, jumlahNode - 1);

        if (tujuan != sumber) break;
        cout << MERAH << "  [!] Node tujuan tidak boleh sama dengan node sumber. Coba lagi." << RESET << "\n";
    }

    cout << "\n  Pilih Mode Optimasi:\n";
    cetakGaris('-', 55);
    cout << "  [1] " << KUNING << "Jarak Terpendek" << RESET << "  - Meminimalkan total jarak (km)\n";
    cout << "                         Gunakan jika ingin rute sesingkat mungkin\n";
    cout << "  [2] " << KUNING << "Waktu Tercepat" << RESET << "   - Meminimalkan total waktu (menit)\n";
    cout << "                         Gunakan jika kecepatan sampai yang utama\n";
    cout << "  [3] " << KUNING << "Gabungan Optimal" << RESET << " - Mempertimbangkan jarak DAN waktu sekaligus\n";
    cout << "                         " << KUNING << "REKOMENDASI" << RESET << " untuk skenario evakuasi\n";
    cetakGaris('-', 55);

    mode = inputInt("\n  Pilihan (1/2/3) : ", 1, 3);

    alpha = 0.5f;
    beta  = 0.5f;

    if (mode == 3) {
        cout << "\n  Atur bobot kepentingan jarak (alpha) dan waktu (beta).\n";
        cout << "  Syarat    : alpha + beta = 1\n";
        cout << "  Rentang   : alpha antara 0.1 sampai 0.9\n";
        cout << "  Default   : alpha=0.5, beta=0.5 (jarak & waktu sama pentingnya)\n";
        cout << "  Contoh    : alpha=0.3 artinya waktu lebih diprioritaskan\n";
        cout << "              alpha=0.7 artinya jarak lebih diprioritaskan\n";
        cout << "  Masukkan 0 untuk pakai nilai default.\n";

        while (true) {
            alpha = inputFloat("\n  Masukkan nilai alpha : ", 0.0f);
            if (alpha == 0.0f) {
                alpha = 0.5f; beta = 0.5f;
                break;
            }
            if (alpha >= 0.1f && alpha <= 0.9f) {
                beta = 1.0f - alpha;
                break;
            }
            cout << MERAH << "  [!] Alpha harus antara 0.1 dan 0.9. Coba lagi." << RESET << "\n";
        }
        cout << "  Menggunakan alpha=" << fixed << setprecision(1) << alpha
             << ", beta=" << fixed << setprecision(1) << beta << "\n";
    }
    cout << "\n";
}

// MAIN
int main() {
    cetakJudul();

    inputGraf();
    tampilKonfirmasiData();
    tampilGrafASCII();

    int   sumber, tujuan, mode;
    float alpha = 0.5f, beta = 0.5f;
    inputPilihanPencarian(sumber, tujuan, mode, alpha, beta);

    // Jalankan Dijkstra 3 mode sekaligus, hanya 3 kali
    vector<int> p1, p2, p3;
    dijkstra(sumber, 1, alpha, beta, p1);
    dijkstra(sumber, 2, alpha, beta, p2);
    dijkstra(sumber, 3, alpha, beta, p3);

    // Rekonstruksi jalur sesuai mode yang dipilih user
    vector<int>* pPilihan = nullptr;
    if      (mode == 1) pPilihan = &p1;
    else if (mode == 2) pPilihan = &p2;
    else                pPilihan = &p3;

    vector<int> jalurPilihan = rekonstruksiJalur(sumber, tujuan, *pPilihan);

    tampilJalur(jalurPilihan, mode, alpha, beta);
    tampilTabelMinimum(sumber, p1, p2, p3);
    tampilPerbandinganTigaMode(sumber, tujuan, alpha, beta, p1, p2, p3);

    cetakGaris('=', 65);
    cout << "  PROGRAM SELESAI\n";
    cout << "  Semoga evakuasi berjalan lancar dan selamat.\n";
    cetakGaris('=', 65);
    cout << "\n";

    return 0;
}