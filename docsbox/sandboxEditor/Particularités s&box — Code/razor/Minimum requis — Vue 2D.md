
Un panel UI en s&box = toujours **3 fichiers** dans le même dossier : 

``` 
monpanel/
├── MonPanel.cs 
├── MonPanelView.razor 
└── MonPanelView.razor.scss
```

> [!warning]
> [ pour que le lien entre les 3 fichier se fasse bien respecter le nommage ]
> nom.cs
> nomView.razor
> nomView.razor.scss


---
## 📄 nom.cs — Le contrôleur


```csharp
//import obligatoire
using Sandbox;
using Sandbox.UI;

// [1] Namespace — doit correspondre au @namespace du .razor
namespace TonJeu.UI.MonPanel;

// [2] partial obligatoire — le .razor génère l'autre moitié de la classe
// [3] hérite de Panel (classe de base de toute UI s&box)
public partial class MonPanel : Panel
{
	// [4] Contrôle QUAND le panel se re-render
	// Mettre ici les variables dont dépend l'affichage
	public override int BuildHash()
	{
		//HashCodem: int unique. représente l'état de tes données à un instant T.
		//s&box compare ce nombre chaque frame. s'il change, le panel se re-render.
		return HashCode.Combine();
	}
 }
```


### Zones
| Zone       | Rôle                                                          |
| ---------- | ------------------------------------------------------------- |
| namespace  | Doit être **identique** à celui du `.razor`                   |
| partial    | Le `.razor` compile en C# — les deux forment une seule classe |
| : Panel    | Classe de base s&box pour toute UI                            |
|  BuildHash | S&box appelle ça chaque frame pour décider si re-render       |

---

## 📄 MonPanelView.razor — La vue
```razor
@* [1] Imports obligatoires *@
@using Sandbox;
@using Sandbox.UI;

@* [2] Namespace — doit correspondre au .cs *@
@namespace TonJeu.UI.MonPanel

@* [3] Lie ce fichier au .cs (code-behind) *@
@inherits MonPanel

@* [4] Balise racine — son nom = sélecteur racine du SCSS *@

	@* [5] Contenu HTML-like du panel *@
	Mon Panel
	
	
```

minimum :
```razor
@using Sandbox;
@using Sandbox.UI;

@namespace TonJeu.UI.MonPanel

@inherits PanelComponent

<root>
    <label>Hello</label>
</root>

@code
{
    protected override int BuildHash() => System.HashCode.Combine( 0 );
}
```
#### Zones
| Zone             | Rôle                                                          |
| ---------------- | ------------------------------------------------------------- |
| `[1]` @using     | Donne accès aux types s&box                                   |
| `[2]` @namespace | Doit matcher le `.cs` exactement                              |
| `[3]` @inherits  | Branche le code-behind — sans ça, le `.cs` est ignoré         |
| `[4]` < root >   | Balise racine obligatoire — son nom devient le sélecteur SCSS |
| `[5]` contenu    | HTML classique + classes CSS                                  |


>[!warning]
> La balise racine peut s'appeler autrement que `<root>` mais son nom 
> **doit correspondre** au sélecteur racine dans le `.scss`.

---

## 📄 MonPanelView.razor.scss — Les styles

```scss
// [1] Sélecteur racine — doit matcher la balise du .razor
root {
	// [2] Le panel doit avoir une taille explicite pour être visible
	width: 100%;
	height: 100%;
	
	// [3] Libérer la souris si ce panel doit être interactif 
	pointer-events: all;
	
	// [4] Styles normaux ensuite
	background-color: rgba(0, 0, 0, 0.8);
	
	// [5] Cibler les enfants normalement
	.titre {
		color: white;
		font-size: 24px;
		}
	}
}
```

#### Zones
| Zone                   | Rôle                                                    |
| ---------------------- | ------------------------------------------------------- |
| `[1]` sélecteur racine | Doit matcher `<root>` (ou le nom choisi dans le .razor) |
| `[2]` taille           | Sans taille explicite, le panel est invisible           |
| `[3]` pointer-events   | Nécessaire si le panel a des boutons / interactions     |
| `[4]` styles           | CSS standard (s&box supporte un sous-ensemble)          |
| `[5]` enfants          | Nesting SCSS classique                                  |
> [!tip]
> Les styles sont **scopés automatiquement** au composant.
> Pas besoin de préfixer `.monpanel .titre` — `.titre` suffit.
