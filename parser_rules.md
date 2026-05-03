
## 1. Базовая грамматика простых арифметических выражений

### Исходные порождающие правила
* $S \rightarrow S + T \mid T$
* $T \rightarrow T * F \mid F$
* $F \rightarrow ( S ) \mid a$

### Преобразование к нестрогой нормальной форме Грейбах
* $S \rightarrow ( S ) V U \mid a V U$
* $U \rightarrow + T U \mid \lambda$
* $T \rightarrow ( S ) V \mid a V$
* $V \rightarrow * F V \mid \lambda$
* $F \rightarrow ( S ) \mid a$

### Семантические действия генератора ОПС
Каждому символу правой части порождающего правила сопоставляется действие. Действия синхронно заносятся в дополнительный магазин и выполняются при извлечении символа из магазина:
* $\Box$ — пустое действие;
* $a$ — запись в ОПС операнда из входной цепочки (переменной или константы);
* $+$ — запись в ОПС операции бинарного сложения;
* $*$ — запись в ОПС операции бинарного умножения.

### Таблица переходов LL(1)-анализатора и генератора ОПС

| Нетерминал | + | * | ( | ) | a | $\perp$ |
|---|---|---|---|---|---|---|
| **S** | | | $( S ) V U$<br>$\Box\Box\Box\Box\Box$ | | $a V U$<br>$a\Box\Box$ | |
| **U** | $+ T U$<br>$\Box\Box+$ | $\lambda$ | $\lambda$ | $\lambda$ | $\lambda$ | $\lambda$ |
| **T** | | | $( S ) V$<br>$\Box\Box\Box\Box$ | | $a V$<br>$a\Box$ | |
| **V** | $\lambda$ | $* F V$<br>$\Box\Box*$ | $\lambda$ | $\lambda$ | $\lambda$ | $\lambda$ |
| **F** | | | $( S )$<br>$\Box\Box\Box$ | | $a$<br>$a$ | |
---

## 2. Грамматика с дополнительными операциями (+, -, *, /, унарный минус)

### Исходные порождающие правила
* $S \rightarrow S + T \mid S - T \mid T$
* $T \rightarrow T * F \mid T / F \mid F$
* $F \rightarrow ( S ) \mid a \mid + G \mid - GZ$
* $G \rightarrow ( S ) \mid a$
* $Z \rightarrow \lambda$

### Преобразование к нестрогой нормальной форме Грейбах
* $S \rightarrow ( S ) V U \mid a V U \mid + G V U \mid - G V U$
* $U \rightarrow + T U \mid - T U \mid \lambda$
* $T \rightarrow ( S ) V \mid a V \mid + G V \mid - G V$
* $V \rightarrow * F V \mid / F V \mid \lambda$
* $F \rightarrow ( S ) \mid a \mid + G \mid - G Z$
* $G \rightarrow ( S ) \mid a$
* $Z \rightarrow \lambda$

### Семантические действия генератора ОПС
* $\Box$ — пустое действие;
* $a$ — запись операнда в ОПС;
* $+$ — запись в ОПС бинарного сложения;
* $-$ — запись в ОПС бинарного вычитания;
* $*$ — запись в ОПС бинарного умножения;
* $/$ — запись в ОПС бинарного деления;
* $-'$ — запись в ОПС унарного минуса.

### Таблица переходов LL(1)-анализатора и генератора ОПС

| Нетерминал | + | - | * | / | ( | ) | a | $\perp$ |
|---|---|---|---|---|---|---|---|---|
| **S** | $+ G V U$<br>$\Box\Box\Box\Box$ | $- G V U$<br>$\Box\Box-'\Box$ | | | $( S ) V U$<br>$\Box\Box\Box\Box\Box$ | | $a V U$<br>$a\Box\Box$ | |
| **U** | $+ T U$<br>$\Box\Box+$ | $- T U$<br>$\Box\Box-$ | $\lambda$ | $\lambda$ | $\lambda$ | $\lambda$ | $\lambda$ | $\lambda$ |
| **T** | $+ G V$<br>$\Box\Box\Box$ | $- G V$<br>$\Box\Box-'$ | | | $( S ) V$<br>$\Box\Box\Box\Box$ | | $a V$<br>$a\Box$ | |
| **V** | $\lambda$ | $\lambda$ | $* F V$<br>$\Box\Box*$ | $/ F V$<br>$\Box\Box/$ | $\lambda$ | $\lambda$ | $\lambda$ | $\lambda$ |
| **F** | $+ G$<br>$\Box\Box$ | $- G Z$<br>$\Box\Box-'$ | | | $( S )$<br>$\Box\Box\Box$ | | $a$<br>$a$ | |
| **G** | | | | | $( S )$<br>$\Box\Box\Box$ | | $a$<br>$a$ | |
| **Z** | $\lambda$ | $\lambda$ | $\lambda$ | $\lambda$ | $\lambda$ | $\lambda$ | $\lambda$ | $\lambda$ |

---
