---
tags:
  - projet/csharp
  - type/concept
  - techno/csharp
  - techno/dotnet
  - sujet/architecture
  - statut/a-jour
aliases:
  - Result pattern
cree: 2026-09-30
maj: 2026-09-30
---

# Résultat d'un cas d'usage (Result)

> [!abstract] En une phrase
> Un service de `Application` renvoie un **`Result<T>`** : soit un **succès** qui porte une valeur, soit un **échec** qui porte un **code d'erreur** métier. On évite ainsi le `null`, qui ne dit pas pourquoi ça a échoué, et les exceptions, qui sont réservées à l'imprévu. À lire avant : [[01 Clean architecture en couches]].

---

## 🧩 La classe

```csharp
// Application.Contracts/Common/Result.cs
public record Result<T>
{
    public bool IsSuccess { get; }
    public T? Value { get; }            // renseigné seulement en cas de succès
    public string? ErrorCode { get; }   // ex : "auth.invalid_credentials"

    // Privé : on passe obligatoirement par Success(...) ou Failure(...).
    private Result(bool isSuccess, T? value, string? errorCode)
    {
        IsSuccess = isSuccess;
        Value = value;
        ErrorCode = errorCode;
    }

    public static Result<T> Success(T value) => new(true, value, null);
    public static Result<T> Failure(string errorCode) => new(false, default, errorCode);
}
```

| Code | Pourquoi |
|---|---|
| `record` | Une classe « donnée » : comparaison par valeur, lecture seule |
| Constructeur `private` | Impossible de fabriquer un résultat incohérent (un succès sans valeur, par exemple) |
| `Success(…)` / `Failure(…)` | Les deux seules façons de créer un résultat, lisibles d'un coup d'œil |
| `ErrorCode` en texte stable | Le front peut réagir au code (`auth.invalid_credentials`) sans lire un message |

---

## 🏷️ Les codes d'erreur en constantes

```csharp
// Application.Contracts/Auth/AuthErrors.cs
public static class AuthErrors
{
    // Volontairement unique pour tous les échecs de login (anti-énumération d'emails).
    public const string InvalidCredentials = "auth.invalid_credentials";
    public const string EmailTaken = "auth.email_taken";
}
```

👉 Des **constantes** plutôt que des chaînes en dur : une faute de frappe devient une **erreur de compilation**.

---

## 🌐 Côté controller

```csharp
[HttpPost("login")]
public async Task<IActionResult> Login([FromBody] LoginRequest request, CancellationToken cancellationToken)
{
    var result = await _authService.LoginAsync(request, cancellationToken);

    if (!result.IsSuccess)
        return Unauthorized(new { code = result.ErrorCode, message = "Email ou mot de passe incorrect." });

    return Ok(result.Value);
}
```

Le corps de l'erreur porte **un code pour la machine** (le front) et **un message pour l'humain**.

---

## ⚖️ Result ou exception ?

| Situation | Choix |
|---|---|
| Échec **prévu** par le métier : mauvais mot de passe, email déjà pris | **`Result.Failure(code)`** |
| Problème **imprévu ou technique** : base injoignable, service externe en panne | **Exception** (attrapée plus haut, qui renvoie un 500 ou un 503) |

---

## 🔗 Liens

- [[01 Clean architecture en couches]] — où rangent `Result` et les codes d'erreur
- [[02 Zabbix — l'API JSON-RPC]] — un exemple d'exception technique (`ZabbixException`)
