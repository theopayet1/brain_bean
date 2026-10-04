---
tags:
  - projet/cpp
  - type/reference
  - techno/cpp
  - techno/cmake
  - statut/a-jour
aliases:
  - Cheat sheet C++
  - Mémo C++
cree: 2026-10-04
maj: 2026-10-04
---

# Antisèche C++

> [!abstract] En une phrase
> La syntaxe la plus utilisée dans cette doc, sur une seule page.

---

## 🧱 Bases

```cpp
int n = 0;                         // entier
std::int64_t id = 42;              // entier 64 bits (#include <cstdint>)
double x = 1.5;
bool ok = true;
std::string s = "texte";
auto v = computeSomething();       // type déduit
const int Max = 10;                // ne change pas
constexpr int Size = 4;            // connu à la compilation

if (a && !b) { } else if (c) { } else { }
for (const auto& item : items) { }
for (int i = 0; i < n; ++i) { }
while (condition) { }
switch (e) { case E::A: return 1; case E::B: return 2; }
```

## 🧩 Fonctions

```cpp
int add(int a, int b);                          // déclaration (.hpp)
int add(int a, int b) { return a + b; }         // définition (.cpp)
void read(const std::string& s);                // lire sans copier
void change(std::string& s);                    // modifier l'original
void keep(std::string s);                       // copie à garder (puis std::move)
[[nodiscard]] bool tryIt();                     // résultat à vérifier
auto f = [&total](int x) { total += x; };       // lambda
```

## 🏗️ Classes

```cpp
struct Point { int x = 0; int y = 0; };
Point p{.x = 1, .y = 2};

class Service
{
public:
    explicit Service(IRepo& repo) : repo_(repo) {}
    [[nodiscard]] int count() const;
private:
    IRepo& repo_;
};

class IRepo                                     // interface
{
public:
    virtual ~IRepo() = default;
    [[nodiscard]] virtual int count() = 0;
};

class RepoSqlite : public IRepo
{
public:
    int count() override;
};
```

## 📦 Conteneurs et types utiles

```cpp
std::vector<int> v{1, 2, 3};   v.push_back(4);   v.size();   v.empty();
std::map<std::string, int> m;  m["a"] = 1;   if (m.contains("a")) { }
std::optional<int> o;          if (o) { *o; }   o.value_or(0);
std::expected<int, Error> r;   if (r) { *r; } else { r.error(); }
std::variant<int, std::string> var;   std::get<int>(var);
std::unique_ptr<T> p = std::make_unique<T>(args);
std::string_view view = "constante";
std::format("{} a {} ans", name, age);
```

## 🔁 Algorithmes

```cpp
std::ranges::sort(books, {}, &Book::title);
auto it = std::ranges::find_if(books, [](const Book& b) { return !b.read; });
auto n = std::ranges::count_if(books, &Book::read);
std::erase_if(books, [](const Book& b) { return b.read; });
```

## 🧵 Fils

```cpp
std::jthread t([] { work(); });                 // join automatique
std::mutex m;  { const std::scoped_lock lock{m}; /* protégé */ }
```

## 📅 Dates

```cpp
using namespace std::chrono;
year_month_day d = year{2026} / 10 / 4;
d.ok();                                          // date valide ?
sys_days later = sys_days{d} + days{30};
std::format("{:%F}", sys_days{d});               // « 2026-10-04 »
```

---

## 🔨 CMake

```cmake
add_library(x STATIC a.cpp b.cpp)
add_library(proj::x ALIAS x)
target_include_directories(x PUBLIC "${SRC}")
target_link_libraries(x PUBLIC proj::y PRIVATE lib::z)
add_executable(app WIN32 main.cpp)
find_package(glaze CONFIG REQUIRED)
option(MY_OPTION "texte" ON)
if(WIN32) ... endif()
```

## ▶️ Commandes

```powershell
cmake --preset debug ; cmake --build --preset debug ; ctest --preset debug
ctest --preset debug -R tests_domain
.\scripts\check.ps1
cd frontend ; npm run dev ; npm run check
```

---

## 🔗 Liens

- [[01 Conventions de code C++]] — les règles de style
- [[03 Glossaire C++]] — les mots
