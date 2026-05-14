# Backgammon in C

Mała implementacja gry Backgammon w terminalu z prostym automatycznym przeciwnikiem.

## Opis

Projekt zawiera implementację klasycznej gry Backgammon napisaną w języku C. Gra działa w terminalu i pozwala graczowi rywalizować z prostym AI.

## Wymagania

- Kompilator C (gcc, clang)
- Terminal Unix/Linux lub kompatybilny

## Kompilacja

```bash
gcc -o backgammon backgammon.c
```

## Uruchomienie

```bash
./backgammon
```

## Sterowanie

Sterowanie w grze polega na wpisaniu:
1. **Numeru kolumny** - z której chcesz przenieść pionek
2. **Jednej z wyrzuconych cyfr na kościach** - o ile pól chcesz się poruszyć

### Przykład:

```
19 3
```

- `19` - numer kolumny, z której poruszasz pionka
- `3` - cyfra wyrzucona na kościach, o ile pól pionek się porusza

## Zasady gry

Backgammon to gra dla dwóch graczy, w której celem jest przesunięcie wszystkich swoich pionków z planszy szybciej niż przeciwnik. Gra składa się z:

- **24 punkty** na planszy
- **15 pionków** dla każdego gracza
- **Dwa kości** do losowania liczby ruchów

## Funkcjonalności

- ✅ Pełna implementacja zasad Backgammona
- ✅ Interfejs tekstowy w terminalu
- ✅ Automatyczny przeciwnik z prostą sztuczną inteligencją
- ✅ Losowanie kości
- ✅ Walidacja ruchów

## Autor

vexwith

## Licencja

MIT
