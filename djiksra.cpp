#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <sstream>

using namespace std;

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
            cout << "  [!] Node tujuan tidak boleh sama dengan node asal. Coba lagi.\n";
        }

        // Cek duplikat
        bool duplikat = false;
        for (auto& e : graf[asal]) {
            if (e.tujuan == tujuan) { duplikat = true; break; }
        }
        if (duplikat) {
            cout << "  [!] Edge " << nodes[asal].kode << " <-> "
                 << nodes[tujuan].kode
                 << " sudah ada sebelumnya. Edge ini dilewati.\n\n";
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
        cout << "\n  [!] Tidak ada jalur yang dapat ditemukan.\n";
        cout << "      Periksa apakah node sumber dan tujuan terhubung.\n\n";
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

    cout << "\n  >> Rekomendasi : " << rekomendasi << "\n";
    cetakGaris('=', 74);
    cout << "\n";
}