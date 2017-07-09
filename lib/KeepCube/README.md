# KeepCUBE Command Library

## Header

## Path

- `setPath()` natvrdo nastaví Path
- `getPath()` natvrdo vrátí Path

## Parameters

- `int numOfParams()` vrátí počet parametrů v příkazu

old:

- `void set(String)` natvrdo nastaví command na dodaný String
- `void readSerial()` načte jeden command ze serial bufferu
- `String getIdentifier()` vrátí identifikátor příkazu (první 3 velká písmena)
- `void setIdentifier(String)` nastaví identifikátor příkazu
- `String getParam(const char)` vrátí obsah parametru jako string. Po přidání .toInt() na konec bude vracet hodnotu jako int.
- `void setParam(const char, int)` nastaví hodnotu parametru. Pokud tam parametr s požadovaným názvem není, vytvoří nový.
- `void setParam(const char, String)` to samé, jen se Stringem.
- `void delParam(const char)` smaže zvolený parametr
- `bool hasParam(const char)` zjistí, zda se daný parametr nachází v příkazu
- `String toString();` vrátí parametr jako String

## Nová syntaxe:

# `#SLP{pAN1}0,ff0000,1000,00ff00,500,0000ff,800;`

Srovnání se starou verzí:

# `#SVPC&#SVAC\&#NRFA1D\\\&#SLPL0C\\\\\\&ff0000,00ff00,0000ff\\\\\\&T\\\\\\&1000,500,800\\\\\\&;\\\&;\&;&;`

musíte uznat že takhle to fakt nejde :) přibližně **50% úspora velikosti** oproti starému formátu (který už tak byl snad 60% JSON varianty btw)

--------------------------------------------------------------------------------

## Pros:

- Žádné šílené zapouzdřování :)
- Žádné šílené escape sekvence (\&)
- Přehledné, jasné a snadno pochopitelné
- Jednoduše parsovatelné jak lidmi tak počítačem
- Malé
- Příkaz je celistvý po celou dobu transportu systémem, při průchodu jenotlivými platformami se nemodifikuje, jen se do něj "nakoukne", aby se zjistilo co s ním

--------------------------------------------------------------------------------

## Cons:

- málo možností v bloku path

--------------------------------------------------------------------------------

## Fičury

-

--------------------------------------------------------------------------------

# Header

## Délka

Header může mít délku od jednoho znaku po 5, z pochopitelných důvodů. Skutečná délka nebude parsery zatím limitována, jen je třeba brát ohledy na ATMegy a ostatní jednoduchý hardware. Může obsahovat i čísla, nesmí obsahovat žádný ze zakázaných znaků.

## Case sensitive?

Ano, header je case sensitive.

--------------------------------------------------------------------------------

# Path

- Je to **náhrada dementního zapouzdřování** příkazů.
- Je to sekvence znaků, určující cestu (path), kudy má daný příkaz putovat.
- Jednotlivé znaky představují body, které má příkaz v naší infrastruktuře překonat.
- Je lemována znaky `{` a `}` z obou stran.
- Je case sensitive, tzn. `T` může znamenat něco jiného než `t`
- Může obsahovat uppercase znaky A - Z (ang. abecedy) a lowercase znaky a - z (ang. abecedy). Za každým písmenem může následovat libovolné​ číslo, **Atribut**, třeba pro NRF adresu atd.
- Softwarové body budou označeny lowercase znaky, hardwarové budou značeny uppercase znaky. (každý bod jeden znak)
- Ve zvláštních případech se může zvolit znak neodpovídající tvrzení výše. Bacha ale na to, jestli už takový náhodou není zabraný.
- `{brpAN1}` by znamenalo třeba backend - redis - python - atmega - NRF (adresa 1)
- Kdyby výše uvedený příklad dorazil Pythonu, najde značku "p" a podívá se co následuje za ní. Zjistí, že je tam "A", takže ví, že má příkaz poslat serialem na atmegu. Nemusí nic parsovat, jen do příkazu nakoukne a pošle ho dál.
- Path může sloužit jako zpětná adresa, kam se má poslat odpověď.

--------------------------------------------------------------------------------

# Zápis parametrů

- Struktura parametrů se váže k headeru příkazu
- Header určuje význam parametrů
- Pro každý header je přesně definován počet, pozice a jiné parametry příkazů
- Parametry se odlišují pozicí
- Jednotlivé parametry jsou odděleny jednoduchou čárkou `,`
- Tento způsob je dobře známý z parametrů funkcí běžných programovacích jazyků. Proč nepoužít něco, co už je běžně používané a funkční :)

**Tento způsob je úspornější z hlediska délky příkazu.**

## Integer

- Integer (číslo) se zapisuje jednoduše tak jak je.

## String

- String (text) se zapisuje jednoduše tak jak je. (bacha na escape znaky)

## Array

- Trochu zvláštní případ, zatím nemám vymyšlenou syntaxi pro více arrayů v jednom příkazu, ale jedno n-dimenzionální array není aktuálně problém.
- Jedno n-dimenzionální pole se zapisuje stejně jako paramery céčkové funkce printf, tzn. několik parametrů na začátku je pevných. Na konci může být několik parametrů "duplikovatelných", takže sekvence těchto parametrů se může opakovat donekonečna. **Příklad parametrů:** `A, B, C, D, E, D, E, D, E, D, E, D, E.......`

**V gui bude potom jakýsi editor příkazů, který bude mít na jedné straně haldu input prvků, skrz které se bude přidávat header, patch a parametry. Na druhé straně bude náhled právě sestavovaného příkazu. Nahoře formátovaný a se zbarvenou syntaxí, dole neformátovaný černobílý připravený na případné zkopírování.**

--------------------------------------------------------------------------------

# Escape sekvence

## Zakázané (escapovatelné) znaky

- ## `#` `;` `{` `}` `,`

- Tyto znaky se zapisují tzv. escape sekvencí, takže `#` --> `\#`

- Escape sekvence se použije fakt jen v extrémních případech. Zapouzdřování příkazů nedává smysl z důvodu existence bloku path.

- Pozor: takto vyescapovaný znak musí být v reálném kódu, takže když zapisujete nějaký DSC napevno ve vašem IDE, vyescapujte si i ten backslash, protože běžné kompilátory a interprety zkompilují (interpretují) `"\\"` jako `"\"`

--------------------------------------------------------------------------------

# FAQ

- Jak se pozná jestli je parametr String, Integer nebo něco dalšího?

  - Nepozná, strukturu budete mít v kódu napevno. Tzn. if header == "slp" then x = getInt(0); y = getString(1); . Prametr určuje pořadí.

- Jak odešlu kód pro 433 transmitter, když už není zapouzdřování?

  - Jednoduše, kód se nastaví jako parametr a v path bloku se nastaví jako poslední bod něco jako I (ism)
