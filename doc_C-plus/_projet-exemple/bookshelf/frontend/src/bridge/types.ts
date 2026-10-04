// Les formes des données échangées avec le C++. Miroir de src/bridge/dto.hpp :
// changer un nom ici sans le changer là-bas casse le contrat.

export interface Book {
  id: number;
  title: string;
  author: string;
  /** Absent quand l'année est inconnue. */
  year?: number;
  read: boolean;
  /** « AAAA-MM-JJ ». */
  addedOn: string;
}

export interface NewBook {
  title: string;
  author: string;
  year?: number;
}
