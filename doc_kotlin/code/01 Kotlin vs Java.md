---
tags:
  - projet/kotlin
  - type/concept
  - techno/kotlin
  - techno/java
  - statut/a-jour
cours: P1
cree: 2026-09-28
maj: 2026-09-29
---

# Kotlin vs Java

> [!abstract] Idée
> Kotlin fait **moins écrire** que Java pour le même résultat (voir la philosophie *[[00 Compose|moins de code = moins de bugs]]*).

---

## 🚫 Pas besoin d'accesseurs

> [!quote] Tes mots
> En Kotlin, dans les classes, **pas besoin d'avoir d'accesseur** genre `get` / `set`. Ces fonctions-là **n'ont pas besoin d'être codées**.

En Java, on écrit les **getters / setters** à la main :

```java
// Java
public class Personne {
    private int id;
    public int getId() { return id; }        // accesseur
    public void setId(int id) { this.id = id; } // mutateur
}
```

En Kotlin, une **propriété** génère ses accesseurs **automatiquement** :

```kotlin
// Kotlin
class Personne(var id: Int)   // get/set fournis automatiquement

val p = Personne(1)
p.id = 2        // appelle le "set" sous le capot
println(p.id)   // appelle le "get" sous le capot
```

> [!tip] À retenir
> On **accède directement** à la propriété (`p.id`) : Kotlin appelle le get/set tout seul. On ne code un accesseur que si on veut un **comportement particulier**.

---

## 🔗 Liens

- [[00 Code]]
- [[02 Les classes]]
